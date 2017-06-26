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

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <eva.h>

/*
 * copy tool command
 */
int main(int argc, char *argv[])
{
    // if are provided all arguments
    if (argc > 2)
    {
        char *pathsrc = argv[1];
        char *pathdest = argv[2];

        // open the file
        FILE *tocopy = fopen(pathsrc, "rb");
        if (tocopy == NULL)
        {
            fprintf(stderr, "file %s not found", pathsrc);
            return 1;
        }

        // create the destination file
        FILE *dest = fopen(pathdest, "w");

        // create a buffer with size of file
        uint32_t buffsize = Length(fileno(tocopy));
        uint8_t *buffer = malloc(sizeof(uint8_t) * buffsize);

        // read and write
        ssize_t length;
        while ((length = fread(buffer, 1, buffsize, tocopy)) > 0);

        // write on new file
        if (fwrite(buffer, 1, buffsize, dest) != buffsize)
        {
            fprintf(stderr, "an error was occoured while writing file %s", pathdest);
            free(buffer);
            return 1;
        }

        // free memory
        free(buffer);
    }

    else
    {
        fprintf(stderr, "usage: %s </path/to/filetocopy> </path/to/destination>\n", argv[0]);
        return 1;
    }

    return 0;
}
