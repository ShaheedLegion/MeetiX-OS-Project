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

#include "textmode.hpp"
#include <Medit.hpp>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <ghost/user.h>
#include <ghost.h>
#include <ghostuser/utils/utils.hpp>
#include <ghostuser/io/keyboard.hpp>

void TextMode_t::TextMode(int argc, char *argv[])
{
	prepare();
	format_space();
	create_intro();

	if (argc > 1)
	{
		char *argument = argv[1];

		if (!strcmp("-new", argument))
		{
			if (argc > 2)
			{
				char *filename = argv[2];
				FILE *fp;

				fp = fopen(filename, "w+");

				printf("Created and opened %s [%s]\n\n", filename, Medit_dir);

				type_file(fp);
				close_file(fp);
			}

			else
			{
				const char *filename = "newfile.txt";
				FILE *newf;

				newf = fopen(filename, "w+");

				printf("Created and opened %s [%s]\n\n", filename, Medit_dir);

				type_file(newf);
				close_file(newf);
			}
		}

		else if (!strcmp("-edit", argument))
		{
			if (argc > 2)
			{
				char *path = argv[2];

				if (argc > 3)
				{
					char *set = argv[3];
					if (!strcmp("-home", set))
					{
						g_set_working_directory(Medit_dir);
					}

					else
						fprintf(stderr, "unrecognized option\n");
				}

				else
					g_set_working_directory("/");
				
				FILE *fd = fopen(path, "a+");

				if (fd == NULL)
				{
					fprintf(stderr, "file not found\n");
				}

				else
				{
					printf("Opened [%s]\n\n", path);

					read_file_content(fd);
					type_file(fd);
					close_file(fd);
				}
				
			}
		}

		else if (!strcmp("-read", argument))
		{
			if (argc > 2)
			{
				char *path = argv[2];

				g_set_working_directory("/");
				FILE *fd = fopen(path, "r");

				if (fd == NULL)
				{
					fprintf(stderr, "file not found\n");
				}

				else 
				{
					printf("Opened (Read Only) [%s]\n", path);
					read_file_content(fd);
				}
			}
		}

		else
			fprintf(stderr, "unrecognized option\n");
	}

	else 
		help();

	uint8_t locker = true;
	g_atomic_lock(&locker);
}

void TextMode_t::prepare()
{
	if (!g_task_register_id("Medit"))
	{
		fprintf(stderr, "   error: can't execute Medit editor with this identifier\n");
		fprintf(stderr, "          [another copy of Medit is already running]\n");
		g_exit(0);
	}

	g_set_working_directory(Medit_dir); 
}

void TextMode_t::format_space()
{
	system("clean");
	system("color -bg");
}

void TextMode_t::create_intro()
{
	std::ifstream flogo("/sys/graphics/Medit_editor/MDlogo.oem-us");
	
	if (flogo.good())
	{	
		std::string mdlogo((std::istreambuf_iterator<char>(flogo)), std::istreambuf_iterator<char>());
		printf("%s", mdlogo.c_str());
	
		printf("v%d.%d\n\n", major_version, minor_version);
	}

	else
		fprintf(stderr, "fail: cannot find Medit Logo\n");
}

void TextMode_t::help()
{
	printf(" |Medit Editor help|\n"
		" Launch Medit with one of this arguments:\n"
		" -> -read <path> (read file content)\n"
		" -> -new <name> (create new file and save it in usr/Medit_saves)\n"
		" -> -edit <path> (edit a text file; if path is null Medit open a new file)\n"
		" After your file's edit type enter, ctrl+d for save file \n"
		" (Medit for now not work properly)\n");
}

void TextMode_t::read_file_content(FILE *fp)
{
	uint32_t bufsize = 4096;
	uint8_t *buf = new uint8_t[bufsize];

	ssize_t len;
	while ((len = fread(buf, 1, bufsize, fp)) > 0) 
	{
		for (ssize_t pos = 0; pos < len; pos++) 
		{
			putc(buf[pos], stdout);
		}
	}
}

void TextMode_t::type_file(FILE *fp)
{
	std::stringstream text;
	std::string line;

	while (true)
	{		
		g_key_info key = g_keyboard::readKey();

		if (key.pressed)
		{
			if (key.ctrl && key.key == "KEY_D")
			{
				text << line;

				save_file(text.str(), fp);
				break;
			}

			if (key.key == "KEY_ENTER")
			{
				text << std::endl;
				text << line;
				line.clear();				

				printf("\n");
			}

			else
				std::cin >> line;
		}
	}
}

void TextMode_t::save_file(std::string text, FILE *fp)
{
	fprintf(stderr, "\n Saving file...\n");
	fwrite(text.c_str(), sizeof(std::string), text.length(), fp);
}

void TextMode_t::close_file(FILE *fp)
{
	fflush(stdout);
	fclose(fp);

	fprintf(stderr, " done!\n");
}