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

#ifndef _EVA_EVAQUERY_
#define _EVA_EVAQUERY_

#include "eva/common.h"
#include "eva/fs.h"
#include "eva/kernel.h"

__BEGIN_C

/**
 *	Query status
 */
typedef int EvaQueryStatus;
static const EvaQueryStatus EVAQUERY_STATUS_SUCCESSFUL = 0;
static const EvaQueryStatus EVAQUERY_STATUS_UNKNOWN_ID = 1;

/**
 * Command IDs
 */
#define EVAQUERY_PCI_COUNT				0x500
#define EVAQUERY_PCI_GET				0x501

#define EVAQUERY_TASK_COUNT				0x600
#define EVAQUERY_TASK_GET_BY_POS		0x601
#define EVAQUERY_TASK_GET_BY_ID			0x601

/**
 * PCI
 */
typedef struct 
{
	uint32_t count;
}__attribute__((packed)) EvaQueryPciCountOut;

typedef struct 
{
	uint32_t position;
}__attribute__((packed)) EvaQueryPciGetIn;

typedef struct 
{
	uint8_t found;

	uint8_t slot;
	uint8_t bus;
	uint8_t function;

	uint16_t vendorId;
	uint16_t deviceId;

	uint8_t classCode;
	uint8_t subclassCode;
	uint8_t progIf;
}__attribute__((packed)) EvaQueryPciGetOut;

/**
 * Processes / threads
 */
typedef struct 
{
	uint32_t count;
}__attribute__((packed)) EvaQueryTaskCountOut;

typedef struct 
{
	uint32_t position;
}__attribute__((packed)) EvaQueryTaskGetByPosIn;

typedef struct 
{
	Tid id;
}__attribute__((packed)) EvaQueryTaskGetByIdIn;

typedef struct 
{
	uint8_t found;

	Tid id;
	Tid parent;
	ThreadType type;
	char identifier[512];
	char sourcePath[PATH_MAX];

	VirtualAddress memoryUsed;
}__attribute__((packed)) EvaQueryTaskGetOut;

__END_C

#endif
