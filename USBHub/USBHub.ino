#include <usbhub.h>
#include "pgmstrings.h"
// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <hid.h>
#include <hiduniversal.h>
#include "saitek_rptparser.h"
#include "thrustmaster_rptparser.h"

#define DEVADDR 1
#define CONFVALUE 1


/*
Thrustmaster: Vendor 1103 Product: 45322
Saitek new : Vendor: 2565 Product: 29201
Saitek (original code): Vendor 1699 Product 1980
*/

USB     Usb;
USBHub  Hub1(&Usb);

HIDUniversal Hid1(&Usb);
HIDUniversal Hid2(&Usb);

bool foundSaitek = false;
bool foundThrustmaster = false;
UsbDevice *SaitekUSBDevice;
UsbDevice *ThrustmasterUSBDevice;

byte ThrustmasterAddress;
byte SaitekAddress;

SaitekJoystickEvents                             JoyEvents;
ThrustmasterJoystickEvents                       JoyEvents1;

SaitekJoystickReportParser                       Joy(&JoyEvents);
ThrustmasterJoystickReportParser                 Joy1(&JoyEvents1);

bool joysticksInitialized = false;

uint32_t next_time;

enum modeTransition_t {
	ONETOTWO,
	TWOTOTHREE,
	THREETOTWO,
	TWOTOONE,
	NONE
};

modeTransition_t modetransition;
int SENSOR_BUCKETS = 5;

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

int oldSensorRange = 3;

void setup()
{
	Serial.begin(115200);

	delay(2000);
	// while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
	Serial.println("Start");

	

	if (Usb.Init() == -1)
		Serial.println("OSC did not start.");

	delay(200);

	next_time = millis() + 10000;


	while(!foundThrustmaster || !foundSaitek)
	{
		Usb.Task();
		if (Usb.getUsbTaskState() == USB_STATE_RUNNING)
		{
			if ((millis() - next_time) >= 0L)
			{
				Usb.ForEachUsbDevice(&PrintAllDescriptors);
				next_time = millis() + 10000;
			}
		}
	}

	if (foundThrustmaster && foundSaitek)
	{
		if (!joysticksInitialized)
		{
			Serial.println("Initialising sticks");

			AddressPool &addrPool = Usb.GetAddressPool();

			/*
			UsbDevice *p = addrPool.GetUsbDevicePtr(ThrustmasterAddress);

			if (!p)
			Serial.println("0 USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL");

			UsbDevice *q = addrPool.GetUsbDevicePtr(SaitekAddress);

			if (!q)
			Serial.println("1 USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL");
			*/

			/*
			if (!Hid1.SetReportParser(0, &Joy1))
				ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);

			if (!Hid2.SetReportParser(1, &Joy))
				ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
			*/

			joysticksInitialized = true;

			Serial.print("Hid 1 Vendor ");
			Serial.print(Hid1.VID);
			Serial.print("Hid 1 Product ");
			Serial.print(Hid1.PID);
			Serial.println("");

			Serial.print("Hid 2 Vendor ");
			Serial.print(Hid2.VID);
			Serial.print("Hid 2 Product ");
			Serial.print(Hid2.PID);
			Serial.println("");

			switch (Hid1.VID)
			{
				case 1103:
					Serial.println("Hid 1: Thrustmaster");
					if (!Hid1.SetReportParser(0, &Joy1))
						ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
					break;
				case 1699:
					Serial.println("Hid 1: Saitek");
					if (!Hid1.SetReportParser(0, &Joy))
						ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
					break;
			}

			switch (Hid2.VID)
			{
				case 1103:
					Serial.println("Hid 2: Thrustmaster");
					if (!Hid2.SetReportParser(0, &Joy1))
						ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
					break;
				case 1699:
					Serial.println("Hid 2: Saitek");
					if (!Hid2.SetReportParser(0, &Joy))
						ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
					break;
			}

			Serial.println("Sticks Initialised");
		}
	}
}

byte getdevdescr(byte addr, byte &num_conf);

void PrintDescriptors(uint8_t addr, UsbDevice *pdev)
{
	uint8_t rcode = 0;
	byte num_conf = 0;

	rcode = getdevdescr((byte)addr, num_conf, pdev);
	
	/*
	if (rcode)
	{
		printProgStr(Gen_Error_str);
		print_hex(rcode, 8);
	}
	Serial.print("\r\n");
	*/
}

void PrintAllDescriptors(UsbDevice *pdev)
{
	/*
	Serial.println("\r\n");
	print_hex(pdev->address.devAddress, 8);
	Serial.println("\r\n--");
	*/
	PrintDescriptors(pdev->address.devAddress, pdev);
}

void loop()
{
	Usb.Task();
}

byte getdevdescr(byte addr, byte &num_conf, UsbDevice *pdev)
{
	USB_DEVICE_DESCRIPTOR buf;
	byte rcode;
	rcode = Usb.getDevDescr(addr, 0, 0x12, (uint8_t *)&buf);
	if (rcode) {
		return(rcode);
	}

	// Useful for debugging IDs of attached devices
	/*
	Serial.print("Vendor: ");
	Serial.print(buf.idVendor);
	Serial.print("Product: ");
	Serial.print(buf.idProduct);
	*/

	if (!foundThrustmaster)
	{
		if (buf.idVendor == 0x044F && buf.idProduct == 0xB10A)
		{
			Serial.print("Found thrustmaster on address ");
			print_hex(addr, 8);
			Serial.println("");
			foundThrustmaster = true;
			ThrustmasterUSBDevice = pdev;
			ThrustmasterAddress = addr;
		}
	}

	if (!foundSaitek)
	{
		if (buf.idVendor == 0x06A3 && buf.idProduct == 0x0762)
		{
			Serial.println("Found Saitek on address ");
			print_hex(addr, 8);
			Serial.println("");
			foundSaitek = true;
			SaitekAddress = addr;
		}
	}

	num_conf = buf.bNumConfigurations;
	return(0);
}


/* prints hex numbers with leading zeroes */
// copyright, Peter H Anderson, Baltimore, MD, Nov, '07
// source: http://www.phanderson.com/arduino/arduino_display.html
void print_hex(int v, int num_places)
{
	int mask = 0, n, num_nibbles, digit;

	for (n = 1; n <= num_places; n++) {
		mask = (mask << 1) | 0x0001;
	}
	v = v & mask; // truncate v to specified number of places

	num_nibbles = num_places / 4;
	if ((num_places % 4) != 0) {
		++num_nibbles;
	}
	do {
		digit = ((v >> (num_nibbles - 1) * 4)) & 0x0f;
		Serial.print(digit, HEX);
	} while (--num_nibbles);
}

/* Print a string from Program Memory directly to save RAM */
void printProgStr(const prog_char str[])
{
	char c;
	if (!str) return;
	while ((c = pgm_read_byte(str++)))
		Serial.print(c);
}

void ThrustmasterJoystickEvents::OnGamePadChanged(const ThrustmasterGamePadEventData *evt)
{
	uint8_t y1 = evt->y1;
	uint8_t y2 = evt->y2;
	uint16_t y = ((uint16_t)y2 << 8) | y1;
	y = y << 2;

	uint8_t yshift = y >> 8;

	Serial.print("Thrustmaster Y ");
	PrintBin<uint16_t>(y, 0x80);
	Serial.print(" ");
	PrintBin<uint8_t>(yshift, 0x80);
	Serial.print(" ");
	Serial.print(yshift);
	Serial.println("");

	Joystick.Xrotate(yshift);	// Throttle
	/*
	// Joystick is actually rotated 90 degrees in current setup, so thrust is actually x not y
	uint8_t x1 = evt->x1;
	uint8_t x2 = evt->x2;
	uint16_t x = ((uint16_t)x2 << 8) | x1;
	
	
	// x = x >> 8;

	uint8_t xshift = x >> 6;

	Serial.print("Thrustmaster X ");
	PrintBin<uint16_t>(x, 0x80);
	Serial.print(" ");
	PrintBin<uint8_t>(xshift, 0x80);
	Serial.print(" ");
	Serial.print(xshift);
	Serial.println("");

	Joystick.Xrotate(xshift);	// Throttle
	*/

	/*
	uint8_t y1 = evt->y1;
	uint8_t y2 = evt->y2;
	uint16_t y = ((uint16_t)y2 << 8) | y1;
	y = y << 2;

	Joystick.Yrotate(x);
	Joystick.Zrotate(y);
	*/

	/*
	Serial.print("X:");
	Serial.print(x);
	Serial.print(" X1:");
	Serial.print(x1);
	Serial.print(" X2:");
	Serial.print(x2);
	Serial.println("");

	Serial.print("Y:");
	Serial.print(y);
	Serial.print(" Y1:");
	Serial.print(y1);
	Serial.print(" Y2:");
	Serial.print(y2);
	Serial.println("");


	Serial.print("Buttons: ");
	PrintBin<uint16_t>(evt->buttons, 0x80);
	Serial.print(" Hat: ");
	PrintBin<uint8_t>(evt->hat, 0x80);
	Serial.print(" X1: ");
	PrintBin<uint8_t>(evt->x1, 0x80);
	Serial.print(" X2: ");
	PrintBin<uint8_t>(evt->x2, 0x80);
	Serial.print(" Y1: ");
	PrintBin<uint8_t>(evt->y1, 0x80);
	Serial.print(" Y2: ");
	PrintBin<uint8_t>(evt->y2, 0x80);/*Serial.print(" Y1: ");
	PrintBin<uint8_t>(evt->y1, 0x80);
	Serial.print(" Y2: ");
	PrintBin<uint8_t>(evt->y2, 0x80);
	Serial.print(" Twist: ");
	PrintBin<uint8_t>(evt->twist, 0x80);
	Serial.print(" Slider: ");
	PrintBin<uint8_t>(evt->slider, 0x80);
	Serial.println("");
	*/
}

void SaitekJoystickEvents::OnGamePadChanged(const SaitekGamePadEventData *evt)
{
	// Serial.println("Saitek");

	Joystick.X(getX(evt));				// Roll
	Joystick.Y(getY(evt));				// Pitch
	Joystick.Z(getZ(evt));				// Yaw

	// XRotate (throttle) comes from thrustmaster
	// Joystick.Xrotate(getXRotate(evt));	// Throttle
	Joystick.Yrotate(getYRotate(evt));	// Thumbstick up/down
	Joystick.Zrotate(getZRotate(evt));	// Thumbstick left/right

	Joystick.slider0(getSlider0(evt));	// Thumb trim
	Joystick.slider1(getSlider1(evt));	// Thumb slider

	setButtons(evt);

	setPOV1(evt);						// Joystick pov 1
	setPOV2(evt);						// Joystick pov 2
	setPOV3(evt);						// Throttle pov 3
	// setPOV4(evt);					// Throttle thumbstick pov 4. Use setPOV4 if using the thumbstick as a POV rather than two axes

	printDebug();
}

inline uint16_t getX(const SaitekGamePadEventData *evt)
{
	uint8_t x1 = evt->b1;
	uint8_t x2 = evt->b2;
	uint16_t x = ((uint16_t)x2 << 8) | x1;
	x = x & 0x3FF;
	return x;
}

inline uint16_t getY(const SaitekGamePadEventData *evt)
{
	uint8_t y1 = (evt->b2 >> 2) << 2;
	uint8_t y2 = evt->b3 & 0xF;
	uint16_t y = ((uint16_t)y2 << 8) | y1;
	y = y >> 2;
	return y;
}

inline uint16_t getZ(const SaitekGamePadEventData *evt)
{
	uint8_t r1 = (evt->b3 >> 6) << 6;
	uint8_t r2 = evt->b4;
	uint16_t rotate = (((uint16_t)r2 << 8) | r1);
	rotate = rotate >> 6;
	return rotate;
}

inline uint8_t getXRotate(const SaitekGamePadEventData *evt)
{
	return evt->b5;
}

inline uint8_t getYRotate(const SaitekGamePadEventData *evt)
{
	uint8_t moveud = evt->b15;
	moveud = moveud >> 4;
	return moveud;
}

inline uint8_t getZRotate(const SaitekGamePadEventData *evt)
{
	return evt->b15 & 0xF;
}

inline uint8_t getSlider0(const SaitekGamePadEventData *evt)
{
	return evt->b6;
}

inline uint8_t getSlider1(const SaitekGamePadEventData *evt)
{
	return evt->b8;
}

inline void setButtons(const SaitekGamePadEventData *evt)
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
	setIndexTrimButtons(evt);									// Index dial maps to button 18 (clockwise) and button 19 (anticlockwise)
	Joystick.button(20, (evt->b11 & flagbit1) == flagbit1);		// Wheel Down
	Joystick.button(21, (evt->b11 & flagbit2) == flagbit2);		// Wheel Up
	Joystick.button(22, (evt->b11 & flagbit3) == flagbit3);		// Wheel Click
	Joystick.button(23, (evt->b12 & flagbit7) == flagbit7);		// Large thumb Button "i"
	Joystick.button(24, (evt->b12 & flagbit8) == flagbit8);		// Throttle base "function button"
}

inline void setModeTransition(const SaitekGamePadEventData *evt)
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

inline void setIndexTrimButtons(const SaitekGamePadEventData *evt)
{
	// Index Rotary
	uint16_t indexTrim = ((uint16_t)evt->b7 << 8);

	int newSensorRange = GetAxisBucket(evt->b7, SENSOR_BUCKETS);

	if (newSensorRange > oldSensorRange)
	{
		Joystick.button(19, false);
		Joystick.button(18, true);	// Index Dial Clockwise, increse sensor range
	}
	else if (newSensorRange < oldSensorRange)
	{
		Joystick.button(18, false);
		Joystick.button(19, true);	// Index Dial Clockwise	Anticlockwise, decrease senor range
	}
	else
	{
		Joystick.button(18, false);
		Joystick.button(19, false);
	}

	oldSensorRange = newSensorRange;
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

inline void setPOV1(const SaitekGamePadEventData *evt)
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

inline void setPOV2(const SaitekGamePadEventData *evt)
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

inline void setPOV3(const SaitekGamePadEventData *evt)
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

//************************************************************************************************/
// Index trim axes to bucket
//************************************************************************************************/

int GetAxisBucket(uint8_t axisValue, int numberOfBuckets)
{
	int maxValue = 256;
	int bucketWidth = floor(maxValue / numberOfBuckets);

	for (int bucket = 0; bucket < numberOfBuckets; bucket++)
	{
		if (axisValue < (bucket * bucketWidth) + bucketWidth)
		{
			return bucket;
		}
	}
	// Due to rounding from the use of floor, need to ensure we still make it into the last bucket
	// if at the maximum value
	return numberOfBuckets;
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

