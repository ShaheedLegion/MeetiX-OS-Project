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
GetWorkingDirectoryStatus GetWorkingDirectory(char *buffer)
{
	return GetWorkingDirectoryL(buffer, PATH_MAX);
}
/**
 *
 */
GetWorkingDirectoryStatus GetWorkingDirectoryL(char *buffer, size_t maxlen)
{
	SyscallFsGetWorkingDirectory data;
	data.buffer = buffer;
	data.maxlen = maxlen;
	
	syscall(SYSCALL_GET_WORKING_DIRECTORY, (uint32_t) &data);
	
	return data.result;
}
