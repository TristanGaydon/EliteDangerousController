/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows 
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef USBjoystick_h_
#define USBjoystick_h_

#if defined(USB_HID) || defined(USB_SERIAL_HID)

#include <inttypes.h>

// C language implementation
#ifdef __cplusplus
extern "C" {
#endif
int usb_joystick_send(void);
extern uint8_t usb_joystick_data[14];
#ifdef __cplusplus
}
#endif

// C++ interface
#ifdef __cplusplus
class usb_joystick_class
{
        public:
        void begin(void) { }
        void end(void) { }

	void button(uint8_t button, bool val) {
		if (--button >= 32) return;

		int index;

		if (button < 8) index = 0;
		else if (button > 7 && button < 16) index = 1;
		else if (button > 15 && button < 24) index = 2;
		else index = 3;
		
		if (val)
		{
			usb_joystick_data[index] |= (1 << (button - 0x8 * index ));
		}
		else
		{
			usb_joystick_data[index] &= ~(1 << (button - 0x8 * index));
		}
		
		if (!manual_mode) usb_joystick_send();
	}

	void X(uint16_t val) {
		if (val > 1023) val = 1023;
		
		usb_joystick_data[4] = usb_joystick_data[4] & 0x0 | (val << 8) >> 8;
		usb_joystick_data[5] = usb_joystick_data[5] & 0xFC | (val >> 8);

		if (!manual_mode) usb_joystick_send();
	}

	void Y(uint16_t val) {
		if (val > 1023) val = 1023;
		
		usb_joystick_data[5] = usb_joystick_data[5] & 0x3 | ((val << 10) >> 8);
		usb_joystick_data[6] = usb_joystick_data[6] & 0xF0 | val >> 6;
		
		if (!manual_mode) usb_joystick_send();
	}
	
	void Z(uint16_t val) {
		if (val > 1023) val = 1023;
		
		usb_joystick_data[6] = usb_joystick_data[6] & 0xF | ((val << 12) >> 8);
		usb_joystick_data[7] = usb_joystick_data[7] & 0xC0 | val >> 4;

		if (!manual_mode) usb_joystick_send();
	}

	void Xrotate(uint8_t val) {
		if (val > 255) val = 255;
		usb_joystick_data[8] = val;
		if (!manual_mode) usb_joystick_send();
	}

	void slider0(uint8_t val) {
		if (val > 255) val = 255;
		usb_joystick_data[9] = val;
		if (!manual_mode) usb_joystick_send();
	}

	void slider1(uint8_t val) {
		if (val > 255) val = 255;
		usb_joystick_data[10] = val;
		if (!manual_mode) usb_joystick_send();
	}

	void Yrotate(uint8_t val) {
		if (val > 255) val = 255;
		usb_joystick_data[11] = val;
		if (!manual_mode) usb_joystick_send();
	}

	void Zrotate(uint8_t val) {
		if (val > 255) val = 255;
		usb_joystick_data[12] = val;
		if (!manual_mode) usb_joystick_send();
	}


	inline int hat(unsigned int num, int angle) {
		uint8_t val = 15;
		if (angle < 0) val = 15;
		else if (angle < 23) val = 0;
		else if (angle < 68) val = 1;
		else if (angle < 113) val = 2;
		else if (angle < 158) val = 3;
		else if (angle < 203) val = 4;
		else if (angle < 245) val = 5;
		else if (angle < 293) val = 6;
		else if (angle < 338) val = 7;
		uint8_t *p = usb_joystick_data;
		switch (num) {
		case 1:
			p[12] = (p[12] & 0xF) | (val << 4); break;
		case 2:
			p[12] = (p[12] & 0xF0) | val; break;
		case 3:
			p[13] = (p[13] & 0xF) | (val << 4); break;
		case 4:
			p[13] = (p[13] & 0xF0) | val; break;
		default:
			return -1;
		}
		if (!manual_mode) usb_joystick_send();

		return val;
	}
	
	void useManualSend(bool mode) {
		manual_mode = mode;
	}
	void send_now(void) {
		usb_joystick_send();
	}
	
	static uint8_t manual_mode;
};
extern usb_joystick_class Joystick;

#endif // __cplusplus

#endif // USB_HID || USB_SERIAL_HID
#endif // USBjoystick_h_

