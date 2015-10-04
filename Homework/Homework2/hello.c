#include <stdio.h>
#include <stdlib.h>

int main ()
{
	// Open file "file.txt"
	char file_name[] = "file.txt";
	FILE *fp = fopen(file_name, "r+a");

	// Check for error opening file	
	if(fp == NULL)
	{
		perror("Error opening file.\n");
		exit(EXIT_FAILURE);
	}

	// Print file contents
	printf("File %s opened and reads as follows: \n", file_name);
	char *line = NULL;
	size_t length = 0;
	ssize_t read;
	while((read = getline(&line, &length,  fp)) != -1)
	{
		printf("%s", line);
	}

	// Write to file
	char message[] = "Hello to you too!\n";
	printf("Writing to file: %s", message);
	fprintf(fp, message);

	// Close file
	fclose(fp);

	// Cleanup
	if(line != NULL)
		free(line);

	// Return
	return 0;
}
