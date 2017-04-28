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

#include "list.hpp"

#include <stdio.h>
#include <eva/utils/local.hpp>
#include <eva/evaquery.h>
#include <algorithm>
#include <iostream>

/**
 *	compare method for sort
 */
static inline bool sortTask(EvaQueryTaskGetOut taskList1, EvaQueryTaskGetOut taskList2) { return taskList1.id < taskList2.id; }

/*
 *
 */
void showTotalMemoryUsage()
{
	SyscallSysinfo memUsage = Sysinfo();
	
	int width = (memUsage.totRam / 1024) / 10;
	int used = (memUsage.totRam / 1024) - (memUsage.freeRam / 1024);
	int percent = used / 100 * width;
	int cells = (width * percent) / 100;

	printf("\n  %-4s", "Total Memory Usage [");
	
	for (int index = 0; index < width; index++)
	{
		if (index <= cells)	putc('|', stdout);
		else putc(' ', stdout);
	}

	std::cout << ']' << used << '/' << memUsage.totRam / 1024 << "MB" << std::endl;
}

/**
 *
 */
int procListText(bool all) 
{
	// read how many devices there are
	EvaQueryTaskCountOut out;

	EvaQueryStatus countstatus = EvaQuery(EVAQUERY_TASK_COUNT, 0, (uint8_t*) &out);
	if (countstatus != EVAQUERY_STATUS_SUCCESSFUL) 
	{
		fprintf(stderr, "failed to query the kernel for the number of tasks (code %i)\n", countstatus);
		return 1;
	}

	// list tasks
	EvaQueryTaskGetOut *infos = new EvaQueryTaskGetOut[out.count];

	// get tasks from kernel lists
	for (uint32_t pos = 0; pos < out.count; pos++) 
	{
		// query task at position
		EvaQueryTaskGetByPosIn in;
		in.position = pos;

		Local<EvaQueryTaskGetOut> out(new EvaQueryTaskGetOut());

		EvaQueryStatus getstatus = EvaQuery(EVAQUERY_TASK_GET_BY_POS, (uint8_t*) &in, (uint8_t*) out());
		if (getstatus != EVAQUERY_STATUS_SUCCESSFUL) 
		{
			fprintf(stderr, "failed to query the kernel for task at position %i (code %i)", pos, getstatus);
			return 1;
		}

		infos[pos] = *(out());
	}

	// show memory usage
	showTotalMemoryUsage();

	// sorting task array
	std::sort(infos, infos + out.count, sortTask);

	// print information sorted
	if (all) println("\n  %-4s %-30s %-12s %-8s %-7s", "Tid", "Source", "Identifier", "Type", "Memory Usage");
	else println("\n  %-4s %-30s %-12s %-8s %-7s", "Pid", "Source", "Identifier", "Type", "Memory Usage");

	for (uint32_t pos = 0; pos < out.count; pos++) 
	{
		EvaQueryTaskGetOut *entry = &infos[pos];

		// print processes and vm86
		if (entry->type == THREAD_TYPE_MAIN || entry->type == THREAD_TYPE_VM86) 
		{

			const char *textType = "?";
			if (entry->type == THREAD_TYPE_MAIN) textType = "normal";

			else if (entry->type == THREAD_TYPE_VM86) textType = "vm86";

			// check if it has threads
			bool hasThreads = false;
			for (uint32_t pos = 0; pos < out.count; pos++) 
			{
				EvaQueryTaskGetOut *subentry = &infos[pos];
				if (subentry->type == THREAD_TYPE_SUB && subentry->parent == entry->id) hasThreads = true;
			}

			// print foot line
			printf(" ");
			printf(hasThreads ? "\xC2" : "\xC4");

			if (entry->memoryUsed / 1024 > 1024) println(" %-4i %-30s %-12s %-8s %-7iMB", entry->id, entry->sourcePath, entry->identifier, textType, entry->memoryUsed / 1024 / 1024);

			else println(" %-4i %-30s %-12s %-8s %-7iKB", entry->id, entry->sourcePath, entry->identifier, textType, entry->memoryUsed / 1024);

			if (all)
			{
				// print subtasks
				for (uint32_t pos = 0; pos < out.count; pos++) 
				{
					EvaQueryTaskGetOut *subentry = &infos[pos];

					// check if there are more threads after this one
					bool moreFollow = false;
					for (uint32_t pos2 = pos + 1; pos2 < out.count; pos2++) 
					{
						EvaQueryTaskGetOut *subfollow = &infos[pos2];
						if (subfollow->type == THREAD_TYPE_SUB && subfollow->parent == entry->id) moreFollow = true;
					}

					if (subentry->type == THREAD_TYPE_SUB && subentry->parent == entry->id) 
					{
						printf(" ");
						printf(moreFollow ? "\xC3" : "\xC0");
						println(" %-4i %-30s %-12s", subentry->id, "", subentry->identifier);
					}
				}
			}
		}
	}

	delete infos;

	return 0;
}