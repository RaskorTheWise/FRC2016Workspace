/*
 * MyJoystick.h
 */
#include "WPILib.h"

#ifndef SRC_MYJOYSTICK_H_
#define SRC_MYJOYSTICK_H_

class MyJoystick {
private:
	Joystick* m_Joystick;
public:
	MyJoystick();
	virtual ~MyJoystick();

	void init(Joystick* theJoystick);

	void readJoystick();

	bool readButton(int buttonNumber);

	float checkLeftStickX();

	float checkLeftStickY();
};

#endif /* SRC_MYJOYSTICK_H_ */
