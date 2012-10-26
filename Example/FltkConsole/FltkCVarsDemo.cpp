/*******************************************************************
 *
 *  CVars demo using FLTK.
 *
 *******************************************************************/

#include <stdio.h>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include "GLHelpers.h"
#include "GLCube.h"

#include <FLConsole/FLConsole.h>

const double g_dFPS = 1.0/20.0;

/// FLTK OpenGL window.
class GLWindow : public Fl_Gl_Window 
{
    /// timer callback drives animation and forces drawing at 20 hz 
    static void Timer(void *userdata) {
        GLWindow *pWin = (GLWindow*)userdata;
        pWin->m_Cube.m_dSpin += 3.0;
        pWin->redraw();
        Fl::repeat_timeout( g_dFPS, Timer, userdata );
    }
    
    public:

    /// Constructor.
    GLWindow(int x,int y,int w,int h,const char *l=0) : Fl_Gl_Window(x,y,w,h,l) 
    {
        Fl::add_timeout( g_dFPS, Timer, (void*)this );
    }

    /// Init OpenGL
    void Init() 
    {
        m_Cube.Init();
        // OpenGL settings
        glShadeModel( GL_FLAT );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );
        glClearColor(.5,.5,.8, 1.0);
    }

    /// Main function called by FLTK to draw the scene.
    void draw() {
        CheckForGLErrors();

        // handle reshaped viewport
        if ( !valid() ) {
            ReshapeViewport( w(), h() );
        }
        // Initialization
        static bool bInitialized = false;
        if ( !bInitialized || !context() ) {
            bInitialized = true;
            Init();
            return;
        }

        // Clear
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Draw objects
        m_Cube.draw();
        m_Console.draw();

        CheckForGLErrors();
    }

    /// Pass input to FLConsole.
    int  handle( int e )
    {
        if( context() ){
            return m_Console.handle( e ); 
        }
        return false;
    }

   
    private: 
        GLCube        m_Cube;
        FLConsole   m_Console;
};


int main( int, char** ) 
{
    GLWindow* pWin = new GLWindow( 10, 10, 640, 480, "Fltk CVars Demo" );
    pWin->end();
    pWin->resizable( pWin );

    pWin->show();
    return( Fl::run() );
}

