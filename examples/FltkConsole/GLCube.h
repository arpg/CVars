#ifndef _CUBE_H_
#define _CUBE_H_

#include <math.h> // for M_PI
#include <FL/gl.h>

#define FPS 1.0/20.0
#define TEX_W 64
#define TEX_H 64

class GLCube
{
    public:
        double m_dSpin;

    public:
        ///  OpenGL stuff to do *only once* on startup.
        void Init();

        /// Main function called by FLTK to draw the scene.
        void draw();

    private:
        GLuint m_nTexID;
};



inline void GLCube::Init() 
{
    // Texture Map Init
    GLubyte img[TEX_W][TEX_H][3]; // after glTexImage2D(), array is no longer needed
    for (int x=0; x<TEX_W; x++) {
        for (int y=0; y<TEX_H; y++) {
            GLubyte c = ((x&16)^(y&16)) ? 255 : 0; // checkerboard
            img[x][y][0] = c;
            img[x][y][1] = c;
            img[x][y][2] = c;
        }
    }

    // Generate and bind the texture 
    glGenTextures( 1, &m_nTexID );
    glBindTexture( GL_TEXTURE_2D, m_nTexID );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D( GL_TEXTURE_2D, 0, 
            GL_RGB, TEX_W, TEX_H, 0, GL_RGB, GL_UNSIGNED_BYTE, &img[0][0][0] );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

/// Main function called by FLTK to draw the scene.
inline void GLCube::draw() 
{

    glEnable( GL_TEXTURE_2D );
    // Setup model matrix
    glLoadIdentity();
    glRotatef( m_dSpin, 0.5, 1.0, 0.0); // show all sides of cube
    // Draw cube with texture assigned to each face
    glBegin(GL_QUADS);
    // Front Face
    glColor3f(1.0, 0.0, 0.0); // red
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0); 
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0); 
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  1.0); 
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  1.0); 
    // Back Face
    glColor3f(0.0, 1.0, 1.0); // cyn
    glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0, -1.0); 
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0, -1.0); 
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, -1.0); 
    glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0, -1.0); 
    // Top Face
    glColor3f(0.0, 1.0, 0.0); // grn
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0); 
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0); 
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0,  1.0,  1.0); 
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0,  1.0,  1.0); 
    // Bottom Face
    glColor3f(1.0, 0.0, 1.0); // mag
    glTexCoord2f(0.0, 1.0); glVertex3f( 1.0, -1.0, -1.0); 
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, -1.0, -1.0); 
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0); 
    glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0); 

    // Right face
    glColor3f(0.0, 0.0, 1.0); // blu
    glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0); 
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0); 
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0); 
    glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0); 

    // Left Face
    glColor3f(1.0, 1.0, 0.0); // yel
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0); 
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0); 
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0); 
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0); 
    glEnd();
    
    glDisable( GL_TEXTURE_2D );
}



#endif

