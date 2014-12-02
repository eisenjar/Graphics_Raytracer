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


	if(ray.intersection.none == true || distance(ray.intersection.point, ray.origin) > distance(modelToWorld * POI, ray.origin)) {
		ray.intersection.point = modelToWorld * POI;

		ray.intersection.none = false;

		ray.intersection.normal = transNorm(worldToModel,normal);
		ray.intersection.normal.normalize();

		return true;
	}
	else return false;
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

	double d;

	//Equation of the intersection of a line and a sphere, we don't need both intersections, just the nearest one
	double d1 = (-(d_vec.dot(o_vec)) - std::sqrt( std::pow(d_vec.dot(o_vec),2) - d_vec.dot(d_vec) * ( o_vec.dot(o_vec) - 1)))/(d_vec.dot(d_vec));
	double d2 = (-(d_vec.dot(o_vec)) + std::sqrt( std::pow(d_vec.dot(o_vec),2) - d_vec.dot(d_vec) * ( o_vec.dot(o_vec) - 1)))/(d_vec.dot(d_vec));
	
	//This is true if d is NaN 
	if(d1!=d1 || d2!=d2)
	{
		ray.dir = modelToWorld * ray.dir;
		ray.origin = original_origin;
		return false;
	}

	
	//We want the smaller intersection
	if(d1 < d2)
		d = d1;
	else
		d = d2;

	//Intersection is behind
	if(d < 0)
	{
		ray.dir = modelToWorld * ray.dir;
		ray.origin = original_origin;
		return false;
	}

	Point3D temp = ray.origin + d * ray.dir;
	if(ray.intersection.none == true || distance(ray.intersection.point, original_origin) > distance(modelToWorld * temp, original_origin)) {
		ray.intersection.point = modelToWorld * temp;
		ray.intersection.none = false;

		ray.intersection.normal = transNorm(worldToModel, temp-origin);
		ray.intersection.normal.normalize();

		//restore
		ray.dir = modelToWorld * ray.dir;
		ray.origin = original_origin;

		return true;
	}
	ray.dir = modelToWorld * ray.dir;
	ray.origin = original_origin;
	return false;
}

bool UnitCylinder::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	//UnitCylinder should have r=1, centered on the z-axis, going from z=-0.5 to z=0.5
	
	Point3D new_origin = worldToModel * ray.origin;
	Vector3D new_dir = worldToModel * ray.dir;
	
	double c = std::pow(new_origin.m_data[0], 2) + std::pow(new_origin.m_data[1], 2) - 1;

	double b = 2*(new_origin.m_data[0]*new_dir.m_data[0] + new_origin.m_data[1]*new_dir.m_data[1]);
	double a = std::pow(new_dir.m_data[0], 2) + std::pow(new_dir.m_data[1], 2);
	if(a == 0) return false;
	double discriminant = std::pow(b, 2) - 4*a*c;
	if(discriminant < 0) return false;

	//two roots
	double scale_factor1 = (-b + std::sqrt(discriminant))/(2*a);
	double scale_factor2 = (-b - std::sqrt(discriminant))/(2*a);

	double scale_factor;

	//chosen root must be positive because it is a ray and not a line, and it must be the closest one
	if(scale_factor1 < 0 && scale_factor2 < 0) return false;
	else if(scale_factor1 < 0) scale_factor = scale_factor2;
	else if(scale_factor2 < 0) scale_factor = scale_factor1;
	else scale_factor = (scale_factor1 < scale_factor2)? scale_factor1 : scale_factor2; //both are positive, so choose the smaller one

	Vector3D scaled_dir = scale_factor*new_dir;
	Point3D POI = new_origin + scaled_dir;

	Vector3D normal(POI.m_data[0],POI.m_data[1],0); //
	normal.normalize();

	if(std::abs(POI.m_data[2]) > 0.5) return false; //make sure it's not outside the bounds

	//std::cout << "This: " << (distance(modelToWorld * POI, ray.origin)) << std::endl;

	if(ray.intersection.none == true || distance(ray.intersection.point, ray.origin) > distance(modelToWorld * POI, ray.origin)) {

		//std::cout << "a: " << a << ", b: " << b << ", c: " << c << std::endl;
		ray.intersection.point = modelToWorld * POI;

		ray.intersection.none = false;

		ray.intersection.normal = transNorm(worldToModel,normal);
		ray.intersection.normal.normalize();

		return true;
	}
	else return false;
}

bool UnitCircle::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

	Vector3D normal(0,0,1);
	
	Point3D new_origin = worldToModel * ray.origin;
	Vector3D new_dir = worldToModel * ray.dir;
	
	//backface cull; modify this if necessary
	if(new_origin.m_data[2] * new_dir.m_data[2] > 0) return false;

	double scale_factor = -new_origin.m_data[2]/new_dir.m_data[2];
	Vector3D scaled_dir = scale_factor*new_dir;
	Point3D POI = new_origin + scaled_dir;

	if(std::sqrt(pow(POI.m_data[0],2)+pow(POI.m_data[1],2)) > 1) return false; //make sure it's not outside the bounds


	if(ray.intersection.none == true || distance(ray.intersection.point, ray.origin) > distance(modelToWorld * POI, ray.origin)) {
		ray.intersection.point = modelToWorld * POI;

		ray.intersection.none = false;

		ray.intersection.normal = transNorm(worldToModel,normal);
		ray.intersection.normal.normalize();

		return true;
	}
	else return false;

}
