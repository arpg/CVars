
#ifndef PYRAMID_H__
#define PYRAMID_H__

#include "Point3D.h"

class Pyramid
{
    public:
        Pyramid();
        ~Pyramid();
        void Draw();
        void Reset();

    private:
        void glVertex( Point3D &p );

    private:
        Point3D &mpPos;   //Centre of the pyramid
        Point3D mpRot;   //use a point to hold rotation information for each axis
        Point3D &mpRotDelta;  //how much to change the rotation by each time
        Point3D &mpScale; //Scale factor for the size of the pyramid
        Point3D &mpP1;    //the 5 points of the pyramid
        Point3D &mpP2;
        Point3D &mpP3;
        Point3D &mpP4;
        Point3D &mpP5;

};

#endif
