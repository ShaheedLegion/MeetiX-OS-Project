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

#include "installed.hpp"

#include <stdio.h>
#include <ghost.h>
#include <string.h>

#define app_path "/bin/"
#define svs_path "/sys/svs/"

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		char *argument = argv[1];

		if (!strcmp(argument, "-l") || !strcmp(argument, "--list"))
		{
			g_fs_open_directory_status stat;
			auto *app_dir = g_open_directory_s(app_path, &stat);
			
			printf("Installed Applications:\n");
			printf("    |TYPE|\t|DIR|\t|NAME|\n");

			g_fs_directory_entry *app_entry;
			while (true)
			{
				g_fs_read_directory_status rstat;
				app_entry = g_read_directory_s(app_dir, &rstat);

				if (rstat == G_FS_READ_DIRECTORY_SUCCESSFUL)
				{
					if (!strcmp(app_entry->name, "CandyShell.mxe"))
					{
						printf("   < Dos > ");
					}

					else if (!strcmp(app_entry->name, "init.mxe"))
					{
						printf("   < Init > ");
					}

					else 
					{
						printf("   < App > ");
					}

					printf("   %s\t%s\n", app_path, app_entry->name);
				}

				else if (rstat == G_FS_READ_DIRECTORY_EOD) 
				{
					break;
				} 
			}
			g_close_directory(app_dir);

			if (argc > 2)
			{
				char *argument2 = argv[2];

				if (!strcmp(argument2, "-k") || !strcmp(argument2, "--kernel"))
				{
					g_fs_open_directory_status stat_root;
					auto *svs_dir = g_open_directory_s(svs_path, &stat_root);

					printf("Evangelion's installed servers:\n");
					printf("    |TYPE|\t   |DIR|\t\t|NAME|\n");

					g_fs_directory_entry *svs_entry;
					while (true)
					{
						g_fs_read_directory_status rstat_svs;
						svs_entry = g_read_directory_s(svs_dir, &rstat_svs);

						if (rstat_svs == G_FS_READ_DIRECTORY_SUCCESSFUL)
						{
							printf("   < server >\t%s\t%s\n", svs_path, svs_entry->name);
						} 

						else if (rstat_svs == G_FS_READ_DIRECTORY_EOD)
						{
							break;
						}
					}
					g_close_directory(svs_dir);
				}
			}
		}

		else if (!strcmp(argument, "--help") || !strcmp(argument, "-h"))
			help();
	}

	else
		help();
}

void help()
{
	printf("MeetiX Package Manager v%d.%d\n" 
		"    Type ist -l for list of installed apps\n"
		"    Type ist -l -k for list of installed apps and Evangelion servers\n"
		"    Or ist -h / ist --help for this help\n", MAJOR, MINOR);
}