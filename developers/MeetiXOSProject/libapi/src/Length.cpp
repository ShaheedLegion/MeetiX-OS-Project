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
#include <string.h>

// redirect
int64_t Length(Fd fd) 
{
	return LengthS(fd, 0);
}

/**
 *
 */
int64_t LengthS(Fd fd, FsLengthStatus *outStatus) 
{

	SyscallFsLength data;
	data.mode = SYSCALL_FS_LENGTH_BY_FD;
	data.fd = fd;
	
	syscall(SYSCALL_FS_LENGTH, (uint32_t) &data);
	
	if (outStatus) 
	{
		*outStatus = data.status;
	}
	return data.length;
}

// redirect
int64_t Flength(const char *path) 
{
	return FlengthSS(path, true, 0);
}

// redirect
int64_t FlengthS(const char *path, uint8_t followSymlinks) 
{
	return FlengthSS(path, followSymlinks, 0);
}

/**
 *
 */
int64_t FlengthSS(const char *path, uint8_t followSymlinks, FsLengthStatus *outStatus) 
{
	SyscallFsLength data;
	int symlinkFlag = (followSymlinks ? SYSCALL_FS_LENGTH_FOLLOW_SYMLINKS : SYSCALL_FS_LENGTH_NOT_FOLLOW_SYMLINKS);
	data.mode = symlinkFlag | SYSCALL_FS_LENGTH_BY_PATH;
	data.path = (char*) path;
	
	syscall(SYSCALL_FS_LENGTH, (uint32_t) &data);
	
	if (outStatus) 
	{
		*outStatus = data.status;
	}
	return data.length;
}

