/** @file main.cpp
	Dump read stream of Joypad 1.
*/
#include <cstdio>
#include <linux/ps2/pad.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <iomanip>

#include "Joypad.h"

void gotoxy(int x, int y) {

	std::cout << "\E[0" << x << ';' << y << 'H';
}

void clrscr() {

	std::cout << "50\E[H\E[J";
}
/** This simple routine check only digital buttons.
*/
void CheckButton(JoypadManager &Joy) {

	cout << "Checking press of the digital buttons\n";
	//First of all set analog mode for the pad, this is not
	//really necessary because analog functions of the pad
	//Are also active in analog mode.
	Joy.SetAnalogMode(false);
	for (; ;) {
		
		//Make JoypdaManager update data
		if (Joy.ReadPadData()) {
		
			for (int I = 0; I < 0x0010; ++I) {

				//Check for every button a changing of state.
				switch (Joy.Button(I).State) {
				
					case Released:
						cout << "Button " << ButtonNameFromIndex(I) << " Released\n";
					break;
					case Pressed:
						cout << "Button " << ButtonNameFromIndex(I) << " Pressed\n";
					break;
					case MPressed:
					case Unpressed:
					break;
				}
			}
			//check for start pressing to stop the program
			if (Joy.Button(START).State == Pressed) {
			
				cout << "Start pressed, ending program\n";
				return;
			}
		}
		else {

			return;
		}
	}
}

/** This routine check pressing of analog button as well as analog stick.
	 Remember that for analog pressure value is normalized to 0.0f - 1.0f
*/
void CheckAnalogButton(JoypadManager &Joy) {

	cout << "Checking press of the Analog buttons\n";
	//Set pad in analog mode.
	Joy.LockAnalogMode();
	//check if joypad support analog buttons if yes enable analog pressure
	if (Joy.AnalogButtonSupported()) {
	
		Joy.SetAnalogButtonStatus(true);
	}
	else {

		cout << "This pad does not suport analog functions\n";
		return;
	}
	timespec TimeToWait;
	TimeToWait.tv_sec = 0;
	TimeToWait.tv_nsec = 300000000;
	for (; ;) {
		
		if (Joy.ReadPadData()) {
		
			for (int I = 0; I < 0x0010; ++I) {

				switch (Joy.Button(I).State) {
				
					case Pressed:
					case MPressed:
						std::cout << "Button " << ButtonNameFromIndex(I) << " Pression " 
							       << std::setprecision(3) << Joy.Button(I).PressureLevel << "\n";
					break;
					case Released:
					case Unpressed:
					break;
				}
			}
			//Now check for analog stick
			if (Joy.AnalogStick(RX) != 0.0f || Joy.AnalogStick(RY) != 0.0f)
				std::cout << "Analog Stick RIGHT X = " << std::setprecision(3) 
							 << Joy.AnalogStick(RX) << " Y = " << std::setprecision(3)
							 << Joy.AnalogStick(RY) << "\n"; 
			if (Joy.AnalogStick(LX) != 0.0f || Joy.AnalogStick(LY) != 0.0f)
				std::cout << "Analog Stick LEFT X = " << std::setprecision(3) 
							 << Joy.AnalogStick(LX) << " Y = " << std::setprecision(3)
							 << Joy.AnalogStick(LY) << "\n"; 
			//check for start pressing to stop the program
			if (Joy.Button(START).State == Pressed) {
			
				cout << "Start pressed, ending program\n";
				return;
			}
		}
		else {

			return;
		}
		nanosleep(&TimeToWait, NULL);
	}
}
/** .
*/
void CheckDualShock(JoypadManager &Joy) {

	cout << "Checking Actuators of dual shock\n" 
		  << "CROSS: While pressed turn on constant actuator\n"
		  << "R1: Toggle constant actuator on\n"
		  << "R2: Toggle constant actuator off\n"
		  << "CIRCLE: Set intensity of Big actuator.\n";
	Joy.SetAnalogMode(true);
	if (Joy.AnalogButtonSupported()) {
	
		Joy.SetAnalogButtonStatus(true);
	}
	Joy.EnableActuators();
	
	std::cout << "Ready\n";
	for (; ;) {
		
		if (Joy.ReadPadData()) {
		
			if (Joy.Button(CROSS).State == Pressed) {
				
				cout << "Const actuator ON\n";
				Joy.SetActuatorConst(true);
			}
			else if (Joy.Button(CROSS).State == Released) {
			
				cout << "Const actuator OFF\n";
				Joy.SetActuatorConst(false);
			}
			else if (Joy.Button(R1).State == Pressed) {
				
				cout << "Const actuator ON\n";
				Joy.SetActuatorConst(true);
			}
			else if (Joy.Button(R2).State == Pressed) {

				cout << "Const actuator OFF\n";
				Joy.SetActuatorConst(false);
			}
			if (Joy.Button(CIRCLE).State != Unpressed) {
			
				cout << "Vibration value = " 
					  << (unsigned int) (Joy.Button(CIRCLE).PressureLevel * 255) 
					  << std::endl;
				Joy.SetActuatorVariable(
					(unsigned char) (Joy.Button(CIRCLE).PressureLevel * 255));
			}
			//check for start pressing to stop the program
			if (Joy.Button(START).State == Pressed) {
			
				Joy.SetActuatorVariable(0);
				Joy.SetActuatorConst(false);
				cout << "Start pressed, ending program\n";
				return;
			}
		}
		else {

			return;
		}
	}
}
int main() {

	//Open the pad and wait until pad is not busy
	JoypadManager Joy(0);
	if (Joy)  {
	
		std::cout << "Select operation to do and press enter:\n" 
			<< "1: Check digital Buttons\n"
			<< "2: Check Analog Buttons\n"
			<< "3: Dual shock actuators\n";
		int Choose;
		std::cin >> Choose;
		switch (Choose) {
		
			case 1:
				CheckButton(Joy);
			break;
			case 2:
				CheckAnalogButton(Joy);
			break;
			case 3:
				CheckDualShock(Joy);
			break;
		}
	}
	else {
		printf("%p\n", (void *) Joy);
		std::cout << "Unable to access Joypad 0" << std::endl;
	}
}
