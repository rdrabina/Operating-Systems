#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

// ./nazwa /home -1 2018 3 18 20 30 50

int go_through_directory(char * directory, int compare_mode, time_t compare_date);
int print_properties(struct stat file);

int main(int argc, char ** argv)
{
	if(argc!=9)
	{
		printf("Invalid number of arguments");
		return 0;
	}

	struct tm tm_date;
	char *directory;
	int index=0;

	directory=argv[1];
	while(directory[index]!= '\0') index++;
	index--;
	if(directory[index]=='/') directory[index]='\0';
	
	if(directory[0]!='/')
	{
		char cwd[500];
		if(getcwd(cwd,sizeof(cwd)) ==NULL) printf("Cannot get cwd");
		strcat(cwd, "/");
		strcat(cwd,directory);
		directory=cwd;
	}

	if(atoi(argv[2]) < -1 || atoi(argv[2]) > 1){
	printf("Invalid comparator!");
	}
	if(atoi(argv[3]) < 1900){
	printf("Invalid year!");
	}

	if(atoi(argv[4]) < 1 || atoi(argv[4]) > 12){
	printf("Invalid month!");
	}

	if(atoi(argv[5]) < 1 || atoi(argv[5]) > 31){
	printf("Invalid day!");
	}

	if(atoi(argv[6]) < 0 || atoi(argv[6]) > 23){
	printf("Invalid hour!");
	}

	if(atoi(argv[7]) < 0 || atoi(argv[7]) > 59){
	printf("Invalid minute!");
	}

	if(atoi(argv[8]) < 0 || atoi(argv[8]) > 59){
	printf("Invalid second!");
	}
	
	int mode=atoi(argv[2]);	
	tm_date.tm_year = atoi(argv[3]) - 1900;
    	tm_date.tm_mon = atoi(argv[4]) - 1;
	tm_date.tm_mday = atoi(argv[5]);
	tm_date.tm_hour = atoi(argv[6]);
	tm_date.tm_min = atoi(argv[7]);
	tm_date.tm_sec = atoi(argv[8]);
	tm_date.tm_isdst = 0;

	time_t time=mktime(&tm_date);

	go_through_directory(directory,mode, time); 
	
return 0;
}


int go_through_directory(char * directory, int compare_mode, time_t compare_date)
{
	DIR *dir=opendir(directory);
	if(dir==NULL) printf("Cannot open directory");
	struct dirent * file = readdir(dir);

	while(file!=NULL)
	{
		char absolute_path[500];
		strcpy(absolute_path,directory);
		strcat(absolute_path, "/");
		strcat(absolute_path, file -> d_name);

		if(strcmp(file->d_name,".") != 0 && strcmp(file->d_name,"..")!= 0)
		{
			struct stat properties;
			lstat(absolute_path,&properties);

			if(S_ISDIR(properties.st_mode))
			{
				pid_t pid = fork();
				if(pid < 0 ) printf("Cannot fork");
				else if (pid == 0)
				{
					go_through_directory(absolute_path,compare_mode,compare_date);
					exit(0);
				}
				else
				{
					wait(NULL);
				}				 
			}
			else if(S_ISREG(properties.st_mode))
			{
				if(compare_mode==-1 && compare_date>properties.st_mtime) 
				{
					printf("Path: \t %s\n", absolute_path);
					print_properties(properties);
				}
				if(compare_mode==0 && compare_date==properties.st_mtime) 
				{
					printf("Path: \t %s\n", absolute_path);
					print_properties(properties);
				}
				if(compare_mode==1 && compare_date<properties.st_mtime) 
				{
					printf("Path: \t %s\n", absolute_path);
					print_properties(properties);
				}	
			}	
		}
		file=readdir(dir);	
	}
	closedir(dir);
return 0;
}

int print_properties(struct stat file)
{
	printf("File size: \t %ld", file.st_size);
	printf("\nProperties: \t");
	printf((file.st_mode & S_IRUSR) ? "r" : "-");
    	printf((file.st_mode & S_IWUSR) ? "w" : "-");
    	printf((file.st_mode & S_IXUSR) ? "x" : "-");
    	printf((file.st_mode & S_IRGRP) ? "r" : "-");
    	printf((file.st_mode & S_IWGRP) ? "w" : "-");
    	printf((file.st_mode & S_IXGRP) ? "x" : "-");
    	printf((file.st_mode & S_IROTH) ? "r" : "-");
    	printf((file.st_mode & S_IWOTH) ? "w" : "-");
    	printf((file.st_mode & S_IXOTH) ? "x" : "-");
	printf("\nModified: \t %s\n", ctime(&file.st_mtime));
return 0;
}

