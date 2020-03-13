/**********************************************************************
File Name: buffer.c
Compiler: MS Visual Studio 2019
Author: Raphael Guerra
Course: CST8152 - Compilers, Lab Section: 011
Assignment: 1
Due date: 30th Jan 2020
Professor: Svillen Ranev
Purpose: Create a character buffer that can operate in three different modes: a “fixedsize” buffer, 
         an “additive self-incrementing” buffer, and a “multiplicative self-incrementing” buffer.
         The buffer implementation is based on two associated data structures: a Buffer Descriptor 
         (or Buffer Handle) and an array of characters (the actual character buffer).
Function List: b_allocate(), b_addc(), b_clear(), b_free(), b_isfull(), b_limit(), b_capacity(), 
               b_markc(), b_mode(), b_incfactor(), b_load(), b_isempty(), b_getc(), b_eob(), b_print(), 
               b_compact(), b_rflag(), b_retract(), b_reset(), b_getcoffset(), b_rewind()
*********************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <limits.h>
#include "buffer.h"

/********************************************************************
Function name: b_allocate
Purpose: This function creates a new buffer in memory (on the program heap).
Function In parameters: initial capacity of buffer, increase factor of buffer, 
                        mode of operation (additive, multiplicative, fixed)
Function Out parameters: Buffer Structure constant pointer
Version: 1.0
Author: Raphael Guerra
Called functions: caloc(), free(), malloc() 
**********************************************************************/
Buffer* b_allocate(short init_capacity, char inc_factor, char o_mode) {
    /*Instantialize a Buffer Structure pointer*/
    Buffer* pBuffer;

    /*Checking parameters*/
    if (init_capacity < 0 || init_capacity > SHRT_MAX -1 || (unsigned char) inc_factor < 0) {
        return NULL;
    }

    /*Dinamically allocating memory to pointer*/
    pBuffer = (Buffer*)calloc(1, sizeof(Buffer));
    if (!pBuffer) {
        return NULL;
    }

    /*capacity bigger than 0 and lower than maximum allowed positive value (short)*/
    if (init_capacity >= 0 && init_capacity <= SHRT_MAX - 1) {
        if (init_capacity == 0) {
            init_capacity = DEFAULT_INIT_CAPACITY;
        }

        /*Dinamically allocating memory to char pointer*/
        pBuffer->cb_head = (char*)malloc(init_capacity);
        if (!pBuffer->cb_head) {
            return NULL;
        }
        /*swithing modes*/
        if (o_mode == 'a' || o_mode == 'm') {
            inc_factor = DEFAULT_INC_FACTOR;
        }
        else if (o_mode == 'f') {
            inc_factor = 0;
        }
        else {
            free(pBuffer->cb_head);
            free(pBuffer);
            return NULL;
        }
    }
    /*if any error return NULL*/
    else {
        free(pBuffer->cb_head);
        free(pBuffer);
        return NULL;
    }
    /*if fixed mode*/
    if (o_mode == 'f') {
        pBuffer->mode = 0;
        pBuffer->inc_factor = 0;
    }
    /*if case for increase factor equals to 0 an initial capacity not zero*/
    else if ((unsigned char) inc_factor == 0 && (unsigned char) init_capacity != 0) {
        pBuffer->mode = 0;
        pBuffer->inc_factor = 0;
    }
    /*if case for additive mode and increase factor between 1 and 255*/
    else if (o_mode == 'a' && (unsigned char)inc_factor >= 1 && (unsigned char)inc_factor <= 255) {
        pBuffer->mode = ADD_MODE;
        pBuffer->inc_factor = inc_factor;
    }
    /*if case for multiplication mode and increase factor between 1 and 100*/
    else if (o_mode == 'm' && inc_factor >= 1 && inc_factor <= 100) {
        pBuffer->mode = MULTI_MODE;
        pBuffer->inc_factor = inc_factor;
    }
    /*if any error free al memory*/
    else {
        free(pBuffer->cb_head);
        free(pBuffer);
        return NULL;
    }
    /*apply values to buffer structure*/
    pBuffer->capacity = init_capacity;
    pBuffer->flags = DEFAULT_FLAGS;

    return pBuffer;
}
/********************************************************************
Function name: b_addc
Purpose: Using a bitwise operation the function resets the flags field 
         r_flag bit to 0 and tries to add the character symbol to the 
         character array of the given buffer pointed by pBD. 
Function In parameters: Buffer Structure constant pointer, symbol character
Function Out parameters: Buffer Structure pointer
Version: 1.0
Author: Raphael Guerra
Called functions: sizeof(), realoc()
**********************************************************************/
pBuffer b_addc(pBuffer const pBD, char symbol) {
    /*Temporary buffer for operations with parameter buffer*/
    char* tempBuffer;
    /*variables for realocing buffer*/
    short avaiableSpace, newCapacity;
    double newIncrement;
    /*initializing variables*/
    avaiableSpace = 0;
    newCapacity = 0;
    newIncrement = 0;

    /*Checking parameters*/
    if (!pBD)
        return NULL;
    /*resetting r_flag*/
    pBD->flags &= RESET_R_FLAG;
    if (pBD->addc_offset == 318) {
        printf("salci fufu!");
    }
    /*if theres avaiable space in buffer add symbol, increase offset and return buffer pointer*/
    if ((short)(pBD->addc_offset * sizeof(char)) < pBD->capacity) {
        pBD->cb_head[pBD->addc_offset++] = symbol;
        return pBD;
    }
    /*if there is not enough avaiable space create a new buffer witrh bigger and size and replace*/
    else {
        /*If fixed mode*/
        if (pBD->mode == 0)
            return NULL;
        /*If additive mode*/
        else if (pBD->mode == 1) {
            if (pBD->capacity == (SHRT_MAX - 1))
                return NULL;
            /*increase local capacity variable for later assignment*/
            newCapacity = pBD->capacity + (unsigned char)pBD->inc_factor * sizeof(char);
            /*If new capacity is bigger than short Max value -1, assign short Max -1*/
            if (newCapacity > SHRT_MAX - 1)
                newCapacity = SHRT_MAX - 1;
        }
        /*If multiplication mode*/
        else if (pBD->mode == -1) {
            /*if capacity is shot Max - 1 limit return NULL*/
            if (pBD->capacity == (SHRT_MAX - 1))
                return NULL;
            /*increase local capacity variable for later assignment*/
            else {
                avaiableSpace = (SHRT_MAX - 1) - pBD->capacity;
                newIncrement = ((float) avaiableSpace * pBD->inc_factor / 100);
                newCapacity = (short)((pBD->capacity + newIncrement));

                if (newCapacity > (SHRT_MAX - 1)) {
                    newCapacity = SHRT_MAX - 1;
                }
            }
        }
        /*dinamic memory for temporary buffer with increased capacity*/
        tempBuffer = (char*)realloc(pBD->cb_head, newCapacity);
        if (tempBuffer == NULL) {
            return NULL;
        }
        /*if relocated set flag*/
        if (tempBuffer != pBD->cb_head) {
            pBD->flags |= SET_R_FLAG;
            pBD->cb_head = tempBuffer;
        }
        /*swapping local tempBuffer with increased capacity to PBD buffer*/
        pBD->cb_head[pBD->addc_offset] = symbol;
        pBD->addc_offset++;
        pBD->capacity = newCapacity;
    }
    return pBD;
}
/********************************************************************
Function name: b_clear
Purpose: The function retains the memory space currently allocated to the buffer, but re-initializes all
         appropriate data members of the given Buffer structure (buffer descriptor) so that the buffer will
         appear as just created to the client functions
Function In parameters: Buffer Structure constant pointer, symbol character
Function Out parameters: integer
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
int b_clear(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    /*reset structure variables*/
    pBD->capacity = 0;
    pBD->addc_offset = 0;
    pBD->getc_offset = 0;
    pBD->flags = DEFAULT_FLAGS;
    return 1;
}

/********************************************************************
Function name: b_free
Purpose: The function de-allocates (frees) the memory occupied by the character buffer and the Buffer
         structure (buffer descriptor). 
Function In parameters: Buffer Structure constant  pointer
Function Out parameters: none
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
void b_free(Buffer* const pBD) {
    /*freein dinamic allocated memory*/
    if (pBD) {
        free(pBD->cb_head);
        free(pBD);
    }
}

/********************************************************************
Function name: b_isfull
Purpose: The function returns 1 if the character buffer is full; it returns 0 otherwise. 
Function In parameters: Buffer Structure constant  pointer
Function Out parameters: integer
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
int b_isfull(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    /*checking if full*/
    if ((short)(pBD->addc_offset * sizeof(char)) == pBD->capacity)
        return 1;
    else
        return 0;
}

/********************************************************************
Function name: b_limit
Purpose: The function returns the current limit of the character buffer. 
Function In parameters: Buffer Structure constant pointer
Function Out parameters: addc_offset(short)
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
short b_limit(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    return pBD->addc_offset;
}
/********************************************************************
Function name: b_capacity
Purpose: The function returns the current capacity of the character buffer
Function In parameters: Buffer Structure constant  pointer
Function Out parameters: Buffer capacity(short)
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
short b_capacity(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    return pBD->capacity;
}
/********************************************************************
Function name: b_markc
Purpose: The function sets markc_offset to mark. The parameter mark must be within the current limit of
         the buffer (0 to addc_offset inclusive). The function returns a pointer to the location of the currently
         set markc_offset.
Function In parameters: Buffer Structure constant  pointer, mark offset (short)
Function Out parameters: Pointer to the beginning of character array (character buffer)
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
char* b_markc(pBuffer const pBD, short mark) {
    /*Checking parameters*/
    if (pBD == NULL) {
        return NULL;
    }
    /*if mark bigger than 0 and lower than current addc_offset*/
    if (mark >= 0 && mark <= pBD->addc_offset) {
        pBD->markc_offset = mark;
        /*returning address of mark position*/
        return &(pBD->cb_head[mark]);
    }
    return NULL;
}
/********************************************************************
Function name: b_mode
Purpose: The function returns the value of mode to the calling function
Function In parameters: Buffer Structure constant pointer
Function Out parameters: integer
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
int b_mode(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    return pBD->mode;
}
/********************************************************************
Function name: b_incfactor
Purpose: The function returns the value of mode to the calling function
Function In parameters: Buffer Structure constant pointer
Function Out parameters: increase factor size
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
size_t b_incfactor(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return 256;
    return (unsigned char)pBD->inc_factor;
}
/********************************************************************
Function name: b_load
Purpose: The function loads (reads) an open input file specified by fi into a buffer specified by pBD. 
Function In parameters: File Structure constant pointer, Buffer Structure constant pointer
Function Out parameters: Number of Characters
Version: 1.0
Author: Raphael Guerra
Called functions: feof(), b_addc(), ungetc()
**********************************************************************/
int b_load(FILE* const fi, Buffer* const pBD) {
    /*character to hold one char at a time from buffer*/
    char c;
    /*counter for number of characters*/
    int cCount; 
    cCount = 0;
    /*Checking parameters*/
    if (fi == NULL || pBD == NULL)
        return RT_FAIL_1;
    /*while not end of file*/
    while (1) {
        if (feof(fi)) {
            break;
        }
        /*get character from file*/
        c = (char)fgetc(fi);
        /*if character is null*/
        if (!b_addc(pBD, c)) {
            /*give back character to file handler*/
            ungetc(c, fi);
            /*return error*/
            return LOAD_FAIL;
        }
        /*if everything ok, increase counter*/
        ++cCount;
    }
    return cCount;
}
/********************************************************************
Function name: b_isempty
Purpose: If the addc_offset is 0, the function returns 1; otherwise it returns 0. 
Function In parameters: Buffer Structure constant pointer
Function Out parameters: integer
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
int b_isempty(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    if (!pBD->addc_offset)
        return 1;
    else
        return 0;
}
/********************************************************************
Function name: b_eob
Purpose: The function returns the value of the flags field determined only by the eob bit.
         A bitwise operation must be used to return the value of the flags field. 
Function In parameters: Buffer Structure constant pointer
Function Out parameters: integer
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
int b_eob(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    /*return EOB bit flag*/
    return pBD->flags & CHECK_EOB;
}
/********************************************************************
Function name: b_getc
Purpose: This function is used to read the buffer. 
Function In parameters: Buffer Structure constant pointer
Function Out parameters: character of buffer
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
char b_getc(Buffer* const pBD) {
    /*Checking parameters*/
    if (pBD == NULL) {
        return RT_FAIL_2;
    }
    /*if getc_offset is equal to addc_offset means End Of Buffer*/
    if (pBD->getc_offset == pBD->addc_offset) {
        pBD->flags |= SET_EOB;
        return 0;
    }
    /*else reset EOB bit flag*/
    else {
        pBD->flags &= RESET_EOB;
    }
    /*Add 1 to offset and return element at the end of buffer*/
    return pBD->cb_head[pBD->getc_offset++];
}
/********************************************************************
Function name: b_print
Purpose: This function is intended to be used for diagnostic purposes only. Using the printf() library
         function the function prints character by character the contents of the character buffer to the
         standard output (stdout).
Function In parameters: Buffer Structure constant pointer, new line character
Function Out parameters: integer
Version: 1.0
Author: Raphael Guerra
Called functions: b_getc(), b_eob(), printf()
**********************************************************************/
int b_print(Buffer* const pBD, char nl) {
    /*character to hold one char at a time from buffer*/
    char c;
    /*counter for number of characters*/
    int cCount; 
    cCount = 0;

    /*Checking parameters*/
    if (!pBD) {
        return RT_FAIL_1;
    }
    /*while loop until flag end_of_file break the loop*/
    while (1) {
        /*get char from buffer*/
        c = b_getc(pBD);
        /*if end of buffer*/
        if (b_eob(pBD)) {
            if (nl != 0) {
                /*print to stdout*/
                printf("\n");
            }
            /*if no elements return NULL*/
            if (cCount == 0) {
                return RT_FAIL_1;
            }
            /*else return char count*/
            else {
                return cCount;
            }
        }
        /*print to stdout*/
        printf("%c", c);
        cCount++;
    }
    if (nl != 0) {
        /*print to stdout*/
        printf("\n");
    }
    return cCount;
}
/********************************************************************
Function name: b_compact
Purpose: For all operational modes of the buffer the function shrinks (or in some cases may expand) the
         buffer to a new capacity
Function In parameters: Buffer Structure constant pointer, symbol character
Function Out parameters: Buffer Structure constant pointer
Version: 1.0
Author: Raphael Guerra
Called functions: realoc()
**********************************************************************/
Buffer* b_compact(Buffer* const pBD, char symbol) {
    /*shot and temporary buffer to work on reallocating structure*/
    short newCapacity; 
    char* tempBuffer; 
    newCapacity = 0; 
    /*Checking parameters*/
    if (!pBD) {
        return NULL;
    }
    pBD->flags &= RESET_R_FLAG;
    /*increasing capacity throuth temporary local variable*/
    newCapacity = (short)(sizeof(char) * (pBD->addc_offset + 1));
    /*if new capacity equals  return NULL*/
    if (newCapacity <= 0) {
        return NULL;
    }
    /*reallocating dinamic memory*/
    tempBuffer = (char*)realloc(pBD->cb_head, newCapacity);
    /*Checking pointer*/
    if (!tempBuffer) {
        return NULL;
    }
    pBD->cb_head = tempBuffer;
    pBD->cb_head[pBD->addc_offset++] = symbol;
    pBD->capacity = newCapacity;
    return pBD;
}
/********************************************************************
Function name: b_rflag
Purpose: The function returns the value of the flags field determined only by the r_flag bit.
Function In parameters: Buffer Structure constant pointer
Function Out parameters: character
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
char b_rflag(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    /*return rflag bit flag*/
    return pBD->flags & CHECK_R_FLAG;
}
/********************************************************************
Function name: b_retract
Purpose: The function decrements getc_offset by 1
Function In parameters: Buffer Structure constant pointer
Function Out parameters: short
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
short b_retract(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    /*subtract getc_offset because of retraction*/
    if (pBD->getc_offset) {
        pBD->getc_offset--;
    }
    return pBD->getc_offset;
}
/********************************************************************
Function name: b_reset
Purpose: The function sets getc_offset to the value of the current markc_offset
Function In parameters: Buffer Structure constant pointer
Function Out parameters: short
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
short b_reset(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    pBD->getc_offset = pBD->markc_offset;
    return pBD->getc_offset;
}
/********************************************************************
Function name: b_getcoffset
Purpose: The function returns getc_offset to the calling function
Function In parameters: Buffer Structure constant pointer
Function Out parameters: short
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
short b_getcoffset(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    return pBD->getc_offset;
}
/********************************************************************
Function name: b_getcoffset
Purpose: The function set the getc_offset and markc_offset to 0, 
         so that the buffer can be reread again.
Function In parameters: Buffer Structure constant pointer
Function Out parameters: integer
Version: 1.0
Author: Raphael Guerra
Called functions: none
**********************************************************************/
int b_rewind(Buffer* const pBD) {
    /*Checking parameters*/
    if (!pBD)
        return RT_FAIL_1;
    /*resetting getc_offset and markc_offset for iteration*/
    pBD->getc_offset = 0;
    pBD->markc_offset = 0;
    return 0;
}

