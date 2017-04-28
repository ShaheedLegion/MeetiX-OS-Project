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


#include <eva/calls/calls.h>
#include <calls/syscall_handler.hpp>
#include <system/io_ports.hpp>
#include <tasking/tasking.hpp>

/*
 *
 */
SYSCALL_HANDLER(shutdown)
{
	if (currentThread->process->securityLevel == SECURITY_LEVEL_KERNEL)
	{
	  	while (true)
	  	{
		    // Funziona per QEMU e Bochs
	 	    IOports::writeShort(0xB004, 0x2000);
		
		    // Magic shutdown per Bochs e QEMU	   
		    for (const char *s = "Shutdown"; *s; ++s) IOports::writeByte(0x8900, *s);

		      // Magic code per VMWare	 
		    asm volatile ("cli");
		    asm volatile ("hlt");
		}
	}
	return currentThread;
}

/*
 *
 */
SYSCALL_HANDLER(reboot)
{
	if (currentThread->process->securityLevel == SECURITY_LEVEL_KERNEL)
	{
		asm ("cli");
		int good = 0x02;
		
		while ((good & 0x02) != 0)
		{
    		good = IOports::readByte(0x64);  
    		IOports::writeByte(0x64, 0xFE);
		}
		asm volatile ("hlt");
	}
	return currentThread;
}