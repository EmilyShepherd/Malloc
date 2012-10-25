#include <stdio.h>
#include <stdlib.h>

int myinit(int *, int);
int* mymalloc(int *, int);
int mydispose(int *);
int myfree(int *, int *);

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
  
	malloc_test(array, 50, 101);
	malloc_test(array, 49, 101);
	malloc_test(array, 48, 101);
	malloc_test(array, 47, 101);
	
	show_array();
  
	return 0;
}
