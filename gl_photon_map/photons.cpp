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
#include <math.h>
#include "photons.h"

// Create the photon map.
void photon_manager::map_create(unsigned int num_photons)
{
	photon p;
	for(unsigned int i = 0; i < num_photons; i++)
	{
		// Initial direction

		double theta = 0.0;
		double phi = 0.0;
		
		theta 	= random(0, M_PI);
		phi 	= random(0, 2*M_PI);

		p.d[0] = sin(theta)*sin(phi);
		p.d[1] = cos(theta);
		p.d[2] = sin(theta)*cos(phi);

		// Initial position

		p.p[0] = light_pos[0];
		p.p[1] = light_pos[1];
		p.p[2] = light_pos[2];

		// Initial colour

		p.c[0] = light_col[0];
		p.c[1] = light_col[1];
		p.c[2] = light_col[2];

		// Follow the photon through the scene, trying to determine
		// whether it hits anything or not.

		p.age = 0;
		photon_trace(p);
	}
}

// Draws the photon map.
void photon_manager::map_draw(void)
{
	for(size_t i = 0; i < events.size(); i++)
	{
		glPushMatrix();

		glBegin(GL_POINTS);
		glColor3f( events[i].c[0], events[i].c[1], events[i].c[2]);
		glVertex3f(events[i].p[0], events[i].p[1], events[i].p[2]);
		glEnd();
		
		glPopMatrix();
	}
}

// Traces a photon through the scene, thereby generating photon map events of
// all kinds.
void photon_manager::photon_trace(photon p)
{
	// We don't want old people here.
	if(p.age > max_age)
		return;

	// Compute the probabilities for the photons to be reflected etc.
	
	double Pr_plane = 0.8;
	double Pd_plane = Pr_plane*(plane_d[0]+plane_d[1]+plane_d[2])/(plane_d[0]+plane_d[1]+plane_d[2]+plane_s[0]+plane_s[1]+plane_s[2]);
	double Ps_plane = Pr_plane-Pd_plane;

	double Pr_sphere = 1.0;
	double Pd_sphere = (sphere_d[0]+sphere_d[1]+sphere_d[2])/(sphere_d[0]+sphere_d[1]+sphere_d[2]+sphere_s[0]+sphere_s[1]+sphere_s[2]);
	double Ps_sphere = Pr_sphere-Pd_sphere;
	
	// Check for collisions with the sphere

	Vector<3> m;
	m[0] = sphere_center[0];
	m[1] = sphere_center[1];
	m[2] = sphere_center[2];
	
	double D = pow((p.p-m)*p.d, 2)-p.d*p.d*((p.p-m)*(p.p-m)-sphere_radius*sphere_radius);
	if(D > 0)
	{
		// Solve the sphere equation

		double a = p.d*p.d;
		double b = 2*(p.d[0]*(p.p[0]-m[0])+p.d[1]*(p.p[1]-m[1])+p.d[2]*(p.p[2]-m[2]));
		double c = (p.p-m)*(p.p-m)-sphere_radius*sphere_radius;

		double t1 = (-b+sqrt(b*b-4*a*c))/(2*a);
		double t2 = (-b-sqrt(b*b-4*a*c))/(2*a);

		Vector<3> p1 = p.p + p.d*t1;
		Vector<3> p2 = p.p + p.d*t2;

		// Only choose a point that is in the photon's ray
		if(t1 < 0.0)
			t1 = t2;
		if(t2 < 0.0)
			t2 = t1;	

		if(t1 >= 0.0 || t2 >= 0.0)
		{
			// Choose the nearest point
			if(t1 <= t2)
				p.p = p1;
			else
				p.p = p2;	
			
			// Prepare the settings for the sphere
			
			Vector<3> colour; // black
			Vector<3> normal = p.p;

			photon_collide(p, Pd_sphere, Ps_sphere, normal, colour, false);
			return;
		}
	}

	// Check for collisions with the "walls"
	
	for(int i = 0; i < 5*3; i += 3)
	{
		Vector<3> normal;
		Vector<3> colour;

		normal[0] = plane_normals[i];
		normal[1] = plane_normals[i+1];
		normal[2] = plane_normals[i+2];

		colour[0] = plane_colours[i];
		colour[1] = plane_colours[i+1];
		colour[2] = plane_colours[i+2];

		// Check for collisions...

		if((normal[0]*p.d[0]+normal[1]*p.d[1]+normal[2]*p.d[2]) != 0)
		{
			double t = (plane_distance_origin-normal[0]*p.p[0]
							 -normal[1]*p.p[1]
							 -normal[2]*p.p[2])
					/(normal[0]*p.d[0]
					 +normal[1]*p.d[1]
					 +normal[2]*p.d[2]);
			if(t > 0)
			{
				// Check whether the point is actually in the bounds of
				// the plane

				Vector<3> pt  = p.p + p.d*t;
				if(	fabs(pt[0]) <= 1.0 && 
					fabs(pt[1]) <= 1.0 &&
					fabs(pt[2]) <= 1.0)
				{
					p.p = pt;
					photon_collide(p, Pd_plane, Ps_plane, normal, colour);
				}
			}
		}
	}
}

// Handle a photon collision.
void photon_manager::photon_collide(const photon& p, double Pd, double Ps, Vector<3> normal, Vector<3> colour, bool show_reflection)
{
	double xi = static_cast<double>(rand())/static_cast<double>((RAND_MAX));
	photon q;
	photon_event e;

	// Choose the type of collision response, i.e.: diffuse/specular reflection or
	// absorption
	if(xi <= Pd)
	{
		// diffuse reflection: choose a random direction for the new photon

		double theta = 0.0;
		double phi = 0.0;
	
		theta 	= random(0, M_PI);
		phi 	= random(0, 2*M_PI);
		
		q.d[0] = sin(theta)*sin(phi);
		q.d[1] = cos(theta);
		q.d[2] = sin(theta)*cos(phi);

		q.c 	= colour;
		q.p 	= p.p;
		q.age 	= p.age + 1;

		e.p	= p.p;
		e.c 	= p.c;
		if(show_reflection)
			events.push_back(e);
		
		photon_trace(q);
	}

	else if(xi > Pd && xi <= Ps+Pd)
	{
		// reflection

		q.p 	= p.p;
		q.d 	= p.d - normal*2*(p.d*normal);
		q.c 	= p.c;
		q.age 	= p.age + 1;

		e.c 	= p.c;
		e.p	= p.p;

		if(show_reflection)
			events.push_back(e);
		
		photon_trace(q);
	}
	else
	{
		// absorption
		e.c = p.c;
		e.p = p.p;
		events.push_back(e);	
	}
}

// My own random number generator. Wheeee....
double photon_manager::random(double min, double max)
{
	return(::random()/(static_cast<double>(RAND_MAX)+1.0)*(max-min)+min);
}
