/*
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
#include "particles.h"

// Emits n new particles with random values. These particles will be visible
// after calling p_draw.
void particle_system::p_emit(unsigned int n)
{
	Vector<3> start_position; 	// = (0,0,0)
	Vector<3> start_colour;		// TBD
	particle p;			// particle data for the new particle

	start_colour[0] = 0.0;
	start_colour[1] = 0.0;
	start_colour[2] = 1.0;
	
	for(unsigned int i = 0; i < n; i++)
	{
		p.p 	= start_position;
		p.c 	= start_colour; 
		p.age	= 0.0;
		p.size 	= max_size;

		double radius = static_cast<double>(random() % max_radius) / 100.0;
		double angle = static_cast<double>((random()%90))*M_PI/180.0;
 
 		p.v[0] = cos(angle)*radius;
		p.v[2] = sin(angle)*radius;

		if(random() % 2)
			p.v[0] = -p.v[0];
		
		if(random() % 2)
			p.v[2] = -p.v[1];	

		p.v[1] = 0.5*0.98*(random() % 500)/100.0;
		
		particles.push_back(p);
	}
}

// Updates the particle system, i.e. perform one step in time, using dt as the
// delta for all movements.
void particle_system::p_update(double dt)
{
	// Prepare the gravity vector. It is also possible to simulate arbitrary
	// force fields here.

	Vector<3> start_position; 	// = (0,0,0)
	Vector<3> a;
	a[0] = 0.0;
	a[1] = gravity;
	a[2] = 0.0;

	for(size_t i = 0; i < particles.size(); i++)
		if(particles[i].age >= max_age)
			particles.erase(particles.begin() + i);

	// Emit new particles, if necessary
	if(particles.size() < min_particles)
		p_emit(default_emit);

	for(size_t i = 0; i < particles.size(); i++)
	{
		// Change the position of all particles; let them decay
		// over time
		particles[i].age += dt;
		particles[i].p = particles[i].v*particles[i].age + a*0.5*particles[i].age*particles[i].age;
		
		// Change the colour of the particles
		particles[i].c[0] = particles[i].age/max_age*1.0;
		particles[i].c[1] = 0.0;
		particles[i].c[2] = (1-particles[i].age/max_age);

		// Change the size of the particles
		particles[i].size = max_size*(1-particles[i].age/max_age);
	}
}

// Draws the particle system.
void particle_system::p_draw(void)
{
	glPushMatrix();
	for(size_t i = 0; i < particles.size(); i++)
	{
		double age 	= particles[i].age;
		double size 	= (1-age/max_age)/100.0;

		glColor3f(	particles[i].c[0],
				particles[i].c[1],
				particles[i].c[2]);
	// OLD:
	//
	//	glVertex3f(	particles[i].p[0],
	//			particles[i].p[1],
	//			particles[i].p[2]);


		glTranslatef(particles[i].p[0], particles[i].p[1], particles[i].p[2]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex3f(-size, size,0.0f);
		glTexCoord2f(0.0f,1.0f); glVertex3f(-size,-size,0.0f);
		glTexCoord2f(1.0f,1.0f); glVertex3f( size,-size,0.0f);
		glTexCoord2f(1.0f,0.0f); glVertex3f( size, size,0.0f);
		glEnd();
		glTranslatef(-particles[i].p[0], -particles[i].p[1], -particles[i].p[2]);

	}
	glPopMatrix();
}
