#include "libstatic.h"
#include "libusefulfunctions.h"

char static_tab[AMOUNT_OF_BLOCKS][SIZE_OF_BLOCK];

char occupied[AMOUNT_OF_BLOCKS]={0};

void zero()
{
	zero_out(static_tab[AMOUNT_OF_BLOCKS],AMOUNT_OF_BLOCKS);
	zero_out(occupied,AMOUNT_OF_BLOCKS);
return;
}



int make_tab(){
	return 0;
}

int add_block(int i)
{
	if(occupied[i]!=0) return -1;
	if(i>AMOUNT_OF_BLOCKS-1) return -2;

	data_generator(static_tab[AMOUNT_OF_BLOCKS], SIZE_OF_BLOCK);
	occupied[i]=1;
return 0;
}

int delete_block(int i)
{
	if(occupied[i]==0) return -1;
	if(i>AMOUNT_OF_BLOCKS-1) return -2;

	zero_out(static_tab[i],AMOUNT_OF_BLOCKS);
	occupied[i]=0;
return 0;
}

int the_closest_element(int i)
{
	if(occupied[0]==0) return -1;
	if(i>SIZE_OF_BLOCK-1) return -2;

	int j;
	long sum=0, sum_in_block=0;
	for(j=0;j<SIZE_OF_BLOCK;j++)
	{
		sum+=(long) static_tab[i][j];
	}

	long min_difference=9223372036854775807, difference;
	int min_index=-1;
	int k;
	
	for(k=0;k<AMOUNT_OF_BLOCKS;k++)
	{
		if(occupied[k]==0) continue;
		if(k==i) continue;
		
		for(j=0;j<SIZE_OF_BLOCK;j++)
		{
			sum_in_block+= (long) static_tab[k][j];
		}
		
		difference=abs(sum-sum_in_block);

		if(difference<min_difference)
		{
			min_difference=difference;
			min_index=k;
		}
	}
return min_index;
}

	





