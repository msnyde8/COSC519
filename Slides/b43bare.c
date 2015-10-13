#include <linux/export.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/in.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/inet.h>
#include "b43bare.h"

#define SERVERPORT 9000
static struct socket *clientsocket=NULL;
static struct socket *udpsocket=NULL;
static DECLARE_COMPLETION( threadcomplete );
static int com_thread_pid;
static char ipaddr [15] = "10.55.12.221";
static char *rcvbuffer = 0;        /* need it for callback function */
static int rcvbuffercount = 0;     /* need for callback function */
static int totalpackets = 0;     /* total packets in the callbackrcv  */


/*---------------------------------------------------------------*/
/*   callbackrcv() derived from udpRecvCallback.c                   */
/* kernel calls this method when a packet is arrived */
/* */
/*---------------------------------------------------------------*/

void callbackrcv(struct sock *sk, int bytes){
        struct sockaddr_in client;
        struct sockaddr *address;
        /*unsigned char buffer[100];*/
        int len = 0;
        int i = 0;
        int err;
        struct msghdr msg;
        struct iovec iov;
        mm_segment_t oldfs;
        struct sk_buff * skb;
        len = skb_queue_len(&sk->sk_receive_queue);
        printk(KERN_DEBUG "RKKW: T301: callbackrcv() %i bytes received len: %i \n", bytes, len);
       /* retrieve one packet at a time from receive Q and use it as needed */
        while(len > 0){
                skb = skb_dequeue(&sk->sk_receive_queue);
                totalpackets++; 
                printk(KERN_DEBUG "RKKW: T1012 vor\n");
                /*important: cut the udp header from skb->data!*/
                /*srcport, destport, len, checksum -> 8 bytes*/
                u16 *sp = (u16 *)skb->data;
                skb->data=(char *)skb->data+2;
                u16 *dp = (u16 *)skb->data;
                skb->data=(char *)skb->data+2;
                u16 *l = (u16 *)skb->data;
                u16 spk = ntohs(*sp);
                u16 dpk = ntohs(*dp);
                u16 lk = ntohs(*l);
                printk(KERN_DEBUG "RKKW: T302 callbackrcv() ports: %hu und %hu len %hu\n",spk, dpk, lk);
                /* sk->data is pointing to data + 4 already */
                /*  now it should point to 4 more bytes to trim 8 byte header */
                /*memcpy(buffer, skb->data+4, skb->len-8); */
                /* accumulate data packets in rcvbuffer as total data */
                memcpy((rcvbuffer+rcvbuffercount), skb->data+4, skb->len-8); 
                rcvbuffercount = rcvbuffercount + skb->len - 8; 
                printk(KERN_DEBUG "RKKW: T303 callbackrcv() rcvbuffercount: %i \n", rcvbuffercount);
                /*
                for(i;i<skb->len; i++){
                        printk("buf[%i] = %c\n",i,buffer[i]);
                }
                */
                /*printk(KERN_DEBUG "T1014 received data %s with len %u\n",buffer, skb->len);*/
                len--;
                kfree_skb(skb);
                /*never do a sleep in this context!*/
                //printk(KERN_ERR "start sleeping\n");
                // msleep_interruptible(HZ*2);
        }
        /* msg.msg_name = &client; */
/*         msg.msg_namelen = sizeof( struct sockaddr_in ); */
/*         msg.msg_control = NULL; */
/*         msg.msg_controllen = 0; */
/*         msg.msg_iov    = &iov; */
/*         msg.msg_iovlen = 1; */
/*         iov.iov_base = buffer; */
/*         iov.iov_len  = sizeof(buffer); */
/*         oldfs = get_fs(); */
/*         set_fs( KERNEL_DS ); */
/*         len = sock_recvmsg( udpsocket, &msg,  */
/*                             sizeof(buffer), 0 ); */
/*         set_fs( oldfs ); */
/*         if( len>0 ) { */
/*                 address = (struct sockaddr *)&client; */
/*                 printk(KERN_INFO "msg %s from %u.%u.%u.%u\n", buffer, */
/*                        (unsigned char)address->sa_data[2], */
/*                        (unsigned char)address->sa_data[3], */
/*                        (unsigned char)address->sa_data[4], */
/*                        (unsigned char)address->sa_data[5] ); */
/*         } */
        return;
}

/**************************************************************/
/*  lb_sendmsg                                                */
/*  sends a UDP message upto 64000                            */
/*  this function opens a socket, sends a mesage and closes  */ 
/*  only one function to do it all                            */ 
/**************************************************************/
int lb_sendmsg (char *msgdata, int size)
{
  int len;
  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;
  struct sockaddr_in to;

 /* initialize and setup socket to send messages to bare */
  printk(KERN_DEBUG "LB_SOCKET T101  start: lb_sendmsg() msg: %s \n", msgdata);
  if( sock_create( PF_INET,SOCK_DGRAM,IPPROTO_UDP,&clientsocket)<0 ){
    printk( KERN_DEBUG "LB_SOCKET: lb_sendmsg()  TERROR001: server: Error creating clientsocket \n" );
    return -EIO;
   }

  memset(&to,0, sizeof(to));
  to.sin_family = AF_INET;
  to.sin_addr.s_addr = in_aton(ipaddr); 
      /* bare PC destination address */
  to.sin_port = htons( (unsigned short) SERVERPORT );
  memset(&msg,0,sizeof(msg));
  msg.msg_name = &to;
  msg.msg_namelen = sizeof(to);
  iov.iov_base = msgdata;
  iov.iov_len  = size;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_iov    = &iov;
  msg.msg_iovlen = 1;
  oldfs = get_fs();
  set_fs( KERNEL_DS );
  len = sock_sendmsg( clientsocket, &msg, size );
  set_fs( oldfs );

  /* close socket */
  if( clientsocket )
    {
    /*msleep_interruptible(HZ*2);*/
    sock_release( clientsocket );
    printk(KERN_DEBUG "LB_SOCKET T102  close socket: lb_sendmsg() \n");
    if( com_thread_pid ) {
      kill_proc_info( com_thread_pid, SIGTERM, 0 );
      wait_for_completion( &threadcomplete );
      }
    } 

  printk(KERN_DEBUG "LB_SOCKET T103  end: lb_sendmsg() \n");

  return len;
}
/**************************************************************/
/*  lb_rcvmsg                                                 */
/*  initialize socket, do a callback and rcv data             */
/*  first packet indicates how many more packets need to      */
/*   received, all packets are received and no of bytes       */
/*   received is returned, caller simply makes this call      */
/*   and everything is managed here                           */
/*   it can receive upto 64000 bytes                         */
/**************************************************************/
int lb_rcvmsg(char *rcvmsg)
{
  /*initialize and setup socket to receive messages from bare*/
  struct sockaddr_in server;
  int servererror;

  rcvbuffer = (char*)kmalloc(64000, __GFP_COLD); 
  rcvbuffercount = 0; 
  totalpackets = 0; 

  printk(KERN_DEBUG "LB_SOCKET: T201 start: lb_rcvmsg()\n");
  if( sock_create( PF_INET,SOCK_DGRAM,
      IPPROTO_UDP,&udpsocket)<0 ) {
    printk(KERN_DEBUG "LB_SOCKET: lb_rcvmsg() TERROR002 server: Error creating udpsocket.\n");
    return -EIO;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( (unsigned short)SERVERPORT);
  servererror = udpsocket->ops->bind(udpsocket, (struct sockaddr *) &server, 
				     sizeof(server ) );
  if( servererror ) {

    printk(KERN_DEBUG "LB_SOCKET: lb_rcvmsg() Server Error: TERROR003: %i \n", servererror); 
    sock_release( udpsocket );
    return -EIO;
  }

  printk(KERN_DEBUG "LB_SOCKET: T202 start lb_rcvmsg()\n");
  /* pointer reallocation does not work in kernel */
  /* copy the data to pass by reference!! assigning pointers will not work*/

  udpsocket->sk->sk_data_ready = callbackrcv;
  /*wait until a packet arrived here, synchronous rcv */

  while (rcvbuffercount == 0) 
      {
       msleep_interruptible(HZ*2);
      }

  memcpy(rcvmsg, rcvbuffer, rcvbuffercount); 
  printk(KERN_DEBUG "LB_SOCKET: T203 lb_rcvmsg: Leaving While Loop \n"); 

  /* close socket */
  if( com_thread_pid ) {
      kill_proc_info( com_thread_pid, SIGTERM, 0 );
      wait_for_completion( &threadcomplete );
      }
  if( udpsocket )
    sock_release( udpsocket );

  printk(KERN_DEBUG "LB_SOCKET: T206 end lb_rcvmsg(): TOTAL BYTES RCVD: %i  TOTAL PACKETS: %i \n", 
                    rcvbuffercount, totalpackets);

  return rcvbuffercount; /* return size */ 
}