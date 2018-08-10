//  Code by Pedro Costa
//  08/2018
//
// Morris Robot Project
// MIT License
// Autonomous bio-inspired biped robot prototype which responds to changes in ambient light

#include <Servo.h>

#define TRUE  1
#define FALSE 0

#define ACTUATORS 8

typedef enum operation {
    LEFT_CONTROL,
    RIGHT_CONTROL,
    DUAL_CONTROL,
    IDLE
} op_t;
typedef unsigned short ushort_t;

// Mode detection and runtime control variables
ushort_t mode;
ushort_t idle, dark;
idle = dark = FALSE;

void attachServos (
    Servo group[],
    ushort_t pins[],
    ushort_t initialGroupPos,
    ushort_t finalGroupPos) {
    for (int i = initialGroupPos; i < finalGroupPos; i++) {
        group[i].attach(pins[i]);
    }
}

void writeServos (
    Servo group[],
    ushort_t degrees[],
    ushort_t initialGroupPos,
    ushort_t finalGroupPos) {
    for (int i = initialGroupPos; i < finalGroupPos; i++) {
        group[i].write(degrees[i]);
    }
}

int detectMode (
    ushort_t leftControlPin,
    ushort_t rightControlPin) {
    pinMode(leftControlPin, INPUT_PULLUP);
    pinMode(rightControlPin, INPUT_PULLUP);
    // Runs the dual board mode as the left board
    if (
        digitalRead(leftControlPin) &&
        !digitalRead(rightControlPin)
        ) {
            return LEFT_CONTROL;
    // Runs the dual board mode as the right board
    } else if (
        !digitalRead(leftControlPin) &&
        digitalRead(rightControlPin)
        ) {
            return RIGHT_CONTROL;
    // Runs the single-board mode
    // WARNING! Need separate energy source to work
    } else if (
        digitalRead(leftControlPin) &&
        digitalRead(rightControlPin)
        ) {
            return DUAL_CONTROL;
    // Both deactivated ports makes the board idle
    } else if (
        !digitalRead(leftControlPin) &&
        !digitalRead(rightControlPin)
        ) {
            return IDLE;
    }
}

void setup()
{
    // LED indication
    pinMode(13, OUTPUT);

    Servo leftServos[4];
    Servo rightServos[4];
    int leftPins[4] = {4, 5, 6, 7};
    int rightPins[4] = {8, 9, 10, 11};
    // Specify all frames of a side or, in that case, used for both
    int frames[9][4] = {
        {90, 90, 90, 90},
        {60, 90, 90, 60},
        {60, 60, 60, 60},
        {60, 90, 90, 60},
        {90, 90, 90, 90},
        {120, 90, 90, 120},
        {120, 120, 120, 120},
        {120, 90, 90, 120},
        {90, 90, 90, 90}
    }

    mode = detectMode(2, 3);
    if (mode == LEFT_CONTROL)
        attachServos(leftServos, leftPins, 4, 8);
    else if (mode == RIGHT_CONTROL)
        attachServos(rightServos, rightPins, 8, 12);
    else if (mode == DUAL_CONTROL) {
        attachServos(leftServos, leftPins, 4, 8);
        attachServos(rightServos, rightPins, 8, 12);
    } else if (mode == IDLE)
        idle = TRUE;

    Serial.begin(9600);
}

void loop()
{
    if (idle == FALSE) {
        if (dark == FALSE) {
            digitalWrite(13, LOW);
            if (mode == LEFT_CONTROL) {
                for (int i = 0; i < 9; i++) {
                    writeServos(leftServos, frames[i], 0, 4);
                    delay(1000);
                }

                if (analogRead(A0) > 512)
                    dark = FALSE;

            } else if (mode == RIGHT_CONTROL) {
                for (int i = 0; i < 9; i++) {
                    writeServos(rightServos, frames[i], 0, 4);
                    delay(1000);
                }

                if (analogRead(A0) > 512)
                    dark = FALSE;
            }
        } else {
          digitalWrite(13, HIGH);
          delay(1000);
        }
    }
}