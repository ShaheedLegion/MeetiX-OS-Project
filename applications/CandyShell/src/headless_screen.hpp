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

#include "screen.hpp"
#include "color.hpp"

/**
 *
 */
class HeadlessScreen : public Screen
{
private:
	uint32_t id;

	uint8_t *outputBuffer;
	uint8_t *outputCurrent;

	uint32_t offset;

	uint32_t activeProcessId;
	uint8_t lock;
	void normalize();
	void moveCursor(uint16_t x, uint16_t y);

	Color *Setup;

public:
	HeadlessScreen();

	void clean();
	void deactivate();
	void activate();

	void close();

	bool setColor(std::string color);

	void backspace();
	void cleanLine(int lineLength);
	void write(std::string message, ScreenColor color = Color::color(SC_STD_BACK, SC_LGRAY), bool visible = true);
	void writeChar(char c, ScreenColor color = Color::color(SC_STD_BACK, SC_LGRAY));
	void updateCursor();

	KeyInfo readInput(bool* cancelCondition);
};
