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

#include <memory/collections/address_stack.hpp>
#include <memory/kernel_heap.hpp>
#include <logger/logger.hpp>
#include <kernel.hpp>

/**
 *
 */
AddressStack::AddressStack() 
{
	current = 0;
	position = 0;
}

/**
 *
 */
void AddressStack::push(Address address) 
{

	// first ever? create current
	if (current == 0) 
	{
		current = new AddressStackFrame;
	}

	// put address on frame
	current->entries[position++] = address;

	// create new frame when at the end
	if (position == ADDRESS_STACK_FRAME_ENTRIES) 
	{
		current->next = new AddressStackFrame;
		current->next->previous = current;
		current = current->next;
		position = 0;
	}
}

/**
 *
 */
Address AddressStack::pop() 
{

	// if no frames, return nothing
	if (current == 0) 
	{
		return 0;
	}

	// if at foot of frame, try to jump back
	if(position == 0) 
	{

		// return nothing if no previous frame
		if(current->previous == 0) 
		{
			return 0;
		}

		// remove frame
		current = current->previous;
		delete current->next;
		current->next = 0;

		position = ADDRESS_STACK_FRAME_ENTRIES;
	}

	return current->entries[--position];
}
