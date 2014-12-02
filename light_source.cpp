/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

//turn lighting components on or off
#define AMB 1
#define DIFF 1
#define SPEC 1


void PointLight::shade( Ray3D& ray, bool shouldshade, float phong_factor ) {

	Vector3D light_to_intersection = ray.intersection.point - this->_pos;
	light_to_intersection.normalize();
	double intersection_dot_normal = -light_to_intersection.dot(ray.intersection.normal);

	Vector3D light_dir = _pos - ray.intersection.point;
	light_dir.normalize();
	
	Vector3D reflect_dir =  2 * (light_dir.dot(ray.intersection.normal)) * ray.intersection.normal - light_dir;
	reflect_dir.normalize();
	
	Vector3D view_dir = -ray.dir;
	view_dir.normalize();
	
	#if AMB
		ray.col = ray.col + ray.intersection.mat->ambient*this->_col_ambient;
	#endif
	
	if(shouldshade)
	{
	#if DIFF
		if(intersection_dot_normal > 0) 
			ray.col = ray.col + phong_factor*intersection_dot_normal*ray.intersection.mat->diffuse*this->_col_diffuse;
		#endif
	


		#if SPEC
		double specular_component =  view_dir.dot(reflect_dir);
		if(specular_component > 0)
		{
			double specular_component_full = std::pow(specular_component,ray.intersection.mat->specular_exp);
			ray.col = ray.col + phong_factor*specular_component_full*ray.intersection.mat->specular*this->_col_specular;
		}
		#endif
	}
	
	ray.reflect_dir = ray.dir - 2*(ray.dir.dot(ray.intersection.normal)) * ray.intersection.normal;
	ray.reflect_dir.normalize();
	//ray.bounce = true;
	ray.col.clamp();

}

