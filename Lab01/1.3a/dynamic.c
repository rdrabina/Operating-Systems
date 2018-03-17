//a-> make_tab()
//b->the_closest_element()
//c-> x* delete_block(), next x* add_block
//d-> x*( delete_block(), add_block() )

#include <stdlib.h>
#include <dlfcn.h>
#include <sys/times.h>
#include <time.h>
#include <string.h>

#include "libdynamic.h"
#include "libusefulfunctions.h"

void find_element_d (int n_times,dynamic_tab *tab,struct tms *time1, struct tms *time2);
void project_time_d(struct tms *time1, struct tms *time2);
void add_delete_d(int n_times,dynamic_tab *tab,struct tms *time1, struct tms *time2,void *handle);
void cross_add_delete_d(int n_times,dynamic_tab *tab,struct tms *time1, struct tms *time2,void *handle);

int size_of_array_d;
int size_of_block_d;
int n_times=640; //for testing, in the other case n_times=1


int main(int argc, char ** argv)
{
	
	if(!(strcmp(argv[3], "dynamic")==0)) return 0;
	if(argc<4) return 0;	

	void *handle = dlopen("./library.so", RTLD_LAZY);
	if(!handle) return 0;
	
	struct tms *time1 = calloc(1,sizeof(struct tms));
    	struct tms *time2 = calloc(1,sizeof(struct tms));

	size_of_array_d=atoi(argv[1]);
	size_of_block_d=atoi(argv[2]);

	dynamic_tab *tab;
	if(strcmp(argv[4], "a")==0)
	{
		dynamic_tab*(*make_tab_d)(int size_of_array_d, int size_of_block_d) = dlsym(handle, "make_tab_d");

		times(time1);
		tab=(*make_tab_d)(size_of_array_d,size_of_block_d);
		times(time2);
		project_time_d(time1,time2);
	}
	else return 0;

	int i;
	for(i=5;i<argc;i++)
	{
		if(strcmp(argv[i], "b")==0)
		{
			find_element_d(n_times,tab,time1,time2);
		}
		
		else if(strcmp(argv[i], "c")==0)
		{
			add_delete_d(n_times,tab,time1,time2,handle);
		}

		else if(strcmp(argv[i], "d")==0)
		{
			cross_add_delete_d(n_times,tab,time1,time2,handle);
		}

		else return 0;		
	}
dlclose(handle);
return 0;
}

void find_element_d (int n_times,dynamic_tab *tab,struct tms *time1, struct tms *time2)
{
	int i;
	times(time1);
	for(i=0;i<n_times;i++)
	{
		the_closest_element_d(tab,rand()%(128*32));	//128 ASCII codes * 32 max size_of_block
	}
	times(time2);
	project_time_d(time1,time2);
return;
}

void project_time_d(struct tms *time1, struct tms *time2)
{
	printf("\nTotal time: \t(+%4.2lf) s\n", (double)(time2->tms_utime + time2->tms_stime-time1->tms_utime + time1->tms_stime));
	printf("\nUser time: \t(+%4.2lf) s\n", (double)(time2->tms_utime-time1->tms_utime));
	printf("\nSystem time: \t(+%4.2lf) s\n\n\n", (double)(time2->tms_stime-time1->tms_stime));
return;
}

void add_delete_d(int n_times,dynamic_tab *tab,struct tms *time1, struct tms *time2,void *handle)
{
	void (*delete_block_d)(dynamic_tab *,int) = dlsym(handle, "delete_block_d");
    	void (*add_block_d)(dynamic_tab *,int) = dlsym(handle, "add_block_d");

	int i;
	times(time1);
	for(i=0;i<n_times;i++)
	{
		add_block_d(tab,rand()%(size_of_array_d)+1);
	}
	for(i=0;i<n_times;i++)
	{
		delete_block_d(tab,rand()%(size_of_array_d)+1);
	}
	times(time2);
	project_time_d(time1,time2);
return;
}
void cross_add_delete_d(int n_times,dynamic_tab *tab,struct tms *time1, struct tms *time2,void *handle)
{
	void (*delete_block_d)(dynamic_tab *,int) = dlsym(handle, "delete_block_d");
    	void (*add_block_d)(dynamic_tab *,int) = dlsym(handle, "add_block_d");	

	
	int i;
	times(time1);
	for(i=0;i<n_times;i++)
	{
		add_block_d(tab,rand()%(AMOUNT_OF_BLOCKS)+1);
		delete_block_d(tab,rand()%(AMOUNT_OF_BLOCKS)+1);
	}
	times(time2);
	project_time_d(time1,time2);
return;
}
