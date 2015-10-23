
#ifndef POINT3D_H__
#define POINT3D_H__

#include <sstream>

class Point3D
{
  public:
    Point3D(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f)
      : x(tx), y(ty), z(tz) {}
    ~Point3D() {}
    
    Point3D operator+(const Point3D&) const;
    Point3D operator*(const Point3D&) const;

    float x, y, z;

};

// All types you wish to use with CVars must overload << and >>.
std::ostream &operator<<(std::ostream &stream, Point3D &point );
std::istream &operator>>(std::istream &stream, Point3D &point );


#endif
