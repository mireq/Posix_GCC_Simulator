/*
	FreeRTOS.org V5.2.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.

	A special exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS.org without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details.


	***************************************************************************
	*                                                                         *
	* Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

#include <stdio.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "fileIO.h"

void vDisplayMessage( const char * const pcMessageToPrint )
{
	vTaskSuspendAll();
	{
		printf( "%s", pcMessageToPrint );
		fflush( stdout );
	}
	xTaskResumeAll();
}
/*-----------------------------------------------------------*/

void vWriteMessageToDisk( const char * const pcMessage )
{
const char * const pcFileName = "RTOSlog.txt";
const char * const pcSeparator = "\r\n-----------------------\r\n";
FILE *pf;

	vTaskSuspendAll();
	{
		pf = fopen( pcFileName, "a" );
		if( pf != NULL )
		{
			fwrite( pcMessage, strlen( pcMessage ), ( unsigned short ) 1, pf );
			fwrite( pcSeparator, strlen( pcSeparator ), ( unsigned short ) 1, pf );
			fclose( pf );
		}
	}
	xTaskResumeAll();
}
/*-----------------------------------------------------------*/

void vWriteBufferToDisk( const char * const pcBuffer, unsigned long ulBufferLength )
{
const char * const pcFileName = "trace.bin";
FILE *pf;

	vTaskSuspendAll();
	{
		pf = fopen( pcFileName, "wb" );
		if( pf )
		{
			fwrite( pcBuffer, ( size_t ) ulBufferLength, ( unsigned short ) 1, pf );
			fclose( pf );
		}
	}
	xTaskResumeAll();
}

