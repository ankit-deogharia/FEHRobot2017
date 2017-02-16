#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

#define DEFAULT_SPEED 25.0
#define RPS_TOLERANCE 0.5

//Motor declaration
FEHMotor left_motor(FEHMotor::Motor0, 7.2); //Motor voltage subject to change!
FEHMotor right_motor(FEHMotor::Motor1, 7.2);

//

int main(void)
{

    float x,y;

    LCD.Clear( FEHLCD::DARKSLATEGRAY );
    LCD.SetFontColor( FEHLCD::SCARLET );

    while( true )
    {
        if( LCD.Touch(&x,&y) )
        {
            LCD.WriteLine( "Mitochondria is the powerhouse of the cell" );
            Sleep( 100 );
        }
    }
    return 0;
}

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
    left_motor.SetPercent(0);
    right_motor.SetPercent(0);
}

/**
 * @brief inRPSRange Returns TRUE or FALSE whether the robot has valid RPS signal
 */
bool inRPSRange() {
    return RPS.X() > 0 && RPS.Y() > 0;
}

/**
 * @brief moveToPos Moves the robot to the specified position at DEFAULT_SPEED according to RPS
 * @param x X-coordinate of desired position
 * @param y Y-coordinate of desired position
 */
void moveToPos(float x, float y) {
    bool atPos = false;
    while (!atPos) {
        
    }
}

/**
 * @brief changeOrientation Changes the orientation by the specified amount
 * @param angle The angle by which to rotate the robot.  Positive angles turn clockwise, negative angles turn counter-clockwise
 */
void changeOrientation(float angle) {
    
}
