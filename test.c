#include <stdio.h>
#include <stdlib.h>

int myinit(int *, int);
int* mymalloc(int *, int);
int mydispose(int *);
int myfree(int *, int *);
int decode_header(int *);
int encode_header(int *, int, int);
void gui_show_array(int *);

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
        newArray[i] = 999;
    }
    
    return newArray;
}

void free_test(int *block)
{
    int size;
    block--;
    
    if (*block < 0)
    {
        size = -1 * *block;
    }
    else
    {
        size = *block;
    }
    
    block++;
    
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
	for (int i = 0; i < 100; i++)
	{
		printf("%d: %d\n", i, array[i]);
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
	init(50);
	
	//printf("%d\n", encode_header(array, 0, 56));
	printf("%d\n", encode_header(array + 1, 0, 564640));
	//printf("%d\n", encode_header(array + 2, 1, 56));
	//printf("%d\n\n", encode_header(array + 3, 1, 64));
	
	//printf("%d\n", decode_header(array));
	printf("%d\n", decode_header(array + 1));
	//printf("%d\n", decode_header(array + 2));
	//printf("%d\n", decode_header(array + 3));
  
	int *p1 = malloc_test(5);
	int *p2 = malloc_test(8);
	int *p3 = malloc_test(3);
	int *p4 = malloc_test(10);
	
	p1[0] = 44;
	p2[0] = 33;
	p3[0] = 22;
	p4[0] = 11;
	
	free_test(p2);
	free_test(p3);
	
	p2 = malloc_test(5);
	p3 = malloc_test(17);
	
	//I expect this to fill the space of size 6. Because using
	//this space leaves one int left over, which can't be used
	//for a header and data. mymalloc() should assign 6  bytes
	//to us anyway, to avoid that byte being lost.
	//The down side of this is that this byte won't be recovered
	//until this block is freed, even if the next block is freed.
	//malloc_test(5);
	
	//I expect this to fail
	//malloc_test(1);
	
	//gui_show_array(array);
	
	if (*p1 + *p2 + *p3 + *p4) {}
  
	return 0;
}
