#include <stdio.h>
#include <stdlib.h>

int myinit(int *, int);
int* mymalloc(int *, int);
int mydispose(int *);
int myfree(int *, int *);
void gui_show_array(int *);

int *array;

int* malloc_test(int *array, int size, int id)
{
    int *newArray = mymalloc(array, size);
    
    if (newArray == (int *)0)
    {
        printf("mymalloc couldn't give array of size %d\n", size);
        return (int *)0;
    }
    
    for (int i = 0; i < size; i++)
    {
        newArray[i] = id;
    }
    
    return newArray;
}

int free_test(int *array, int *block, int size)
{
    for (int i = 0; i < size; i++)
    {
        block[i] = 0;
    }
    
    return myfree(array, block);
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
  
	int *p1 = malloc_test(array, 5, 101);
	int *p2 = malloc_test(array, 8, 108);
	int *p3 = malloc_test(array, 3, 103);
	int *p4 = malloc_test(array, 10, 110);
	
	myfree(array, p2);
	myfree(array, p3);
	
	p2 = malloc_test(array, 5, 105);
	p3 = malloc_test(array, 18, 118);
	
	//I expect this to fill the space of size 6. Because using
	//this space leaves one int left over, which can't be used
	//for a header and data. mymalloc() should assign 6  bytes
	//to us anyway, to avoid that byte being lost.
	//The down side of this is that this byte won't be recovered
	//until this block is freed, even if the next block is freed.
	//malloc_test(array, 5, 666);
	
	//I expect this to fail
	malloc_test(array, 1, 777);
	
	gui_show_array(array);
	
	if (*p1 + *p2 + *p3 + *p4) {}
  
	return 0;
}
