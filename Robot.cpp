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
	VictorSP leftDriveVictor;
	VictorSP rightDriveVictor;
	VictorSP raiseVictor;
	VictorSP fireVictor;
	RobotDrive perseusDrive;
	Joystick leftStick;
	Joystick rightStick;
	Joystick controller;
	Solenoid liftSolenoid;
//	Encoder driveEncoder;

	MyJoystick* handheld = NULL;
	DigitalInput* dio0;
	DigitalInput* dio1;

//	bool raiseLimitSwitch;
//	bool lowerLimitSwitch;

	bool upSensor = true;
	bool downSensor = true;

	enum {
		Up,
		Down,
		None
	} armState = None;

	enum {
		Activated,
		Deactivated
	} liftState = Deactivated;

	enum {
		Step0, //Do nothing
		Step1, //go under low bar -> line of sight of angled high goal
		Step2, //rotating toward goal
		Step3, //moving closer to goal
		Step4, //aiming up
		Step5, //firing
		Done   //finished with auto
	} autoStep = Step0;

public:
	SendableChooser *chooser;
//	NetworkTable *table;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
	std::string autoSelected;

	Robot() :
		leftDriveVictor(PWM0),
		rightDriveVictor(PWM1),
		raiseVictor(PWM2),
		fireVictor(PWM3),
		perseusDrive(leftDriveVictor,rightDriveVictor),
		leftStick(USB0),
		rightStick(USB1),
		controller(USB2),
		liftSolenoid(DIO2) //--------------------------- Placeholder Values
//		driveEncoder()  //---------------------|

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

//		raiseLimitSwitch = dio0->Get();
//		lowerLimitSwitch = dio1->Get();
		upSensor = dio0->Get();
		downSensor = dio1->Get();

	}


	void AutonomousInit()
	{
		autoStep = Step0;
	}

	void AutonomousPeriodic()
	{
		if (autoStep == Step0)
		{
			autoStep = Step1;
		}
		else if (autoStep == Step1)
		{
			//Set motors to drive forward for a while. (Needs encoder values? Or try using Wait().)
			autoStep = Step2;
		}
		else if (autoStep == Step2)
		{
			//Set left to move forward and right to move backward... for a while. (Use Gyro values? Or try using Wait()...)
			autoStep = Step3;
		}
		else if (autoStep == Step3)
		{
			//Move forward for less of a while. (Encoder or Wait())
			autoStep = Step4;
		}
		else if (autoStep == Step4)
		{
			//Lift arms up. Use sensors.
			RunAutoAim(upSensor);
			if (!upSensor)
			{
				autoStep = Step5;
			}
		}
		else if (autoStep == Step5)
		{
			//Fire ball into goal. Once. Use Wait()
			fireVictor.Set(1.0);
			Wait(0.75);
			fireVictor.Set(0.0);
			autoStep = Done;
		}
		else
		{
			//Set everything to 0.
		}
	}

	void RunAutoAim(bool upSensor)
	{
		if (upSensor)
		{
//			'hunubnhunyhuhn';
			raiseVictor.Set(0.5);
		}
		else
		{
			raiseVictor.Set(0.0);
		}
	}

	void TeleopInit()
	{
		handheld->init(&controller);
//
//		fireVictor.Set(0.5);
//		Wait(0.5);
//		fireVictor.Set(-0.5);
//		Wait(0.5);
//		fireVictor.Set(0.0);
//
//		raiseVictor.Set(0.5);
//		Wait(0.5);
//		raiseVictor.Set(-0.5);
//		Wait(0.5);
//		raiseVictor.Set(0.0);
	}

	void TeleopPeriodic()
	{
		perseusDrive.TankDrive(leftStick, rightStick);
		handheld->readJoystick();

//		RunRaise_Button(handheld->readButton(6));
//		RunLower_Button(handheld->readButton(8));
		RunCollect(handheld->readButton(7));
		RunFire(handheld->readButton(5));
		StopFireVictors(handheld->readButton(6));
		RunAim(handheld->checkLeftStickY(), dio0->Get(), dio1->Get());
		RunLift(handheld->readButton(2));
		CheckHallSensor(handheld->readButton(1));
	}

//	void RunRaise_Button(bool raiseButton)
//	{
//		if (raiseButton)
//		{
//			if (!raiseLimitSwitch)
//			{
//				raiseVictor.Set(0.1); // Will set to negative if necessary
//			}
//			raiseVictor.Set(0.0);
//		}
//	}

//	void RunLower_Button(bool lowerButton)
//	{
//		if (lowerButton)
//		{



//			if (!lowerLimitSwitch)
//			{
//				raiseVictor.Set(-0.1); // Will set to positive if necessary
//			}
//			raiseVictor.Set(0.0);
//		}
//	}

	void RunAim(bool aimButton, bool upSensor, bool downSensor)
	{
		GetArmState(upSensor, downSensor);
		if (aimButton)
		{
			if (armState == None || armState == Down)
			{
				if (upSensor)
				{
					raiseVictor.Set(0.5);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
			else if (armState == Up)
			{
				if (downSensor)
				{
					raiseVictor.Set(-0.5);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
		}
	}

	void RunCollect(bool collectButton)
	{
		if (collectButton)
		{
			fireVictor.Set(-0.45);
		}
		else
		{
			fireVictor.Set(0.0);
		}
	}

	void RunFire(bool fireButton)
	{
		if (fireButton)
		{
			fireVictor.Set(1.0);
		}
		else
		{
			fireVictor.Set(0.0);
		}
	}

	void StopFireVictors(bool stopButton)
	{
		if (stopButton)
		{
			fireVictor.Set(0.0);
		}
	}

	void CheckHallSensor(bool checkButton)
	{
		if (checkButton)
		{
			upSensor = dio0->Get();
			SmartDashboard::PutBoolean("Up Sensor Value", upSensor);
			downSensor = dio0->Get();
			SmartDashboard::PutBoolean("Down Sensor Value", downSensor);
		}
	}

	void GetArmState(bool upSensor, bool downSensor)
	{
		if (!upSensor)
		{
			armState = Up;
		}
		else if (!downSensor)
		{
			armState = Down;
		}
		else
		{
			armState = None;
		}

	}

	void GetLiftState()
	{
		if (liftSolenoid.Get() == true)
		{
			liftState = Activated;
		}
		else
		{
			liftState = Deactivated;
		}
	}

	void RunLift(bool liftButton)
	{
		GetLiftState();
		if (liftButton && liftState == Activated)
		{
			liftSolenoid.Set(false);
		}
		else if (liftButton && liftState == Deactivated)
		{
			liftSolenoid.Set(true);
		}
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
