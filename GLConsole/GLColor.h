#if 0
/*

    Cross platform OpenGL console using the "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: GLColor.h 192 2012-03-06 01:12:01Z gsibley $

 */

#ifndef GLCOLOR_H__
#define GLCOLOR_H__

#include <sstream>
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////
struct GLColor
{
    GLColor(
            float tr = 1.0f,
            float tg = 1.0f, 
            float tb = 1.0f, 
            float ta = 1.0f 
           )
    {
        r = tr;
        g = tg;
        b = tb;
        a = ta;
    }

    GLColor( 
            int tr,
            int tg,
            int tb,
            int ta = 255 
           ) 
    {
        r = tr/255.0f; 
        g = tg/255.0f;
        b = tb/255.0f;
        a = ta/255.0f;
    }

    //        float fColor[4];
    float fColor[0];
    struct{
        float r;
        float g;
        float b;
        float a;
    };
};


#ifndef NAME_MAX
#   define NAME_MAX 1024
#endif

////////////////////////////////////////////////////////////////////////////////
/// All types you wish to use with CVars must overload << and >>.
inline std::ostream &operator<<(std::ostream &stream, GLColor &color)
{
    int r = int( 255*color.r );
    int g = int( 255*color.g );
    int b = int( 255*color.b );
    int a = int( 255*color.a );

    stream << "[ " << r << ",  " << g << ",  " << b << ",  " << a << " ]";
    return stream;
}

////////////////////////////////////////////////////////////////////////////////
/// All types you wish to use with CVars must overload << and >>.
inline std::istream &operator>>(std::istream &stream, GLColor &color)
{
    int r=0,g=0,b=0,a=0;

    char str[NAME_MAX] = {0};
    stream.readsome( str, NAME_MAX );
    sscanf( str, "[ %d, %d, %d, %d ]", &r, &g, &b, &a );

    color.r = (float)r/255.0f;
    color.g = (float)g/255.0f;
    color.b = (float)b/255.0f;
    color.a = (float)a/255.0f;
 
    return stream;
}

#endif
#endif
