//
// Created by hansen tian on 2020/12/14.
//

#ifndef BAZIER_BEZIER_H
#define BAZIER_BEZIER_H

#include "math.h"

typedef struct Node {
    double speed;
    double influence;
    double time;
    double valuex;
    double valuey;
} BezierNode;


double GetPercent(BezierNode *Key, BezierNode *LastKey, double time);

double OneOrder(double A, double B, double t);

//double OneOrderSum(double AX, double AY,double BX,double BY, double t);
double ThreeOrder(double A, double B, double C, double D, double t); //A D: POINT  B C: CONTROLPOINT
//double ThreeOrderSum(double AX, double AY, double BX, double BY, double CX, double CY, double DX, double DY, double t);

double GetOneOrderT(double A, double B, double X);

double GetThreeOrderT(double A, double B, double C, double D, double X);

//double Sum;
//const int sections = 4;
//double SectionSum[4];


#endif //BAZIER_BEZIER_H
