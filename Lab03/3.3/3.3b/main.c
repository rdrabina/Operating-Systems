#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <zconf.h>
#include <sys/time.h>
#include <sys/resource.h>

int parseArguments(char * buffer, char ** tokens);
void setLimits(int seconds, int megabytes);
double getTime(struct timeval time);
int SIZE_OF_LINE=50;
int AMOUNT_OF_LINES=100;


int main(int argc, char ** argv)
{
	if(argc!=4)
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

	struct rusage time_start;
  	struct rusage time_end;
	
	while(fgets(buffer,SIZE_OF_LINE-1, file) != NULL)
	{
		getrusage(RUSAGE_CHILDREN, &time_start);		
		printf("%s", buffer);
		char ** tokens = (char **) calloc(AMOUNT_OF_LINES, sizeof(char*));
		int size = parseArguments(buffer, tokens);
		
		pid_t child = fork();

		if(child == 0)
		{
			setLimits((int) strtol(argv[2], NULL, 10), (int) strtol(argv[3], NULL, 10));		
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
		getrusage(RUSAGE_CHILDREN, &time_end);
		double system = getTime(time_end.ru_utime) - getTime(time_start.ru_utime);
		double user = getTime(time_end.ru_stime) - getTime(time_start.ru_stime);

		printf("User time: %lf   ", user);
		printf("System time: %lf     ", system);
		printf("\n\n");
		}
		free (tokens[size]);
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
			size++;
		}
		else
		{
			size_t length = strlen(tmp);
			tokens[size] = (char*) calloc (length, sizeof(char));
			memcpy(tokens[size], tmp, length - 1);
			size++;
		}
		tmp= strtok(NULL, " ");
	}
return size;
}

void setLimits(int seconds, int megabytes)
{
	struct rlimit setAS, setCPU;
	int bytes = megabytes*1024*1024;
	setAS.rlim_cur = (rlim_t) bytes*1/2;
	setAS.rlim_max = (rlim_t) bytes;
	setrlimit(RLIMIT_AS, &setAS);
	setCPU.rlim_cur = (rlim_t) seconds*1/2;
	setCPU.rlim_max = (rlim_t) seconds;
	setrlimit(RLIMIT_CPU, &setCPU);
return;
}

double getTime(struct timeval time) {
  return (double)(time.tv_sec * 100000000.0 + time.tv_usec) / 100000000.0;
}

