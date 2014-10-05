#if !defined(__HIDJOYSTICKRPTPARSER2_H__)
#define __HIDJOYSTICKRPTPARSER2_H__

#include <hid.h>

struct ThrustmasterGamePadEventData
{
	uint16_t buttons;
	uint8_t hat;
	uint8_t x1;
	uint8_t x2;
	uint8_t y1;
	uint8_t y2;
	uint8_t twist;
	uint8_t slider;
	uint8_t pad1;
	uint8_t pad2;
	uint8_t pad3;
};


/*
struct GamePadEventData
{
  union { //axes and hut switch
    uint32_t axes;
    struct {
      uint32_t x : 10;
      uint32_t y : 10;
      uint32_t hat : 4;
      uint32_t twist : 8;      
    };
  };
  uint8_t buttons_a;
  uint8_t slider;
  uint8_t buttons_b;
};
*/

class ThrustmasterJoystickEvents
{
public:
	virtual void OnGamePadChanged(const ThrustmasterGamePadEventData *evt);
};

#define THRUSTMASTER_RPT_GAMEPAD_LEN	sizeof(ThrustmasterGamePadEventData)/sizeof(uint8_t)

class ThrustmasterJoystickReportParser : public HIDReportParser
{
	ThrustmasterJoystickEvents		*joyEvents;

	uint8_t oldPad[THRUSTMASTER_RPT_GAMEPAD_LEN];

public:
	ThrustmasterJoystickReportParser(ThrustmasterJoystickEvents *evt);

	virtual void Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

#endif // __HIDJOYSTICKRPTPARSER_H__
