/*********************************************************************************
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]                  *
* 																			     *
* This program is free software; you can redistribute it and/or                  *
* modify it under the terms of the GNU General Public License                    *
* as published by the Free Software Foundation; either version 2				 *
* of the License, or (char *argumentat your option) any later version.			 *
*																				 *
* This program is distributed in the hope that it will be useful,				 *
* but WITHout ANY WARRANTY; without even the implied warranty of                 *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 				 *
* GNU General Public License for more details.									 *
*																				 *
* You should have received a copy of the GNU General Public License				 *
* along with this program; if not, write to the Free Software                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
**********************************************************************************/

#include "eva/user.h"

/**
 *
 */
EvaQueryStatus EvaQuery(uint16_t command, const uint8_t *query, uint8_t *outbuffer) 
{
	// fill data struct
	SyscallEvaQuery data;
	data.command = command;
	data.query = query;
	data.outbuffer = outbuffer;
	
	// perform call
	syscall(SYSCALL_EVAQUERY, (uint32_t) &data);
	
	// return status
	return data.status;
}
