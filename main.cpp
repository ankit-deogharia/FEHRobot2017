    #include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <math.h>

#define DEFAULT_SPEED 25.0
#define RPS_TOLERANCE 0.5

bool stayOn = true;

//Motor declaration
FEHMotor left_motor(FEHMotor::Motor0, 12); //Motor voltage subject to change!
FEHMotor right_motor(FEHMotor::Motor1, 12);
FEHServo arm_servo(FEHServo::Servo0);

//Sensor declaration
AnalogInputPin Cds_cell(FEHIO::P0_0); //NO LIGHT: ~2.9  BLUE LIGHT: ~1.6 RED LIGHT: ~0.5

AnalogInputPin optoLeft(FEHIO::P1_0);
AnalogInputPin optoCenter(FEHIO::P1_1);
AnalogInputPin optoRight(FEHIO::P1_2);

//INTERFACE METHODS

void test() {
    LCD.Clear();
    left_motor.SetPercent(DEFAULT_SPEED);
    LCD.WriteLine("Moving left motor forward");
    Sleep(1.0);

    LCD.Clear();
    left_motor.SetPercent(-DEFAULT_SPEED);
    LCD.WriteLine("Moving left motor backward");
    Sleep(1.0);

    left_motor.Stop();

    LCD.Clear();
    right_motor.SetPercent(DEFAULT_SPEED);
    LCD.WriteLine("Moving right motor forward");
    Sleep(1.0);

    LCD.Clear();
    right_motor.SetPercent(-DEFAULT_SPEED);
    LCD.WriteLine("Movin right motor backward");
    Sleep(1.0);

    right_motor.Stop();

    LCD.Clear();
    arm_servo.SetDegree(0);
    LCD.WriteLine("Testing claw servo forward");
    Sleep(1.0);
    LCD.Clear();
    LCD.WriteLine("Testing claw servo backward");
    arm_servo.SetDegree(180);
    Sleep(1.0);

    LCD.Clear();
}

void calibrate() {
    LCD.Clear();
    arm_servo.TouchCalibrate();
}

void menu() {
    // declare an array of four icons called menu
    FEHIcon::Icon iconMenu[4] = {};

    // define the four menu labels
    char menu_labels[4][20] = {"TEST","RUN","EXIT","CALIBRATE"};

    // draw the menu in a 2 by 2 array with top and bottom
    // margins of 10 and left and right margins of 5
    // with the menu labels, black borders, and white text

    bool exit = true;
    LCD.Clear();
    while (exit) {
        FEHIcon::DrawIconArray(iconMenu, 2, 2, 10, 10, 5, 5, menu_labels, GOLD, GOLD);
        float x, y;
        LCD.Touch(&x, &y);
        if (iconMenu[0].Pressed(x, y, 1)) {
            LCD.Write("Touch Received on Test");
            test();
        } else if (iconMenu[1].Pressed(x, y, 1)) {
            LCD.Clear();
            LCD.WriteLine("Running robot program...");
            exit = false;
        } else if (iconMenu[2].Pressed(x, y, 0)) {

            stayOn = false;
            verboseMode = !verboseMode;
        } else if (iconMenu[3].Pressed(x, y, 1)) {
            LCD.Clear();
            calibrate();
        }
    }
}

//FUNCTIONAL METHODS

//Level 1
/**
 * @brief moveForwardBackrward Runs the robot forward or backward for a duration of time
 * @param speed Float value from -1 to +1 determining the speed of the robot.  Positive speeds runs the robot forward and negative speeds runs it backwards.
 * @param time Float value indicating how long to turn motors
 */
void moveForwardBackward(float speed, float time) {
    float start = TimeNow();
    while (TimeNow() - start < time) {
        left_motor.SetPercent(speed * 100);
        right_motor.SetPercent(speed * 100);
    }
    left_motor.Stop();
    right_motor.Stop();
}

/**
 * @brief turnLeftRight Turns the robot in the specified direction for a duraton of time
 * @param speed Float value from -1 to +1 determining the turning veloctity.  Positive values turn the robot clockwise and negative values turn the robot counter-clockwise.
 * @param time Float value determining the turn duration
 */
void turnLeftRight(float speed, float time) {
    float start = TimeNow();
    while (TimeNow() - start < time) {
        left_motor.SetPercent(speed * 100);
        right_motor.SetPercent(-speed * 100);
    }
    left_motor.Stop();
    right_motor.Stop();
}

/**
 * @brief inRPSRange Returns TRUE or FALSE whether the robot has valid RPS signal
 */
bool inRPSRange() {
    return RPS.X() > 0 && RPS.Y() > 0;
}

//Level 2
/**
 * @brief setOrientation Sets the orientation to the specified angle
 * @param angle The angle to turn the robot towards
 */
void setOrientation(float angle) {
    const float tolerance = 3.0, time = 5.0;
    float start = TimeNow();

    while(true) {
        LCD.Clear();
        LCD.Write("Current RPS Heading: ");
        LCD.Write(RPS.Heading());
        LCD.WriteLine("Desired robot angle: ");
        LCD.Write(angle);
        Sleep(10);
    }

    /*while (TimeNow() - start < time) {
        if (abs(RPS.Heading() - angle) < tolerance) {
            left_motor.Stop();
            right_motor.Stop();
        } else if (RPS.Heading() < angle) {
            if (angle > 180) {
                left_motor.SetPercent(-DEFAULT_SPEED);
                right_motor.SetPercent(DEFAULT_SPEED);
            } else {
                left_motor.SetPercent(DEFAULT_SPEED);
                right_motor.SetPercent(-DEFAULT_SPEED);
            }
        } else {
            if (angle > 180) {
                left_motor.SetPercent(DEFAULT_SPEED);
                right_motor.SetPercent(-DEFAULT_SPEED);
            } else {
                left_motor.SetPercent(-DEFAULT_SPEED);
                right_motor.SetPercent(DEFAULT_SPEED);
            }
        }
    }*/
}

/**
 * @brief moveToPos Moves the robot to the specified position at DEFAULT_SPEED according to RPS
 * @param x X-coordinate of desired position
 * @param y Y-coordinate of desired position
 */
void moveToPos(float x, float y) {
    LCD.WriteLine("Starting moveToPos function...\n");
    bool atPos = false;
    float xPos = RPS.X() - x,
            yPos = RPS.Y() - y,
            tolerance = 3.0,
            startDistance = sqrt(xPos * xPos + yPos * yPos),
            angle = atan2(y - yPos, x - xPos);
    LCD.WriteLine("Turning Angle: ");
    setOrientation(angle);
    while (!atPos) {
        xPos = RPS.X() - x;
        yPos = RPS.Y() - y;
        float distance = sqrt(xPos * xPos + yPos * yPos),
                ratio = distance/startDistance;
        left_motor.SetPercent(DEFAULT_SPEED * ratio);
        right_motor.SetPercent(DEFAULT_SPEED * ratio);
    }
}

void followLine(float time) {
    float start = TimeNow(),
            threshold = 1.0;
    while (TimeNow() - start < time) {
        bool left = optoLeft.Value() < threshold,
                center = optoCenter.Value() < threshold,
                right = optoRight.Value() < threshold;
        if (center) {
            left_motor.SetPercent(DEFAULT_SPEED);
        }
    }
}

//Level 0
int main(void)
{
    float x,y;

    LCD.Clear();
    LCD.SetBackgroundColor(BLUE);
    LCD.SetFontColor(GOLD);

    arm_servo.SetMin(797);
    arm_servo.SetMax(2333);

    arm_servo.SetDegree(180);

    while (true) {
        menu();
        if (!stayOn)
            break;
        //RPS.InitializeTouchMenu();

        //while (Cds_cell.Value() > 1);

        //TODO: Test using level 1 methods navigation to the button and holding the button.  After that calibrate level 2 methods to ensure functionality.
        moveForwardBackward(-0.25, 1.65);
        turnLeftRight(-0.24, 1.2); //90 degrees
        moveForwardBackward(-0.25, 2.22);
        turnLeftRight(-0.24, 1.2); //90 degrees
        moveForwardBackward(-.3, 3.0);
        turnLeftRight(0.24, 1.2); //90 degrees
        //moveForwardBackward(0.25, 1.0);
        //arm_servo.SetDegree();

        //turnLeftRight(-0.15, 0.45);
        //moveForwardBackward(-0.25, 1);
        //turnLeftRight(-0.25, 1.0);
        //moveForwardBackward(-0.25, 1.5);
        //moveForwardBackward(-0.05, 5.0);
        //moveForwardBackward(0.25, 3.0);
        //turnLeftRight(-0.25, 1.1);
        //moveForwardBackward(-0.25, 3.0);
        LCD.WriteLine("Program Finished.  \nReturning to main menu screen.");
    }

    LCD.Clear();
    LCD.WriteLine("Ready to shutoff.");
    return 0;
}

