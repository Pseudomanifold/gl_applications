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

#include <iostream>
#include <cmath>

#include "vector.h"

using namespace std;

template<unsigned long n> Vector<n>::Vector(void)
{
	for(unsigned long i = 0; i < n; i++)
		v[i] = 0;
}

template<unsigned long n> Vector<n> Vector<n>::operator+(const Vector a) const
{
	Vector<n> res;
	for(unsigned long i = 0; i < n; i++)
		res.v[i] = v[i] + a.v[i];

	return(res);
}

template<unsigned long n> Vector<n>& Vector<n>::operator+=(const Vector<n> a)
{
	for(unsigned long i = 0; i < n; i++)
		v[i] += a.v[i];

	return(*this);
}

template<unsigned long n> Vector<n> Vector<n>::operator-(const Vector<n> a) const
{
	Vector<n> res;
	for(unsigned long i = 0; i < n; i++)
		res.v[i] = v[i] - a.v[i];

	return(res);	
}

template<unsigned long n> Vector<n>&  Vector<n>::operator-=(const Vector<n> a)
{
	for(unsigned long i = 0; i < n; i++)
		v[i] -= a.v[i];

	return(*this);
}

template<unsigned long n> Vector<n> Vector<n>::operator*(double a) const
{
	Vector<n> res;
	for(unsigned long i = 0; i < n; i++)
		res.v[i] = v[i] * a;

	return(res);
}

template<unsigned long n> Vector<n>& Vector<n>::operator*=(double a)
{
	for(unsigned long i = 0; i < n; i++)
		v[i] *= a;

	return(*this);
}

template<unsigned long n> Vector<n> Vector<n>::operator/(double a) const
{
	if(a == 0.0)
		throw "Attempted division by zero. Very bad.\n";
	else
		return(operator*(1/a));
}

template<unsigned long n> Vector<n>& Vector<n>::operator/=(double a)
{
	if(a == 0.0)
		throw "Attempted division by zero. Very bad.\n";
	else
		return(operator*=(1/a));	
}

template<unsigned long n> Vector<n>& Vector<n>::operator=(const Vector a)
{
	for(unsigned long i = 0; i < n; i++)
		v[i] = a.v[i];

	return(*this);
}

// Cross product
template<unsigned long n> Vector<n> Vector<n>::operator|(const Vector a) const
{
	if( n != 3 )
		cout << "WARNING: Attempting to use the cross product with non-3D vectors.\n";

	Vector<n> res;

	res.v[0] = v[1]*a.v[2]-v[2]*a.v[1];
	res.v[1] = v[2]*a.v[0]-v[0]*a.v[2];
	res.v[2] = v[0]*a.v[1]-v[1]*a.v[0]; 

	return(res);
}

template<unsigned long n> double& Vector<n>::operator[](unsigned long i) 
{
	if(i < n)
		return(v[i]);
	else
		throw "Index out of range!\n";
}

template<unsigned long n> Vector<n> Vector<n>::normalize(void)
{
	if( length() == 0 )
		return(*this);
	else
		return(operator/(length()));
}

// Euclidian length
template<unsigned long n> double Vector<n>::length(void)
{
	double len = 0.0;
	for(unsigned long i = 0; i < n; i++)
		 length += v[i]*v[i];

	return( sqrt(len) );
}
