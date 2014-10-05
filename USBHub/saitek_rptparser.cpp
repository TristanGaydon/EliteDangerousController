#include "saitek_rptparser.h"

SaitekJoystickReportParser::SaitekJoystickReportParser(SaitekJoystickEvents *evt) :
	joyEvents(evt)
{}

void SaitekJoystickReportParser::Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
	bool match = true;

	// Checking if there are changes in report since the method was last called
	for (uint8_t i = 0; i<SAITEK_RPT_GAMEPAD_LEN; i++) {
		if( buf[i] != oldPad[i] ) {
			match = false;
			break;
		}
  }
  	// Calling Game Pad event handler
	if (!match && joyEvents) {
		joyEvents->OnGamePadChanged((const SaitekGamePadEventData*)buf);

		for (uint8_t i = 0; i<SAITEK_RPT_GAMEPAD_LEN; i++) oldPad[i] = buf[i];
	}
}
