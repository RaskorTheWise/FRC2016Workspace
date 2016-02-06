/*
 * MyJoystick.cpp
 */
#include "WPILib.h"
#include <MyJoystick.h>

#define MAXBUTTONS 10

Joystick* m_joystick = NULL;
JoystickButton* buttonArray[MAXBUTTONS];
bool buttonValueArray[MAXBUTTONS];

MyJoystick::MyJoystick() {

}

MyJoystick::~MyJoystick() {
	if( m_joystick != NULL)
	{
		for (int i = 0; i < MAXBUTTONS; i++)
		{
			if( buttonArray[i] != NULL )
			{
				delete buttonArray[i];
				buttonArray[i] = NULL;
			}
		}
//		delete m_joystick;
		m_joystick = NULL;

	}
}

void MyJoystick::init(Joystick* theJoystick)
{
	m_joystick = theJoystick;

	for (int i = 0; i < MAXBUTTONS; i++)
	{
		buttonArray[i] = new JoystickButton(m_joystick, i + 1);
	}
}

void MyJoystick::readJoystick()
{
	// get all the button values
	for (int i = 0; i < MAXBUTTONS; i++)
	{
		buttonValueArray[i] = buttonArray[i]->Get();
	}
}

bool MyJoystick::readButton(int buttonNumber){
	// get the correct button value

	return buttonValueArray[buttonNumber - 1];
}



