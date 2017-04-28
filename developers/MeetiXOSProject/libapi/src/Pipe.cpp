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
void Pipe(Fd *outWrite, Fd *outRead) 
{
	return PipeS(outWrite, outRead, 0);
}

/**
 *
 */
void PipeS(Fd *outWrite, Fd *outRead, FsPipeStatus *outStatus) 
{

	SyscallFsPipe data;
	
	syscall(SYSCALL_FS_PIPE, (uint32_t) &data);
	
	*outWrite = data.writeFd;
	*outRead = data.readFd;
	if (outStatus) 
	{
		*outStatus = data.status;
	}
}

