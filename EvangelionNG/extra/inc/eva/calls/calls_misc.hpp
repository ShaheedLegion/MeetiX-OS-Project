 /*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (char *argumentat your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHout ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EVA_API_CALLS_MISCCALLS
#define EVA_API_CALLS_MISCCALLS

#include "eva/evaquery.h"

/**
 * @field message
 * 		the message
 */
typedef struct 
{
	char *message;
}__attribute__((packed)) SyscallLog;

/**
 * @field enabled
 * 		whether or not to enable the video log
 */
typedef struct 
{
	uint8_t enabled;
}__attribute__((packed)) SyscallSetVideoLog;

/**
 * @field test
 * 		test value
 *
 * @field result
 * 		test result
 */
typedef struct 
{
	uint32_t test;

	uint32_t result;
}__attribute__((packed)) SyscallTest;

/**
 * @field command
 * 		the query command
 *
 * @field query
 * 		input query buffer pointer
 *
 * @field outbuffer
 * 		output buffer pointer
 *
 * @field status
 * 		one of the {EvaQueryStatus} status codes
 */
typedef struct 
{
	uint16_t command;
	const uint8_t *query;
	uint8_t *outbuffer;
	EvaQueryStatus status;
}__attribute__((packed)) SyscallEvaQuery;

/**
 *  @field versionMJ
 *  	major version
 *
 *	@field versionMN
 *		minor version
 *
 *	@field versionSB
 *		sub version
 *
 *	@field versionPT
 *		patch level
 */
typedef struct
{
	size_t LversionMJ;
	size_t LversionMN;
	uint8_t LversionPT;

	size_t versionMJ;
	size_t versionMN;
	size_t versionSB;
	uint8_t versionPT;

} __attribute__((packed)) SyscallUname;


typedef struct
{
	uint32_t numberOfCore;
	char *cpuVendor;

	uint32_t totRam;
	uint32_t freeRam;

} __attribute__((packed)) SyscallSysinfo;

#endif
