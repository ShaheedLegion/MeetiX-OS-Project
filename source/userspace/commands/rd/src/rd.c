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
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <eva.h>

/*
 * read utils
 */
int main(int argc, const char *argv[])
{
    // only if there are arguments
    if (argc >= 2)
    {
        // create format
        char format[] = "%s";
        if (!strcmp(argv[1], "-x") || !strcmp(argv[2], "-x")) strcpy(format, "%x");

        // open the file (try with first argument)
        Fd file = OpenF(argv[1], O_RDONLY);
        if (file == -1)
        {
            // if is not the first try width second
            file = OpenF(argv[2], O_RDONLY);
            if (file == -1)
            {
                fprintf(stderr, "unable to open the file");
                return 1;
            }
        }

        // allocate buffer
        uint32_t length = Length(file);
        uint8_t buffer[length];

        // read the file
        if (Read(file, buffer, length) < length)
        {
            fprintf(stderr, "unable to read all the file");
            return 1;
        }

        // print on screen
        printf(format, buffer);
    }

    else
    {
        fprintf(stderr, "no argument provided");
        return 1;
    }
    return 0;
}
