#include "mymalloc.h"
#include <pthread.h>
/*
 * Alexander Shepherd ams2g11
 * Alex Hutson ah16g11
 *
 * Our solution uses a single integer as a header for each block of
 * allocated data. The size of each block is the absolute value of the
 * integer. Positive integer headers indicate that the block is free,
 * while negative integer headers indicate used blocks. The first
 * integer in the array is used to store the total size of the array,
 * and the second space is used as the first header. Initially this
 * header represents a block which takes up the remaining space of the
 * array (total size - 2). When space is allocated, the free block is
 * split into the allocated block of required size and a new free block
 * bellow it, with the exception of the case when the free block is the
 * required size + 1, in this case, the extra integer is allocated to
 * the block as well, because it can't be used for anything. We've
 * improved this by using best fit, instead of first fit. When freeing a
 * block, it will be combined with other free blocks either side of it,
 * if such blocks exist. My free has to iterate across the entire array
 * to confirm that a given pointer is the start of a valid block.
 */

/*
 * This mutex is used for the whole solution when multithreaded.
 */
static pthread_mutex_t mutex;

/*
 * Increments the given header index to point to the next header
 *
 * Returns 1 on success or 0 on failure (when the end of the array has
 * been reached)
 */
static int inc_header(int *array, int *curheader)
{
    //Jump to the next header
    if (array[*curheader] < 0)
    {
        *curheader += -1*array[*curheader] + 1;
    }
    else
    {
        *curheader += array[*curheader] + 1;
    }

    //If header is outside of the array, there are no more blocks
    if (*curheader >= array[0])
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*
 * Intialises the array by saving the total size in the first space, and
 * creating the first header which is for a free block taking up the
 * whole of the remaining space.
 *
 * Returns 1 on success or 0 on failure (if the given array is too small
 * to use)
 */
int myinit(int *array, int size)
{
    //Smallest size available chosen to be usable, otherwise return 0
    //1 int for total array size
    //1 int for block header
    //1 int for content
    if (size < 3) return 0;

    //Put size of array into first position
    array[0] = size;

    //Make the rest of the array into a free block
    array[1] = size - 2;

    return 1;
}

/*
 * Returns a pointer to a block within the array of the given size if
 * one is availiabe. (int *)0 otherwise.
 */
int* mymalloc(int *array, int size)
{
    //Validates given size is not garbage (<= 0) or too big to use
    if (size <= 0 || size >= array[0]) return (int *)0;
    
    int curheader   = 1;
    int bestheader  = 0;
    int bestsize    = array[0];
    int wasteheader = 0;

    while (1)
    {
        // positive header int, implies free space of this size
        if (array[curheader] >= size)
        {
            //Perfect fit, use it.
            if (size == array[curheader])
            {
                array[curheader] *= -1;
                return &array[curheader+1];
            }
            //If this space is used, an integer will be wasted at the
            //end. We would rather use something else, but save it
            //way incase no other space exists
            else if (size + 1 == array[curheader])
            {
                wasteheader = curheader;
            }
            //Space is bigger than needed, save it if it's the smallest
            //too big block
            else if (array[curheader] < bestsize)
            {
                bestheader = curheader;
                bestsize   = array[curheader];
            }
        }
        
        if (!inc_header(array, &curheader)) break;
    } 
    
    //We have a space saved, use it and break it into a free block after
    //the allocated size
    if (bestheader != 0)
    {
        //Allocate space, then put header after data for new section of
        //free data
        array[bestheader + size + 1] = array[bestheader] - size - 1;
        array[bestheader] = -1*size;

        return &array[bestheader+1];
    }
    //If only space avaialable will waste an int
    else if (wasteheader != 0)
    {
        array[wasteheader] *= -1;

        return &array[wasteheader+1];
    }

    return (int *)0;
}

/*
 * Frees the given block
 *
 * Returns 1 on success or 0 on failure (when the given pointer is not
 * the start of a used block within the array)
 */
int myfree(int *array, int *block)
{
    //If the block pointer is outside the bounds of the array, its
    //obviously wrong
    if (block <= array || block >= array + array[0]) return 0;
    
    int curheader  = 1;
    int lastheader = 0;

    while (1)
    {
        if (&array[curheader + 1] == block)
        {
            //This block exists, but it is not being used
            //ignore this free request
            if (array[curheader] > 0)
            {
                return 0;
            }

            //Mark it as free
            array[curheader] *= -1;

            int nextheader = curheader + 1 + array[curheader];

            //Next block is free - combine
            if (nextheader < array[0] && array[nextheader] > 0)
            {
                array[curheader] += array[nextheader] + 1;
            }

            //Last block is free - combine
            if (lastheader != 0 && array[lastheader] > 0)
            {
                array[lastheader] += array[curheader] + 1;
            }

            return 1;
        }

        //Save this header and move unto the next one
        lastheader = curheader;
        if (!inc_header(array, &curheader) || &array[curheader] > block)
        {
            return 0;
        }
    }
}

int mydispose(int *array)
{
    //Return one if free space pointer correctly pointers to the rest of
    //the array
    if (array[1] == array[0] - 2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int myinit_mt(int *array, int size)
{
    // Lock around myinit
    pthread_mutex_lock(&mutex);
    int result = myinit(array, size);
    pthread_mutex_unlock(&mutex);

    return result;
}

int* mymalloc_mt(int *array, int size)
{
    // Lock around mymalloc
    pthread_mutex_lock(&mutex);
    int *result = mymalloc(array, size);
    pthread_mutex_unlock(&mutex);

    return result;
}

int myfree_mt(int *array, int *block)
{
    // Lock around myfree
    pthread_mutex_lock(&mutex);
    int result = myfree(array, block);
    pthread_mutex_unlock(&mutex);

    return result;
}

int mydispose_mt(int *array)
{
    // Lock around my dispose
    pthread_mutex_lock(&mutex);
    int result = mydispose(array);
    pthread_mutex_unlock(&mutex);

    return result;
}
