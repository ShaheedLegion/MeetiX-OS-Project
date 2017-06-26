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
#include <eva.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 *
 */
int main(int argc, char** argv)
{
	if (argc >= 2)
	{
		bool append = false;
		if (argc == 3 && strcmp(argv[2], "-a") == 0) append = true;

		char *filepath = argv[1];
		FILE *file = fopen(filepath, append ? "a" : "w");

		if (file == NULL) fprintf(stderr, "file could not be opened\n");

		else
		{
			uint32_t bufsize = 4096;
			uint8_t *buf = (uint8_t*)malloc(sizeof(uint8_t) * bufsize);

			ssize_t len;
			while ((len = fread(buf, 1, bufsize, stdin)) > 0)
			{
				// TODO write buffered
				for (ssize_t pos = 0; pos < len; pos++) putc(buf[pos], file);
			}

			fclose(file);
		}

	}

	else fprintf(stderr, "usage: write <filename>\n");
}
