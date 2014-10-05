#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include <hid.h>

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
