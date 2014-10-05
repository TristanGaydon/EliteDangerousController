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

void PrintAllAddresses(UsbDevice *pdev);
void PrintAddress(uint8_t addr);
//
void PrintDescriptors(uint8_t addr);
void PrintAllDescriptors(UsbDevice *pdev);
//
byte getdevdescr(byte addr, byte &num_conf);
void printhubdescr(uint8_t *descrptr, uint8_t addr);
byte getconfdescr(byte addr, byte conf);
void print_hex(int v, int num_places);
void printconfdescr(uint8_t* descr_ptr);
void printintfdescr(uint8_t* descr_ptr);
void printepdescr(uint8_t* descr_ptr);
void printunkdescr(uint8_t* descr_ptr);
void printProgStr(const prog_char str[]);

#include "C:\Code\Arduino1.0.5\hardware\teensy\cores\teensy3\arduino.h"
#include "C:\Code\Teensy\Sketches\USBHubTest\USBHubTest.ino"
#include "C:\Code\Teensy\Sketches\USBHubTest\pgmstrings.h"
#endif
