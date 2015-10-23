#include "Point3D.h"

#include <iostream>
#include <limits.h>

#ifndef NAME_MAX
#   define NAME_MAX 1024
#endif


/**
 * Overloaded stream out for Point3D. Converts it to a string
 */
std::ostream &operator<<(std::ostream &stream, Point3D &point)
{
  stream << "[ " << point.x << ",  " << point.y << ",  " << point.z << " ]";
  return stream;
}

/**
 * Overloaded stream in for Point3D. Converts it from a string
 */
std::istream &operator>>(std::istream &stream, Point3D &point)
{
    char str[NAME_MAX] = {0};
    stream.readsome( str, NAME_MAX );
    sscanf( str, "[ %f, %f, %f ]", &point.x, &point.y, &point.z );

    return stream;
}

/**
 * add points together
 */
Point3D Point3D::operator+(const Point3D& that) const
{
  Point3D result;
  result.x = this->x + that.x;
  result.y = this->y + that.y;
  result.z = this->z + that.z;
  return result;
}

/**
 * multiply points together
 */
Point3D Point3D::operator*(const Point3D& that) const
{
  Point3D result;
  result.x = this->x * that.x;
  result.y = this->y * that.y;
  result.z = this->z * that.z;
  return result;
}
