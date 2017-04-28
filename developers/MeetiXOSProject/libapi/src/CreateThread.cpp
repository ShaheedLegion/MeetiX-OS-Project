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
#include "__internal.h"

/**
 * Thread setup routine, used by the thread creation call. Assumes that the created thread
 * has a valid "userEntry" - otherwise exits with error code -1.
 */
void ThreadSetupRoutine() 
{
	SyscallGetThreadEntry data;
	syscall(SYSCALL_GET_THREAD_ENTRY, (uint32_t) &data);

	void (*userEntry)(void*) = (void(*)(void*)) (data.userEntry);

	if (userEntry) 
	{
		// Call the entry
		(userEntry)(data.userData);

		return __ExitThread();
	}

	return __ExitThread();
}

// redirect
Tid CreateThread(void *function) 
{
	return CreateThreadD(function, 0);
}

// redirect
Tid CreateThreadD(void *function, void *userData) 
{
	return CreateThreadDS(function, userData, 0);
}

/**
 *
 */
Tid CreateThreadDS(void *function, void *userData, CreateThreadStatus *outStatus) 
{
	SyscallCreateThread data;
	data.initialEntry = (void*) ThreadSetupRoutine;
	data.userEntry = function;
	data.userData = userData;
	
	syscall(SYSCALL_CREATE_THREAD, (uint32_t) &data);
	
	if (outStatus) 
	{
		*outStatus = data.status;
	}
	return data.threadID;
}
