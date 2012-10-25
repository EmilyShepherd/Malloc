#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *array = malloc(500 * sizeof(int));
	char *arr  = (char *)array;

	printf("Myinit: %d \n", myinit(array, 500));

	printf("Is the array initialised? %d\n", mydispose(array));

	//Test with another random array to make sure we don't have false
	//positives
	int *otherArray = malloc(10 * sizeof(int));
	printf("Is the other array wrong? %d\n", mydispose(otherArray));

	int *pointer = mymalloc(array, 1);
	*pointer = 5;
	printf("My pointer points to: %p \n", pointer);
	printf("My pointer points to: %d \n", *pointer);
	//printf("My pointer points to: %d \n", mymalloc(array, 1));
	printf("My array points to: %p \n", array);

}