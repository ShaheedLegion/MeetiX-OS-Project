/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __CANVAS__
#define __CANVAS__

#include <components/component.hpp>

/**
 *
 */
struct BufferInfo 
{
	uint8_t *localMapping;
	uint8_t *remoteMapping;
	uint16_t pages;
	bool acknowledged;
};

/**
 *
 */
class Canvas_t : public Component_t 
{
public:
	Pid partnerProcess;
	Tid partnerThread;

	BufferInfo currentBuffer;
	BufferInfo nextBuffer;

	bool mustCheckAgain;

	Canvas_t(Tid partnerThread);
	virtual ~Canvas_t();

	/*
	 * Components
	 */
	virtual void paint();
	virtual bool handle() { return false; }
	virtual void handleBoundChange(Rectangle oldBounds);

	void createNewBuffer(uint16_t requiredPages);
	void clientHasAcknowledgedCurrentBuffer();
	void requestClientToAcknowledgeNewBuffer();
	void blit();

private:
	void checkBuffer();
};

#endif
