/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]                       *
*                                                                                     *
*         DERIVED FROM THE GHOST OPERATING SYSTEM                                     *
*         This software is derived from the Ghost operating system project,           *
*         written by Max Schlüssel <lokoxe@gmail.com>. Copyright 2012-2017            *
*         https://ghostkernel.org/                                                    *
*         https://github.com/maxdev1/ghost                                            *
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

#ifdef __cplusplus

#ifndef EVA_SHARED_UTILS_LOCAL
#define EVA_SHARED_UTILS_LOCAL

#include "eva/common.h"

/**
 *
 */
template<typename T> class Local 
{
public:

	/**
	 *
	 */
	Local(T *value) : value(value) {}

	/**
	 *
	 */
	Local() : value(0) {}

	/**
	 *
	 */
	~Local() 
	{
		if (value) delete value;
	}

	/**
	 *
	 */
	Local(const Local &rhs) = delete;

	/**
	 *
	 */
	Local &operator=(const Local &rhs) = delete;

	/**
	 *
	 */
	T *operator()() const { return value; }

	/**
	 *
	 */
	T *release() 
	{
		T *releasedValue = value;
		value = 0;
		return releasedValue;
	}

private:
	T *value = 0;
};

#endif

#endif
