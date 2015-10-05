#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <windows.h>

int main ()
{
	char line[256];
	size_t length;
	FILE *fp = NULL;
	char message[] = "Hello to you too!\n";
	
	// Open file "file.txt"
	char file_name[] = "file.txt";
	fp = fopen(file_name, "r");

	// Check for error opening file	
	if(fp == NULL)
	{
		perror("Error opening file.\n");
		exit(EXIT_FAILURE);
	}

	// Check file type
	if(GetFileType(fp) != FILE_TYPE_CHAR)
	{
		printf("File type not char\n");
	}

	// Print file contents
	length = 0;
	printf("File %s opened and reads as follows: \n", file_name);
	while (fgets(line, sizeof(line), fp))
	{
		printf("%s", line);
	}

	// Close file
	fclose(fp);
	// Open file for appending
	fp = fopen(file_name, "a");
	// Write to file
	printf("Writing to file: %s", message);
	fprintf(fp, "%s", message);

	// Close file
	fclose(fp);

	// Return
	return 0;
}
