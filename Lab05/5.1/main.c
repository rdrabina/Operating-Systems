#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <memory.h>

int parseArguments(char * buffer, char ** tokens);
int parseProgram(char * buffer, char ** programs);
int forkBuilder(char ** programs, int size);



int main(int argc, char ** argv)
{
	if(argc!=2)
	{	
		printf("Invalid number of arguments");
		return 0;
	}
	
	
	int status = 0, process_number = 0;
	char buffer[200];
	char *programs[10];	

	FILE * file = fopen(argv[1], "r");
	if(file == NULL) 
	{
		printf("Invalid file path");
		return 0;
	}
	

	while(fgets(buffer,200, file) != NULL)
	{
		process_number = parseProgram(buffer, programs);
		pid_t pid = fork();

		if(!pid)
		{
			forkBuilder(programs, process_number - 1);
		}
		else
		{
			printf("I am waiting for %d\n", pid);
			if(waitpid(pid, &status, 0) == -1)
			{
				printf("Cannot use waitpid\n");
				exit(1);
			}

			const int exit_status = WEXITSTATUS(status);
			printf("Succeed: %d\n", pid);

			if(!exit_status) printf("Line succeed\n");
			else
			{
				printf("Line failed\n");
				exit(1);
			}
		}
	}
	fclose(file);	
	
return 0;
}

int parseArguments(char * buffer, char ** tokens)
{
	int size = 0;	
	char *tmp = strtok(buffer, " \n");
	while(tmp!=NULL)
	{
		tokens[size++]= tmp;
		tmp = strtok(NULL, " \n");
	}
	
	tokens[size] = NULL;
return 0;
}

int parseProgram(char * buffer, char ** programs)
{
	int size = 0;
	char *tmp = strtok(buffer, "|");
	while(tmp!= NULL)
	{
		programs[size++] = tmp;
		tmp = strtok(NULL, "|");
	}
	programs[size++] = NULL;
return size - 1;
}

int forkBuilder(char ** programs, int size)
{
	char *args[200];
	int tab[2]={0,0};
	int status = 0;

	parseArguments(programs[size], args);

	pipe(tab);
	pid_t pid = fork();

	if(pid == -1) 
	{
		printf("Cannot fork");
		exit(1);
	}
	else if(pid == 0) // child
	{
		int child = 0;
		close(tab[0]);
		dup2(tab[1], STDOUT_FILENO);

		if (size > 1) child = forkBuilder(programs, size - 1);
		else if (size > 0) 
		{
			size -= 1;
			parseArguments(programs[size], args);
			printf("New process %s, pid %d and ppid: %d\n", args[0], getpid(), getppid());
			child = execvp(args[0], args);
			close(tab[1]);
		}
		
		printf("Child is out: %s %d\n", args[0], pid);
		exit(child);		
	}
	else //parent
	{
		printf("New process %s, pid %d and ppid: %d\n", args[0], getpid(), getppid());
		close(tab[1]);
		dup2(tab[0], STDIN_FILENO);

		printf("I am waiting for %d\n", pid);

		if(waitpid(pid, &status, 0) == -1)
		{
			printf("Cannot use waitpid");
			exit(1);
		}
		
		const int  exit_status = WEXITSTATUS(status);

		printf("Exit status: %d, pid %d\n", exit_status, pid);

		if(!exit_status)
		{	
			printf("Succeed: %d\n", pid);
			int succeed = execvp(args[0],args);
			close(tab[0]);
		
			if(!succeed)
			{
				printf("Failed: %d\n", getpid());
				exit(1);
			}
			exit(0);
		}
		else
		{		
			printf("Failed: %d\n", pid);
			exit(1);
		}
	}

}

