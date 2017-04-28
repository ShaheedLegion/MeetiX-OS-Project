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

#include <tasking/process.hpp>

/**
 *
 */
Process::Process(SecurityLevel securityLevel) : securityLevel(securityLevel) 
{
	parent = 0;
	main = 0;

	pageDirectory = 0;

	imageStart = 0;
	imageEnd = 0;
	heapStart = 0;
	heapBreak = 0;
	heapPages = 0;

	// empty string fields
	cliArguments = 0;
	sourcePath = 0;

	// set default working directory
	workingDirectory = new char[PATH_MAX];
	workingDirectory[0] = '/';
	workingDirectory[1] = 0;

	tlsMasterInProcLocation = 0;
	tlsMasterCopysize = 0;
	tlsMasterTotalsize = 0;
	tlsMasterAlignment = 0;
}

/**
 *
 */
Process::~Process() 
{
	if (cliArguments) delete cliArguments;
	if (sourcePath) delete sourcePath;
	if (workingDirectory) delete workingDirectory;
}

