#include <iostream>
#include "Bezier.h"


double
getOffset_bezier(double speed0, double influence0, double time0, double value0x, double value0y,
                 double speed1, double influence1, double time1, double value1x, double value1y, double time) {
    BezierNode *Key = (BezierNode *) malloc(sizeof(BezierNode));
    BezierNode *LastKey = (BezierNode *) malloc(sizeof(BezierNode));
    Key->speed = speed1;
    Key->influence = influence1;
    Key->time = time1;
    Key->valuex = value1x;
    Key->valuey = value1y;

    LastKey->speed = speed0;
    LastKey->influence = influence0;
    LastKey->time = time0;
    LastKey->valuex = value0x;
    LastKey->valuey = value0y;

    return GetPercent(Key, LastKey, time);;
}


int main() {
    double currentTime = 1.480;
    double startTime = 0.4;
    double endTime = 3.24;
    double startValue =360;
    double endValue = 381;
    double speed0 = 0.0;
    double speed1 = 0;
    double influence0 = 1.0*100;
    double influence1 = 0.33*100;


    double Time = (currentTime - startTime) / (double) (endTime - startTime);
    double P = getOffset_bezier(speed0, influence0, startTime / 1000.0, startValue, 0, speed1, influence1,
                                endTime / 1000.0, endValue, 0, Time);
    double Y = startValue + (endValue - startValue) * P;


    std::cout << "Hello, World!==" << Y << std::endl;
    return 0;
}
