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
	DigitalInput* dio0;
	DigitalInput* dio1;

//	bool raiseLimitSwitch;
//	bool lowerLimitSwitch;

	bool upThumb = true;
	bool downThumb = true;

	float gyroAngle;

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
//	NetworkTable *table;
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
		driveEncoder(DIO8, DIO9),
		gyro(ANALOG0)

	{
		dio0 = new DigitalInput(DIO0);
		dio1 = new DigitalInput(DIO1);

		gyroAngle = 0;

		handheld = new MyJoystick();
//		table = NetworkTable::GetTable("GRIP/myContoursReport");

		theseusDrive.SetExpiration(0.1);

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
		upThumb = dio0->Get();
		downThumb = dio1->Get();

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
//			RunAutoAim(upThumb);
//			if (!upThumb)
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
	{
		if (autoStep == Step0)
		{
			//go forward
			leftDriveTalon.Set(0.5);
			rightDriveTalon.Set(0.5);
			autoStep = Step1;
		}
		else if (autoStep == Step1)
		{
			if (driveEncoder.GetDistance() < 24)
			{
				autoStep = Step0;
			}
			else if (driveEncoder.GetDistance() >= 24)
			{
				autoStep = Step2;
			}
		}
		else if (autoStep == Step2)
		{
			//stop
			leftDriveTalon.Set(0.0);
			rightDriveTalon.Set(0.0);
			autoStep = Done;
		}
	}

	void RunAutoAim(bool upThumb)
	{
		if (upThumb)
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
//		leftFireTalon.Set(0.5);
//		Wait(0.5);
//		leftFireTalon.Set(-0.5);
//		Wait(0.5);
//		leftFireTalon.Set(0.0);
//
//		raiseVictor.Set(0.5);
//		Wait(0.5);
//		raiseVictor.Set(-0.5);
//		Wait(0.5);
//		raiseVictor.Set(0.0);
	}

	void TeleopPeriodic()
	{
		theseusDrive.TankDrive(leftStick, rightStick);
		handheld->readJoystick();

		int driveEncoderValue = driveEncoder.Get();
		SmartDashboard::PutNumber("Encoder Value", driveEncoder.Get());
		SmartDashboard::PutNumber("Gyro Value", gyro.GetAngle());
//		RunRaise_Button(handheld->readButton(6));
//		RunLower_Button(handheld->readButton(8));
		RunCollect(handheld->readButton(7), handheld->readButton(5));
		RunFire(handheld->readButton(5), handheld->readButton(7));
		StopleftFireTalons(handheld->readButton(3));
//		RunAim(handheld->checkLeftStickY(), dio0->Get(), dio1->Get());
		RunAim_TriggerControls(handheld->readButton(6), handheld->readButton(8));
		RunLiftSolenoid(handheld->readButton(1));
		RunLiftMotor(handheld->readButton(4));
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

	void RunAim(bool aimButton, bool upThumb, bool downThumb)
	{
		GetArmState(upThumb, downThumb);
		if (aimButton)
		{
			if (armState == None || armState == Down)
			{
				if (upThumb)
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
				if (downThumb)
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

	void RunCollect(bool collectButton, bool fireButton)
	{
		if (collectButton && !fireButton)
		{
			leftFireTalon.Set(-0.5);
			rightFireVictor.Set(0.5);
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
			triggerVictor.Set(1.0);
			Wait(1.0);
			triggerVictor.Set(0.0);
		}
	}

	void CheckAllSensors(bool checkButton)
	{
		if (checkButton)
		{
			upThumb = dio0->Get();
			SmartDashboard::PutBoolean("Up Sensor Value", upThumb);
			downThumb = dio0->Get();
			SmartDashboard::PutBoolean("Down Sensor Value", downThumb);
			SmartDashboard::PutNumber("Gyro Value", gyro.GetAngle());
			SmartDashboard::PutNumber("Encoder Value", driveEncoder.Get());
		}
	}

	void GetArmState(bool upThumb, bool downThumb)
	{
		if (!upThumb)
		{
			armState = Up;
		}
		else if (!downThumb)
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

	void RunLiftMotor(bool liftMotorButton)
	{
		if (liftMotorButton)
		{
			theArouser1.Set(-0.5);
			theArouser2.Set(-0.5);
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

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
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
	DigitalInput* dio0;
	DigitalInput* dio1;

//	bool raiseLimitSwitch;
//	bool lowerLimitSwitch;

	bool upThumb = true;
	bool downThumb = true;

	float gyroAngle;

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
//	NetworkTable *table;
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
		driveEncoder(DIO8, DIO9),
		gyro(ANALOG0)

	{
		dio0 = new DigitalInput(DIO0);
		dio1 = new DigitalInput(DIO1);

		gyroAngle = 0;

		handheld = new MyJoystick();
//		table = NetworkTable::GetTable("GRIP/myContoursReport");

		theseusDrive.SetExpiration(0.1);

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
		upThumb = dio0->Get();
		downThumb = dio1->Get();

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
//			RunAutoAim(upThumb);
//			if (!upThumb)
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
	{
		if (autoStep == Step0)
		{
			//go forward
			leftDriveTalon.Set(0.5);
			rightDriveTalon.Set(0.5);
			autoStep = Step1;
		}
		else if (autoStep == Step1)
		{
			if (driveEncoder.GetDistance() < 24)
			{
				autoStep = Step0;
			}
			else if (driveEncoder.GetDistance() >= 24)
			{
				autoStep = Step2;
			}
		}
		else if (autoStep == Step2)
		{
			//stop
			leftDriveTalon.Set(0.0);
			rightDriveTalon.Set(0.0);
			autoStep = Done;
		}
	}

	void RunAutoAim(bool upThumb)
	{
		if (upThumb)
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
//		leftFireTalon.Set(0.5);
//		Wait(0.5);
//		leftFireTalon.Set(-0.5);
//		Wait(0.5);
//		leftFireTalon.Set(0.0);
//
//		raiseVictor.Set(0.5);
//		Wait(0.5);
//		raiseVictor.Set(-0.5);
//		Wait(0.5);
//		raiseVictor.Set(0.0);
	}

	void TeleopPeriodic()
	{
		theseusDrive.TankDrive(leftStick, rightStick);
		handheld->readJoystick();

		int driveEncoderValue = driveEncoder.Get();
		SmartDashboard::PutNumber("Encoder Value", driveEncoder.Get());
		SmartDashboard::PutNumber("Gyro Value", gyro.GetAngle());
//		RunRaise_Button(handheld->readButton(6));
//		RunLower_Button(handheld->readButton(8));
		RunCollect(handheld->readButton(7), handheld->readButton(5));
		RunFire(handheld->readButton(5), handheld->readButton(7));
		StopleftFireTalons(handheld->readButton(3));
//		RunAim(handheld->checkLeftStickY(), dio0->Get(), dio1->Get());
		RunAim_TriggerControls(handheld->readButton(6), handheld->readButton(8));
		RunLiftSolenoid(handheld->readButton(1));
		RunLiftMotor(handheld->readButton(4));
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

	void RunAim(bool aimButton, bool upThumb, bool downThumb)
	{
		GetArmState(upThumb, downThumb);
		if (aimButton)
		{
			if (armState == None || armState == Down)
			{
				if (upThumb)
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
				if (downThumb)
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

	void RunCollect(bool collectButton, bool fireButton)
	{
		if (collectButton && !fireButton)
		{
			leftFireTalon.Set(-0.5);
			rightFireVictor.Set(0.5);
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
			triggerVictor.Set(1.0);
			Wait(1.0);
			triggerVictor.Set(0.0);
		}
	}

	void CheckAllSensors(bool checkButton)
	{
		if (checkButton)
		{
			upThumb = dio0->Get();
			SmartDashboard::PutBoolean("Up Sensor Value", upThumb);
			downThumb = dio0->Get();
			SmartDashboard::PutBoolean("Down Sensor Value", downThumb);
			SmartDashboard::PutNumber("Gyro Value", gyro.GetAngle());
			SmartDashboard::PutNumber("Encoder Value", driveEncoder.Get());
		}
	}

	void GetArmState(bool upThumb, bool downThumb)
	{
		if (!upThumb)
		{
			armState = Up;
		}
		else if (!downThumb)
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

	void RunLiftMotor(bool liftMotorButton)
	{
		if (liftMotorButton)
		{
			theArouser1.Set(-0.5);
			theArouser2.Set(-0.5);
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

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
