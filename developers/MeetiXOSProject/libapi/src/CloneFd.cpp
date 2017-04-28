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

#include "eva.h"
#include <stdarg.h>

// redirect
Fd CloneFd(Fd sourceFd, Pid sourceProcess, Pid targetProcess) 
{
	return CloneFdTS(sourceFd, sourceProcess, -1, targetProcess, 0);
}

// redirect
Fd CloneFdS(Fd sourceFd, Pid sourceProcess, Pid targetProcess, FsClonefdStatus *outStatus) 
{
	return CloneFdTS(sourceFd, sourceProcess, -1, targetProcess, outStatus);
}

// redirect
Fd CloneFdT(Fd sourceFd, Pid sourceProcess, Fd targetFd, Pid targetProcess) 
{
	return CloneFdTS(sourceFd, sourceProcess, targetFd, targetProcess, 0);
}

/**
 *
 */
Fd CloneFdTS(Fd sourceFd, Pid sourceProcess, Fd targetFd, Pid targetProcess, FsClonefdStatus *outStatus) 
{
	SyscallFsClonefd data;
	data.sourceFd = sourceFd;
	data.sourcePid = sourceProcess;
	data.targetFd = targetFd;
	data.targetPid = targetProcess;
	
	syscall(SYSCALL_FS_CLONEFD, (uint32_t) &data);
	
	if (outStatus) 
	{
		*outStatus = data.status;
	}
	return data.result;
}
