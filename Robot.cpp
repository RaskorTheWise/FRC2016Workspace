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

#define PCH0 0

#define ANALOG0 0

#define PULSEIN -0.03865131578
#define PULSECM -0.09815789473

class Robot: public IterativeRobot
{
	Talon leftDriveTalon;
	Talon rightDriveTalon;
	VictorSP raiseVictor;
	Talon leftFireTalon;
	VictorSP rightFireVictor;
	VictorSP triggerVictor;
	VictorSP theArouser1;
	VictorSP theArouser2;
	RobotDrive theseusDrive;
	Joystick leftStick;
	Joystick rightStick;
	Joystick controller;
	Solenoid liftSolenoid;
	AnalogGyro gyro;
	Encoder driveEncoder;

	MyJoystick* handheld = NULL;
	DigitalInput* dio1;
	DigitalInput* dio2;
	DigitalInput* dio3;

//	bool raiseLimitSwitch;
//	bool lowerLimitSwitch;

	bool upHallSensor = true;
	bool downHallSensor = true;
	bool middleHallSensor = true;

	float gyroAngle;

	enum {
		Up,
		Down,
		Middle,
		None
	} armState = None;

	enum {
		Activated,
		Deactivated
	} liftState = Deactivated;

	enum {
		Zero,
		Positive,
		Negative
	} gyroState = Zero;;

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
	std::shared_ptr<NetworkTable> table;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
	std::string autoSelected;

	Robot() :
		leftDriveTalon(PWM2),
		rightDriveTalon(PWM3),
		raiseVictor(PWM4),
		leftFireTalon(PWM1),
		rightFireVictor(PWM5),
		triggerVictor(PWM6),
		theArouser1(PWM0),
		theArouser2(PWM7),
		theseusDrive(leftDriveTalon,rightDriveTalon),
		leftStick(USB0),
		rightStick(USB1),
		controller(USB2),
		liftSolenoid(PCH0),
		gyro(ANALOG0),
		driveEncoder(DIO8, DIO9)

	{
		dio1 = new DigitalInput(DIO1);
		dio2 = new DigitalInput(DIO2);
		dio3 = new DigitalInput(DIO3);

		gyroAngle = 0;

		handheld = new MyJoystick();
		table = NetworkTable::GetTable("GRIP/myContoursReport");

		theseusDrive.SetExpiration(0.1);

		chooser = new SendableChooser();
	}

	~Robot()
	{
		delete dio1;
		delete dio2;
		delete dio3;
		delete handheld;
	}

	void RobotInit()
	{
		CameraServer::GetInstance()->StartAutomaticCapture("cam0");

//		raiseLimitSwitch = dio0->Get();
//		lowerLimitSwitch = dio1->Get();
		upHallSensor = dio1->Get();
		downHallSensor = dio3->Get();
		middleHallSensor = dio2->Get();

		gyro.Reset();
		driveEncoder.Reset();

		driveEncoder.SetDistancePerPulse(PULSEIN);
	}


	void AutonomousInit()
	{
		autoStep = Step0;
		gyro.Reset();
		gyroAngle = gyro.GetAngle();
	}

	void AutoResetEncoders()
	{
		gyro.Reset();
		driveEncoder.Reset();
	}
	void AutonomousPeriodic()

	//Basic Frame Auto
//	{
//		if (autoStep == Step0)
//		{
//			autoStep = Step1;
//		}
//		else if (autoStep == Step1)
//		{
//			//Set motors to drive forward for a while. (Needs encoder values? Or try using Wait().)
//			autoStep = Step2;
//		}
//		else if (autoStep == Step2)
//		{
//			//Set left to move forward and right to move backward... for a while. (Use Gyro values? Or try using Wait()...)
//			autoStep = Step3;
//		}
//		else if (autoStep == Step3)
//		{
//			//Move forward for less of a while. (Encoder or Wait())
//			autoStep = Step4;
//		}
//		else if (autoStep == Step4)
//		{
//			//Lift arms up. Use sensors.
//			RunAutoAim(upHallSensor);
//			if (!upHallSensor)
//			{
//				autoStep = Step5;
//			}
//		}
//		else if (autoStep == Step5)
//		{
//			//Fire ball into goal. Once. Use Wait()
//			leftFireTalon.Set(1.0);
//			Wait(0.75);
//			leftFireTalon.Set(0.0);
//			autoStep = Done;
//		}
//		else
//		{
//			//Set everything to 0.
//		}
//	}

	// Jessica's Test Auto:

//	{
//		if (autoStep == Step0)
//		{
//			//go forward
//			leftDriveTalon.Set(0.5);
//			rightDriveTalon.Set(0.5);
//			autoStep = Step1;
//		}
//		else if (autoStep == Step1)
//		{
//			if (driveEncoder.GetDistance() < 24)
//			{
//				autoStep = Step0;
//			}
//			else if (driveEncoder.GetDistance() >= 24)
//			{
//				autoStep = Step2;
//			}
//		}
//		else if (autoStep == Step2)
//		{
//			//stop
//			leftDriveTalon.Set(0.0);
//			rightDriveTalon.Set(0.0);
//			autoStep = Done;
//		}
//	}

	//Testing Auto functions

	{
		AutoAim_X(table->GetNumber("centerX", 0.0));
		AutoAim_Y(table->GetNumber("centerY", 0.0), 360);
	}

	void AutoAim_X(double xCoord)
	{
		if (xCoord <= 1280 && xCoord > 640)
		{
			leftDriveTalon.Set(0.5);
			rightDriveTalon.Set(-0.5);
		}
		else if (xCoord >= 0 && xCoord < 640)
		{
			leftDriveTalon.Set(-0.5);
			rightDriveTalon.Set(0.5);
		}
	}

	void AutoAim_Y(double yCoord, double prefYVal)
	{
		if (yCoord > prefYVal)
		{
			raiseVictor.Set(-0.3);
		}
		else if (yCoord < prefYVal)
		{
			raiseVictor.Set(0.3);
		}
		else
		{
			raiseVictor.Set(0.0);
		}
	}

	void AutoReorientation() //to reorient the robot such that the robot is back to a very close angle.
	{
		if (gyro.GetAngle() > 1)
		{
			leftDriveTalon.Set(-0.1);
			rightDriveTalon.Set(0.1);
		}
		else if (gyro.GetAngle() < -1)
		{
			leftDriveTalon.Set(0.1);
			rightDriveTalon.Set(-0.1);
		}
		else
		{
			leftDriveTalon.Set(0.0);
			rightDriveTalon.Set(0.0);
		}

	}

	void TeleopInit()
	{
		handheld->init(&controller);
	}

	void TeleopPeriodic()
	{
		theseusDrive.TankDrive(leftStick, rightStick);
		handheld->readJoystick();

		SmartDashboard::PutNumber("Encoder Value", driveEncoder.Get());
		SmartDashboard::PutNumber("Gyro Value", gyro.GetAngle());
//		RunRaise_Button(handheld->readButton(6));
//		RunLower_Button(handheld->readButton(8));
		RunCollect(handheld->readButton(7), handheld->readButton(5));
		RunFire(handheld->readButton(5), handheld->readButton(7));
		StopleftFireTalons(handheld->readButton(3));
		RunAim(handheld->readButton(6), handheld->readButton(8), dio1->Get(), dio2->Get(), dio3->Get());
//		RunAim_TriggerControls(handheld->readButton(6), handheld->readButton(8));
		RunAim_ManualOverride(handheld->checkLeftStickY());
		RunLiftSolenoid(handheld->readButton(1));
		RunLiftMotor(handheld->readButton(4), handheld->getPOV());
		RunTrigger(handheld->readButton(2));
		CheckAllSensors(handheld->readButton(10));
//		CheckGyro_Manual(handheld->readButton(10));
		ResetGyro(handheld->readButton(9));
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

	void RunAim(bool upAimButton, bool downAimButton, bool upHallSensor, bool middleHallSensor, bool downHallSensor)
	{
		GetArmState(upHallSensor, downHallSensor, middleHallSensor);
		if (upAimButton)
		{
			if (armState == Down)
			{
				if (middleHallSensor)
				{
					raiseVictor.Set(0.3);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
			else if (armState == Middle)
			{
				if (upHallSensor)
				{
					raiseVictor.Set(0.3);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
			else if (armState == None)
			{
				if (upHallSensor)
				{
					raiseVictor.Set(0.3);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
		}
		else if (downAimButton)
		{
			if (armState == Up)
			{
				if (middleHallSensor)
				{
					raiseVictor.Set(-0.3);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
			else if (armState == Middle)
			{
				if (downHallSensor)
				{
					raiseVictor.Set(-0.3);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
			else if (armState == None)
			{
				if (downHallSensor)
				{
					raiseVictor.Set(-0.3);
				}
				else
				{
					raiseVictor.Set(0.0);
				}
			}
		}
	}

	void RunCollect(bool collectButton, bool fireButton)
	{
		if (collectButton && !fireButton)
		{
			leftFireTalon.Set(-0.375);
			rightFireVictor.Set(0.375);
		}
		else if (!collectButton && !fireButton)
		{
			leftFireTalon.Set(0.0);
			rightFireVictor.Set(0.0);
		}
	}

	void RunFire(bool fireButton, bool collectButton)
	{
		if (fireButton && !collectButton)
		{
			leftFireTalon.Set(1.0);
			rightFireVictor.Set(-1.0);
		}
		else if (!fireButton && !collectButton)
		{
			leftFireTalon.Set(0.0);
			rightFireVictor.Set(0.0);
		}
	}

	void StopleftFireTalons(bool stopButton)
	{
		if (stopButton)
		{
			leftFireTalon.Set(0.0);
			rightFireVictor.Set(0.0);
		}
	}

	void RunTrigger(bool triggerButton)
	{
		if (triggerButton)
		{
			triggerVictor.Set(0.25);
			Wait(1.0);
			triggerVictor.Set(0.0);
		}
	}

	void CheckAllSensors(bool checkButton)
	{
		if (checkButton)
		{
			upHallSensor = dio1->Get();
			SmartDashboard::PutBoolean("Up Sensor Value", upHallSensor);
			downHallSensor = dio3->Get();
			SmartDashboard::PutBoolean("Down Sensor Value", downHallSensor);
			middleHallSensor = dio2->Get();
			SmartDashboard::PutBoolean("Middle Sensor Value", middleHallSensor);
			SmartDashboard::PutNumber("Gyro Value", gyro.GetAngle());
			SmartDashboard::PutNumber("Encoder Value", driveEncoder.Get());
		}
	}

	void GetArmState(bool upHallSensor, bool downHallSensor, bool middleHallSensor)
	{
		if (!upHallSensor)
		{
			armState = Up;
		}
		else if (!downHallSensor)
		{
			armState = Down;
		}
		else if (!middleHallSensor)
		{
			armState = Middle;
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

	void RunLiftSolenoid(bool liftButton)
	{
		if (liftButton)
		{
			GetLiftState();
			if (liftState == Activated)
			{
				liftSolenoid.Set(false);
			}
			else
			{
				liftSolenoid.Set(true);
			}
		}
	}

	void RunLiftMotor(bool liftMotorButton, int pov)
	{
		if (liftMotorButton)
		{
			theArouser1.Set(-0.5);
			theArouser2.Set(-0.5);
		}
		else if (pov <= 135 && pov >= 45)
		{
			theArouser1.Set(0.5);
			theArouser2.Set(0.5);
		}
		else
		{
			theArouser1.Set(0.0);
			theArouser2.Set(0.0);
		}
	}

	void RunAim_TriggerControls(bool upTrigValue, bool downTrigValue)
	{
		if (upTrigValue && !downTrigValue)
		{
			raiseVictor.Set(0.5);
		}
		else if (!upTrigValue && downTrigValue)
		{
			raiseVictor.Set(-0.5);
		}
		else
		{
			raiseVictor.Set(0.0);
		}
	}

//	void CheckGyro_Manual(bool gyroButton)
//	{
//		if (gyroButton)
//		{
//			SmartDashboard::PutNumber("Gyro Value", gyro.GetAngle());
//		}
//	}

	void ResetGyro(bool resetButton)
	{
		if (resetButton)
		{
			gyro.Reset();
		}
	}

	void RunAim_ManualOverride(float yAxis)
	{
		if (yAxis > 0.25)
		{
			raiseVictor.Set(0.6);
		}
		else if (yAxis < -0.25)
		{
			raiseVictor.Set(-0.3);
		}
		else
		{
			raiseVictor.Set(0.0);
		}
	}
	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
