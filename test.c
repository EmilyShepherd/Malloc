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
	init(100);
  
	int *pointer = malloc_test(array, 1, 101);
	int *pointer2 = malloc_test(array, 1, 102);

  printf("Is the array initialised (and empty) now? %d\n", mydispose(array));
  
  myfree(array, pointer);
  myfree(array, pointer2);
  
  show_array();
  
  printf("Is the array empty now? %d\n", mydispose(array));
  
  /*
	show_array();

	printf("\nFREEING MEMORY\n");
	printf("Free first pointer: %d\n\n", free_test(array, pointer, 1));

	show_array();

	printf("\nASKING FOR NEW SPACE\n");
	int *pointer3 = malloc_test(array, 1, 103);
	int *pointer4 = malloc_test(array, 2, 104);
	int *pointer5 = malloc_test(array, 2, 105);

	free_test(array, pointer2, 1);
	free_test(array, pointer4, 2);

	int *pointer6 = malloc_test(array, 3, 106);
	
	show_array();
	
	*/
	
	//Use the pointers for nothing, so the compiler doesn't complain
	//it's unused
	if (*pointer + *pointer2) {}
  
	return 0;
}
