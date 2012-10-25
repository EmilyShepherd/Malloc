#include <stdio.h>
#include <stdlib.h>

void show_array(int *);
int myinit(int *, int);
int* mymalloc(int *, int);
int mydispose(int *);
int myfree(int *, int *);

int main()
{
	int *array = malloc(500 * sizeof(int));

	printf("Myinit: %d \n", myinit(array, 500));

	printf("Is the array initialised? %d\n", mydispose(array));

	//Test with another random array to make sure we don't have false
	//positives
	int *otherArray = malloc(10 * sizeof(int));
	printf("Is the other array wrong? %d\n", mydispose(otherArray));

	int *pointer = mymalloc(array, 1);
	*pointer = 5;
	int *pointer2 = mymalloc(array, 1);
	*pointer2 = 10;

	show_array(array);

	printf("\nFREEING MEMORY\n");
	printf("Free first pointer: %d\n\n", myfree(array, pointer));

	show_array(array);

	printf("\nASKING FOR NEW SPACE\n");
	int *pointer3 = mymalloc(array, 1);
	*pointer3 = 28;

	int *pointer4 = mymalloc(array, 2);
	*pointer4 = 28;
	int *pointer5 = mymalloc(array, 2);
	*pointer5 = 69;

	myfree(array, pointer2);
	myfree(array, pointer4);

	int *pointer6 = mymalloc(array, 3);
	*pointer6 = 101;
	
	show_array(array);

	return 0;
}

void show_array(int *array)
{
	for (int i = 0; i < 100; i++)
	{
		printf("%d: %d\n", i, array[i]);
	}
}