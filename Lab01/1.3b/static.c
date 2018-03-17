//a-> make_tab()
//b->the_closest_element()
//c-> x* delete_block(), next x* add_block
//d-> x*( delete_block(), add_block() )

#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include <stdio.h>

#include "libstatic.h"
#include "libusefulfunctions.h"

void find_element (int n_times,struct tms *time1, struct tms *time2);
void project_time(struct tms *time1, struct tms *time2);
void add_delete(int n_times,struct tms *time1, struct tms *time2);
void cross_add_delete(int n_times,struct tms *time1, struct tms *time2);

int size_of_array;
int size_of_block;
int n_times=16; //for testing, in the other case n_times=1


int main(int argc, char ** argv)
{	
	
	if(!(strcmp(argv[3], "static")==0)) return 0;
	struct tms *time1 = calloc(1,sizeof(struct tms));
    	struct tms *time2 = calloc(1,sizeof(struct tms));

	if(argc<4) return 0;
	size_of_array=atoi(argv[1]); //atoi -> maps char to int
	size_of_block=atoi(argv[2]);


	int i;
	for(i=4;i<argc;i++)
	{		
		if(strcmp(argv[i], "a")==0)
		{
			times(time1);
			make_tab();
			times(time2);
			project_time(time1,time2);
		}

		else if(strcmp(argv[i], "b")==0)
		{
			find_element(n_times,time1,time2);
		}
		
		else if(strcmp(argv[i], "c")==0)
		{
			add_delete(n_times,time1,time2);
		}

		else if(strcmp(argv[i], "d")==0)
		{
			cross_add_delete(n_times,time1,time2);
		}

		else return 0;		
	}
return 0;
}

void find_element (int n_times,struct tms *time1, struct tms *time2)
{
	int i;
	times(time1);
	for(i=0;i<n_times;i++)
	{
		the_closest_element(rand()%(128*32));	//128 ASCII codes * 32 max size_of_block
	}
	times(time2);
	project_time(time1,time2);
return;
}

void project_time(struct tms *time1, struct tms *time2)
{
	printf("Total time: \t(+%4.2lf) s\n", (double)(time2->tms_utime + time2->tms_stime-time1->tms_utime + time1->tms_stime));
	printf("\nUser time: \t(+%4.2lf) s\n", (double)(time2->tms_utime-time1->tms_utime));
	printf("\nSystem time: \t(+%4.2lf) s\n\n\n", (double)(time2->tms_stime-time1->tms_stime));

return;
}

void add_delete(int n_times,struct tms *time1, struct tms *time2)
{
	int i;
	times(time1);
	for(i=0;i<n_times;i++)
	{
		add_block(rand()%(size_of_array)+1);
	}
	for(i=0;i<n_times;i++)
	{
		delete_block(rand()%(size_of_array)+1);
	}
	times(time2);
	project_time(time1,time2);
return;
}

void cross_add_delete(int n_times,struct tms *time1, struct tms *time2)
{
	int i;
	times(time1);
	for(i=0;i<n_times;i++)
	{
		add_block(rand()%(size_of_array)+1);
		delete_block(rand()%(size_of_array)+1);
	}
	times(time2);
	project_time(time1,time2);
return;
}
	
