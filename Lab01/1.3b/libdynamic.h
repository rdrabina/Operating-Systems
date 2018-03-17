#ifndef DYNAMIC
#define DYNAMIC

#include <stdlib.h>

#ifndef AMOUNT_OF_BLOCKS
#define AMOUNT_OF_BLOCKS 32
#endif

#ifndef SIZE_OF_BLOCK
#define SIZE_OF_BLOCK 1024
#endif

typedef char bloc;
typedef char *block;
typedef char **array_of_blocks;

typedef struct dynamic_tab{
array_of_blocks tab;
int size_of_array;
int size_of_block;
} 
dynamic_tab;

dynamic_tab * make_tab_d(int size_of_array, int size_of_block);
int free_tab_d(dynamic_tab ** t);
void add_block_d(dynamic_tab *t, int i);
void delete_block_d(dynamic_tab *t, int i);
int the_closest_element_d(dynamic_tab *t, int i);

#endif
