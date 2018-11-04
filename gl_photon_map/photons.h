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

#ifndef PHOTONS_H
#define PHOTONS_H

#include <vector>	// This is the vector class of the STL, describing arrays
#include "vector.cpp"	// This is my Vector class, describing n-dimensional vectors

// Constants concerning the photon mapping

const double light_pos[] = {0.0, 0.0, 0.0};
const double light_col[] = {1.0, 1.0, 1.0};

// Constants for the plane

const double plane_normals[] 	= {
					 0.0, 0.0,-1.0, // behind
					 /*
					
					 We don't check this wall

					 0.0, 0.0,-1.0, // front
					 */
					-1.0, 0.0, 0.0, // left
					 1.0, 0.0, 0.0, // right
					 0.0,-1.0, 0.0, // down
					 0.0, 1.0, 0.0  // up 

};

const double plane_colours[]	= {
					1.0, 0.0, 0.0,
					0.0, 1.0, 0.0,
					0.0, 0.0, 1.0,
					0.5, 0.5, 0.5,
					0.5, 0.5, 0.5
};

const double plane_d[] = {0.7, 0.7, 0.7};
const double plane_s[] = {0.1, 0.1, 0.1};

const double plane_distance_origin 	= 1;
const double plane_distance_min		= 0.001;


// Constants for the sphere

const double sphere_center[] 	= {0.5, -0.75, 0.5};
const double sphere_d[]		= {0.4, 0.4, 0.4};
const double sphere_s[]		= {0.6, 0.6, 0.6};
const double sphere_radius 	= 0.25;

// Constants for all photons

const unsigned int max_age = 4;

// Describes a single photon that will be "tracked" through the scene and,
// ultimately, discarded because of its age.
class photon
{
	public:
		Vector<3> p; // position
		Vector<3> d; // direction
		Vector<3> c; // colour = "Energy"

		// Counts the numbers of reflections this photon
		// had to endure.
		unsigned int age;
};

// Saves a photon event, i.e. the position and the colour of a reflection etc.
// so that a global photon map may be drawn.
class photon_event
{
	public:
		Vector<3> p; // position
		Vector<3> c; // colour
};

// Manages all photons and saves their data in the photon map.
class photon_manager
{
	public:
		void map_create(unsigned int num_photons);
		void map_draw(void);

	private:
		double random(double min, double max);

		void photon_trace(photon p);
		void photon_collide(const photon& p, double Pd, double Ps, Vector<3> normal, Vector<3> colour, bool show_reflection = true);

		std::vector<photon_event> events;
};

#endif
