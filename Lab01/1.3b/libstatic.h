#ifndef STATIC
#define STATIC

#include <stdlib.h>

#ifndef AMOUNT_OF_BLOCKS
#define AMOUNT_OF_BLOCKS 32
#endif

#ifndef SIZE_OF_BLOCK
#define SIZE_OF_BLOCK 1024
#endif

extern char staticTab[AMOUNT_OF_BLOCKS][SIZE_OF_BLOCK];
extern char occupied [AMOUNT_OF_BLOCKS];

void zero();
int make_tab();
int add_block(int i);
int delete_block(int i);
int the_closest_element(int i);

#endif

