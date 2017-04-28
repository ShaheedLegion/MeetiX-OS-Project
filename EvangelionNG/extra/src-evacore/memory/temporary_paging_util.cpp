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

#include <memory/temporary_paging_util.hpp>
#include <memory/address_space.hpp>
#include <memory/paging.hpp>
#include <memory/constants.hpp>
#include <memory/physical/pp_allocator.hpp>
#include <memory/collections/address_stack.hpp>
#include <kernel.hpp>
#include <logger/logger.hpp>

static AddressStack addressStack;

/**
 *
 */
void TemporaryPagingUtil::initialize() 
{

	uint32_t start = CONST_KERNEL_TEMPORARY_VIRTUAL_RANGES_START;
	uint32_t end = CONST_KERNEL_TEMPORARY_VIRTUAL_ADDRESS_RANGES_END;
	logDebug("%! initializing with range %h to %h", "vtemp", start, end);

	for (VirtualAddress i = start; i < end; i = i + PAGE_SIZE) 
	{
		addressStack.push(i);
	}
}

/**
 *
 */
VirtualAddress TemporaryPagingUtil::map(PhysicalAddress phys) 
{
	VirtualAddress virt = addressStack.pop();

	if (virt == 0) 
	{
		EvaKernel::panic("%! unable to temporary map physical address %h, no free addresses", "vtemp", phys);
	}

	AddressSpace::map(virt, phys, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS);
	return virt;
}

/**
 *
 */
void TemporaryPagingUtil::unmap(VirtualAddress virt) 
{
	AddressSpace::unmap(virt);
	addressStack.push(virt);
}
