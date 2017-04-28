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
#include <stdarg.h>

// redirect
Fd Open(const char *name) 
{
	return OpenFMS(name, 0, 0, 0);
}

// redirect
Fd OpenF(const char *name, int32_t flags) 
{
	return OpenFMS(name, flags, 0, 0);
}

/**
 *
 */
Fd OpenFS(const char *name, int32_t flags, FsOpenStatus *outStatus) 
{
	return OpenFMS(name, flags, 0, outStatus);
}

/**
 *
 */
Fd OpenFMS(const char *name, int32_t flags, int32_t mode, FsOpenStatus *outStatus) 
{

	SyscallFsOpen data;
	data.path = (char*) name;
	data.mode = mode;
	data.flags = flags;
	
	syscall(SYSCALL_FS_OPEN, (uint32_t) &data);
	
	if (outStatus) 
	{
		*outStatus = data.status;
	}
	return data.fd;
}
