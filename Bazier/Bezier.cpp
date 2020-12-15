//
// Created by hansen tian on 2020/12/14.
//

#include "Bezier.h"
#include "stdio.h"


#define TAG "Bezier"


double OneOrder(double A, double B, double t) {
    double X = (B - A) * t + A;
    return X;
}

double OneOrderSum(double AX, double AY, double BX, double BY, double t) {
    double S = (BX - AX) * ((BY - AY) * t * t * 0.5 + AY * t);
    return S;
}

double ThreeOrder(double A, double B, double C, double D, double t) {
    double X = (D - 3 * C + 3 * B - A) * t * t * t + (3 * C - 6 * B + 3 * A) * t * t + (3 * B - 3 * A) * t + A;
    return X;
}

double ThreeOrderSum(double AX, double AY, double BX, double BY, double CX, double CY, double DX, double DY, double t) {
    double IX = (DX - 3 * CX + 3 * BX - AX);
    double JX = (3 * CX - 6 * BX + 3 * AX);
    double KX = (3 * BX - 3 * AX);
    double LX = AX;

    double IY = (DY - 3 * CY + 3 * BY - AY);
    double JY = (3 * CY - 6 * BY + 3 * AY);
    double KY = (3 * BY - 3 * AY);
    double LY = AY;

    double S = IX * IY / 2.0 * t * t * t * t * t * t + (3 * IX * JY + 2 * JX * IY) / 5.0 * t * t * t * t * t +
               (3 * IX * KY + 2 * JX * JY + KX * IY) / 4.0 * t * t * t * t +
               (3 * IX * LY + 2 * JX * KY + KX * JY) / 3.0 * t * t * t + (2 * JX * LY + KX * KY) / 2.0 * t * t +
               KX * LY * t;
    return S;
}

double GetOneOrderT(double A, double B, double X) {
    double T = (X - A) / (B - A);
    return T;
}

double GetThreeOrderT(double A, double B, double C, double D, double X) {
    double left = 0.0, right = 1.0, mid = 0.0, leftX, midX, rightX;
    while (right - left >= 0.001) {
        mid = (right + left) / 2.0;
        leftX = ThreeOrder(A, B, C, D, left) - X;
        midX = ThreeOrder(A, B, C, D, mid) - X;
        rightX = ThreeOrder(A, B, C, D, right) - X;
        if (midX == 0)
            return mid;
        if (leftX == 0)
            return left;
        if (rightX == 0)
            return right;
        if (leftX * midX < 0)
            right = mid;
        else if (rightX * midX < 0)
            left = mid;;
    }
    return (right + left) / 2.0;
}


double GetPercent(BezierNode *key, BezierNode *lastKey, double time) {
    double Point[4][2];

    Point[0][0] = 0.00;
    Point[0][1] = 0.00;
    Point[3][0] = 1.00;
    Point[3][1] = 1.00;
    double duration = key->time - lastKey->time; //定义关键帧持续时间
    double diffx = (key->valuex - lastKey->valuex);//获取两关键帧差的绝对值
    double diffy = (key->valuey - lastKey->valuey);
    double diff = sqrt(diffx * diffx + diffy * diffy);
    double averageSpeed = fabs(diff / duration); //获取关键帧平均速度
    if (averageSpeed == 0)
        averageSpeed = averageSpeed + 0.0001;
    //当关键帧的值是数组时，可以通过开两者平方的方式算出平均速度，多维同理增加
    //float averageSpeed = sqrt( xAverageSpeed*xAverageSpeed + yAverageSpeed*yAverageSpeed );
    //var bezierIn = {};//定义贝塞尔曲线入柄点的xy轴值
    Point[2][0] = 1 - key->influence / 100;
    Point[2][1] = 1 - key->speed / averageSpeed * key->influence / 100;
    //var bezierOut = {};//定义贝塞尔曲线出柄点的xy轴值
    Point[1][0] = lastKey->influence / 100;
    Point[1][1] = lastKey->speed / averageSpeed * lastKey->influence / 100;

    printf("bezierIn = {%llf, %llf}", Point[1][0], Point[1][1]);
    printf("bezierOut = {%llf, %llf}", Point[2][0], Point[2][1]);

    double t = GetThreeOrderT(Point[0][0], Point[1][0], Point[2][0], Point[3][0], time);
    double percent = ThreeOrder(Point[0][1], Point[1][1], Point[2][1], Point[3][1], t);

    printf("time=%llf, t=%llf", time, t);
    printf("Percent=%llf, Ptime=%llf", percent, ThreeOrder(Point[0][1], Point[1][1], Point[2][1], Point[3][1], time));

    return percent;

    //上述条件中的influence即关键帧速度当中的“影响" speed也即其中的“速度”
}