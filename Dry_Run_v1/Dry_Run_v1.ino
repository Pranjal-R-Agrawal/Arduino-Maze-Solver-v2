#define STBY 13

#define pwm1 5
#define m1a 22
#define m1b 23

#define pwm2 6
#define m2a 24
#define m2b 25

#define pwm3 7
#define m3a 26
#define m3b 27

#define pwm4 8
#define m4a 28
#define m4b 29

#define button1 33
#define button2 32

#define led1 41
#define led2 40
#define led3 38
#define redled 20

int motorPins[4][3] = {{pwm1, m1a, m1b}, {pwm2, m2a, m2b}, {pwm3, m3a, m3b}, {pwm4, m4a, m4b}};
int threshold[3][16];

int cDir = 0, lDir, rDir, uDir;
int nTop, nLeft, nCenter, nRight;
int eTop, eCenter, eBottom, eRight;
int wTop, wCenter, wBottom, wLeft;
int sBottom, sLeft, sCenter, sRight;

bool debug = false, adjustSpeed = true, scaleSensors = false;

double optimumVoltage = 7.7, currentVoltage = 8.1, constant = 1.35;

void setup() {
    if (debug) Serial.begin (9600);

    pinMode (STBY, OUTPUT);
    for (int i = 0; i < 4; i++) for (int j = 0; j < 3; j++) pinMode (motorPins [i][j], OUTPUT);

    pinMode (button1, INPUT_PULLUP);
    pinMode (button2, INPUT_PULLUP);

    analogWrite(pwm1, 0);
    analogWrite(pwm2, 0);
    analogWrite(pwm3, 0);
    analogWrite(pwm4, 0);

    pinMode (led1, OUTPUT);
    pinMode (led2, OUTPUT);
    pinMode (led3, OUTPUT);
    pinMode (redled, OUTPUT);
    digitalWrite (redled, LOW);
    changeDir (0);
    calibrate (60, 1000);
    wait (button1);
    delay (500);
}

bool left = false, straight = true, right = false, back = true;
long timer;
void loop() {

    while (!left && straight && !right) {
        left = onLine(wTop) || onLine (wBottom) || onLine (wCenter) || onLine (wLeft);
        straight = onLine(nTop) || onLine (nRight) || onLine (nCenter) || onLine (nLeft);
        right = onLine(eTop) || onLine (eBottom) || onLine (eCenter) || onLine (eRight);
        lineFollowing ();
    }

    digitalWrite (redled, HIGH);

    timer = millis ();
    if (left || right) {
        while ((millis () - timer) <= 75) {
            if (onLine(wTop) || onLine (wBottom) || onLine (wCenter) || onLine (wLeft)) left = true;
            if (onLine(nTop) || onLine (nRight) || onLine (nCenter) || onLine (nLeft)) straight = true;
            if (right = onLine(eTop) || onLine (eBottom) || onLine (eCenter) || onLine (eRight)) right = true;
            omni (30);
        }
    }
    else if (!straight) {
        while ((millis () - timer) <= 50) {
            if (onLine(wTop) || onLine (wBottom) || onLine (wCenter) || onLine (wLeft)) left = true;
            if (right = onLine(eTop) || onLine (eBottom) || onLine (eCenter) || onLine (eRight)) right = true;
            back = onLine(sBottom) || onLine (sRight) || onLine (sCenter) || onLine (sLeft);
            omni (35);
            if (!back) break;
            //if (left || right || !back) break;
        }
        if (!back) {
            omni (uDir, 30);
            delay (30);
        }
    }

    decelerate (30, 2);
    brake (200);

    if (left) {
        changeDir (1);
    }
    else if (straight) {
        changeDir (0);
    }
    else if (right) {
        changeDir (-1);
    }
    else {
        changeDir (2);
    }

    timer = millis ();
    double spd = 20;
    while ((millis () - timer) <= 500) {
        if (spd < 70) spd += 0.1;
        lineFollowing ((int)spd);
    }

    digitalWrite (redled, LOW);
    left = false; straight = true; right = false;

}
