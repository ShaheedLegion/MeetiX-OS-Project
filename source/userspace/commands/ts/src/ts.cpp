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

#include <eva.h>
#include <stdio.h>
#include <mxuser/tasking/tasking.hpp>

/**
 *	compare method for sort, sort task list in increase
 */
static inline bool compare(const EvaQueryTaskGetData &task1, const EvaQueryTaskGetData &task2)
{
	return task1.id > task2.id;
}

/*
 *
 */
static inline uint32_t getMemoryAmount(uint32_t used, bool *mb)
{
	if ((used / 1024) > 1024)
	{
		*mb = true;
		return used / 1048576;
	}
	return used / 1024;
}

/*
 *
 */
int main(int argc, char *argv[])
{
	// get thread list from kernel
	llist<EvaQueryTaskGetData> tlist = Tasking::getThreads();
	tlist.sort(compare);

	// print on screen
	println("%5s  %-56s %5s", "Tid", "Name", "Memory");
	for (llist<EvaQueryTaskGetData>::iterator it = tlist.begin(); it != tlist.end(); ++it)
	{
		bool mega = false;
		println("%5i  %-56s %5i%s", (*it).id, (*it).identifier, getMemoryAmount((*it).memoryUsed, &mega), mega ? "MB" : "KB");
	}

	return 0;
}
