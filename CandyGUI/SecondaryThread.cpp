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

#include "SecondaryThread.hpp"

g_label *taskLabel;
g_label *hourLabel;

void SecondaryThread_t::HourManagerThread()
{
	g_task_register_id("CandyGUI/HourManagerThread");

	g_syscall_getTime time = g_getTime();
	
	while (true)
	{
		std::stringstream hourStr;

		hourStr << "| ";
		hourStr << time.hour;
		hourStr << ":";
		hourStr << time.minute;
		hourStr << ":";
		hourStr << time.second;
		hourStr << " |";

		hourLabel->setTitle(hourStr.str());

		g_sleep(1000);
		time.second++;

		if (time.second > 59)
		{
			time.second = 0;
			time.minute++;

			if (time.minute > 59)
			{
				time.minute = 0;
				time.hour++;
			}
		}
	}
}

void SecondaryThread_t::TaskManagerThread()
{	
	g_task_register_id("CandyGUI/TaskManagerThread");

	while (true)
	{
		std::stringstream process;

		// read how many devices there are
		g_kernquery_task_count_out out;

		g_kernquery(G_KERNQUERY_TASK_COUNT, 0, (uint8_t*) &out);

		// list tasks
		g_kernquery_task_get_out *infos = new g_kernquery_task_get_out[out.count];

		for (uint32_t pos = 0; pos < out.count; pos++) 
		{
			// query task at position
			g_kernquery_task_get_by_pos_in in;
			in.position = pos;

			g_local<g_kernquery_task_get_out> out(new g_kernquery_task_get_out());

			g_kernquery(G_KERNQUERY_TASK_GET_BY_POS, (uint8_t*) &in, (uint8_t*) out());

			infos[pos] = *(out());
		}

		for (uint32_t pos = 0; pos < out.count; pos++) 
		{
			g_kernquery_task_get_out *entry = &infos[pos];

			// print processes
			if (entry->type == G_THREAD_TYPE_MAIN) 
			{	
				if (!strcmp(entry->identifier, "spawner") || !strcmp(entry->identifier, "ps2driver") || 
					!strcmp(entry->identifier, "vbedriver") || !strcmp(entry->identifier, "zipNET") || 
					!strcmp(entry->identifier, "CandyGui") || !strcmp(entry->identifier, "CandyText"))
					continue;

				else
				{
					process << " [";
					process << entry->identifier;
					process << "] ";
				}
			}
		}
		taskLabel->setTitle(process.str());
		g_sleep(1000);
	}
}