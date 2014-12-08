/**
 * @brief           Netcruzer RTOS Memory functions
 * @file            nzos_mem1.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 & XC32 Compilers
 *  
 **********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2014-02-02, David H. (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_NZOS_MEM_C

#include "HardwareProfile.h"
#include "nzos_mem.h"

//Add debugging to this file. The DEBUG_CONF_NZOS_MEM macro sets debugging to desired level, and is configured in "Debug Configuration" section of projdefs.h file
#if !defined(DEBUG_CONF_NZOS_MEM)
    #define DEBUG_CONF_NZOS_MEM       DEBUG_CONF_DEFAULT   //Default Debug Level, disabled if DEBUG_LEVEL_ALLOFF defined, else DEBUG_LEVEL_ERROR
#endif
#define MY_DEBUG_LEVEL   DEBUG_CONF_NZOS_MEM
#include "nz_debug.h"


////////// Defines //////////////////////////////
#ifndef traceMALLOC
    #define traceMALLOC( pvAddress, uiSize )
#endif
#ifndef traceFREE
    #define traceFREE( pvAddress, uiSize )
#endif


////////// Function Prototypes //////////////////


////////// Variables ////////////////////////////

#if defined(__C30__)
#define portBYTE_ALIGNMENT          (2)
#define portBYTE_ALIGNMENT_MASK     (0xfffe)
#define portPOINTER_SIZE_TYPE       UINT16
#elif defined(__PIC32MX__)
#define portBYTE_ALIGNMENT          (4)
#define portBYTE_ALIGNMENT_MASK     (0xfffe)
#define portPOINTER_SIZE_TYPE       UINT32
#else
    #error "Unknown processor or compiler."
#endif


/* A few bytes might be lost to byte aligning the heap start address. */
#define configADJUSTED_HEAP_SIZE    (nzosHEAP_SIZE - portBYTE_ALIGNMENT)


////////////////////////////////////////////////////////////////////////////////
////////// Method 1 ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if (nzosMEM1_ENABLE==1)
/* Allocate the memory for the heap. */
static BYTE   ucHeap[ nzosHEAP_SIZE ];
static WORD xNextFreeByte = ( WORD ) 0;


void * nzMemMalloc( WORD xWantedSize )
{
void *pvReturn = NULL;
static BYTE *pucAlignedHeap = NULL;

    // Ensure that blocks are always aligned to the required number of bytes.
    #if portBYTE_ALIGNMENT != 1
        if( xWantedSize & portBYTE_ALIGNMENT_MASK )
        {
            // Byte alignment required.
            xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );

            DEBUG_PUT_STR(DEBUG_LEVEL_INFO, "\nzMemMalloc adjusted size to");
            DEBUG_PUT_WORD(DEBUG_LEVEL_INFO, xWantedSize);
        }
    #endif

    //vTaskSuspendAll();

    {
        if( pucAlignedHeap == NULL )
        {
            // Ensure the heap starts on a correctly aligned boundary.
            pucAlignedHeap = ( BYTE * ) ( ( ( portPOINTER_SIZE_TYPE ) &ucHeap[ portBYTE_ALIGNMENT ] ) & ( ( portPOINTER_SIZE_TYPE ) ~portBYTE_ALIGNMENT_MASK ) );
        }

        // Check there is enough room left for the allocation.
        if( ( ( xNextFreeByte + xWantedSize ) < configADJUSTED_HEAP_SIZE ) &&
            ( ( xNextFreeByte + xWantedSize ) > xNextFreeByte )    )/* Check for overflow. */
        {
            // Return the next free byte then increment the index past this block.
            pvReturn = pucAlignedHeap + xNextFreeByte;
            xNextFreeByte += xWantedSize;
        }
        else {
            DEBUG_PUT_STR(DEBUG_LEVEL_ERROR, "\nzMemMalloc Not Enough Memory!");
        }

        traceMALLOC( pvReturn, xWantedSize );
    }

    //xTaskResumeAll();

    #if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if( pvReturn == NULL )
        {
            extern void nzMemAppMallocFailedHook( void );
            nzMemAppMallocFailedHook();
        }
    }
    #endif

    return pvReturn;
}


void nzMemFree( void *pv )
{
    //Memory cannot be freed using this scheme.  See nzos_mem2.c, nzos_mem3.c and
    //nzos_mem4.c for alternative implementations
    ( void ) pv;

    /* Force an assert as it is invalid to call this function. */
    //configASSERT( pv == NULL );
}


void nzMemInitialiseBlocks( void )
{
    //Only required when static memory is not cleared.
    xNextFreeByte = ( WORD ) 0;
}


WORD nzMemGetFreeHeapSize( void ) {
    return ( configADJUSTED_HEAP_SIZE - xNextFreeByte );
}
#endif  //if (nzosMEM1_ENABLE==1)



////////////////////////////////////////////////////////////////////////////////
////////// Method 2 ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if (nzosMEM2_ENABLE==1)

#endif  //if (nzosMEM2_ENABLE==1)



////////////////////////////////////////////////////////////////////////////////
////////// Method 3 ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if (nzosMEM3_ENABLE==1)
void *nzMemMalloc( WORD xWantedSize )
{
void *pvReturn;

	//vTaskSuspendAll();

	{
		pvReturn = malloc( xWantedSize );
		traceMALLOC( pvReturn, xWantedSize );
	}

	//xTaskResumeAll();

	#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
	}
	#endif

	return pvReturn;
}

void nzMemFree( void *pv )
{
	if( pv )
	{
		//vTaskSuspendAll();

		{
			free( pv );
			traceFREE( pv, 0 );
		}

		//xTaskResumeAll();
	}
}

#endif  //if (nzosMEM3_ENABLE==1)



////////////////////////////////////////////////////////////////////////////////
////////// Method 4 ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if (nzosMEM4_ENABLE==1)

#endif  //if (nzosMEM4_ENABLE==1)