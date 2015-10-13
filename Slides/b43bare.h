#ifndef __B43_BARE__
#define __B43_BARE__

#define AOAbool short
#define BARE_PC 0



/* these are currently used */
/* prototype for callback function*/

void callbackrcv (struct sock *sk, int bytes);
int lb_sendmsg (char *msgdata, int size);
int lb_rcvmsg  (char *msgdata);




#endif  /* ifndef _B43_BARE_ */
