    #include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <math.h>

#define DEFAULT_SPEED 25.0
#define RPS_TOLERANCE 0.5

bool stayOn = true, verboseMode = false;

//Motor declaration
FEHMotor left_motor(FEHMotor::Motor0, 7.2); //Motor voltage subject to change!
FEHMotor right_motor(FEHMotor::Motor1, 7.2);
FEHServo claw_servo(FEHServo::Servo1);

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

    LCD.Clear();
    right_motor.SetPercent(DEFAULT_SPEED);
    LCD.WriteLine("Moving right motor forward");
    Sleep(1.0);

    LCD.Clear();
    right_motor.SetPercent(DEFAULT_SPEED);
    LCD.WriteLine("Movin right motor backward");
    Sleep(1.0);

    LCD.Clear();
    claw_servo.SetDegree(90);
    LCD.WriteLine("Testing claw servo forward");
    Sleep(1.0);
    LCD.Clear();
    LCD.WriteLine("Testing claw servo backward");
    claw_servo.SetDegree(0);
    Sleep(1.0);

    LCD.Clear();
}

void menu() {
    // declare an array of four icons called menu
    FEHIcon::Icon iconMenu[4] = {};

    // define the four menu labels
    char menu_labels[4][20] = {"TEST","RUN","VERBOSE","CALIBRATE"};

    // draw the menu in a 2 by 2 array with top and bottom
    // margins of 10 and left and right margins of 5
    // with the menu labels, black borders, and white text

    bool exit = true;
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
            if (!verboseMode) {
                iconMenu[2].ChangeLabelString("VERBOSE");
            } else {
                iconMenu[2].ChangeLabelString("QUIET");
            }
            verboseMode = !verboseMode;
        } else if (iconMenu[3].Pressed(x, y, 1)) {
            LCD.Clear();
            claw_servo.Calibrate();
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
    left_motor.SetPercent(speed * 100);
    right_motor.SetPercent(speed * 100);
    Sleep(time);
    left_motor.SetPercent(0);
    right_motor.SetPercent(0);
}

/**
 * @brief turnLeftRight Turns the robot in the specified direction for a duraton of time
 * @param speed Float value from -1 to +1 determining the turning veloctity.  Positive values turn the robot clockwise and negative values turn the robot counter-clockwise.
 * @param time Float value determining the turn duration
 */
void turnLeftRight(float speed, float time) {
    left_motor.SetPercent(speed * 100);
    right_motor.SetPercent(-speed * 100);
    Sleep(time);
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
    const float tolerance = 3.0, time = 2.5;
    float start = TimeNow();
    while (TimeNow() - start < time) {
        if (abs(RPS.Heading() - angle) < tolerance) {
            left_motor.Stop();
            right_motor.Stop();
        } else if (angle < 180) {
            if (RPS.Heading() - angle < 180) {
                left_motor.SetPercent(DEFAULT_SPEED);
                right_motor.SetPercent(-DEFAULT_SPEED);
            } else {
                left_motor.SetPercent(-DEFAULT_SPEED);
                right_motor.SetPercent(DEFAULT_SPEED);
            }
        } else {
            if (RPS.Heading() - angle < 180) {
                left_motor.SetPercent(DEFAULT_SPEED);
                right_motor.SetPercent(-DEFAULT_SPEED);
            } else {
                left_motor.SetPercent(-DEFAULT_SPEED);
                right_motor.SetPercent(DEFAULT_SPEED);
            }
        }
    }
}

/**
 * @brief moveToPos Moves the robot to the specified position at DEFAULT_SPEED according to RPS
 * @param x X-coordinate of desired position
 * @param y Y-coordinate of desired position
 */
void moveToPos(float x, float y) {
    bool atPos = false;
    float xPos = RPS.X() - x,
            yPos = RPS.Y() - y,
            tolerance = 3.0,
            startDistance = sqrt(xPos * xPos + yPos * yPos);
    setOrientation(atan2(yPos, xPos));
    while (!atPos) {
        xPos = RPS.X() - x;
        yPos = RPS.Y() - y;
        float distance = sqrt(xPos * xPos + yPos * yPos),
                ratio = distance/startDistance;
        left_motor.SetPercent(DEFAULT_SPEED * ratio);
        right_motor.SetPercent(DEFAULT_SPEED * ratio);
    }
}

//Level 0
int main(void)
{
    float x,y;

    LCD.Clear();
    LCD.SetBackgroundColor(BLUE);
    LCD.SetFontColor(GOLD);

    menu();
    while(stayOn) {
        //General robot code goes here
        moveForwardBackward(DEFAULT_SPEED, 1.0);
        //TODO: Test using level 1 methods navigation to the button and holding the button.  After that calibrate level 2 methods to ensure functionality.
    }

    return 0;
}

