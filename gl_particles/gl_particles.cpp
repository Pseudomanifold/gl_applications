/*
* gl_particles
*
* A simple particle system using OpenGL. 
*
* This code has been created for a University assignment. Don't expect 
* too much.
*
* Copyright (c) 2009, Bastian Rieck
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* - Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.  
* - Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.  
* - Neither the name of the University of Heidelberg nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include <GL/glut.h>

#include <time.h>
#include <sys/time.h>

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <ctime>

#include "particles.h"

using namespace std;

// Window-related variables

const char* w_name = "gl_particles";

const double w_z_near = 0.01;
const double w_z_far = 100.0;

int w_self 	= 0;
int w_width 	= 600;
int w_height 	= 600;

// Timing variables

double dt		= 0.0;
double time_total 	= 0.0;

unsigned long fps 	= 0;
unsigned long frames 	= 0;

particle_system particles;

// Computes the time that has passed between two subsequent calls to this
// function
double diff_seconds()
{
	struct timeval tv;
	int diese_sec;
	int diese_usec;
	double diff;
	static int letzte_sec;
	static int letzte_usec;
	static bool initialisiert=false;

	gettimeofday(&tv, NULL);

	diese_sec=tv.tv_sec;
	diese_usec=tv.tv_usec;

	if(!initialisiert)
	{
		initialisiert=true;
		letzte_sec =  diese_sec;
		letzte_usec =  diese_usec;
	}

	diff= (diese_sec-letzte_sec) + (diese_usec-letzte_usec)/1000000.0;

	letzte_sec =  diese_sec;
	letzte_usec =  diese_usec;

	return diff;
}

// Idle function. Throws redisplay interrupts.
void w_idle(void)
{
	glutPostRedisplay();
}

// Prints the string at the desired coordinates. Assumes that
// a proper raster is present (via gluOrtho2D, for example).
void b_print(double x, double y, string msg)
{
	glRasterPos2f(x, y);
	for(unsigned int i = 0; i < msg.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, msg[i]);
}

// Contains the init function that sets all desired OpenGL
// parameters.
void w_init(int argc, char* argv[])
{
	srandom(time(NULL));
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	glutInitWindowSize(w_width, w_height);
	glutInitWindowPosition(10, 10);
	w_self = glutCreateWindow(w_name);

	glClearColor(0.0,0.0,0.0,1.0);

	glShadeModel(GL_SMOOTH);	
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	frames = 0;
}

// Destroys the window. This is where customized data structures should be
// deleted.
void w_destroy(void)
{
	glutDestroyWindow(w_self);
}

// Custom display function for the window. This is where
// the fun starts.
void w_display(void)
{
	// Compute the FPS
	
	dt = diff_seconds();

	frames++;
	time_total += dt;
	if(time_total >= 1.0)
	{
		fps = frames;
		frames = 0;
		
		time_total = 0.0;
	}

	// Emit some new particles
	if(dt != 0.0)
		particles.p_emit(5*dt*default_emit);
	
	// Draw...

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	// Print messages

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0,1.0,1.0);
	gluOrtho2D(-100.0,100.0,-90.0,100.0);
	b_print(-90,-80, "Press Q to exit the application.");

	// FPS display
	
	stringstream converter;
	converter << "FPS: ";
	converter << fps;
	b_print(70.0, 90.0, converter.str().c_str());
	
	glPopMatrix();

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	// After setting a fixed position that allows us to see the particles,
	// the particle system is calculated and one "step in time" is made,
	// thus changing the positions of all particles according to their
	// trajectories.
	
	gluLookAt(	0,
			0,
			2,
			0,
			0,
			0,
			0.0,
			1.0,
			0.0);

	glTranslatef(0.0, -0.5, 0.0);
	particles.p_update(dt);
	particles.p_draw();

	glPopMatrix();
	glutSwapBuffers();
}

// Resets the window. This should contain everything 
// that is needed to reset the window to the beginning.
void w_reset(void)
{
	glutPostRedisplay();
}

// Reshape function. The aspect ratio is recalculated so that
// the scene does not look distorted.
void w_reshape(GLsizei width, GLsizei height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	w_width = width;
	w_height = height;

	gluPerspective(45.0, width/static_cast<GLfloat>(height), w_z_near, w_z_far);
}

// Keyboard callback function that is called whenever a key has been pressed.
void k_callback(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'q':
			w_destroy();
			exit(0);
		
		case ' ':	
		case 'r':
			w_reset();
			break;

		default:
			break;

	}
	
	glutPostRedisplay();
}

// Nothing to see here, move along...
int main(int argc, char* argv[])
{
	w_init(argc, argv);

	glutSetWindow(w_self);
	glutIdleFunc(w_idle);
	glutDisplayFunc(w_display);
	glutReshapeFunc(w_reshape);
	glutKeyboardFunc(k_callback);
	
	glutPostRedisplay();
	glutMainLoop();
	return(0);
}
