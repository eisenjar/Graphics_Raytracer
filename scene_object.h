/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		classes defining primitives in the scene

***********************************************************/

#include "util.h"

// All primitives should provide a intersection function.  
// To create more primitives, inherit from SceneObject.
// Namely, you can create, Sphere, Cylinder, etc... classes
// here.
class SceneObject {
public:
	// Returns true if an intersection occured, false otherwise.
	virtual bool intersect( Ray3D&, const Matrix4x4&, const Matrix4x4& ) = 0;

	//if the object has been texture mapped, get the color
	virtual Colour get_color( Point3D intersection ) = 0;

	SceneObject() {t_mapped = false;}

	//texture mapping
	unsigned char **rarray; unsigned char **garray; unsigned char **barray;
	unsigned long int i_width; long int i_height;
	bool t_mapped;
};

// Example primitive you can create, this is a unit square on 
// the xy-plane.
class UnitSquare : public SceneObject {
public:
	bool intersect( Ray3D& ray, const Matrix4x4& worldToModel,
			const Matrix4x4& modelToWorld );

	Colour get_color( Point3D intersection );

};

class UnitSphere : public SceneObject {
public:
	bool intersect( Ray3D& ray, const Matrix4x4& worldToModel,
			const Matrix4x4& modelToWorld );

	Colour get_color( Point3D intersection );
};

class UnitCylinder : public SceneObject {
public:
	bool intersect( Ray3D& ray, const Matrix4x4& worldToModel,
			const Matrix4x4& modelToWorld );

	Colour get_color( Point3D intersection );
};

class UnitCircle : public SceneObject {
public:
	bool intersect( Ray3D& ray, const Matrix4x4& worldToModel,
			const Matrix4x4& modelToWorld );

	Colour get_color( Point3D intersection );
};
