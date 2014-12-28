/*
 * G15Interface.h
 *
 *  Created on: Nov 27, 2014
 *      Author: frank
 */

#ifndef G15INTERFACE_H_
#define G15INTERFACE_H_

#include <stdint.h>
#include <hidapi/hidapi.h>

#include <vector>
using std::vector;
#include <string>
using std::string;

#define G15_KEY_READ_LENGTH 9

// Helper macro for defining the support table...
#define DEVICE(name, vendorid, productid, caps) { \
    name, \
    vendorid, \
    productid, \
    caps \
}

// The supported devices list structure...
typedef struct
{
  const char 	*name;			// Name used by the detect code for the device we're detecting against...
  uint32_t 		vendorid;		// USB vendor ID
  uint32_t		productid;		// USB product ID
  uint32_t		caps; 			// capability bitfield, e.g. G15_LCD|G15_KEYS;
} libg15_devices_t;

// Define our error types...
typedef enum
{
	G15_NO_ERROR,
	G15_ERROR_OPENING_USB_DEVICE,
	G15_ERROR_WRITING_USB_DEVICE,
	G15_ERROR_READING_USB_DEVICE,
	G15_ERROR_TIMEOUT,
	G15_ERROR_TRY_AGAIN,
	G15_ERROR_WRITING_PIXMAP,
	G15_ERROR_WRITING_BUFFER,
	G15_ERROR_UNSUPPORTED
} G15_ERROR;


// Define logging levels...
typedef enum
{
	G15_LOG_NONE,
	G15_LOG_INFO,
	G15_LOG_WARN
} G15_LOG_LEVEL;


// Define some constant values.  It's somewhat cleaner using
// a user specified enum value than a #define and doesn't eat
// resources like using a const <foo> value to do it.

// Capability bits.
typedef enum
{
	G15_NONE				= 0,
	G15_LCD 				= 1<<0,
	G15_RGB_LCD				= 1<<1,
	G15_KEYS 				= 1<<2,
	G15_MKEYS				= 1<<3,
	G15_BACKLIGHT_CNTL 		= 1<<4,
	G15_RED_BLUE_BKLT_CNTL  = 1<<5,
	G15_RGB_BKLT_CNTL		= 1<<6,
	G15_CONTRAST_CNTL		= 1<<7,
	G15_DEVICE_5BYTE_RETURN = 1<<8,
	G15_DUAL_ENDPOINT		= 1<<9,
	G19_LCD					= 1<<10,		// Not supported YET- got to get a G19 in hand...
	G15_IS_G13				= 1<<11			// Is a special-case override for the G13 for READ ops and a few other things...
} G15_CAPABILITY_BITS;

// LCD size values...
typedef enum
{
  G15_LCD_OFFSET = 32,
  G15_LCD_HEIGHT = 43,
  G15_LCD_WIDTH = 160
} G15_LCD_SIZES;

// Contrast settings for the LCD
typedef enum
{
  G15_CONTRAST_LOW=0,
  G15_CONTRAST_MEDIUM,
  G15_CONTRAST_HIGH
} G15_CONTRAST;

// Brightness settings for the LCD
typedef enum
{
  G15_BRIGHTNESS_DARK=0,
  G15_BRIGHTNESS_MEDIUM,
  G15_BRIGHTNESS_BRIGHT
} G15_BRIGHTNESS;

// Buffer length.
typedef enum
{
  G15_BUFFER_LEN = 0x03e0
} G15_BUFFER;

// Macro key highlight LED values.
typedef enum
{
  G15_LED_M1 = 1<<3,
  G15_LED_M2 = 1<<2,
  G15_LED_M3 = 1<<1,
  G15_LED_MR = 1<<0
} G15_LED;

typedef enum : uint64_t
{
  G15_KEY_G1  = uint64_t(1)<<0,
  G15_KEY_G2  = uint64_t(1)<<1,
  G15_KEY_G3  = uint64_t(1)<<2,
  G15_KEY_G4  = uint64_t(1)<<3,
  G15_KEY_G5  = uint64_t(1)<<4,
  G15_KEY_G6  = uint64_t(1)<<5,
  G15_KEY_G7  = uint64_t(1)<<6,
  G15_KEY_G8  = uint64_t(1)<<7,
  G15_KEY_G9  = uint64_t(1)<<8,
  G15_KEY_G10 = uint64_t(1)<<9,
  G15_KEY_G11 = uint64_t(1)<<10,
  G15_KEY_G12 = uint64_t(1)<<11,
  G15_KEY_G13 = uint64_t(1)<<12,
  G15_KEY_G14 = uint64_t(1)<<13,
  G15_KEY_G15 = uint64_t(1)<<14,
  G15_KEY_G16 = uint64_t(1)<<15,
  G15_KEY_G17 = uint64_t(1)<<16,
  G15_KEY_G18 = uint64_t(1)<<17,
  G15_KEY_G19 = uint64_t(1)<<18,
  G15_KEY_G20 = uint64_t(1)<<19,
  G15_KEY_G21 = uint64_t(1)<<20,
  G15_KEY_G22 = uint64_t(1)<<21,

  G15_KEY_M1  = uint64_t(1)<<22,
  G15_KEY_M2  = uint64_t(1)<<23,
  G15_KEY_M3  = uint64_t(1)<<24,
  G15_KEY_MR  = uint64_t(1)<<25,

  G15_KEY_L1  = uint64_t(1)<<26,
  G15_KEY_L2  = uint64_t(1)<<27,
  G15_KEY_L3  = uint64_t(1)<<28,
  G15_KEY_L4  = uint64_t(1)<<29,
  G15_KEY_L5  = uint64_t(1)<<30,

  G15_KEY_LIGHT_ON = uint64_t(1)<<31,
  G15_KEY_LIGHT_OFF = uint64_t(1)<<32,

  G15_KEY_PLAY = uint64_t(1)<<33,
  G15_KEY_STOP = uint64_t(1)<<34,
  G15_KEY_REW = uint64_t(1)<<35,
  G15_KEY_FWD = uint64_t(1)<<36,
  G15_KEY_VOL_UP = uint64_t(1)<<37,
  G15_KEY_VOL_DOWN = uint64_t(1)<<38,
} G15_KEY;


// Set aside a vector type for us to get a list of the interfaces
// we've got available on the PC we're running on.
class G15Interface;
typedef vector<G15Interface> G15InterfaceList;

// This is the main class- it handles the interface rules for
// each category of device we have hooked up.  The library
// through this C++ class object, doesn't *CARE* which one
// of the Logitech devices you're dinking with at a given time
// (and shouldn't...the daemon might (but it probably shouldn't
// EITHER...)) so you can have differing feeds going to differing
// devices.  Like the media player going to a Z10 speaker,
// and something like weather going to the keyboard...  It shouldn't
// presume something silly like first device except as a default
// for the tools apps.
class G15Interface
{
public:
	G15Interface();
	virtual ~G15Interface();

	// Static helper method to nab the list of current interfaces
	// that we support....
	static G15InterfaceList getAvailableInterfaces();

	// enable or disable debugging
	static void libg15Debug(G15_LOG_LEVEL level);

	// Initialize the current interface -- If we get an ENODEV reply somewhere
	// along the lines for *ANY* of these methods, you need to drop the instance
	// of this interface like a hot potato and re-call getAvailableInterfaces
	// and re-run init() on whatever you're supposed to be working with.
	int init(void);

	// Are we inited?
	bool isInited(void) { return (_hidDev != NULL); };

	// Reset the device...you'll need/want to discard the instance
	// of this object you're holding at that point because it's
	// not assured it'll be in the same place in the HID dev
	// order once you've DONE this.
	int reset(void);

	// Close down the session to the interface.  Needs re-init if
	// this is called.
	void close(void);

	// Return the detected model string from the "getAvailableInterfaces"
	// call.  This should be the "means" for a caller to know what to
	// re-grab for if it's lost it's device on a hotplug out and back in.
	string deviceModelName(void) { return string(g15_devices[_modelIndex].name); };

	// Return the device's capability bits list, so that a developer
	// can know they're dinking with something that may not have, say
	// the macro LEDs, for example.
	uint32_t getCapabilities(void) { return g15_devices[_modelIndex].caps; };

	// Here's the operations you can *potentially* do to the keyboard.
	// (You may be barred from doing it due to lack of capabilities.)
	int writeMonoPixmapToLCD(unsigned char const *data);
	int setLEDs(unsigned int leds);
	int setLCDBrightness(unsigned int level);
	int setLCDContrast(unsigned int level);
	int setKBBrightness(unsigned int level);
	int setRGBLEDColor(unsigned char r, unsigned char g, unsigned char b);

	/* Please be warned
	* the g15 sends two different usb msgs for each key press
	* but only one of these two is used here. Since we do not want to wait
	* longer than timeout we will return on any msg recieved. in the good
	* case you will get G15_NO_ERROR and ORd keys in pressed_keys
	* in the bad case you will get G15_ERROR_TRY_AGAIN -> try again
	*/
	int getPressedKeys(uint64_t *pressed_keys, unsigned int timeout);


private:
	static const libg15_devices_t 	g15_devices[];
	static G15_LOG_LEVEL 			logLevel;

	uint32_t 			_modelIndex;
	string 				_devPath;
	hid_device			*_hidDev;
	uint8_t				_buf[G15_BUFFER_LEN];

	static int log (FILE *fd, unsigned int level, const char *fmt, ...);
	int grabInputEvents(void);
	int releaseInputEvents(void);
	void setModelIndex(uint32_t modelIndex) { _modelIndex = modelIndex; };
	void setDevPath(const char *path) { _devPath = string(path); };
	void dumpPixmapIntoLCDFormat(unsigned char *lcd_buffer, unsigned char const *data);
	void processKeyEvent9ByteG13(uint64_t *pressed_keys, unsigned char *buffer);
	void processKeyEvent9Byte(uint64_t *pressed_keys, unsigned char *buffer);
	void processKeyEvent5Byte(uint64_t *pressed_keys, unsigned char *buffer);
	void processKeyEvent4Byte(uint64_t *pressed_keys, unsigned char *buffer);
	void processKeyEvent2Byte(uint64_t *pressed_keys, unsigned char *buffer);

	uint8_t ReverseBitsInByte(uint8_t v)
	{
	    return (v * 0x0202020202ULL & 0x010884422010ULL) % 1023;
	}
};


#endif /* G15INTERFACE_H_ */
