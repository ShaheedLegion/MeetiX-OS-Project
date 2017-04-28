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

#ifndef EVA_SHARED_UTILS_ADDRESS_SPACE_BOUND
#define EVA_SHARED_UTILS_ADDRESS_SPACE_BOUND

#include "memory/address_space.hpp"
#include "kernel.hpp"

/**
 * Safe wrapper for variables that are bound to a specific address space (context).
 * Is used when the kernel operates on multiple address spaces to make sure that
 * page directory switches are done properly and erroneous reads/writes don't occur.
 */
template<typename T> class Contextual 
{
public:
	/**
	 *
	 */
	Contextual() : value(0), space(0) 
	{
	}

	/**
	 *
	 */
	Contextual(T value, PageDirectory space) : value(value), space(space) 
	{
	}

	/**
	 *
	 */
	Contextual(const Contextual &rhs) 
	{
		this->value = rhs.value;
		this->space = rhs.space;
	}

	/**
	 *
	 */
	Contextual &operator=(const Contextual &rhs) 
	{
		this->value = rhs.value;
		this->space = rhs.space;
		return *this;
	}

	/**
	 *
	 */
	T operator()() const 
	{
		if (space != 0) 
		{
			PageDirectory current_space = AddressSpace::getCurrentSpace();
			if (current_space != space) 
			{
				EvaKernel::panic("%! tried to access a value from within another context", "contextual");
			}
		}
		return value;
	}

	/**
	 *
	 */
	void set(T value, PageDirectory space) 
	{
		this->value = value;
		this->space = space;
	}

private:
	T value;
	PageDirectory space;

};

#endif
