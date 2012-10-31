#include "mymalloc.h"

/*
 * @depreciated
 */
int inc_header(int *array, int *curheader)
{
    if (array[*curheader] < 0)
    {
        *curheader += -1*array[*curheader] + 1;
    }
    else
    {
        *curheader += array[*curheader] + 1;
    }

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
 * Reads a header, returning whether it is free or not, and setting the
 * passed blocksize to be the size of the header's block.
 *
 * NB blocksize is the number of ints of the block's data
 *    headersize is the number of bytes of the block's header.
 */
int decode_header(int *header, int *blocksize, int *headersize)
{
    unsigned char *byte = (unsigned char *)header;
    unsigned usize      = 0;
    int free            = 0;
    int bytes           = 0;
    
    //The 7th bit has in the first byte is the free flag, so check that.
    //If it's 1, set it to 0 so it doesn't get used as part of the value
    //by the code below.
    if ((*byte & 0x40) == 0x40)
    {
        free   = 1;
        *byte &= 0xBF; //1011 1111
    }
    
    while (1)
    {
        //Shift the value up 7 bits and put the next 7 bits in after it.
        usize = (usize << 7) + (*byte & 0x7F); //0111 1111
        
        bytes++;
        
        //If the 8th bit is 1, there is another byte to follow
        if (*byte >> 7 == 1)
        {
            byte++;
        }
        else
        {
            *blocksize  = (signed int)usize;
            *headersize = bytes;

            // Reset bit to be free
	     byte = (unsigned char *)header;
            if (free)
            {
                *byte |= 0x40;
            }

            return free;
        }
    }
}

/*
 * Encodes the given free flag and size into the the given array,
 * returning how many bytes it used.
 */
int encode_header(int *iarray, int free, int size)
{
    unsigned char *barray = (unsigned char *)iarray;
    unsigned int usize    = (unsigned int)size;
    int pos               = 0;
    
    //Split the number into 5 bytes, each containing 7 bits of the
    //value.
    unsigned char byte[5];
    byte[0] = usize >> 28 & 0x0F;
    byte[1] = usize >> 21 & 0x7F;
    byte[2] = usize >> 14 & 0x7F;
    byte[3] = usize >> 7  & 0x7F;
    byte[4] = usize       & 0x7F;
    
    //Loop through the bytes, looking for the first one that's used - so
    //we need not save the higher unused bytes.
    for (pos = 0; byte[pos] == 0; pos++);
    
    //Check if this byte uses its 7th bit. If it does we'll need to
    //create a new byte, because we need the 7th bit of the first byte
    //for the sign.
    if ((byte[pos] & 0x40) == 0x40)
    {
        pos--;
    }
    
    //If we want to mark this header's block as free, save 1 into the
    //7th bit of the first byte
    if (free)
    {
        byte[pos] = byte[pos] | 0x40; //0100 0000
    }
    
    //Save all except the last byte, with the MSB bit set to 1 to show
    //there are more bytes to follow
    for (int i = pos; i < 4; i++)
    {
        *barray = byte[i] | 0x80; //1000 0000
        barray++;
    }
    
    //Save the last byte
    //The 8th bit must be 0, but we know it is, because of the 0x7F mask
    //we used above.
    *barray = byte[4];
    
    return 5 - pos;
}

unsigned char* create_free_block(unsigned char *barray, int blocksize)
{
    int headersize = encode_header((int *)barray, 1, blocksize - 1);
    unsigned char* sarray = barray;

    if (headersize == 3)
    {
        barray   += 4*blocksize - 1;
        barray[0] = 64;
    }
    else if (headersize == 2)
    {
        barray   += 4*blocksize - 2;
        barray[0] = 0xC0;
        barray[1] = 0;
    }
    else if (headersize == 1)
    {
        barray   += 4*blocksize - 3;
        barray[0] = 0xC0;
        barray[1] = 0;
        barray[2] = 0;
    }

    return sarray + headersize;
}

int myinit(int *array, int size)
{
    //Reject the memory space if it's too small
    //14 is a random number we picked - it could be smaller
    if (size < 14) return 0;

    unsigned char* barray = (unsigned char *)array;
    
    //Save the total size of the memory
    *array = size;
    
    create_free_block(barray + 4, size - 1);

    return 1;
}

int* mymalloc(int *array, int size)
{   
    if (size <= 0) return (int *)0;

    unsigned char* barray = (unsigned char *)array;
    
    //The end address of the memory space
    unsigned char* end    = barray + array[0] * 4;
    
    //Move into the start header of the memory space
    barray += 4;

    while (1)
    {
        int blocksize;
        int headersize;
    
        //Positive header int, implies free space of this size
        if (decode_header((int *)barray, &blocksize, &headersize))
        {
            if (blocksize == size)
            {
                *barray |= 0x40;

                return (int *)(barray + headersize);
            } 
            else if (blocksize > size)
            {
                barray += encode_header((int *)barray, 0, size);
                
                create_free_block(barray + size * 4, blocksize - size);

                return (int *)barray;
            }
        }

        barray += 4 * blocksize;

        if (barray >= end)
        {
            return (int*)0;
        }
    }
}

int myfree(int *array, int *block)
{
    int curheader  = 1;
    int lastheader = 0;

    while (1)
    {
        if (&array[curheader + 1] == block)
        {
            //This block exists, but it is not being used
            //ignore this free request
            if (array[curheader] > 0) return 0;

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
        //if (!inc_header(array, &curheader)) return 0;
    }
}

/*
 * Returns one if the first header (array[1]) equals the total memory
 * space (array[0]) minus two
 */
int mydispose(int *array)
{
    int blocksize;
    int headersize;
    if (decode_header(array + 1, &blocksize, &headersize))
    {
        if (blocksize == array[0] - 2)
        {
            return 1;
        }
    }

    return 0;
}
