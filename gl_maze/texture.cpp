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

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "texture.h"

using namespace std;

// Loads one PPM file. The array is allocated -- all other memory
// operations have to be performed by the programmer!
int load_ppm(const char* filename, tex_info* texture_info )
{
	fstream file(filename, ios::in);
	if(!file.good())
	{
		cerr << "* Error while opening " << filename << ".\n";
		return(-1);
	}

	bool ascii = false;
	
	string line;
	stringstream stream;

	unsigned long width;
	unsigned long height;
	unsigned long max;
	unsigned long size = 0;
	
	// Read the header

	getline(file, line);
	if(line == "P6")
	{
		ascii = false;
		cout << "* " << filename << " is a RAW file.\n";
	}
	else if(line == "P3")
	{
		ascii = true;
		cout << "* " << filename << " is an ASCII file.\n";
	}
	else
	{
		cerr << "* Error: This file is NOT a PPM file. Quitting...\n";
		return(-1);
	}

	// Skip comments

	getline(file, line);
	while(!file.eof() && line.find("#", 0) != string::npos)
		getline(file, line);

	stream.clear();
	stream.str(line);
	stream >> width >> height;

	getline(file, line);
	stream.clear();
	stream.str(line);
	stream >> max;

	texture_info->width = width;
	texture_info->height = height;

	cout << "* width = " << width << ", height = " << height << ", max colour value = " << max << "\n";

	// Depending on the max value, a bigger array may be needed

	if(max <= 255)
	{
		size = width*height*3;
		texture_info->data = new GLubyte[size];
	}
	else
	{
		size = width*height*3*2;
		texture_info->data = new GLubyte[size];
	}

	// Now the file can be read in one big sweep...
	if(!ascii)
	{
		cout << "* Reading the file...\n";
		file.read(reinterpret_cast<char*>(texture_info->data), size);
	}
	else
		cout << "* Error: Reading ASCII files is not yet implemented.\n";

	return(0);
}

// Loads two PPM files: One is treated as the original texture, the
// other one will be used to determine an Alpha map. This allows us
// to use neat things such as masks.
int load_ppm_alpha(const char* file_texture, const char* file_mask, tex_info* info)
{
	tex_info texture_info;
	tex_info mask_info;

	load_ppm(file_texture, &texture_info);
	load_ppm(file_mask, &mask_info);

	info->data = new GLubyte[texture_info.width * texture_info.height * 4];
	info->width = texture_info.width;
	info->height = texture_info.height;


	// Mix it, baby!
	unsigned long j = 0; 
	for(unsigned long i = 0; i < texture_info.width*texture_info.height*4; i+= 4)
	{
		info->data[i]   = texture_info.data[j  ];
		info->data[i+1] = texture_info.data[j+1];
		info->data[i+2] = texture_info.data[j+2];
		info->data[i+3] = mask_info.data[j];

		j += 3;
	}

	delete[] texture_info.data;
	delete[] mask_info.data;

	return(0);
}

