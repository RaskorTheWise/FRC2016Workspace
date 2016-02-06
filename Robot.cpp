#include "WPILib.h"
#include "Joystick.h"
#include "MyJoystick.h"

#define USB0 0
#define USB1 1
#define USB2 2

#define PWM0 0
#define PWM1 1
#define PWM2 2
#define PWM3 3
#define PWM4 4
#define PWM5 5
#define PWM6 6
#define PWM7 7
#define PWM8 8
#define PWM9 9

#define DIO0 0
#define DIO1 1
#define DIO2 2
#define DIO3 3
#define DIO4 4
#define DIO5 5
#define DIO6 6
#define DIO7 7
#define DIO8 8
#define DIO9 9

class Robot: public IterativeRobot
{
	RobotDrive perseusDrive;
	Joystick leftStick;
	Joystick rightStick;
	Joystick controller;
	Spark raiseSpark;
	Spark collectSpark;

	MyJoystick* handheld = NULL;

	DigitalInput* dio0;
	DigitalInput* dio1;

	bool raiseLimitSwitch = true;
	bool lowerLimitSwitch = true;

public:
	LiveWindow *lw = LiveWindow::GetInstance();
	SendableChooser *chooser;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
	std::string autoSelected;

	Robot() :
		perseusDrive(PWM0,PWM1),
		leftStick(USB0),
		rightStick(USB1),
		controller(USB2),
		raiseSpark(PWM2),
		collectSpark(PWM3)


	{
		dio0 = new DigitalInput(DIO0);
		dio1 = new DigitalInput(DIO1);

		handheld = new MyJoystick();

		perseusDrive.SetExpiration(0.1);

		chooser = new SendableChooser();
	}

	~Robot()
	{
		delete dio1;
		delete dio0;
		delete handheld;
	}

	void RobotInit()
	{
		chooser->AddDefault(autoNameDefault, (void*)&autoNameDefault);
		chooser->AddObject(autoNameCustom, (void*)&autoNameCustom);
		SmartDashboard::PutData("Auto Modes", chooser);

		raiseLimitSwitch = dio0->Get();
		lowerLimitSwitch = dio1->Get();
	}


	/**
	 * This autonomous (along with the chooser code above) shows how to select between different autonomous modes
	 * using the dashboard. The sendable chooser code works with the Java SmartDashboard. If you prefer the LabVIEW
	 * Dashboard, remove all of the chooser code and uncomment the GetString line to get the auto name from the text box
	 * below the Gyro
	 *
	 * You can add additional auto modes by adding additional comparisons to the if-else structure below with additional strings.
	 * If using the SendableChooser make sure to add them to the chooser code above as well.
	 */
	void AutonomousInit()
	{
		autoSelected = *((std::string*)chooser->GetSelected());
		//std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);
		std::cout << "Auto selected: " << autoSelected << std::endl;

		if(autoSelected == autoNameCustom){
			//Custom Auto goes here
		} else {
			//Default Auto goes here
		}
	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{
		handheld->init(&controller);
	}

	void TeleopPeriodic()
	{
		perseusDrive.TankDrive(leftStick, rightStick);
		handheld->readJoystick();

		RunRaise(handheld->readButton(6));
		RunLower(handheld->readButton(8));
		RunCollect(handheld->readButton(5));
		RunFire(handheld->readButton(7));
	}

	void RunRaise(bool raiseButton)
	{
		if (raiseButton == true)
		{
			while (raiseLimitSwitch != true)
			{
				raiseSpark.Set(0.1); // Will set to negative if necessary
			}
		}
	}

	void RunLower(bool lowerButton)
	{
		if (lowerButton == true)
		{
			while (lowerLimitSwitch != true)
			{
				raiseSpark.Set(-0.1); // Will set to positive if necessary
			}
		}
	}

	void RunCollect(bool collectButton)
	{
		if (collectButton == true)
		{
			collectSpark.Set(-0.1); // Will set to positive if necessary
		}
	}

	void RunFire(bool fireButton)
	{
		if (fireButton == true)
		{
			collectSpark.Set(0.1); // Will set to negative if necessary
		}
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot)
