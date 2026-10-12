/** @file Joypad.cpp
	 Contains implementation of JoypadManagerClass
*/
#include <iostream>
#include <string.h>
#include "Joypad.h"

/** .
	Constructor simply open file of selected joypad, remember to 
	check if all went good. Since this first version of the 
	class was tested only with DualShock all other JoyPad
	are discharded. Remember that even DualShock JoyPad is recognized
	as PS2PAD_TYPE_DIGITAL if analog mode is not enabled.
	@param JoypadNumber 0 mean left port pad and 1 means right port pad
*/
JoypadManager::JoypadManager(int JoypadNumber) : m_AStickTreshold(30) {

	//Open device file.
	PadHandle = open(JoyDev[JoypadNumber], O_RDONLY);
	if (Status() == PS2PAD_STAT_READY) {
	
		m_UpdateType();
		if (m_Type != PS2PAD_TYPE_DUALSHOCK && 
			 m_Type != PS2PAD_TYPE_DIGITAL) 
			Close();
	}
	m_AnalogLocked = false;
	//Reset all the variables of the class, all enum values are set to zero
	//so all the button are in Unpressed state
	memset(m_AnalogStick, 0, sizeof(m_AnalogStick));
	memset(m_Buttons, 0, sizeof(m_Buttons));
}

/** .
	 Cleanup routine:
	 1)try to unlock the pad, routine UnlockAnalogMode automatically
	 check for locked pad.
	 2)Invoke Close() function that release the handle to the pad device
*/
JoypadManager::~JoypadManager() {

	UnlockAnalogMode();
	Close();
}

/** .
	 Update type of the pad, remember that pad can be init in digital mode
	 and then switched to analog mode, so there is the need to recheck type
	 of the pad whenever its state can change
*/
void JoypadManager::m_UpdateType() {

	m_ReadData();
	PadDataType *Data = (PadDataType *) m_PadData;
	m_Type = Data->Type;	
 }

/** .
	Check the status of the pad, remember to check also if the
	handle to the file is correct. If file handle is not valid
	then it is sufficient to return PS2PAD_STAT_ERROR so the 
	user know that some error occurred.
	This function will not return until status of the pad
	is busy
	@return Status of the pad
*/
int JoypadManager::Status() {

	if (PadHandle > 0) {
	
		ioctl(PadHandle, PS2PAD_IOCGETSTAT, &m_Status);
		while(m_Status == PS2PAD_STAT_BUSY) {
		
			ioctl(PadHandle, PS2PAD_IOCGETSTAT, &m_Status);
		} 
	}
	else {
		
		m_Status = PS2PAD_STAT_ERROR;
	}
	return m_Status;
}

/** .
	 This is a simple routine that cycle until status of the pad
	 is ready.
*/
void JoypadManager::WaitUntilBusy() {

	if (PadHandle > 0) {
	
		int Status;
		ioctl(PadHandle, PS2PAD_IOCGETSTAT, &Status);
		while(Status == PS2PAD_STAT_BUSY) {
		
			ioctl(PadHandle, PS2PAD_IOCGETSTAT, &Status);
		} 
	}
}
/** .
    Call internal function m_ReadData that does not only read pad data
	 with read() but update also internal structures to change status of 
	 the buttons.
	 @returns Status of reading data, rememeber that read() function can fail.
*/
bool JoypadManager::ReadPadData() {

	return m_ReadData();
}
/** .
    
*/
int JoypadManager::GetType() {

	return m_Type;	
}

/** .
	 Since analog mode mantains all functionality of digital mode there is
	 no need in making a SetDigitalMode or a SetMode function. Only 
	 SetAnalogMode is useful. 
	 @param AMEnable If true enable analog mode, but if set
	 to fase disable analog mode effectively activating
	 digital mode.
	 @return status of the operation.
*/
bool JoypadManager::SetAnalogMode(bool AMEnable) {	
	
	ps2pad_mode PS2PMode;
	PS2PMode.lock = 1;
	if (AMEnable) 
		PS2PMode.offs = 1;
	else
		PS2PMode.offs = 0;
	ioctl(PadHandle, PS2PAD_IOCSETMODE, &PS2PMode);
	//If pad is in analog mode its type is PS2PAD_TYPE_DIGITAL
	//when analog mode is enabled its type change into PS2PAD_TYPE_DUALSHOCK
	m_UpdateType();
	return true;
}

/** .
	 Remember that if the pad is set to analog mode there is
	 the need to unlock the pad before exiting the class 
	 if not the pad will remain lock. Unlocking of the pad is done
	 automatically in the distructor of the class.
*/
bool JoypadManager::LockAnalogMode() {
	
	m_AnalogLocked = true;
	ps2pad_mode PS2PMode;
	PS2PMode.offs = 1;
	PS2PMode.lock = 3;
	ioctl(PadHandle, PS2PAD_IOCSETMODE, &PS2PMode);
	return true;
}

/** .
    This function does not change the mode of the pad, it is used only to 
	 unlock a previously locked analog mode.
*/
bool JoypadManager::UnlockAnalogMode() {

	if (m_AnalogLocked) {
	
		ps2pad_mode PS2PMode;
		PS2PMode.offs = 1;
		PS2PMode.lock = 2;
		ioctl(PadHandle, PS2PAD_IOCSETMODE, &PS2PMode);	
	}
	return true;
}
/** .
    Before setting buttons state to analog mode there is the need to 
	 check for their support. 
	 @return True if the pad supports analog buttons, false otherwise
*/
bool JoypadManager::AnalogButtonSupported() {

	int Supported = 0;
	if (Status() == PS2PAD_STAT_READY) {

		ioctl(PadHandle, PS2PAD_IOCPRESSMODEINFO, &Supported);
	}
	return Supported == 1;
}

/** .
	Remember that it is possible also to check if actuators are supported
	by the pad.
*/
void JoypadManager::EnableActuators() {

	//Enable actuators
	ps2pad_act PSact;
	memset(PSact.data, -1, sizeof(PSact.data));
	PSact.len = 6;
	PSact.data[0] = 0;
	PSact.data[1] = 1;
	ioctl(PadHandle, PS2PAD_IOCSETACTALIGN, &PSact);

	WaitUntilBusy();
	m_constActuatorStatus	= false;
	m_VariableActuatorValue = 0;
}

/** .
	 Remember to change this functions if some other type of controller 
	 is used
*/
bool JoypadManager::ActuatorsSupported() {

	ps2pad_actinfo PSActInfo;

	//Remember that when actno = -1 term value is unused 
	PSActInfo.actno = -1;
	PSActInfo.term = 0;
	PSActInfo.result = 0;
	ioctl(PadHandle, PS2PAD_IOCACTINFO, &PSActInfo);

	//Since only supported pad is DualShock there must be exactly 2 actuators
	return (PSActInfo.result == 2);
}

/**.
	It is important to remember that is not possibile to handle the two 
	actuators independently, so to set only the constant actuator value of 
	variable actuators must be stored and set, if not actuator vibrations will be
	stopped
	@return Status Boolean value to Enable/Disable the constant actuator
*/
void  JoypadManager::SetActuatorConst(bool Status) {

	//Enable only if pad is of DualShock type
	if (m_Type == PS2PAD_TYPE_DUALSHOCK) {
	
		ps2pad_act PSact;
		PSact.len = 6;
		memset(PSact.data, -1, sizeof(PSact.data));
		if (Status) 
			PSact.data[0] = 1;
		else
			PSact.data[0] = 0;
		//Set back current value of variable actuator
		PSact.data[1] = m_VariableActuatorValue;
		//Update the status of const actuator
		m_constActuatorStatus = Status;
		ioctl(PadHandle, PS2PAD_IOCSETACT, &PSact);
	}
}

/** .
	 Same consideration as for JoypadManager::SetActuatorConst() function 
	 will apply
	 @param Value Intensity of the vibration in range [0,255]
*/
void  JoypadManager::SetActuatorVariable(unsigned char Value) {

	if (m_Type == PS2PAD_TYPE_DUALSHOCK) {
	
		ps2pad_act PSact;
		PSact.len = 6;
		memset(PSact.data, -1, sizeof(PSact.data));
		PSact.data[1] = Value;
		PSact.data[0] = m_constActuatorStatus;
		m_VariableActuatorValue = PSact.data[1];
		ioctl(PadHandle, PS2PAD_IOCSETACT, &PSact);
	}
}
/** .
	 Enable or disable Analog button status
	 @return Result of the operation, the function returns true
				if operation is successful or false if the status
				of the pad is not ready
*/
bool JoypadManager::SetAnalogButtonStatus(bool ABEnable) {

	if (Status() == PS2PAD_STAT_READY) {

		if (ABEnable) 
			ioctl(PadHandle, PS2PAD_IOCENTERPRESSMODE);
		else
			ioctl(PadHandle, PS2PAD_IOCEXITPRESSMODE);
		m_AnalogButtonEnabled = ABEnable;
		return true;
	}
	return false;
}

/** .
	 Simply close the handle to device. 
*/
void JoypadManager::Close() {

	if (PadHandle >= 0) 
		close(PadHandle);
	PadHandle = -1;
}


/** .
	 Before actually read the data status is checked, rememeber
	 that checking status do also a busy loop to check if both
	 handle to joypad is good and joypad is not in busy state.
*/
bool JoypadManager::m_ReadData() {

	if (Status() == PS2PAD_STAT_READY) {

		int ReadBytes = read(PadHandle, m_PadData, PS2PAD_DATASIZE);
		if (ReadBytes > 0) {
			
			//Call member function to analyze data of the pad 
			m_AnalyzeRead();
			return true;
		}
	}
	//If data cannot be read from the pad the user must be informed
	//that something went wrong
	return false;
}

/** .
	 Unbias position of the pad, use threshold to zero value of digital
	 stick if value is less than threshold and finally normalize value
	 to float range -1, 1.
	 @param Value Value to be normalized
*/
float	JoypadManager::m_ReadStickValueX(unsigned char Value) {

	//First of all subtract half the maximum value and then normalize
	//to float range [-1, 1]
	float Unbiased = Value - 127.5;
	if (Unbiased < m_AStickTreshold && Unbiased > -m_AStickTreshold) {
		
		return 0.0f;
	}
	return Unbiased / 127.5f;
}

/** .
	 Unbias position of the pad, use threshold to zero value of digital
	 stick if value is less than threshold and finally normalize value
	 to float range -1, 1.
	 @param Value Value to be normalized
*/
float	JoypadManager::m_ReadStickValueY(unsigned char Value) {

	float Unbiased = Value - 127.5;
	if (Unbiased < m_AStickTreshold && Unbiased > -m_AStickTreshold) {
		
		return 0.0f;
	}
	//Remember that Y axix of analog stick grow from up to bottom, since 
	//in a right coordinate system Y axix point up it is more natural
	//to negate the original value.
	return -Unbiased / 127.5f;
}

/** .
	 This function do the real work and set all button value
	 into Buttons internal structure as well to set analog
	 stick position. It is called after data is read into
	 internal array.
*/
void JoypadManager::m_AnalyzeRead() {

	//First of all analyze digital button state that is contained in third and
	//fourth byte of DataRead. To read data cast raw binary format to 
	//PadDataType structure
	PadDataType *Data = (PadDataType *) m_PadData;
	for (int I = 0; I < 16; ++I) {
	
		int CurStatus = Data->DButtons & (1 << I);
		if (CurStatus) {
			
			//Remember that 1 means button released while 0 means 
			//button pressed so if (Curstatus) == true button is not
			//pressed
			if (m_Buttons[I].State == Pressed || m_Buttons[I].State == MPressed)
				m_Buttons[I].State = Released;
			else
				m_Buttons[I].State = Unpressed;
		}
		else {
		
			if (m_Buttons[I].State == Unpressed || m_Buttons[I].State == Released)
				m_Buttons[I].State = Pressed;
			else
				m_Buttons[I].State = MPressed;
		}
	}

	//Take value of analog stick. Remember to check threshold of the stick
	m_AnalogStick[RX] = m_ReadStickValueX(Data->AnalogSRX);
	m_AnalogStick[RY] = m_ReadStickValueY(Data->AnalogSRY);
	m_AnalogStick[LX] = m_ReadStickValueX(Data->AnalogSLX);
	m_AnalogStick[LY] = m_ReadStickValueY(Data->AnalogSLY);

	//Now take level of pressure for analog button if needed.
	if (m_AnalogButtonEnabled) {
	
		m_Buttons[CIRCLE].PressureLevel		= Data->PadCircle / 255.0f;
		m_Buttons[SQUARE].PressureLevel		= Data->PadSquare / 255.0f;
		m_Buttons[TRIANGLE].PressureLevel	= Data->PadTriangle / 255.0f;
		m_Buttons[CROSS].PressureLevel		= Data->PadCross / 255.0f;
		m_Buttons[R1].PressureLevel			= Data->PadR1 / 255.0f;
		m_Buttons[R2].PressureLevel			= Data->PadR2 / 255.0f;
		m_Buttons[L1].PressureLevel			= Data->PadL1 / 255.0f;
		m_Buttons[L2].PressureLevel			= Data->PadL2 / 255.0f;
		m_Buttons[LEFT].PressureLevel			= Data->PadL / 255.0f;
		m_Buttons[DOWN].PressureLevel			= Data->PadD / 255.0f;
		m_Buttons[RIGHT].PressureLevel		= Data->PadR / 255.0f;
		m_Buttons[UP].PressureLevel			= Data->PadU / 255.0f;
	}
}
