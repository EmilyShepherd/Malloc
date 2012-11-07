#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "mymalloc.c"
#include "GUIview.c"

int header_size(int);

int *array;

int* malloc_test(int size)
{
    int *newArray = mymalloc(array, size);
    
    if (newArray == (int *)0)
    {
        printf("mymalloc couldn't give array of size %d\n", size);
        return (int *)0;
    }
    
    for (int i = 0; i < size; i++)
    {
        newArray[i] = 0x63636363;
    }
    
    return newArray;
}

void free_test(int *block, int size)
{    
    for (int i = 0; i < size; i++)
    {
        block[i] = 0;
    }
    
    if (!myfree(array, block))
    {
        printf("!!! Couldn't free memory !!!");
    }
}

void show_array()
{
    unsigned char *barray = (unsigned char *)array;
    int fours = 0;
    int ints  = 0;
    
    printf("\n\nArray Dump:\n\n");
    
    for (int i = 0; i < 60; i++)
    {
        printf("%p: %d\n", &barray[i], barray[i]);
        
        fours++;
        if (fours == 4)
        {
            ints++;
            fours = 0;
            printf("%d ------------\n", ints);
        }
    }
}

void init(int size)
{
    array = malloc(size * sizeof(int));
    
    if (!myinit(array, size))
    {
        printf("!!! myinit() failed !!!\n");
    }
    
    if (!mydispose(array))
    {
        printf("!!! mydispose() failed after myinit() !!!\n");
    }
    
    //Test with another random array to make sure we don't have false
    //positives
    int *otherArray = malloc(sizeof(int));
    
    if (mydispose(otherArray))
    {
        printf("!!! mydispose() returing false positive !!!\n");
    }
    
    free(otherArray);
}

int main()
{
	init(15);
	
	printf("Header size 4 63: %d\n", header_size(63));
	printf("Header size 4 64: %d\n", header_size(64));

	int *p1 = malloc_test(1);
	int *p2 = malloc_test(2);
	
	show_array();
	
	free_test(p1, 1);

	show_array();
  
  if (*p1 + *p2) {}
  
	return 0;
}
