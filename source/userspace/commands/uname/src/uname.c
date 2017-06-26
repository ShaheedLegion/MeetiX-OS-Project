/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <eva.h>

/*
 * mode for application
 */
enum UnameMode_t
{
	ALL,
	USER,
	UNAME,
	LOADER,
	KERNEL,
	INFO,
};

/*
 *
 */
enum UnameMode_t parseArgs(int argc, char *argv[])
{
	if (argc == 2)
	{
		if (!strcmp(argv[1], "-a") || !strcmp(argv[1], "--all")) return ALL;
		else if (!strcmp(argv[1], "-o") || !strcmp(argv[1], "--os")) return USER;
		else if (!strcmp(argv[1], "-u") || !strcmp(argv[1], "--uname")) return UNAME;
		else if (!strcmp(argv[1], "-l") || !strcmp(argv[1], "--loader")) return LOADER;
		else if (!strcmp(argv[1], "-k") || !strcmp(argv[1], "--kernel")) return KERNEL;
		else if (!strcmp(argv[1], "-i") || !strcmp(argv[1], "--info")) return INFO;
	}

	return ALL;
}

/*
 *
 */
int main(int argc, char *argv[])
{
	// parse the arguments and get mode
	enum UnameMode_t mode = parseArgs(argc, argv);

	// perform mode
	switch (mode)
	{
		case ALL:
		{
			// call kernel to get uname info
			SyscallUname uname;
			Uname(&uname);

			// print infos
			println("");
			println("%s on %s %s [Copyright (C) 2017, MeetiX OS Project]", getenv("USER"), getenv("HOSTNAME"), getenv("VERSION"));
			println(" Loader: %-15s v%d.%d.%c", uname.loaderName, uname.LversionMJ,  uname.LversionMN, uname.LversionPT);
			println(" Kernel: %-15s v%d.%d.%d.%c", uname.kernelName, uname.versionMJ, uname.versionMN, uname.versionSB, uname.versionPT);

			// call kernel to get some system infos
			SyscallSysinfo info;
			Sysinfo(&info);

			// print infos
			println("");
			println(" RAM: used %iMB on %iMB", (info.totRam - info.freeRam) / 1024, info.totRam / 1024);
			println(" CPU: %d avaible core, vendor: %s", info.numberOfCore, info.cpuVendor);
			println("");
		}
		break;

		case USER:
		{
			println("");
			println("%s on %s %s [Copyright (C) 2017, MeetiX OS Project]", getenv("USER"), getenv("HOSTNAME"), getenv("VERSION"));
			println("");
		}
		break;

		case UNAME:
		{
			// call kernel to get uname info
			SyscallUname uname;
			Uname(&uname);

			// print infos
			println("");
			println(" Loader: %-15s v%d.%d.%c", uname.loaderName, uname.LversionMJ,  uname.LversionMN, uname.LversionPT);
			println(" Kernel: %-15s v%d.%d.%d.%c", uname.kernelName, uname.versionMJ, uname.versionMN, uname.versionSB, uname.versionPT);
			println("");
		}
		break;

		case LOADER:
		{
			// call kernel to get uname info
			SyscallUname uname;
			Uname(&uname);

			// print infos
			println("%-15s v%d.%d.%c", uname.loaderName, uname.LversionMJ,  uname.LversionMN, uname.LversionPT);
		}
		break;

		case KERNEL:
		{
			// call kernel to get uname info
			SyscallUname uname;
			Uname(&uname);

			// print infos
			println("%-15s v%d.%d.%d.%c", uname.kernelName, uname.versionMJ, uname.versionMN, uname.versionSB, uname.versionPT);
		}
		break;

		case INFO:
		{
			// call kernel to get some system infos
			SyscallSysinfo info;
			Sysinfo(&info);

			// print infos
			println("");
			println(" RAM: used %iMB on %iMB", (info.totRam - info.freeRam) / 1024, info.totRam / 1024);
			println(" CPU: %d avaible core, vendor: %s", info.numberOfCore, info.cpuVendor);
			println("");
		}
		break;
	}
}
