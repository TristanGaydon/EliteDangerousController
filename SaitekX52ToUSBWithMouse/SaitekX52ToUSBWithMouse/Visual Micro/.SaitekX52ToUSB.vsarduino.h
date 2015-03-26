/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Teensy 3.1, Platform=avr, Package=teensy
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define _VMDEBUG 1
#define ARDUINO 105
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 96000000
#define USB_SERIAL_HID
#define LAYOUT_US_ENGLISH
#define __cplusplus
#define __MK20DX256__
#define TEENSYDUINO 118
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

//
//
inline uint16_t getX(const GamePadEventData *evt);
inline uint16_t getY(const GamePadEventData *evt);
inline uint16_t getZ(const GamePadEventData *evt);
inline uint8_t getXRotate(const GamePadEventData *evt);
inline uint8_t getYRotate(const GamePadEventData *evt);
inline uint8_t getZRotate(const GamePadEventData *evt);
inline uint8_t getSlider0(const GamePadEventData *evt);
inline uint8_t getSlider1(const GamePadEventData *evt);
inline void setButtons(const GamePadEventData *evt);
inline void setModeTransition(const GamePadEventData *evt);
inline void setIndexTrimButtons(const GamePadEventData *evt);
inline void printDebug();
inline void setPOV1(const GamePadEventData *evt);
inline void setPOV2(const GamePadEventData *evt);
inline void setPOV3(const GamePadEventData *evt);
int GetCurrentMode(int currentMode, bool mode1, bool mode2, bool mode3);
void GetModeTransition(int oldMode, int newMode);
int GetAxisBucket(uint8_t axisValue, int numberOfBuckets);

#include "C:\Code\Arduino1.0.5\hardware\teensy\cores\teensy3\arduino.h"
#include "C:\Code\EliteDangerousController\SaitekX52ToUSB\SaitekX52ToUSB\SaitekToUSB.ino"
#include "C:\Code\EliteDangerousController\SaitekX52ToUSB\SaitekX52ToUSB\SaitekToUSB.h"
#include "C:\Code\EliteDangerousController\SaitekX52ToUSB\SaitekX52ToUSB\saitek_rptparser.cpp"
#include "C:\Code\EliteDangerousController\SaitekX52ToUSB\SaitekX52ToUSB\saitek_rptparser.h"
#endif
