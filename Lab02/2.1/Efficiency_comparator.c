#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>



double prevUserTime;
double prevSystemTime;
void reset_time();
void print_time();
struct tms *run_time;
struct tms *prev;

int ifInt(char *input);
int generate(FILE ** libHandleFrom,FILE ** libHandleTo, int amount_of_records, int size_of_records);
int sysSort(int * sysHandle, int amount_of_records, int size_of_records);
int libSort(FILE ** libHandle, int amount_of_records, int size_of_records);
int sysCopy(int * sysHandle, int *sysHandleTo, int amount_of_records, int size_of_records);
int libCopy(FILE ** fromHanlde, FILE ** toHandle, int amount_of_records, int size_of_records);


int main(int argc, char ** argv)
{
	
	if(argc<5 || argc>7)
	{
	printf("Invalid amount of arguments \n");
	return 0;
	}

	char * operation, *lib_sys, *file1, *file2="copy.txt";;
	int amount_of_records, size_of_records, sysHandle,sysHandleTo;
	FILE *libHandle1=NULL, *libHandle2=NULL;
	operation = argv[1];
	
	run_time=(struct tms*)malloc(sizeof(struct tms));
  	times(run_time);

	if(argc==5)
	{
	if(!strcmp(operation, "generate")==0) return 0;
		else{
			file1=argv[2];
			if((libHandle1=fopen(file1, "w"))==NULL){
				printf("Argument no. 2 is invalid file name!");
				return 0;
			}
			libHandle2=fopen(file2, "w");
			
			if(!ifInt(argv[3])){
				printf("Argument no. 3 is invalid number");
				return 0;
			}
			amount_of_records=atoi(argv[3]);

			if(!ifInt(argv[4])){
				printf("Argument no. 4 is invalid number");
				return 0;
			}
			size_of_records=atoi(argv[4]);
		    }
	printf("Amount: %d \t Size: %d	\n", amount_of_records, size_of_records);
	generate(&libHandle1,&libHandle2,amount_of_records,size_of_records);
	}


	if(argc==6)
	{
		if(!strcmp(operation, "sort") == 0) return 0;
		else
		{	
				file1=argv[2];			
				if((libHandle1=fopen(file1, "rw"))==NULL){
				printf("Argument no. 2 is invalid file name!");
				return 0;
			}
			else libHandle1=fopen(file1, "rw");
			if(!ifInt(argv[3])){
				printf("Argument no. 3 is invalid number");
				return 0;
			}
			amount_of_records=atoi(argv[3]);

			if(!ifInt(argv[4])){
				printf("Argument no. 4 is invalid number");
				return 0;
			}
			size_of_records=atoi(argv[4]);
			lib_sys=argv[5];

		
				fseek(libHandle1, 0, SEEK_SET);
				fclose(libHandle1);	
				libHandle1=NULL;

			
				if(!strcmp(lib_sys, "lib"))
				{
					if((libHandle1=fopen(file1, "r+"))==NULL)
					{		
						printf("Argument no. 5 is invalid file source");
						return 0;
					}
					else libHandle1=fopen(file1, "r+");
				}
				else if(!strcmp(lib_sys, "sys"))
				{	
					if((sysHandle=open(file1, O_RDWR))==-1)
					{	
						printf("Argument no. 5 is invalid file source");
						return 0;
					}
					else	sysHandle=open(file1, O_RDWR);
				}
				else 
				{
					printf("Argument no. 5 is invalid file source");
					return 0;
				}
		}
		if(!strcmp(lib_sys, "lib"))
		{
			printf(" Lib Sorting \n");
			reset_time(); 
			libSort(&libHandle1,amount_of_records,size_of_records);
			print_time();
		}
		else
		{
			printf(" Sys Sorting \n");
			reset_time(); 
			sysSort(&sysHandle,amount_of_records,size_of_records);
			print_time();
		}
	}

	
	if(argc==7)
	{
		if(!strcmp(operation, "copy") == 0) return 0;
		else
		{	
			file1=argv[2];
			file2=argv[3];
			if((libHandle1=fopen(file1, "r"))==NULL){
				printf("Argument no. 2 is invalid file name!");
				return 0;
			}
			if((libHandle2=fopen(file2, "w+"))==NULL){
				printf("Argument no. 3 is invalid file name!");
				return 0;
			}
			if(!ifInt(argv[4])){
				printf("Argument no. 4 is invalid number");
				return 0;
			}
			amount_of_records=atoi(argv[4]);

			if(!ifInt(argv[5])){
				printf("Argument no. 5 is invalid number");
				return 0;
			}
			size_of_records=atoi(argv[5]);
			lib_sys=argv[6];

			
				if(!strcmp(lib_sys, "lib"))
				{
					if((libHandle1=fopen(file1, "r"))==NULL)
					{		
						printf("Argument no. 6 is invalid file source[1]");
						return 0;
					}
					if((libHandle2=fopen(file1, "w+"))==NULL)
					{		
						printf("Argument no. 6 is invalid file source[2]");
						return 0;
					}
					else
					{	
						libHandle1=fopen(file1, "r");
						libHandle2=fopen(file2, "w+");
					}
				}
				else if(!strcmp(lib_sys, "sys"))
				{	
					if((sysHandle=open(file1, O_RDONLY))==-1)
					{	
						printf("Argument no. 6 is invalid file source[1]");
						return 0;
					}
					if((sysHandleTo=open(file2, O_CREAT | O_WRONLY))==-1)
					{	
						printf("Argument no. 6 is invalid file source[2]");
						return 0;
					}
					else
					{
						sysHandle=open(file1, O_RDONLY);
						sysHandleTo=open(file2, O_CREAT | O_WRONLY);
					}	
				}
				else 
				{
					printf("Argument no. 6 is invalid file source");
					return 0;
				}
		}
		if(!strcmp(lib_sys, "lib"))
		{
			printf(" Lib Copying \n");
			reset_time(); 
			libCopy(&libHandle1,&libHandle2,amount_of_records,size_of_records);
			print_time();
		}
		else
		{
			printf(" Sys Copying \n");
			reset_time(); 
			sysCopy(&sysHandle,&sysHandleTo,amount_of_records,size_of_records);
			print_time();
		}
	}

return 0;
} 

int ifInt(char * input)
{
	int i;
	for(i=0;input[i]!='\0';i++)
	{
		if(input[i]<'0' || input[i]>'9') return 0;	
	}	
return 1;
}

int generate(FILE ** libHandle, FILE ** libHandleCopy, int amount_of_records, int size_of_records)
{	
	FILE *randomFile;
	int c=amount_of_records*size_of_records;
	char random[c];

	  if((randomFile=fopen("/dev/urandom","r"))==NULL){
	    printf("Cannot open /dev/random\n");
	    return 0;
	  }
	  fread(random,sizeof(char),c,randomFile);
	  fclose(randomFile);
	  fwrite(random,sizeof(char),c,*libHandle);
	  fclose(*libHandle);
	  fwrite(random,sizeof(char),c,*libHandleCopy);
	  fclose(*libHandleCopy);

return 0;
}
int sysSort(int * sysHandle, int amount_of_records, int size_of_records)
{
	int n=amount_of_records;
	int i,j;
	unsigned char * array1=malloc(size_of_records), *array2=malloc(size_of_records);
		for(i=0;i<n-1;i++)
		{
			
			lseek(*sysHandle,i*size_of_records,0);
			if(read(*sysHandle,array1,size_of_records)==0) return 0;
			for(j=i-1;j>=0;j--)
			{
				lseek(*sysHandle,j*size_of_records,0);
				if(read(*sysHandle,array2,size_of_records)==0) return 0;
				if(array1[0]>=array2[0]) break;
				if(write(*sysHandle,array2,size_of_records)==0) return 0;
			}
			lseek(*sysHandle,(j+1)*size_of_records,0);
			if(write(*sysHandle,array1,size_of_records)==0) return 0;
		 }
		    
	close(*sysHandle);
	free(array1);
	free(array2);
return 0;
}
int libSort(FILE ** libHandle, int amount_of_records, int size_of_records)
{
	int n=amount_of_records;
	int i,j;
	unsigned char * array1=malloc(size_of_records), *array2=malloc(size_of_records);
		for(i=0;i<n-1;i++)
		{
			
			fseek(*libHandle,i*size_of_records,0);
			if(fread(array1,1,size_of_records,*libHandle)!=size_of_records) return 0;
			for(j=i-1;j>=0;j--)
			{
				fseek(*libHandle,j*size_of_records,0);
				if(fread(array2,1,size_of_records,*libHandle)!=size_of_records) return 0;
				if(array1[0]>=array2[0]) break;
				if(fwrite(array2,1,size_of_records,*libHandle)!=size_of_records) return 0;
			}
			fseek(*libHandle,(j+1)*size_of_records,0);
			if(fwrite(array1,1,size_of_records,*libHandle)!=size_of_records) return 0;
		 }
		    
	fclose(*libHandle);
	free(array1);
	free(array2);
return 0;
}

int sysCopy(int * sysHandle, int * sysHandleTo, int amount_of_records, int size_of_records)
{
char buffer[size_of_records];
int i;
for(i=0;i<amount_of_records;i++)
{
	read(*sysHandle, buffer, i*size_of_records);
	write(*sysHandleTo, buffer, i*size_of_records);
}
close(*sysHandle);
close(*sysHandleTo);
return 0;
}

int libCopy(FILE ** fromHandle, FILE ** toHandle, int amount_of_records, int size_of_records)
{
char buffer[size_of_records];
int i;
for(i=0;i<amount_of_records;i++)
{
	fread(buffer, 1, size_of_records, *fromHandle);
	fwrite(buffer, 1, size_of_records, *toHandle);
}
fclose(*fromHandle);
fclose(*toHandle);
return 0;
}


void reset_time()
{
	prev = run_time;
	times(run_time);

	double totalUserTime = (double) run_time -> tms_utime;
	double totalSystemTime = (double) run_time -> tms_stime;

	double totalUserTime_ms = (totalUserTime / CLOCKS_PER_SEC) * 10000.0d;
	double totalSystemTime_ms = (totalSystemTime / CLOCKS_PER_SEC) * 10000.0d;

	prevUserTime = totalUserTime_ms;
	prevSystemTime = totalSystemTime_ms;
}
void print_time()
{
	prev = run_time;
	times(run_time);

	double totalUserTime = (double) run_time -> tms_utime;
	double totalSystemTime = (double) run_time -> tms_stime;

	double totalUserTime_ms = (totalUserTime / CLOCKS_PER_SEC) * 10000.0d;
	double totalSystemTime_ms = (totalSystemTime / CLOCKS_PER_SEC) * 10000.0d;

	printf("\tUser time:   \t(+%4.4lf) s\n", totalUserTime_ms - prevUserTime);
    	printf("\tSystem time: \t(+%4.4lf) s\n\n", totalSystemTime_ms - prevSystemTime);

	prevUserTime = totalUserTime_ms;
	prevSystemTime = totalSystemTime_ms;

}

