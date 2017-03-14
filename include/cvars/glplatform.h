/* This file is ported from the Pangolin project
 *
 * http://github.com/stevenlovegrove/Pangolin
 *
 * as part of the CVars project.
 *
 * Copyright (c) 2011 Steven Lovegrove, Richard Newcombe,
 *               2015 Christoffer Heckman
 *
 */

#ifndef CVARS_GLPLATFORM_H
#define CVARS_GLPLATFORM_H

//////////////////////////////////////////////////////////
// Attempt to portably include Necessary OpenGL headers
//////////////////////////////////////////////////////////

#include <cvars/config.h>

#ifdef _WIN_
    // Define maths quantities when using <cmath> to match posix systems
    #define _USE_MATH_DEFINES

    // Don't define min / max macros in windows.h or other unnecessary macros
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    // Undef nuisance Windows.h macros which interfere with our methods
    #undef LoadImage
    #undef near
    #undef far
#endif

#ifdef HAVE_GLEW
    #include <GL/glew.h>
#endif

#ifdef HAVE_GLES
    #if defined(_ANDROID_)
        #include <EGL/egl.h>
        #ifdef HAVE_GLES_2
            #include <GLES2/gl2.h>
            #include <GLES2/gl2ext.h>
        #else
            #include <GLES/gl.h>
            #define GL_GLEXT_PROTOTYPES
            #include <GLES/glext.h>
        #endif
    #elif defined(_APPLE_IOS_)
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #endif
#else
    #ifdef _OSX_
      #include <OpenGL/gl.h>
      #ifdef HAVE_APPLE_OPENGL_FRAMEWORK
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <GLUT/glut.h>
      #elif defined HAVE_FREEGLUT
        #include <GL/glut.h>
      #else
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <GLUT/glut.h>
      #endif
    #else
        #include <GL/gl.h>
    #endif
#endif // HAVE_GLES

#ifdef HAVE_FREEGLUT
 #include <GL/gl.h>
 #include <GL/glu.h>
 #include <GL/glut.h>
#endif

#endif // CVARS_GLPLATFORM_H
