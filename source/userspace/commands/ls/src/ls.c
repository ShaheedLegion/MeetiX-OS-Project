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
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

/*
 * output mode
 */
enum OutputMode_t
{
	BASE,
	ALL,
};

/*
 * print the node with mode and type
 */
void printNode(FsDirectoryEntry *node, enum OutputMode_t mode)
{
	// if we havent mode immediatly return
	if (node->name[0] == '.' && mode == BASE) return;

	// if node isn't node
	if (node->type == FS_NODE_TYPE_NONE)
	{
		// create type from mode
		char strtype[7];
		strcpy(strtype, mode == BASE ? "n/" : "<none>");

		// print node
		println("%10s, %10s", strtype, node->name);
	}

	// if node is root
	else if (node->type == FS_NODE_TYPE_ROOT)
	{
		// create type from mode
		char strtype[7];
		strcpy(strtype, mode == BASE ? "rt" : "<root>");

		// print node
		println("%10s, %10s", strtype, node->name);
	}

	// if node is a mountpoint
	else if (node->type == FS_NODE_TYPE_MOUNTPOINT)
	{
		// create type from mode
		char strtype[13];
		strcpy(strtype, mode == BASE ? "mnt" : "<mountpoint>");

		// print node
		println("%10s, %10s", strtype, node->name);
	}

	// if node is directory
	else if (node->type == FS_NODE_TYPE_FOLDER)
	{
		// create type from mode
		char strtype[6];
		strcpy(strtype, mode == BASE ? ">" : "<dir>");

		// print node
		println("%10s %10s", strtype, node->name);
	}

	// if node is file
	else if (node->type == FS_NODE_TYPE_FILE)
	{
		// create type from mode
		char strtype[7];
		strcpy(strtype, mode == BASE ? "  " : "<file>");

		// get size of file
		Fd file = OpenF(node->name, O_RDONLY);
		uint32_t filelen = Length(file);
		Close(file);

		// create multipler
		char mult = ' ';
		if (filelen > 1024) mult = 'K';
		else if (filelen > 1048576) mult = 'M';

		// print the node
		println("%10s %10s %d%cB", strtype, node->name, filelen, mult);
	}

	// if node is pipe
	else if (node->type == FS_NODE_TYPE_PIPE)
	{
		// create type from mode
		char strtype[7];
		strcpy(strtype, mode == BASE ? "pp" : "<pipe>");

		// print node
		println("%10s, %10s", strtype, node->name);
	}
}

/*
 * ls utils command
 */
int main(int argc, char *argv[])
{
	// set option to base
	enum OutputMode_t mode = BASE;

	// flag for scandir
	bool current = true;
	int index;

	// create buffer
	char scandir[PATH_MAX];

	// only if there are arguments
	if (argc > 2)
	{
		// parse all arguments
		for (int arg = 0; arg < argc; arg++)
		{
			if (!strcmp(argv[arg], "-a")) mode = ALL;
			else
			{
				current = false;
				index = arg;
			}
		}
	}

	// from parser copy on buffer the directory to scan
	if (current) GetWorkingDirectory(scandir);
	else strcpy(scandir, argv[index]);

	// open the directory
	FsOpenDirectoryStatus stat;
	FsDirectoryIterator *it = OpenDirectoryS(scandir, &stat);

	// check for errors
	if (stat != FS_OPEN_DIRECTORY_SUCCESSFUL)
	{
		fprintf(stderr, "failed to open directory %s\n", scandir);
		return 1;
	}

	// read all nodes
	while (true)
	{
		// read each node
		FsReadDirectoryStatus stat;
		FsDirectoryEntry *node = ReadDirectoryS(it, &stat);

		// node readed succesful
		if (stat == FS_READ_DIRECTORY_SUCCESSFUL)
		{
			if (node) printNode(node, mode);
			else break;
		}

		// reached the end of directory
		else if (stat == FS_READ_DIRECTORY_EOD) break;
	}

	// free memory
	CloseDirectory(it);

	return 0;
}
