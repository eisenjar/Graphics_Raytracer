/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.

	Vector3D normal(0,0,1);
	
	Point3D new_origin = worldToModel * ray.origin;
	Vector3D new_dir = worldToModel * ray.dir;
	
	//backface cull; modify this if necessary
	if(new_origin.m_data[2] * new_dir.m_data[2] > 0) return false;

	double scale_factor = -new_origin.m_data[2]/new_dir.m_data[2];
	Vector3D scaled_dir = scale_factor*new_dir;
	Point3D POI = new_origin + scaled_dir;

	if(std::abs(POI.m_data[0]) > 0.5 || std::abs(POI.m_data[1]) > 0.5) return false; //make sure it's not outside the bounds

	ray.intersection.point = modelToWorld * POI;

	ray.intersection.none = false;
	Colour col(1.0, 0.0, 0.0);
	ray.col = col;

	ray.intersection.normal = transNorm(worldToModel,normal);
	ray.intersection.normal.normalize();

	return true;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.

	//not sure if this is right
	Point3D original_origin = ray.origin;

	ray.origin = worldToModel * ray.origin;
	ray.dir = worldToModel * ray.dir;

	Point3D origin(0,0,0);
	Vector3D o_vec = ray.origin - origin;
	Vector3D d_vec = ray.dir;

	double d = (-(d_vec.dot(o_vec)) + std::sqrt( std::pow(d_vec.dot(o_vec),2) - d_vec.dot(d_vec) * ( o_vec.dot(o_vec) - 1)))/(d_vec.dot(d_vec));
	
	//This is true if d is NaN 
	if(d!=d)
	{

	ray.dir = modelToWorld * ray.dir;
	ray.origin = original_origin;
		return false;
	}
	ray.intersection.point = ray.origin + d * ray.dir;
	ray.intersection.none = false;

//	std::cout << ray.intersection.point << std::endl;

	Colour col(1.0, 1.0, 1.0);
	ray.col = col;

	ray.dir = modelToWorld * ray.dir;
	ray.origin = original_origin;
	return false;
}

