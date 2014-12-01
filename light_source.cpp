/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

#define AMB 1
#define DIFF 1
#define SPEC 1

void PointLight::shade( Ray3D& ray ) {

	Vector3D light_to_intersection = ray.intersection.point - this->_pos;
	double intersection_dot_normal = -light_to_intersection.dot(ray.intersection.normal);

	#if AMB
	if(!ray.amb) ray.col = ray.intersection.mat->ambient;
	#endif

	#if DIFF
	
	if(intersection_dot_normal > 0) ray.col = ray.col + ray.intersection.mat->diffuse;
	#endif

	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

}

