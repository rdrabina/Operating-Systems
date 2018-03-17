#include "libusefulfunctions.h"

void zero_out(char *t, int block_size)
{
	int i;
	for(i=0; i<block_size; i++)
	{
		t[i]=0;
	}
}


void data_generator(char *t,int block_size)
{
	time_t tt;
	int i,x;
	x=time(&tt);
	srand(x);
	for (i = 0; i < block_size; ++i) {
		t[i] = (int) rand() % 128; //127 ASCII codes
	}
}

int abs(int x)
{ 
	if(x<0) return -x;
	else return x;
}
