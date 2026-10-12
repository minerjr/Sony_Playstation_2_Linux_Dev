/** @file Joypad.h
*/

#ifndef _JOY_MANAGER__
#define _JOY_MANAGER__

#include <sys/ioctl.h>
#include <linux/ps2/pad.h>
#include <unistd.h>
#include <fcntl.h>

#include "JoypadConst.h"

//!Name of the device corresponding to joypad in port 1
#define JOYPAD_DEV0 "/dev/ps2pad00"
//!Name of the device corresponding to joypad in port 2
#define JOYPAD_DEV1 "/dev/ps2pad10"

static const char * const JoyDev[] = {JOYPAD_DEV0, JOYPAD_DEV1};

/** Structure used to hold button status.
	 Remember that some button can be analog so a pressure value
	 can be associated to them, other button can only have 0 or
	 1 value. Pressure is normalized in range [-1, 1].
*/
typedef struct _tagAnalogButton {

	float			  PressureLevel; //!< Pressure of button
	ButtonState   State;			  //!< Status of the button.
} AnalogButton;

/** Used to easy reading into array of byte read from the pad.
    Pay particular attenction because DButtons is two byte data 
	 and the structure is constructed with bitset, since
	 PS2 is little endian, byte 1 and 2 are swapped so be sure
	 to swap button index in JoypadConst.h
*/
typedef struct _tagPadReadDataType {

	unsigned char			Unused			: 8;
	unsigned char			Status			: 2;
	unsigned char			AsyStatus		: 2;
	unsigned char			Type				: 4;
	unsigned short int	DButtons			: 16;
	unsigned char			AnalogSRX		: 8;
	unsigned char			AnalogSRY		: 8;
	unsigned char			AnalogSLX		: 8;
	unsigned char			AnalogSLY		: 8;
	unsigned char			PadR				: 8;
	unsigned char			PadL				: 8;
	unsigned char			PadU				: 8;
	unsigned char			PadD				: 8;
	unsigned char			PadTriangle		: 8;
	unsigned char			PadCircle		: 8;
	unsigned char			PadCross			: 8;
	unsigned char			PadSquare		: 8;
	unsigned char			PadL1				: 8;
	unsigned char			PadR1				: 8;
	unsigned char			PadL2				: 8;
	unsigned char			PadR2				: 8;
} PadDataType;

/** This class is a Pad manager, helping the user to access
    pad ioctl function and reading data.
	 Array m_Buttons contains all digital and analog buttons
	 as well as digital pad, that is nothing more that a four 
	 button stick. Structure for analog button is used
	 even for digital ones but this is done for sake of
	 simplicity.
*/
class JoypadManager {

	//! Internal handle to joypad device file
	int	PadHandle;
	//! Internal array used to read data from pad.
	unsigned char m_PadData[PS2PAD_DATASIZE];
	//! Internal status flag
	int	m_Status;
	//! Type of controller
	int	m_Type;
	//! Tells if the pad is locked in analog mode
	bool	m_AnalogLocked;
	//! Status of Analog pressing buttons
	bool  m_AnalogButtonEnabled;
	//! Represent treshold level for analog stick
	int	m_AStickTreshold;

	//! Used to read Pad Type
	void  m_UpdateType();
	//! Function used to read data into internal char array
	bool	m_ReadData();
	//! Function used to analyze data read 
	void	m_AnalyzeRead();
	
	//! Status of const actuator
	bool m_constActuatorStatus;
	//! Status of VariableIntensity actuator.
	unsigned char m_VariableActuatorValue;

	//! These represent all button of the pad, to avoid confusion AnalogButton is used
	AnalogButton m_Buttons[16];
   //! This array contains normalized value for position of analog stick
	float			 m_AnalogStick[4];
	//! This read normalized thresholded value of the stick
	float			 m_ReadStickValueX(unsigned char Value);
	//! This read normalized thresholded value of the stick
	float			 m_ReadStickValueY(unsigned char Value);
public:
	
	//! Constructor needs only the ID of the pad to use (0, 1)
	JoypadManager(int PadNumber);
	//! Destructor of the class
	~JoypadManager();
	//! Used to read the status of the pad.
	int  Status();
	//! Wait till joypad is busy
	void WaitUntilBusy();
	//! Used to make joypad read joy data
	bool ReadPadData();
	//! Used to access button status
	const AnalogButton &Button(int Index) {return m_Buttons[Index];}
	//! Used to access AnalogStick status
	float AnalogStick(int Index) {return m_AnalogStick[Index];}
	//! Enable use of DualShock Actuators
	void EnableActuators();
	//! Check for presence of the two actuators
	bool ActuatorsSupported();
	//! Enable disable constant actuator
	void  SetActuatorConst(bool Status);
	//! Enable disable constant actuator
	void  SetActuatorVariable(unsigned char Value);

	//! Get type of the controller.
	int  GetType();
	//! Ask for supporting of analog button
	bool AnalogButtonSupported();
	//! Force analog or digital mode of the pad
	bool SetAnalogMode(bool AMEnable);
	//! Lock analog mode of the pad
	bool LockAnalogMode();
	//! Unlock previus locked pad
	bool UnlockAnalogMode();

	//! Enable or disable Analog button retrieval
	bool SetAnalogButtonStatus(bool ABEnable);
	//! Close the handle to the pad
	void Close();

	//! Providing conversion to void *, to use the class in if () test
	operator void*() {return (void *)(Status() == PS2PAD_STAT_READY);}
};


#endif
