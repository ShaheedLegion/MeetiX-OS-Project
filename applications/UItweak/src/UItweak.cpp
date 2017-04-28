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

#include <UItweak.hpp>

uint8_t blocker;

/*
 *	Main of tweak
 */
int main(int argc, char *argv[])
{
	// create object to class
	UItweak *tweak = new UItweak();

	// initialize object
	if (tweak->init()) 
	{
		// set semaphore block to true
		blocker = true;

		// call painter of class
		tweak->paintComponents();

		// semaphore the execution of task
		AtomicBlock(&blocker);	
	}

	else
	{
		delete tweak;
		
		Exit(-1);
	}

	delete tweak;

	return 0;
}