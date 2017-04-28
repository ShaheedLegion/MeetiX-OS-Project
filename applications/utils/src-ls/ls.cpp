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

#include <stdio.h>
#include <eva.h>
#include <string.h>

using namespace std;

enum
{
	BASE,
	ALL
};

void nodeType(int option, int entryType)
{
	switch (entryType)
	{
	case FS_NODE_TYPE_FILE:
		if (!option) printf("   ");

		else printf("<file> ");

		break;

	case FS_NODE_TYPE_FOLDER:
		if (!option)printf(" > ");

		else printf(" <dir> ");

		break;

	default:
		if (!option) printf(" ~ ");

		else printf(" <ukn> ");

		break;
	}
}

/**
 *
 */
int main(int argc, char *argv[]) 
{
	// decide which directory to read
	char *directory;
	char *argument;
	char *path;
	int option = BASE;

	if (argc > 1) 
	{
		argument = argv[1];

		if (!strcmp(argument, "-a"))
		{
			option = ALL;

			if (argc > 2)
			{
				path = argv[2];

				if (strlen(path) < PATH_MAX) directory = path;

				else 
				{
					fprintf(stderr, "given path is not a valid path name\n");

					return 1;
				}
			}

			else 
			{
				directory = new char[PATH_MAX];
				GetWorkingDirectory(directory);
			}
		}

		else
		{
			if (strlen(argument) < PATH_MAX) directory = argument;

			else 
			{
				fprintf(stderr, "given argument is not a valid path name\n");

				return 1;
			}
		}

	} 

	else 
	{
		directory = new char[PATH_MAX];
		GetWorkingDirectory(directory);
	}

	// open the directory
	FsOpenDirectoryStatus stat;
	FsDirectoryIterator *iter = OpenDirectoryS(directory, &stat);

	// check for errors
	if (stat != FS_OPEN_DIRECTORY_SUCCESSFUL) 
	{
		fprintf(stderr, "failed to read directory\n");
		return 1;
	}



	FsDirectoryEntry *entry;
	while (true) 
	{
		FsReadDirectoryStatus rstat;
		entry = ReadDirectoryS(iter, &rstat);
		
		if (rstat == FS_READ_DIRECTORY_SUCCESSFUL) 
		{
			if (entry == 0) break;
			
			else
			{
				// write the tipe of node
				nodeType(option, entry->type);

				// open the file
				Fd file = Open(entry->name);
				
				// get size of file
				float length = (float)Length(file);
				char mlt = ' ';
				if (entry->type != FS_NODE_TYPE_FOLDER && length > 1024)
				{
					length = length / 1024;
					mlt = 'K';

					if (length > 1024)
					{
						length = length / 1024;
						mlt = 'M';
					}
				}

				// print and close
				println(" %-25s %-10f %cB", entry->name, length, mlt);
				Close(file);
				
			}
		} 

		else if (rstat == FS_READ_DIRECTORY_EOD) break;

		else 
		{
			fprintf(stderr, "failed to read directory\n");
			break;
		}
	}

	delete argument;
	delete path;

	delete iter;
	delete directory;
	delete entry;

	CloseDirectory(iter);

}
