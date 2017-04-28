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

#include <eva.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

// recorsivity in arguments
bool recursive;

// object to search in file system
string fileName;

// list for recursivity
vector<string> dirList;

/*
 * return path of current directory
 */
string getCurrentDirectory()
{
	// create buffer and do syscall
	char *buffer = new char[PATH_MAX];
	GetWorkingDirectory(buffer);

	// copy buffer to string object
	string dir = buffer;

	// delete buffer
	delete buffer;

	// return string with current dir
	return dir;
}

/*
 * method that search filename in current directory and return path
 */
string checkFilenameInCurrentDirectory(string name, string directory)
{
	// open the directory
	FsOpenDirectoryStatus stat;
	FsDirectoryIterator *iter = OpenDirectoryS(directory.c_str(), &stat);

	// check for open errors
	if (stat != FS_OPEN_DIRECTORY_SUCCESSFUL) 
	{
		cerr << "failed to read directory" << endl;
		return string("");
	}

	// creating pointer to directory descriptor
	FsDirectoryEntry *entry;
	while (true) 
	{
		// read directory
		FsReadDirectoryStatus rstat;
		entry = ReadDirectoryS(iter, &rstat);
		
		// if read stat check into
		if (rstat == FS_READ_DIRECTORY_SUCCESSFUL) 
		{
			// if pointer is null exit
			if (entry == 0) break;
			
			// push to list directory name
			else if (entry->type == FS_NODE_TYPE_FOLDER) dirList.push_back(entry->name);

			// if find name return path
			else if (name == entry->name) return directory;
		} 

		// if we reach end of directory exit
		else if (rstat == FS_READ_DIRECTORY_EOD) break;

		// else show error and exit
		else 
		{
			cerr << "failed to read directory" << endl;
			break;
		}
	}

	// return empty string
	return string("");
}

/*
 * method that search filename from current directory to end of filesystem
 */
string checkFilenameRecursive(string name, string firstDirectory)
{
	// local path to file
	string pathToFile;

	// push to list the first directory
	dirList.push_back(firstDirectory);


	return checkFilenameInCurrentDirectory(name, firstDirectory);
}

/*
 * main
 */
int main(int argc, char *argv[])
{
	// parsing arguments
	for (int index = 0; index < argc; index++)
	{
		// if there are option to argument save
		if (!strcmp("-R", argv[index])) recursive = true;

		// else take argument as filename to search
		else fileName = argv[index];
	}

	// if filename is filled start find
	if (!fileName.empty())
	{
		// if recursive is setted to true use it
		if (recursive) string path = checkFilenameRecursive(fileName, getCurrentDirectory());

		// else use only find in current dir
		else
		{
			string path = checkFilenameInCurrentDirectory(fileName, getCurrentDirectory());
			if (!path.empty()) cout << "File finded in: " << path << endl;
		}
	}

	// else show error and return with error
	else 
	{
		cerr << "Error: no fileName provided";
		return -1;
	}

	return 0;
}