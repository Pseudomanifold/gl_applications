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

#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>	// This is the vector class of the STL, describing arrays
#include "vector.cpp"	// This is my Vector class, describing n-dimensional vectors

// Default values for the particle system. These _could_ be changed from inside
// the program, but for now, it is easier to do it manually.

const double max_size 	= 9.0;
const double max_age 	= 1;
const double gravity 	= -0.90;
const unsigned int min_particles = 200;
const unsigned int default_emit = 100;
const unsigned int max_radius	= 100.0;

// Describes a single particle of the simple particle system. It's attributes
// will be tracked as time progresses and, utimately, deleted.
class particle
{
	public:
		Vector<3> p; // position
		Vector<3> v; // velocity
		Vector<3> c; // colour

		double age;
		double size; 
};


// Describes a simple particle system.
class particle_system
{
	public:
		void p_emit	(unsigned int n);
		void p_update	(double dt);
		void p_draw	(void);

	private:
		std::vector<particle> particles;	
};

#endif
