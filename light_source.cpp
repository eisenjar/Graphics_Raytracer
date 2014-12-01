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
	if(ray.col.m_data[0] == 0 && ray.col.m_data[1] == 0 && ray.col.m_data[2] == 0) ray.col = ray.col + ray.intersection.mat->ambient; //make sure ambient light hasn't already been added
	#endif

	#if DIFF
	if(intersection_dot_normal > 0) ray.col = ray.col + ray.intersection.mat->diffuse;
	#endif

	ray.col.clamp();

}

