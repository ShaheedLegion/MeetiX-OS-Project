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

#ifndef EVA_RAMDISK_RAMDSIK
#define EVA_RAMDISK_RAMDSIK

#include "eva/stdint.h"
#include "eva/ramdisk.h"
#include <ramdisk/ramdisk_entry.hpp>
#include <multiboot/multiboot.hpp>

/**
 * Ramdisk class
 */
class Ramdisk 
{
private:
	RamdiskEntry *firstHeader;
	RamdiskEntry *root;
	uint32_t nextUnusedId = 0;

public:
	/**
	 * Initializes the empty ramdisk. A ramdisk is never deleted, therefore
	 * there is no destructor.
	 */
	Ramdisk();

	/**
	 * Loads the ramdisk by creating actual {RamdiskEntry} objects. Reads the "module"
	 * and interprets it as a ramdisk filesystem
	 *
	 * @param module	the ramdisk multiboot module
	 */
	RamdiskEntry *load(MultibootModule *module);

	/**
	 * Searches in the folder parent for a file/folder with the given name
	 *
	 * @param parent the parent folder to search through
	 * @param childName the name of the child to find
	 * @return the entry if found, else 0
	 */
	RamdiskEntry *findChild(RamdiskEntry *node, const char* name);

	/**
	 * Searches for the entry at the given absolute path
	 *
	 * @param name	name of the entry to find
	 * @return the entry if found, else 0
	 */
	RamdiskEntry *findAbsolute(const char* name);

	/**
	 * Searches for a child with of "node" with the given relative path
	 *
	 * @param node	the parent node
	 * @param path	the relative child path
	 * @return the entry if found, else 0
	 */
	RamdiskEntry *findRelative(RamdiskEntry *node, const char* path);

	/**
	 * Returns the entry with the given "id"
	 *
	 * @param name the name
	 * @return the entry if it exists, else 0
	 */
	RamdiskEntry *findById(uint32_t id);

	/**
	 * Returns the number of children that the entry with "id" has
	 *
	 * @param id	the entries id
	 * @return the number of children
	 */
	uint32_t getChildCount(uint32_t id);

	/**
	 * Returns the child at "index" of the node "id"
	 *
	 * @param id	the parent entries id
	 * @param index	the index of the child
	 * @return the entry if found, else 0
	 */
	RamdiskEntry *getChildAt(uint32_t id, uint32_t index);

	/**
	 * Returns the root.
	 */
	RamdiskEntry *getRoot() const;

	/**
	 *
	 */
	RamdiskEntry *createChild(RamdiskEntry *parent, char* filename);

};

#endif
