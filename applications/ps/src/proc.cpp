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

#include "list/list.hpp"
#include <stdio.h>
#include <sstream>
#include <string.h>

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
			printf("");
			printf("proc, v%d.%d", MAJOR, MINOR);
			printf("Task management utility");
			printf("");
			printf("The following commands are available:");
			printf("");
			printf("\t-a/--all\t\tcomplete list about running tasks");
			printf("\t-kl/kill <id>\tkills the task with the given id");
			printf("");
		} 

		else println("unknown command: %s", command);
	} 

	else return procListText(false);
	
	return 0;
}
