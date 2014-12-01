/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

#define AMB 1
#define DIFF 0
#define SPEC 0

void PointLight::shade( Ray3D& ray ) {

	#if AMB
	if(!ray.amb) ray.col = ray.intersection.mat->ambient;
	#endif

	#if DIFF

	#endif

	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

}

