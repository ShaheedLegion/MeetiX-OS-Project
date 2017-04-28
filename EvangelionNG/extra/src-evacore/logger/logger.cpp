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

#include <logger/logger.hpp>
#include <system/smp/global_recursive_lock.hpp>
#include <utils/string.hpp>
#include <video/console_video.hpp>

#include <system/system.hpp>
#include <tasking/tasking.hpp>

static GlobalRecursiveLock printLock;

/**
 *
 */
void Logger::print(const char* message, ...) 
{
	printLock.lock();
	va_list valist;
	va_start(valist, message);
	Logger::printFormatted(message, valist);
	va_end(valist);
	printLock.unlock();
}

/**
 *
 */
void Logger::println(const char* message, ...) 
{
	printLock.lock();
	va_list valist;
	va_start(valist, message);
	Logger::printFormatted(message, valist);
	va_end(valist);
	Logger::printCharacter('\n');
	printLock.unlock();
}

/**
 *
 */
void Logger::manualLock() 
{
	printLock.lock();
}

/**
 *
 */
void Logger::manualUnlock() 
{
	printLock.unlock();
}

