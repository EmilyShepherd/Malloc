#include "mymalloc.h"
#include <stdio.h>

typedef unsigned char ubyte;

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
    ubyte* byte    = (ubyte *)header;
    unsigned usize = 0;
    int free       = 0;
    int bytes      = 0;
    
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
            byte = (ubyte *)header;
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
    ubyte* barray = (ubyte *)iarray;
    unsigned int usize    = (unsigned int)size;
    int pos               = 0;
    
    //Split the number into 5 bytes, each containing 7 bits of the
    //value.
    ubyte byte[5];
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

int header_size(int headerval)
{
    int i;
    unsigned int uval = (unsigned int)headerval;
    ubyte byte;
    ubyte mask = 0x0F;
     
    for (i = sizeof(int); i >= 0; i--)
    {
        byte = (uval >> (7 * i)) & mask;
        mask = 0x7F;
        
        if (byte != 0)
        {
            break;
        }
    }
    
    if ((byte & 0x40) == 0x40)
    {
        return i + 2;
    }
    else
    {
        return i + 1;
    }
}

void create_free_block(ubyte *barray, ubyte* endspace, ubyte* end)
{
    ubyte *nextheader  = endspace + 1;
    int blocksizebytes = endspace - barray + 1;
    
    //Pass next header?
    //Check if next block(s) is free
    if (nextheader != (ubyte *)0)
    {
        while (nextheader < end)
        {
            int nextheadersize;
            int nextblocksize;
            
            if (decode_header((int *)nextheader, &nextblocksize, &nextheadersize))
            {
                nextheader     += 4 * nextblocksize + nextheadersize;
                blocksizebytes += 4 * nextblocksize + nextheadersize;
            }
            else
            {
                break;
            }
        }
    }
    
    int blocksize      = blocksizebytes / 4;
    int extrabytes     = blocksizebytes - blocksize * 4;

    int headersize = header_size(blocksize);

    if (extrabytes==0 && headersize==sizeof(int)+1)
    {
        blocksize-= 2;
        extrabytes= 2*sizeof(int);
    }
    else if (extrabytes < headersize)
    {
        blocksize--;
        extrabytes+= sizeof(int);
    }

    if (blocksize != 0)
    {
        encode_header((int *)barray, 1, blocksize);

        extrabytes -= headersize;
    }
    else
    {
        headersize = 0;
    }

    ubyte* endofblock = barray + headersize + (4*blocksize);

    if (extrabytes == 1)
    {
        endofblock[0] = 64;
    }
    else if (extrabytes != 0)
    {
        endofblock[0]= 0xC0;
        for (int i=1; i<extrabytes - 1; i++)
        {
            endofblock[i]=0x80;
        }
        endofblock[extrabytes - 1]=0;
    }
}

int myinit(int *array, int size)
{
    //Reject the memory space if it's too small
    //14 is a random number we picked - it could be smaller
    if (size < 14) return 0;

    ubyte* barray = (ubyte *)array;
    
    //Save the total size of the memory
    *array = size;
    
    create_free_block(barray + 4, barray + array[0] * 4 - 1, barray + array[0] * 4);

    return 1;
}

int* mymalloc(int *array, int size)
{   
    if (size <= 0) return (int *)0;

    ubyte* barray = (ubyte *)array;
    
    //The end address of the memory space
    ubyte* end    = barray + array[0] * 4;
    
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
                *barray &= 0xBF; //1011 1111

                return (int *)(barray + headersize);
            } 
            else if (blocksize > size)
            {
		ubyte *nextheader = barray + headersize + 4 * blocksize;

                barray += encode_header((int *)barray, 0, size);
                
                create_free_block(barray + size * 4, nextheader - 1, end);

                return (int *)barray;
            }
        }

        barray += headersize + 4 * blocksize;

        if (barray >= end)
        {
            return (int*)0;
        }
    }
}

int myfree(int *array, int *block)
{
    ubyte *curheader  = (unsigned char *)array + 4;
    ubyte *lastheader = (unsigned char *)0;
    //int    firstsize  = 0;
    ubyte *bblock     = (unsigned char *)block;
    
    //The end address of the memory space
    ubyte* end        = curheader + array[0] * 4 - 4;
    
    //The passed pointer is outside the valid bounds of an int
    //pointer so we can immediately fail
    if (bblock <= curheader || bblock >= end) return 0;

    while (1)
    {
        int blocksize;
        int headersize;
        int free = decode_header((int *)curheader, &blocksize, &headersize);
        
        if (curheader + headersize == bblock)
        {
            //This block exists, but it is not being used
            //ignore this free request
            if (free) return 0;
            
            ubyte *nextheader = curheader + headersize + 4 * blocksize;
            if (nextheader >= end)
            {
                nextheader = (ubyte *)0;
            }

            //We have a free block(s) above us, we will define one big
            //free block encompassing all of it
            if (0 && lastheader != (ubyte *)0)
            {
                //create_free_block(lastheader, firstsize, (ubyte *)0);
            }
            else
            {
                create_free_block(curheader, nextheader - 1, end);
            }

            return 1;
        }

        //If this is the start of a free block of headers, save it
        if (free && lastheader == (ubyte *)0)
        {
            lastheader = curheader;
            //firstsize  = blocksize;
        }
        else
        {
            lastheader = (ubyte *)0;
        }
        
        curheader += headersize + 4 * blocksize;
        
        //We've missed the block, fail
        //Equals is a valid comparrison here, because if bblock were
        //equal, that would mean it is pointing at a header, not the
        //start of the block so it's invalid
        if (curheader >= bblock) return 0;
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
