#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <zconf.h>

int parseArguments(char * buffer, char ** tokens);
int SIZE_OF_LINE=1000;
int AMOUNT_OF_LINES=10;


int main(int argc, char ** argv)
{
	if(argc!=2)
	{	
		printf("Invalid number of arguments");
		return 0;
	}
	
	char *buffer = (char*)calloc(SIZE_OF_LINE, sizeof(char));	

	FILE * file = fopen(argv[1], "r");
	if(file == NULL) 
	{
		printf("Invalid file path");
		return 0;
	}
	

	while(fgets(buffer,SIZE_OF_LINE-1, file) != NULL)
	{
		printf("%s", buffer);
		char ** tokens = (char **) calloc(AMOUNT_OF_LINES, sizeof(char*));
		parseArguments(buffer, tokens);
		
		pid_t child = fork();

		if(child == 0)
		{
			execvp(tokens[0], tokens);
			exit(1);
		}
		else
		{
			int status;
			wait(&status);
			if(status != 0)
			{
				printf("Some problems occur\n");
				break;
			}
		}
		free (tokens);
	}
	fclose(file);	
	
return 0;
}

int parseArguments(char * buffer, char ** tokens)
{
	int size = 0;	
	char *tmp = strtok(buffer, " ");
	while(tmp!=NULL)
	{
		if(strchr(tmp, (int) '\n') == NULL)
		{
			tokens[size]=tmp;
		}
		else
		{
			size_t length = strlen(tmp);
			tokens[size] = (char*) calloc (length, sizeof(char));
			memcpy(tokens[size], tmp, length - 1);
		}
		tmp= strtok(NULL, " ");
		size++;
	}
return 0;
}

