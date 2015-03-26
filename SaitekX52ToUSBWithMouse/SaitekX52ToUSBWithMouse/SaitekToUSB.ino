/* Simplified Logitech Extreme 3D Pro Joystick Report Parser */

#include <hid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <math.h>

#include "saitek_rptparser.h"

// #include <SaitekToUSB.h>
// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

enum modeTransition_t { 
	ONETOTWO, 
	TWOTOTHREE, 
	THREETOTWO, 
	TWOTOONE, 
	NONE };

modeTransition_t modetransition;

USB                                             Usb;
USBHub                                          Hub(&Usb);
HIDUniversal                                    Hid(&Usb);
JoystickEvents                                  JoyEvents;
JoystickReportParser                            Joy(&JoyEvents);

static const int flagbit1 = 0x1;
static const int flagbit2 = 0x2;
static const int flagbit3 = 0x4;
static const int flagbit4 = 0x8;
static const int flagbit5 = 0x10;
static const int flagbit6 = 0x20;
static const int flagbit7 = 0x40;
static const int flagbit8 = 0x80;

static const uint8_t hat0 = 0x10;
static const uint8_t hat45 = 0x20;
static const uint8_t hat90 = 0x30;
static const uint8_t hat135 = 0x40;
static const uint8_t hat180 = 0x50;
static const uint8_t hat225 = 0x60;
static const uint8_t hat270 = 0x70;
static const uint8_t hat315 = 0x80;

int oldMode = 1;
int newMode;

int angle = 0;

void setup()
{
	Serial.begin(115200);
	//while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
	delay(2000);
	
	Serial.println("Start");

	if (Usb.Init() == -1)
		Serial.println("OSC did not start.");

	delay(200);

	if (!Hid.SetReportParser(0, &Joy))
		ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);

	// Mouse.begin();

	Serial.print("Here ");
}

void loop()
{
	Usb.Task();
}

inline uint16_t getX(const GamePadEventData *evt)
{
	uint8_t x1 = evt->b1;
	uint8_t x2 = evt->b2;
	uint16_t x = ((uint16_t)x2 << 8) | x1;
	x = x & 0x3FF;
	return x;
}

inline uint16_t getY(const GamePadEventData *evt)
{
	uint8_t y1 = (evt->b2 >> 2) << 2;
	uint8_t y2 = evt->b3 & 0xF;
	uint16_t y = ((uint16_t)y2 << 8) | y1;
	y = y >> 2;
	return y;
}

inline uint16_t getZ(const GamePadEventData *evt)
{
	uint8_t r1 = (evt->b3 >> 6) << 6;
	uint8_t r2 = evt->b4;
	uint16_t rotate = (((uint16_t)r2 << 8) | r1);
	rotate = rotate >> 6;
	return rotate;
}

inline uint8_t getXRotate(const GamePadEventData *evt)
{
	return evt->b5;
}

inline uint8_t getYRotate(const GamePadEventData *evt)
{
	uint8_t moveud = evt->b15;
	moveud = moveud >> 4;
	return moveud;
}

inline uint8_t getZRotate(const GamePadEventData *evt)
{
	return evt->b15 & 0xF;
}

inline uint8_t getSlider0(const GamePadEventData *evt)
{
	return evt->b6;
}

inline uint8_t getIndexSlider(const GamePadEventData *evt)
{
	return evt->b7;
}

inline uint8_t getSlider1(const GamePadEventData *evt)
{
	return evt->b8;
}


void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt)
{
	Joystick.X(getX(evt));				// Roll
	Joystick.Y(getY(evt));				// Pitch
	Joystick.Z(getZ(evt));				// Yaw

	Joystick.Xrotate(getXRotate(evt));	// Throttle
	
	Joystick.Yrotate(getIndexSlider(evt));	// Thumbstick up/down
	// Joystick.Yrotate(getSlider0(evt));	// Thumbstick up/down
	// Joystick.Zrotate(getZRotate(evt));	// Thumbstick left/right


	Joystick.slider0(getSlider0(evt));	// Thumb trim
	Joystick.slider1(getSlider1(evt));	// Thumb slider



	setButtons(evt);

	setPOV1(evt);						// Joystick pov 1
	setPOV2(evt);						// Joystick pov 2
	setPOV3(evt);						// Throttle pov 3
	// setPOV4(evt);					// Throttle thumbstick pov 4. Use setPOV4 if using the thumbstick as a POV rather than two axes


	// Mouse
	// read and scale the two axes:
	int yrotate = getYRotate(evt);
	int zrotate = getZRotate(evt);
	int xReading = readAxis(getZRotate(evt));
	int yReading = readAxis(getYRotate(evt));
	
	Serial.print("yRotate: ");
	Serial.print(yrotate);
	Serial.println("");
	Serial.print("zRotate: ");
	Serial.print(zrotate);
	Serial.println("");

	Serial.print("xReading: ");
	Serial.print(yrotate);
	Serial.println("");
	Serial.print("zRotate: ");
	Serial.print(yReading);
	Serial.println("");

	// if the mouse control state is active, move the mouse:
	if (yrotate != 8 || zrotate != 8)
	{
		Mouse.begin();
		Mouse.move(xReading, yReading, 0);
		Mouse.end();
	}

	printDebug();
}

inline void setButtons(const GamePadEventData *evt)
{
	/******************** Joystick buttons ****************************************************/
	Joystick.button(1, evt->b9 & flagbit1 == flagbit1);			// Main trigger
	Joystick.button(2, (evt->b9 & flagbit2) == flagbit2);		// Safety button
	Joystick.button(3, (evt->b9 & flagbit3) == flagbit3);		// button "A"
	Joystick.button(4, (evt->b9 & flagbit4) == flagbit4);		// button "B"
	Joystick.button(5, (evt->b9 & flagbit5) == flagbit5);		// button "C"
	Joystick.button(6, (evt->b9 & flagbit6) == flagbit6);		// Pinkie switch
	setModeTransition(evt);										// Buttons 7 and 8 are mapped to state transitions of the mode switch
	Joystick.button(9, (evt->b10 & flagbit1) == flagbit1);		// Base Button 1 Up
	Joystick.button(10, (evt->b10 & flagbit2) == flagbit2);		// Base Button 1 Down
	Joystick.button(11, (evt->b10 & flagbit3) == flagbit3);		// Base Button 2 Up
	Joystick.button(12, (evt->b10 & flagbit4) == flagbit4);		// Base Button 2 Down
	Joystick.button(13, (evt->b10 & flagbit5) == flagbit5);		// Base Button 3 Up
	Joystick.button(14, (evt->b10 & flagbit6) == flagbit6);		// Base Button 3 Down

	/******************** Throttle buttons ****************************************************/
	Joystick.button(15, (evt->b10 & flagbit8) == flagbit8);		// Mouse button
	Joystick.button(16, (evt->b9 & flagbit7) == flagbit7);		// Small Thumb Button "D"
	Joystick.button(17, (evt->b9 & flagbit8) == flagbit8);		// Large index button "E"
	// setIndexTrimButtons(evt);									// Index dial maps to button 18 (clockwise) and button 19 (anticlockwise)
	Joystick.button(20, (evt->b11 & flagbit1) == flagbit1);		// Wheel Down
	Joystick.button(21, (evt->b11 & flagbit2) == flagbit2);		// Wheel Up
	Joystick.button(22, (evt->b11 & flagbit3) == flagbit3);		// Wheel Click
	Joystick.button(23, (evt->b12 & flagbit7) == flagbit7);		// Large thumb Button "i"
	Joystick.button(24, (evt->b12 & flagbit8) == flagbit8);		// Throttle base "function button"
}

inline void setModeTransition(const GamePadEventData *evt)
{
	// Mode 1->2
	// Mode 3->2
	// mode switch
	bool isMode1 = (evt->b12 & flagbit6) == flagbit6;
	bool isMode2 = (evt->b12 & flagbit5) == flagbit5;
	bool isMode3 = (evt->b12 & flagbit4) == flagbit4;

	newMode = GetCurrentMode(oldMode, isMode1, isMode2, isMode3);

	GetModeTransition(oldMode, newMode);

	switch (modetransition)
	{
	case ONETOTWO:
		Joystick.button(7, true);
		break;
	case TWOTOTHREE:
		Joystick.button(8, true);
		break;
	case TWOTOONE:
		Joystick.button(7, true);
		break;
	case THREETOTWO:
	default:
		Joystick.button(7, false);
		Joystick.button(8, false);
		break;
	}

	oldMode = newMode;
}

inline void printDebug()
{
	// Debugging serial print statements
	/*
	Serial.print("X: ");
	Serial.print(x);
	Serial.print(" ");
	PrintBin<uint16_t>(x, 0x10);
	Serial.println("");
	Serial.print(" Y: ");
	Serial.print(y);
	Serial.print(" ");
	PrintBin<uint16_t>(y, 0x10);
	Serial.print(" Rotate: ");
	Serial.print(rotate);
	Serial.print(" Z: ");
	Serial.print(z);
	Serial.print(" Thumb Trim: ");
	Serial.print(thumbTrim);
	Serial.print(" Thumb Slider: ");
	Serial.print(sliderTrim);
	Serial.print(" Move up/down: ");
	Serial.print(moveud);
	Serial.print(" Move up/down: ");
	PrintBin<uint8_t>(moveud, 0x80);
	Serial.print(" Move left/right: ");
	Serial.print(movelr);
	Serial.print(" Move left/right: : ");
	PrintBin<uint8_t>(movelr, 0x80);
	Serial.print(" Index Trim: ");
	Serial.print(indexTrim);
	Serial.print(" modetransition: ");
	Serial.print(modetransition);
	Serial.println("");
	*/
}


//*****************************************************************************/
// POV Mappings 
//*****************************************************************************/

inline void setPOV1(const GamePadEventData *evt)
{
	// POV Hat 1
	uint8_t pov1 = evt->b14;

	if (pov1 == hat0)
	{
		Joystick.hat(4, 0);
	}
	else if (pov1 == hat45)
	{
		Joystick.hat(4, 45);
	}
	else if (pov1 == hat90)
	{
		Joystick.hat(4, 90);
	}
	else if (pov1 == hat135)
	{
		Joystick.hat(4, 135);
	}
	else if (pov1 == hat180)
	{
		Joystick.hat(4, 180);
	}
	else if (pov1 == hat225)
	{
		Joystick.hat(4, 225);
	}
	else if (pov1 == hat270)
	{
		Joystick.hat(4, 270);
	}
	else if (pov1 == hat315)
	{
		Joystick.hat(4, 315);
	}
	else
	{
		Joystick.hat(4, -1);
	}
}

inline void setPOV2(const GamePadEventData *evt)
{
	// TODO Make this do the 45 deg properly
	if ((evt->b11 & flagbit4) == flagbit4)
	{
		Joystick.hat(3, 0);
	}
	else if ((evt->b11 & flagbit5) == flagbit5)
	{
		Joystick.hat(3, 90);
	}
	else if ((evt->b11 & flagbit6) == flagbit6)
	{
		Joystick.hat(3, 180);
	}
	else if ((evt->b11 & flagbit7) == flagbit7)
	{
		Joystick.hat(3, 270);
	}
	else
	{
		Joystick.hat(3, -1);
	}
}

inline void setPOV3(const GamePadEventData *evt)
{
	// Throttle POV
	if ((evt->b11 & flagbit8) == flagbit8)
	{
		Joystick.hat(1, 0);
	}
	else if ((evt->b12 & flagbit1) == flagbit1)
	{
		Joystick.hat(1, 90);
	}
	else if ((evt->b12 & flagbit2) == flagbit2)
	{
		Joystick.hat(1, 180);
	}
	else if ((evt->b12 & flagbit3) == flagbit3)
	{
		Joystick.hat(1, 270);
	}
	else
	{
		Joystick.hat(1, -1);
	}
}

//*****************************************************************************/
// Mode transition functions 
//*****************************************************************************/

int GetCurrentMode(int currentMode, bool mode1, bool mode2, bool mode3)
{
	if (mode1)
	{
		return 1;
	}
	else if (mode2)
	{
		return 2;
	}
	else if (mode3)
	{
		return 3;
	}

	return currentMode;
}

void GetModeTransition(int oldMode, int newMode)
{
	if (oldMode == 1 && newMode == 2)
	{
		modetransition = ONETOTWO;
	}
	else if (oldMode == 2 && newMode == 3)
	{
		modetransition = TWOTOTHREE;
	}
	else if (oldMode == 3 && newMode == 2)
	{
		modetransition = THREETOTWO;
	}
	else if (oldMode == 2 && newMode == 1)
	{
		modetransition = TWOTOONE;
	}
	else
	{
		modetransition = NONE;
	}
}

int readAxis(int thisAxis) {
	
	int center = 8;
	int distance = thisAxis - center;
	return distance;

	/*
	// http://arduino.cc/en/Tutorial/JoystickMouseControl
	// read the analog input:
	int range = 16;
	
	int center = range / 2;         // resting position value
	int threshold = range / 4;      // resting threshold

	// map the reading from the analog input range to the output range:
	int reading = map(thisAxis, 0, 1023, 0, range);

	// if the output reading is outside from the
	// rest position threshold,  use it:
	int distance = reading - center;

	if (abs(distance) < threshold) {
		distance = 0;
	}

	// return the distance for this axis:
	return distance;
	*/
}


/************************************************************************************************/
/* The following functions are only used if setting the thumbstick to a POV rather than two axes */
/************************************************************************************************/
/*
inline void setThumbstickPOV(const GamePadEventData *evt)
{
	// This will map the thumbstick to a POV rather than two axes
	uint8_t tsY = evt->b15 >> 4;
	uint8_t tsX = evt->b15 & 0xF;

	int tsAngle = getAngle(tsX, tsY);

	Joystick.hat(2, tsAngle);
}

inline double to_degrees(double radians) {
	return radians*(180.0 / M_PI);
}

int getAngle(uint8_t tsX, uint8_t tsY)
{
	int tsOx;
	int tsOy;

	if (tsX > 0x8 && tsY < 0x8)
	{
		tsOx = tsX - 0x8;
		tsOy = 0x8 - tsY;

		if (tsOy == 0)
			return 90;

		return to_degrees(atan(tsOx / tsOy));
	}
	else if (tsX >= 0x8 && tsY >= 0x8)
	{
		tsOx = tsX - 0x8;
		tsOy = tsY - 0x8;

		if (tsOy == 0 && tsOy == 0)
			return -1;

		if (tsOx == 0)
			return 180;

		if (tsOy == 0)
			return 90;

		return 90 + to_degrees(atan(tsOy / tsOx));
	}
	else if (tsX < 0x8 && tsY > 0x8)
	{
		tsOx = 0x8 - tsX;
		tsOy = tsY - 0x8;
		
		return 180 + to_degrees(atan(tsOx / tsOy));
	}
	else if (tsX <= 0x8 && tsY <= 0x8)
	{
		tsOx = 0x8 - tsX;
		tsOy = 0x8 - tsY;

		if (tsOy == 0 || tsOy == 0)
				return 270;

		return 360 - to_degrees(atan(tsOx / tsOy));
	}
}
*/

