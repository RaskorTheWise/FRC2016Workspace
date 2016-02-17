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
	Encoder driveEncoder;

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
		chooser->AddDefault(autoNameDefault, (void*)&autoNameDefault);
		chooser->AddObject(autoNameCustom, (void*)&autoNameCustom);
		SmartDashboard::PutData("Auto Modes", chooser);

//		raiseLimitSwitch = dio0->Get();
//		lowerLimitSwitch = dio1->Get();
		upSensor = dio0->Get();
		downSensor = dio1->Get();

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

	void GetLiftState(Solenoid lift)
	{
		if (lift.Get() == true)
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
		GetLiftState(liftSolenoid);
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
