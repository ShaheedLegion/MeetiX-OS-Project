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
#include <stdint.h>
#include <string.h>

// redirect
int32_t Write(Fd file, const void *buffer, uint64_t length) 
{
	return WriteS(file, buffer, length, 0);
}

/**
 *
 */
int32_t WriteS(Fd file, const void *buffer, uint64_t length, FsWriteStatus *outStatus) 
{

	SyscallFsWrite data;
	data.fd = file;
	data.buffer = (uint8_t*) buffer;
	data.length = length;
	
	syscall(SYSCALL_FS_WRITE, (uint32_t) &data);
	
	if (outStatus) 
	{
		*outStatus = data.status;
	}
	
	return data.result;
}
