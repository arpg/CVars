#include <string.h> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <CVars/glplatform.h>
#include <GLConsole/GLConsole.h>

#include <CVars/CVarVectorIO.h>
#include <CVars/CVarMapIO.h>

#include <vector>
#include <map>

#include "Pyramid.h"

// Single global instance so glut can get access
GLConsole theConsole;

Pyramid pyramid;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * What to draw on the display
 */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0,0.0f,-2.0f);

    pyramid.Draw();

    theConsole.RenderConsole();

    glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * resize the window to the desired size
 * @param w Window width
 * @param h Window height
 */
void reshape (int w, int h)
{
    glViewport     ( 0, 0, w, h );
    glMatrixMode   ( GL_PROJECTION );
    glLoadIdentity ( );

    if ( h == 0 )
        gluPerspective ( 80, ( float ) w, 1.0, 5000.0 );
    else
        gluPerspective ( 80, ( float ) w / ( float ) h, 1.0, 5000.0 );

    glMatrixMode   ( GL_MODELVIEW );
    glLoadIdentity ( );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * just redisplay constantly to let the console update...
 */
void idle()
{
#ifdef WIN32
	Sleep( 1 );
#else
    usleep( (int)1e4 );
#endif
    glutPostRedisplay(); // we have 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Pass keboard events to the console.
 * Up and down arrows to scroll through console history.
 * Shift+up or shift+down to scroll the console window.
 * Pageup or pagedown to scroll the console window as well.
 */
void special(int key, int, int )
{
    if( theConsole.IsOpen() )
    {
        //pass all key strokes to the console
        theConsole.SpecialFunc( key );
    }
    else
    {
        //do your own thing with the keys
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * handle incoming key events and send to console
 */
void keyfunc( unsigned char key, int, int )
{
    switch( key ) {

        case 27:  //escape
            exit ( 0 );
            break;

        case GLCONSOLE_KEY: //~ key opens console on US keyboards.
            //On UK keyboards it is the ` key (the one above the Tab and left of the 1 keys)
            theConsole.ToggleConsole();
            break;

        default:
            if( theConsole.IsOpen() ) {
                //send keystroke to console
                theConsole.KeyboardFunc( key );
            }
            else {
                //do your own thing with the keys
            }
            break;
    }
}

using CVarUtils::operator<<;
using CVarUtils::operator>>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Main. initialises GLUT and window, and some CVars then starts 
 * the main GLUT loop.
 */
int main (int argc, const char * argv[])
{
    glutInit(&argc, (char **)argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (300, 50);
    glutInitWindowSize (800, 600);
    glutCreateWindow("GLConsole Demo 2");

    // standard GL init
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable ( GL_COLOR_MATERIAL );
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutKeyboardFunc (keyfunc);
    glutSpecialFunc (special);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
