#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include <hid.h>

// **************************************************************
//   The USB data provided by the Saitek X52 Pro
// **************************************************************
//			1		2		4		8		16		32		64		128
// Byte 0	X		X		X		X		X		X		X		X
// Byte 1	X		X		Y		Y		Y		Y		Y		Y
// Byte 2	Y		Y		Y		Y		Padding	Padding	RZ		RZ
// Byte 3	RZ		RZ		RZ		RZ		RZ		RZ		RZ		RZ
// Byte 4	DZ		DZ		DZ		DZ		DZ		DZ		DZ		DZ
// Byte 5	RX		RX		RX		RX		RX		RX		RX		RX
// Byte 6	RY		RY		RY		RY		RY		RY		RY		RY
// Byte 7	S0		S0		S0		S0		S0		S0		S0		S0
// Byte 8	Button	Button	Button	Button	Button	Button	Button	Button
// Byte 9	Button	Button	Button	Button	Button	Button	Button	Button
// Byte 10	Button	Button	Button	Button	Button	Button	Button	Button
// Byte 11	Button	Button	Button	Button	Button	Button	Button	Button
// Byte 12	Button	Button	Button	Button	Button	Button	Button	Padding
// Byte 13	Padding	Padding	Padding	Padding	Hat		Hat		Hat		Hat
// Byte 14	moveR/L	moveR/L	moveR/L	moveR/L	moveu/d	moveu/d	moveu/d	moveu/d


struct SaitekGamePadEventData
{
	uint8_t b1;
	uint8_t b2;
	uint8_t b3;
	uint8_t b4;
	uint8_t b5;
	uint8_t b6;
	uint8_t b7;
	uint8_t b8;
	uint8_t b9;
	uint8_t b10;
	uint8_t b11;
	uint8_t b12;
	uint8_t b13;
	uint8_t b14;
	uint8_t b15;
};

class SaitekJoystickEvents
{
public:
	virtual void OnGamePadChanged(const SaitekGamePadEventData *evt);
};

#define SAITEK_RPT_GAMEPAD_LEN	sizeof(SaitekGamePadEventData)/sizeof(uint8_t)

class SaitekJoystickReportParser : public HIDReportParser
{
	SaitekJoystickEvents		*joyEvents;

	uint8_t oldPad[SAITEK_RPT_GAMEPAD_LEN];

public:
	SaitekJoystickReportParser(SaitekJoystickEvents *evt);

	virtual void Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

#endif // __HIDJOYSTICKRPTPARSER_H__
