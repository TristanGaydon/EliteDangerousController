EliteDangerousController - Saitek X52 To USB remapping
======================================================

This sketch for the Teensy microcontroller remaps the Saitek X52 joystick to a USB HID device as discussed in greater length here http://elite.deftindustries.local.com/hacking-the-saitek-x52-pro

The advantage of this interception and remapping is that it; 

· Enables remapping the throttle thumbstick to either two axes or a pov hat. This is useful as the throttle thumbstick is a good candidate to be used for controlling vertical/lateral thrusters. 

· Enables access to the state of the joystick’s “mode” switch. This is useful as we can use state transitions of the mode button to map to a button press.

· Enables remapping of superfluous axes to buttons. This is useful because within Elite Dangerous the throttle trims and thumb slider don’t really have a natural mapping. By remapping one of these axes to a button press when moving in one direction and another button press when moving in the other direction we can repurpose one of these axes to increase/decrease the sensor range. 

· Enables remapping the second joystick thumb, and throttle index finger pov to true pov hat’s rather than individual buttons. This has the advantage that they will be seen by DirectX as true 8 way hats. 

· By remapping joystick buttons to pov hats, we free up some of the 32 buttons giving us the opportunity to add new physical buttons to the Teensy. 

· Removes the need to have the Saitek SST application installed. 


Included here is the Visual Studio 2013 project for users of Visual Studio 2013 + Visual Micro.

If not using Visual Studio 2013 / Visual Micro, simply compile the SaitekToUSB.ino sketch.

usb_desc.h, usb_desc.c, usb_joystick.h and usb_joystick.c are the modified HID descriptors. When compiling the sketch, you'll need to (temporarily) overwrite the existing files in Arduino\hardware\teensy\cores\teensy3\ with these updated files.

