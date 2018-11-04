/*
* gl_maze
*
* A laughable attempt at creating a small OpenGL game. Actually just
* an assignment concerning keyboard handling with OpenGL and GLUT, but
* I got carried away and added textures, a ridiculous story etc.
*
* You might want to disable the calls to xmessage if you want to keep
* your sanity.
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

#include "texture.h"

using namespace std;

// Auxiliary functions

#define SIGN(x) (x == 0? 0.0 : (x > 0? 1.0 : -1.0))

// Window-related variables

const char* w_name = "E13: Tastaturabfrage";

const double w_z_near = 0.01;
const double w_z_far = 100.0;

const float w_fog_colour[]	= {1.0, 0.0, 0.0, 1.0};
const float w_fog_density 	= 0.25;

int w_self 	= 0;
int w_width 	= 600;
int w_height 	= 600;
int w_last_x 	= 0.5*w_width;
int w_last_y	= 0.5*w_height;

// Rotation/Movement variables

double w_theta 	= 0.0;
double w_phi 	= 0.0;

double x, y, z 		= 0.0;	// Position variables
double nx, ny, nz	= 0.0;	// Normal variables

// Texture variables

GLuint textures[5];
tex_info walls[4];
tex_info ground;

// If a value is true in this array, the corresponding
// key is pressed. The mapping is:
//
// w --> 0
// s --> 3
// a --> 1
// d --> 2
bool keys[4] = {false};
bool print_collision = false;

// Timing/collision variables

double dt		= 0.0;
double time_total 	= 0.0;
double collision_offset = 0.1;

unsigned long fps 	= 0;
unsigned long frames 	= 0;

// Maze variables and macros

const unsigned char MAZE_LEFT 		= 0x01;
const unsigned char MAZE_RIGHT 		= 0x02;
const unsigned char MAZE_BOTTOM		= 0x04;
const unsigned char MAZE_TOP 		= 0x08;
const unsigned char MAZE_VISITED 	= 0x10;
const unsigned char MAZE_COMPLETE	= 0x20;

#define LEFT(x) 	(x & MAZE_LEFT)
#define RIGHT(x)	(x & MAZE_RIGHT)
#define TOP(x)		(x & MAZE_TOP)
#define BOTTOM(x)	(x & MAZE_BOTTOM)
#define VISITED(x)	(x & MAZE_VISITED)
#define COMPLETE(x)	(x & MAZE_COMPLETE)

#define SET_LEFT(x)	(x |= MAZE_LEFT)
#define SET_RIGHT(x)	(x |= MAZE_RIGHT)
#define SET_TOP(x)	(x |= MAZE_TOP)
#define SET_BOTTOM(x)	(x |= MAZE_BOTTOM)
#define SET_VISITED(x)	(x |= MAZE_VISITED)
#define SET_COMPLETE(x)	(x |= MAZE_COMPLETE)

#define UNSET_LEFT(x)	(x &= ~MAZE_LEFT)
#define UNSET_RIGHT(x)	(x &= ~MAZE_RIGHT)
#define UNSET_TOP(x)	(x &= ~MAZE_TOP)
#define UNSET_BOTTOM(x)	(x &= ~MAZE_BOTTOM)

const unsigned int MAZE_X = 7;
const unsigned int MAZE_Y = 7;

unsigned int exit_x = 0;
unsigned int exit_y = 0;

unsigned int maze[MAZE_X][MAZE_Y];
unsigned int maze_textures[MAZE_X][MAZE_Y];

void k_callback(unsigned char key, int x, int y);

// Generates a random maze using the "Hunt-and-Kill"-Algorithm.
void maze_gen(void)
{
	for(unsigned int x = 0; x < MAZE_X; x++)
		for(unsigned int y = 0; y < MAZE_X; y++)
			maze[x][y] = (MAZE_TOP | MAZE_BOTTOM | MAZE_LEFT | MAZE_RIGHT); 
		
	long x;
	long y;
	unsigned int direction;
	unsigned int valid_directions;

	x = random() % MAZE_X;
	y = random() % MAZE_Y;

	while(1)
	{
		SET_VISITED(maze[x][y]);
		maze_textures[x][y] = random() % 4;

		// Check valid directions and iterate the process
		// if no valid directions are found
		
		valid_directions = MAZE_TOP | MAZE_RIGHT | MAZE_BOTTOM | MAZE_LEFT;

		if(	(x + 1) >= static_cast<long>(MAZE_X) || 
			VISITED(maze[x+1][y]) != 0)
			valid_directions &= ~MAZE_RIGHT;
		if(	(x - 1) < 0 ||
			VISITED(maze[x-1][y]) != 0)
			valid_directions &= ~MAZE_LEFT;
		if(	(y + 1) >= static_cast<long>(MAZE_Y) ||
			VISITED(maze[x][y+1]) != 0)
			valid_directions &= ~MAZE_TOP;
		if(	(y - 1) < 0 ||
			VISITED(maze[x][y-1]) != 0)
			valid_directions &= ~MAZE_BOTTOM;

		if(valid_directions == 0)
		{
			SET_COMPLETE(maze[x][y]);
			
			// Search the next visited cell that is NOT complete

			bool found = false;
			for(unsigned int i = 0; i < MAZE_X; i++)
			{
				for(unsigned int j = 0; j < MAZE_Y; j++)
				{
					if(VISITED(maze[i][j]) != 0 && COMPLETE(maze[i][j]) == 0)
					{
						x = i;
						y = j;

						found = true;
						break;
					}
				}
			}


			// If there is no incomplete cell left, quit the algorithm
			if(!found)
				break;

			continue;
		}

		// Is the chosen direction valid? If not, choose
		// a direction at random until a suitable one has
		// been found	

		direction = random() % 4;
		direction = pow(2.0, static_cast<int>(direction));
				 
		while((valid_directions & direction) == 0)
		{
			direction = random() % 4;
			direction = pow(2.0, static_cast<int>(direction));
		}

		// Create a corridor between the chosen cell
		// and the current cell

		if(TOP(direction))
		{
			UNSET_TOP(maze[x][y]);
			UNSET_BOTTOM(maze[x][y+1]);
			
			y++;
		}
		else if(BOTTOM(direction))
		{
			UNSET_BOTTOM(maze[x][y]);
			UNSET_TOP(maze[x][y-1]);
			
			y--;
		}
		else if(LEFT(direction))
		{
			UNSET_LEFT(maze[x][y]);
			UNSET_RIGHT(maze[x-1][y]);
			
			x--;
		}
		else
		{
			UNSET_RIGHT(maze[x][y]);
			UNSET_LEFT(maze[x+1][y]);

			x++;
		}
	}

	// Clean the maze: Remove double walls; these will not look good when rendered
	// later.
	for(unsigned int x = 0; x < MAZE_X; x++)
	{
		for(unsigned int y = 0; y < MAZE_Y; y++)
		{
			if(y < MAZE_Y - 1)
				if(TOP(maze[x][y]) && BOTTOM(maze[x][y+1]))
					UNSET_TOP(maze[x][y]);
		
			if(x < MAZE_X - 1)	
				if(RIGHT(maze[x][y]) && LEFT(maze[x+1][y]))
					UNSET_RIGHT(maze[x][y]);
		}
	}
}

// Draws a cell inside the maze. The "direction" parameter defines the placement
// of the walls.
void maze_draw_cell(unsigned int direction)
{
	glPushMatrix();
	glBegin(GL_QUADS);
	
	if(TOP(direction))
	{
		// DEBUG
		//glColor3f(1.0, 1.0, 1.0);
		
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(1.0, 0.0, 1.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(0.0, 1.0, 1.0);
	}

	if(BOTTOM(direction))
	{
		// DEBUG
		//glColor3f(1.0, 0.0, 0.0);
		
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(1.0, 1.0, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(0.0, 1.0, 0.0);
	}

	if(LEFT(direction))	
	{
		// DEBUG
		//glColor3f(0.0, 1.0, 0.0);
		
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(0.0, 1.0, 1.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(0.0, 1.0, 0.0);
	}
	
	if(RIGHT(direction))
	{
		// DEBUG
		//glColor3f(0.0, 0.0, 1.0);

		glTexCoord2f(0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(1.0, 0.0, 1.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(1.0, 1.0, 0.0);
	}

	glEnd();

	// A cell always has a ground tile
	
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 0.0, 1.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, 0.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, 1.0, 1.0);

	glEnd();
	glPopMatrix();
}

// Draws a generated maze
void maze_draw(void)
{
	for(unsigned int x = 0; x < MAZE_X; x++)
	{
		for(unsigned int y = 0; y < MAZE_Y; y++)
		{
			glPushMatrix();
			glColor3f(1.0, 1.0, 1.0);
			glTranslatef(x, -0.5, y);
	
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textures[maze_textures[x][y]]);	
			maze_draw_cell(maze[x][y]);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		}
	}
}

// Check whether the player would collide with a wall. pos_x and pos_y refer to
// the player's position in the maze, i.e. y is actually the z-coordinate.
long check_collision(double pos_x, double pos_y)
{
	long m_x = floor(pos_x);
	long m_y = floor(pos_y);

	if(	m_x < 0 || m_x >= static_cast<long>(MAZE_X) ||
		m_y < 0 || m_y >= static_cast<long>(MAZE_Y))
		return(true);

	double new_x = x	+keys[0]*nx*1.0*0.1 
				-keys[3]*nx*1.0*0.1
				+keys[1]*nz*1.0*0.1
				-keys[2]*nz*1.0*0.1;
		
	double new_y = z	+keys[0]*nz*1.0*0.1
				-keys[3]*nz*1.0*0.1
				-keys[1]*nx*1.0*0.1
				+keys[2]*nx*1.0*0.1;
	
	new_x = floor(new_x);
	new_y = floor(new_y);

	if(print_collision)
		cout << "From: " << m_x << "," << m_y << " to " << new_x << "," << new_y << "\n";

	// Moving east...
	if(new_x > m_x)
	{
		new_x = floor(new_x + collision_offset);
		if(	LEFT(maze[m_x+1][m_y]) ||
			RIGHT(maze[m_x][m_y]))
			return(true);
	}

	// Moving west...
	else if(new_x < m_x)
	{
		new_x = floor(new_x - collision_offset);
		if(	RIGHT(maze[m_x-1][m_y]) ||
			LEFT(maze[m_x][m_y]))
			return(true);
	}

	// Moving north...
	if(new_y > m_y)
	{
		new_y = floor(new_y + collision_offset);
		if(	BOTTOM(maze[m_x][m_y+1]) ||
			TOP(maze[m_x][m_y]))
			return(true);
	}

	// Moving south...
	else if(new_y < m_y)
	{
		new_y = floor(new_y - collision_offset);
		if(	TOP(maze[m_x][m_y-1]) ||
			BOTTOM(maze[m_x][m_y]))
			return(true);
	}

	// Moving north-east
	if(new_x > m_x && new_y > m_y)
	{
		if(	LEFT(maze[m_x+1][m_y+1]) ||
			BOTTOM(maze[m_x+1][m_y+1]))
			return(true);	
	}

	// Moving south-east	
	else if(new_x > m_x && new_y < m_y)
	{
		if(	LEFT(maze[m_x+1][m_y-1]) ||
			TOP(maze[m_x+1][m_y-1]))
			return(true);
	}

	// Moving south-west
	else if(new_x < m_x && new_y < m_y)
	{
		if(	RIGHT(maze[m_x-1][m_y-1]) ||
			TOP(maze[m_x-1][m_y-1]))
			return(true);
	}

	// Moving north-west
	else if(new_x < m_x && new_y > m_y)
	{
		if(	RIGHT(maze[m_x-1][m_y+1]) ||
			BOTTOM(maze[m_x-1][m_y+1]))
			return(true);
	}

	return(false);
}

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

// Mouse callback functions that is called whenever the mouse is moved. This
// does not depend on the buttons that are currently pressed.
void m_motion(int x, int y)
{
	// The "warped" flag tells us that we one have to evaluate
	// each second call to this function -- otherwise, the mouse
	// movement would be very tedious.
	static bool warped = false;
	if(warped)
	{
		warped = false;
		return;
	}

	w_phi   += SIGN(w_last_x-x)*fabs(0.1*(w_last_x-x))*M_PI/180.0;
	w_last_x = x;
	
	w_theta   += SIGN(w_last_y-y)*fabs(0.1*(w_last_y-y))*M_PI/180.0;
	w_last_y = y;

	if(w_theta >= 0.5*M_PI)
		w_theta = 0.5*M_PI;
	if(w_theta <= -0.5*M_PI)
		w_theta = -0.5*M_PI;	

	fmod(w_phi, 2*M_PI);
	fmod(w_theta, 0.5*M_PI);
	
	// Refresh the normals
	
	nx = sin(w_phi);
	ny = sin(w_theta);
	nz = cos(w_phi);

	// Center the mouse pointer, thus granting us more
	// precise moves.

	w_last_x = 0.5*w_width;
	w_last_y = 0.5*w_height;

	glutWarpPointer(0.5*w_width, 0.5*w_height);
	warped  = true;
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
	
	glEnable(GL_FOG);
	glFogi( GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, w_fog_colour);
	glFogf( GL_FOG_DENSITY, w_fog_density);
	glHint(GL_FOG_HINT, GL_NICEST);

	frames = 0;

	// Load the textures

	cout << "Loading textures...\n";

	load_ppm("textures/wall_01.ppm", &walls[0]);
	load_ppm("textures/wall_02.ppm", &walls[1]);
	load_ppm("textures/wall_03.ppm", &walls[2]);
	load_ppm("textures/wall_04.ppm", &walls[3]);
	load_ppm("textures/ground.ppm", &ground);


	glGenTextures(5, &textures[0]);
	for(int i = 0; i < 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D (		GL_TEXTURE_2D, 
					0, 
					GL_RGB,
					walls[i].width,
					walls[i].height,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					walls[i].data);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glTexImage2D (		GL_TEXTURE_2D, 
				0, 
				GL_RGB,
				ground.width,
				ground.height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				ground.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	cout << "Textures generated. Ready to go...\n";

	// Set the start position (a bit redundant)
	y = 0.0;
	x = z = 0.5;
	nx = sin(w_phi);
	ny = 0;
	nz = -cos(w_phi);

	// Generate the maze and the exit
	
	exit_x = (random() % (MAZE_X - 1)) + 1;
	exit_y = (random() % (MAZE_Y - 1)) + 1;
	maze_gen();

	// Position the pointer so that the mouse movement/rotation
	// functions are set up properly
	glutWarpPointer(0.5*w_width, 0.5*w_height);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	// Works better without...
	//glutFullScreen();
}

// Destroys the window. This is where customized data structures should be
// deleted.
void w_destroy(void)
{
	glutDestroyWindow(w_self);

	for(int i = 0; i < 4; i++)
		delete[] walls[i].data;
	
	delete[] ground.data;	
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

	// Check for collisions

	if(!check_collision(x,z))
	{
		// Process movement keys

		x += keys[0]*nx*1.0*dt;
		x -= keys[3]*nx*1.0*dt;

		z += keys[0]*nz*1.0*dt;
		z -= keys[3]*nz*1.0*dt;

		x += keys[1]*nz*1.0*dt;
		x -= keys[2]*nz*1.0*dt;

		z -= keys[1]*nx*1.0*dt;
		z += keys[2]*nx*1.0*dt;
	}

	// Draw...

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	// Print messages

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glDisable(GL_FOG);
	glLoadIdentity();

	glColor3f(1.0,1.0,1.0);
	gluOrtho2D(-100.0,100.0,-90.0,100.0);

	// FPS display
	
	stringstream converter;
	converter << "FPS: ";
	converter << fps;
	b_print(70.0, 90.0, converter.str().c_str());
	
	glEnable(GL_FOG);
	glPopMatrix();

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	// Set the player's position

	gluLookAt(	x,
			y,
			z,
			x+nx,
			y+ny,
			z+nz,
			0.0,
			1.0,
			0.0);

	// If the exit is read, notify the user

	if(floor(x) == exit_x && floor(z) == exit_y)
	{
		glutSetCursor(GLUT_CURSOR_DESTROY);
		system("xmessage -file end.txt");
		k_callback('q', 0, 0);

	}

	// Draw the maze and perhaps, some day, some other
	// objects.

	maze_draw();

	glPushMatrix();
	glTranslatef(exit_x+0.5, 0, exit_y+0.5);
	glColor3f(0.0, 0.0, 0.0);
	glutSolidSphere(0.35, 20, 20);
	glPopMatrix();
	
	glPopMatrix();
	glutSwapBuffers();
}

// Resets the window. This should contain everything 
// that is needed to reset the window to the beginning.
void w_reset(void)
{
	w_theta = 0.0;
	w_phi 	= 0.0;

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
		
		case 'w':
			keys[0] = true;
			break;	
	
		case 's':
			keys[3] = true;
			break;	

		case 'a':
			keys[1] = true;
			break;	
		
		case 'd':
			keys[2] = true;
			break;
		
		case 'p':
			print_collision = true;
			break;		

		case ' ':	
		case 'r':
			w_reset();
			break;

		default:
			break;

	}
	
	glutPostRedisplay();
}

// Keyboard callback function that is called whenever a key is released.
void k_callback_up(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'w':
			keys[0] = false;
			break;
		case 's':
			keys[3] = false;
			break;
		case 'a':
			keys[1] = false;
			break;
		case 'd':
			keys[2] = false;
			break;
		
		case 'p':
			print_collision = false;
			break;			
		
		default:
			break;	
	}
}

// Nothing to see here, move along...
int main(int argc, char* argv[])
{
	// Show the introduction
	system("xmessage -file begin.txt");

	w_init(argc, argv);

	glutSetWindow(w_self);
	glutSetWindow(w_self);
	glutIdleFunc(w_idle);
	glutDisplayFunc(w_display);
	glutReshapeFunc(w_reshape);
	glutKeyboardFunc(k_callback);
	glutKeyboardUpFunc(k_callback_up);
	glutPassiveMotionFunc(m_motion);
	
	glutPostRedisplay();
	glutMainLoop();
	return(0);
}
