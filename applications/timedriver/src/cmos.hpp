/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _CMOS_HPP_
#define _CMOS_HPP_

#include <sstream>
#include <stdint.h>
#include <mxuser/utils/time.hpp>
#include <mxuser/utils/utils.hpp>
#include <mxuser/io/files/file_log.hpp>

using namespace std;

/**
 *
 */
struct TIME
{
	size_t second;
	size_t minute;
	size_t hour;
	size_t day;
	size_t month;
	size_t year;
	size_t century;

	uint16_t upSecond;
};


/**
 *
 */
class Cmos_t
{
public:
	/**
	 *
	 */
	Cmos_t() {}

	/**
	 *
	 */
	void readRTC();

	/**
	 *
	 */
	static void timer(Cmos_t *instance);

private:
	/**
	 *
	 */
	int getUpdateInProgressFlag();

	/**
	 *
	 */
	uint8_t getRTCregister(int reg);

	/**
	 *
	 */
	const uint32_t cmosAddress = 0x70;
	const uint32_t cmosData    = 0x71;
};

#endif
