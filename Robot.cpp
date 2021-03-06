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
	Spark leftSpark;
	Spark rightSpark;
	Spark raiseSpark;
	Spark collectSpark;
	RobotDrive perseusDrive;
	Joystick leftStick;
	Joystick rightStick;
	Joystick controller;

	MyJoystick* handheld = NULL;
	DigitalInput* dio0;
	DigitalInput* dio1;

	bool raiseLimitSwitch = true;
	bool lowerLimitSwitch = true;

public:
	SendableChooser *chooser;
//	NetworkTable *table;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
	std::string autoSelected;

	Robot() :
		leftSpark(PWM0),
		rightSpark(PWM1),
		raiseSpark(PWM2),
		collectSpark(PWM3),
		perseusDrive(leftSpark,rightSpark),
		leftStick(USB0),
		rightStick(USB1),
		controller(USB2)

	{
		dio0 = new DigitalInput(DIO0);
		dio1 = new DigitalInput(DIO1);

		handheld = new MyJoystick();
//		table = NetworkTable::GetTable("GRIP/myContoursReport");

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
		CameraServer::GetInstance()->StartAutomaticCapture("cam0");
//		chooser->AddDefault(autoNameDefault, (void*)&autoNameDefault);
//		chooser->AddObject(autoNameCustom, (void*)&autoNameCustom);
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
//
//		collectSpark.Set(0.5);
//		Wait(0.5);
//		collectSpark.Set(-0.5);
//		Wait(0.5);
//		collectSpark.Set(0.0);
//
//		raiseSpark.Set(0.5);
//		Wait(0.5);
//		raiseSpark.Set(-0.5);
//		Wait(0.5);
//		raiseSpark.Set(0.0);
	}

	void TeleopPeriodic()
	{
		perseusDrive.TankDrive(handheld->checkLeftStickY(), handheld->checkRightStickY());
		handheld->readJoystick();

//		RunRaise_Button(handheld->readButton(6));
//		RunLower_Button(handheld->readButton(8));
		RunCollect(handheld->readButton(7));
		RunFire(handheld->readButton(5));
		StopCollectSpark(handheld->readButton(6));
		RunAim(handheld->checkLeftStickY());
	}

//	void RunRaise_Button(bool raiseButton)
//	{
//		if (raiseButton)
//		{
//			if (!raiseLimitSwitch)
//			{
//				raiseSpark.Set(0.1); // Will set to negative if necessary
//			}
//			raiseSpark.Set(0.0);
//		}
//	}

//	void RunLower_Button(bool lowerButton)
//	{
//		if (lowerButton)
//		{
//			if (!lowerLimitSwitch)
//			{
//				raiseSpark.Set(-0.1); // Will set to positive if necessary
//			}
//			raiseSpark.Set(0.0);
//		}
//	}

	void RunAim(float yAxis)
	{
		int dpadDirection = handheld->getPOV();
		SmartDashboard::PutNumber("dpad Direction", dpadDirection);

		if (dpadDirection >= 315 || dpadDirection <= 45) //UP
		{
			raiseSpark.Set(-0.25);
		}
		else if (dpadDirection <= 225 && dpadDirection >= 135) //DOWN
		{
			raiseSpark.Set(0.25);
		}
		else //STOP
		{
			raiseSpark.Set(0.0);
		}

	}

	void RunCollect(bool collectButton)
	{
		if (collectButton)
		{
			collectSpark.Set(-0.45);
		}
	}

	void RunFire(bool fireButton)
	{
		if (fireButton)
		{
			collectSpark.Set(1.0);
		}
	}

	void StopCollectSpark(bool stopButton)
	{
		if (stopButton)
		{
			collectSpark.Set(0.0);
		}
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
