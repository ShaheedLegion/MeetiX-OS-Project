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
#include <sstream>
#include <string.h>

#include <stdio.h>
#include <mxuser/tasking/tasking.hpp>

/**
 *	compare method for sort, sort task list in increase
 */
static bool compare(const EvaQueryTaskGetData &taskList1, const EvaQueryTaskGetData &taskList2)
{
	return taskList1.id > taskList2.id;
}

/*
 *
 */
void showTotalMemoryUsage()
{
	SyscallSysinfo info;
	Sysinfo(&info);

	int width = (info.totRam / 1024) / 10;
	int used = (info.totRam / 1024) - (info.freeRam / 1024);
	int percent = used / 100 * width;
	int cells = (width * percent) / 100;

	printf("\n  %-4s", "Total Memory Usage [");

	for (int index = 0; index < width; index++)
	{
		if (index <= cells)	putc('|', stdout);
		else putc(' ', stdout);
	}

	printf("] %d/%dMB\n\n", used, info.totRam / 1024);
}

/**
 *
 */
int procListText(bool all)
{
	// show memory usage
	showTotalMemoryUsage();

	// get task list from kernel
	llist<EvaQueryTaskGetData> list = Tasking::getProcess();

	// sorting task array
	list.sort(compare);

	println("%5s   %6s %-18s %-38s", "PID", "PHYS MEM", "NAME", "PATH");
	for (llist<EvaQueryTaskGetData>::iterator it = list.begin(); it != list.end(); ++it)
	{
		// print foot line
		println("%5i %8i%s %-18s %-38s", (*it).id, ((*it).memoryUsed / 1024) > 1024 ? (*it).memoryUsed / 1048576 : (*it).memoryUsed / 1024, ((*it).memoryUsed / 1024) > 1024 ? "MB" : "KB", (*it).identifier, (*it).sourcePath);
	}

	return 0;
}


/**
 *	task manager utility
 */
int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		const char *command = argv[1];

		if (!strcmp("-a", command) || !strcmp("--all", command)) return procListText(true);
		else if (!strcmp("kill", command) || !strcmp("-kl", command))
		{
			if (argc > 2)
			{
				Tid target;

				std::stringstream converter;
				converter << argv[2];
				converter >> target;

				if (target)
				{
					KillStatus status = Kill(target);
					if (status == KILL_STATUS_SUCCESSFUL) println("task %d successfully killed", target);
					else if (status == KILL_STATUS_NOT_FOUND) println("task %d does not exist", target);
					else println("failed to kill task %d with status %d", target, status);
				}

				else println("Please supply a valid task TID.");
			}

		}

		else if (!strcmp("--help", command) || !strcmp("-h", command))
		{
			println("");
			println("Task management utility");
			println("");
			println("The following commands are available:");
			println("");
			println("\t-a/--all\t\tcomplete list about running tasks");
			println("\t-kl/kill <id>\tkills the task with the given id");
			println("");
		}

		else println("unknown command: %s", command);
	}

	else return procListText(false);

	return 0;
}
