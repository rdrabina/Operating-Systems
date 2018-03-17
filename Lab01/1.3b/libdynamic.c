#include "libdynamic.h"
#include "libusefulfunctions.h"


dynamic_tab * make_tab_d(int size_of_array, int size_of_block)
{	dynamic_tab *tab = (dynamic_tab*) calloc(1, sizeof(dynamic_tab));
	if(tab == NULL) return NULL;
	
	tab->size_of_array=size_of_array;
	tab->size_of_block=size_of_block;
	tab->tab=(array_of_blocks) calloc (size_of_array, sizeof(block));

return tab;
}

int free_tab_d(dynamic_tab ** t)
{
	if(*t==NULL) return -1;
	if((*t)->tab==NULL) return -2;
	
	int i;
	for(i=0;i<(*t) -> size_of_array;i++)
	{
		if((*t)->tab[i]!=NULL) free ((*t)->tab[i]);
	}
	free((*t)->tab);
	*t=NULL;
	
return 0;
}

void add_block_d(dynamic_tab *t, int i)
{
	if(t==NULL) return ;
	if(t->tab==NULL) return ;
	if(t->size_of_array<i-1) return ;
	if(t->tab[i] !=NULL) return ;
	
	t->tab[i]=(block) calloc (t->size_of_block, sizeof(bloc));
	
	data_generator(t->tab[i],t->size_of_block);

return;
}

void delete_block_d(dynamic_tab *t, int i)
{
	if(t==NULL) return ;
	if(t->tab==NULL) return ;
	if(t->size_of_array<i-1) return ;
	if(t->tab[i] !=NULL) return;

	free(t->tab[i]);
	t->tab[i]=NULL;

return;
}

int the_closest_element_d(dynamic_tab *t, int i)
{
	if(t==NULL) return -1;
	if(t->tab==NULL) return -2;
	if(t->size_of_array<i-1) return -3;
	if(t->tab[i] !=NULL) return -4;

	int j;
	long sum=0, sum_in_block=0;
	for(j=0;j<t->size_of_block;j++)
	{
		sum_in_block+=(long) t->tab[i][j];
	}

	long min_difference=9223372036854775807, difference;
	int min_index=-1;
	int k;
	for(k=0;k<t->size_of_array;k++)
	{
		if(t->tab[i]==NULL) continue;
		if(k==i) continue;

		for(j=0;j<t->size_of_block;j++)
		{
			sum+= (long) t->tab[k][j];
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
	
	
