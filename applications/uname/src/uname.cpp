/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (char *argumentat your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <mxuser/utils/environment.hpp>
#include <stdio.h>
#include <eva.h>

/*
 *
 */
int main(int argc, char *argv[])
{
	// call kernel to get uname info
	SyscallUname u = Uname();
	SyscallSysinfo i = Sysinfo();

	// print infos from shell and kernel
	println("");
	println("%s on %s %s [Copyright (C) 2017, MeetiX OS Project]", Environment::getEnvVar("USER").c_str(), Environment::getEnvVar("HOSTNAME").c_str(), Environment::getEnvVar("VERSION").c_str());
	println(" Kernel:");
	println("    EvaLoader:    v%d.%d.%d", u.LversionMJ,  u.LversionMN, u.LversionPT);
	println("    EvangelionNG: v%d.%d.%d.%c", u.versionMJ, u.versionMN, u.versionSB, u.versionPT);

	println("");
	println(" RAM: Free %f on %f", i.freeRam, i.totRam);
	println(" CPU: %d avaible core", i.numberOfCore);
	println("");

	return 0;
}