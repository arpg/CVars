
#include <CVars/CVar.h>
#include <CVars/glplatform.h>
#include "Pyramid.h"

/**
 * Construct the pyramid object and initialise the CVars
 */
Pyramid::Pyramid() :
  mpPos( CVarUtils::CreateCVar<Point3D>( "pyramid.position", Point3D(0.0f, 0.0f, -2.0f), "Centre of the pyramid" ) ),
  mpRot( 0.0f, 0.0f, 0.0f),
  mpRotDelta( CVarUtils::CreateCVar<Point3D>( "pyramid.rotationDelta", Point3D(0.0f, 0.2f, 0.0f), "How quickly to rotate the pyramid on each axis" ) ),
  mpScale( CVarUtils::CreateCVar<Point3D>( "pyramid.scale", Point3D(1.0f, 1.0f, 1.0f), "The scale of the pyramid in each axis" ) ),
  mpP1( CVarUtils::CreateCVar<Point3D>( "pyramid.point.1", Point3D(0.0f, 1.0f, 0.0f), "The top point" ) ),
  mpP2( CVarUtils::CreateCVar<Point3D>( "pyramid.point.2", Point3D(-1.0f,-1.0f, 1.0f), "The front left point" ) ),
  mpP3( CVarUtils::CreateCVar<Point3D>( "pyramid.point.3", Point3D(1.0f,-1.0f, 1.0f), "The front right point" ) ),
  mpP4( CVarUtils::CreateCVar<Point3D>( "pyramid.point.4", Point3D(1.0f,-1.0f, -1.0f), "The back right point" ) ),
  mpP5( CVarUtils::CreateCVar<Point3D>( "pyramid.point.5", Point3D(-1.0f,-1.0f, -1.0f), "The back left point" ) )
{

}

/**
 * Destructor.
 */
Pyramid::~Pyramid()
{

}

/**
 * Helper funciton to draw a point in OpenGL
 */
inline void Pyramid::glVertex(Point3D &p)
{
  Point3D sp = mpScale * p;
  glVertex3f(sp.x, sp.y, sp.z);
}


/**
 * Call this to draw the pyramid
 */
void Pyramid::Draw()
{
  glTranslatef(mpPos.x,mpPos.y,mpPos.z);             // Move Left 1.5 Units And Into The Screen 6.0
        
  mpRot = mpRot + mpRotDelta;
  glRotatef(mpRot.x,1.0f,0.0f,0.0f);               // Rotate The Pyramid On The X axis 
  glRotatef(mpRot.y,0.0f,1.0f,0.0f);               // Rotate The Pyramid On The Y axis 
  glRotatef(mpRot.z,0.0f,0.0f,1.0f);               // Rotate The Pyramid On The Z axis 

  // draw a pyramid (in smooth coloring mode)
  glBegin(GL_POLYGON);                          // start drawing a pyramid

  // front face of pyramid
  glColor3f(1.0f,0.0f,0.0f);                    // Set The Color To Red
  glVertex( mpP1 );                 // Top of triangle (front)
  glColor3f(0.0f,1.0f,0.0f);                    // Set The Color To Green
  glVertex(mpP2 );                // left of triangle (front)
  glColor3f(0.0f,0.0f,1.0f);                    // Set The Color To Blue
  glVertex( mpP3 );                 // right of traingle (front)    

  // right face of pyramid
  glColor3f(1.0f,0.0f,0.0f);                    // Red
  glVertex( mpP1 );                // Top Of Triangle (Right)
  glColor3f(0.0f,0.0f,1.0f);                    // Blue
  glVertex( mpP3 );                // Left Of Triangle (Right)
  glColor3f(0.0f,1.0f,0.0f);                    // Green
  glVertex( mpP4 );               // Right Of Triangle (Right)

  // back face of pyramid
  glColor3f(1.0f,0.0f,0.0f);                    // Red
  glVertex( mpP1 );                // Top Of Triangle (Back)
  glColor3f(0.0f,1.0f,0.0f);                    // Green
  glVertex( mpP4 );               // Left Of Triangle (Back)
  glColor3f(0.0f,0.0f,1.0f);                    // Blue
  glVertex( mpP5 );               // Right Of Triangle (Back)

  // left face of pyramid.
  glColor3f(1.0f,0.0f,0.0f);                    // Red
  glVertex( mpP1 );                // Top Of Triangle (Left)
  glColor3f(0.0f,0.0f,1.0f);                    // Blue
  glVertex( mpP5 );                // Left Of Triangle (Left)
  glColor3f(0.0f,1.0f,0.0f);                    // Green
  glVertex( mpP2 );                // Right Of Triangle (Left)

  glEnd();                                      // Done Drawing The Pyramid

}


/**
 * Reset the pyramid to initial conditions
 */
void Pyramid::Reset()
{
  mpPos = Point3D(0.0f, 0.0f, -2.0f);
  mpRot = Point3D( 0.0f, 0.0f, 0.0f);
  mpRotDelta = Point3D( 0.0f, 0.2f, 0.0f);
  mpScale = Point3D(1.0f, 1.0f, 1.0f);
  mpP1 = Point3D(0.0f, 1.0f, 0.0f);
  mpP2 = Point3D(-1.0f,-1.0f, 1.0f);
  mpP3 = Point3D(1.0f,-1.0f, 1.0f);
  mpP4 = Point3D(1.0f,-1.0f, -1.0f);
  mpP5 = Point3D(-1.0f,-1.0f, -1.0f);
}
