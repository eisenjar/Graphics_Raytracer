/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h, 
		and the main function which specifies the 
		scene to be rendered.	

***********************************************************/


#include "raytracer.h"
#include "bmp_io.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

#define AA 0
#define REFL 1
#define DOF 1

#define MAX_REFLECT_BOUNCES 2
#define MAX_REFRAC_BOUNCES 2
#define MAX_GLOSSINESS_RAYS 4
#define NUM_FRAMES 1

Point3D DOF_point = Point3D(-.1,-.1,-2);

int frames_rendered = 1;

Raytracer::Raytracer() : _lightSource(NULL) {
	_root = new SceneDagNode();
}

Raytracer::~Raytracer() {
	delete _root;
}

SceneDagNode* Raytracer::addObject( SceneDagNode* parent, 
		SceneObject* obj, Material* mat ) {
	SceneDagNode* node = new SceneDagNode( obj, mat );
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;
	
	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}
	
	return node;;
}

LightListNode* Raytracer::addLightSource( LightSource* light ) {
	LightListNode* tmp = _lightSource;
	_lightSource = new LightListNode( light, tmp );
	return _lightSource;
}

void Raytracer::rotate( SceneDagNode* node, char axis, double angle ) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
		    node->trans = node->trans*rotation; 	
			angle = -angle;
		} 
		else {
			node->invtrans = rotation*node->invtrans; 
		}	
	}
}



//A truly random number generator
int randomest_number()
{

			//This is not very random, but adding a mersenne twister is hard and this is not
			int rand = (std::rand()) % 10000;
			
			return rand-5000;
}

void Raytracer::translate( SceneDagNode* node, Vector3D trans ) {
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	node->trans = node->trans*translation; 	
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	node->invtrans = translation*node->invtrans; 
}

void Raytracer::scale( SceneDagNode* node, Point3D origin, double factor[3] ) {
	Matrix4x4 scale;
	
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	node->trans = node->trans*scale; 	
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	node->invtrans = scale*node->invtrans; 
}

Matrix4x4 Raytracer::initInvViewMatrix( Point3D eye, Vector3D view, 
		Vector3D up ) {
	Matrix4x4 mat; 
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];

	return mat; 
}

void Raytracer::traverseScene( SceneDagNode* node, Ray3D& ray ) {
	SceneDagNode *childPtr;

	// Applies transformation of the current node to the global
	// transformation matrices.
	_modelToWorld = _modelToWorld*node->trans;
	_worldToModel = node->invtrans*_worldToModel; 
	if (node->obj) {
		// Perform intersection.
		if (node->obj->intersect(ray, _worldToModel, _modelToWorld)) {
			ray.intersection.mat = node->mat;
			if(node->obj->t_mapped)
			{
				//replace the colors of the material with those found in the texture map
				Colour map_colour = node->obj->get_color(ray.intersection.untransformed);
				ray.intersection.mat->diffuse /*= ray.intersection.mat->diffuse = ray.intersection.mat->specular */= map_colour;
				//std::cout << ray.intersection.mat->ambient << "\n\n";
			}
		}
	}
	// Traverse the children.
	childPtr = node->child;
	while (childPtr != NULL) {
		traverseScene(childPtr, ray);
		childPtr = childPtr->next;
	}

	// Removes transformation of the current node from the global
	// transformation matrices.
	_worldToModel = node->trans*_worldToModel;
	_modelToWorld = _modelToWorld*node->invtrans;
}

void Raytracer::computeShading( Ray3D& ray ) {
	LightListNode* curLight = _lightSource;
	for (;;) {
		if (curLight == NULL) break;

		// Implement shadows here if needed.

		Point3D light_pos = curLight->light->get_position();
		
		
		Point3D intersection_point = ray.intersection.point;
		
		Vector3D shadow_dir = light_pos - intersection_point;
		shadow_dir.normalize();
		Ray3D shadow_ray(intersection_point + .1*shadow_dir,shadow_dir);
		shadow_ray.dir.normalize();

		traverseScene(_root, shadow_ray);

		Point3D shadow_intersection_point = shadow_ray.intersection.point; 

		//Disables specular/diffuse lighting when false
		bool use_phong = false;
		float phong_factor = 1.0;

		//Checks if the ray pointing towards the light hits another object, or a object far away
		if(shadow_ray.intersection.none || distance(intersection_point,light_pos) < distance(shadow_intersection_point,intersection_point))
			use_phong = true;

		if(!shadow_ray.intersection.none && shadow_ray.intersection.mat->transparency)
		{
			use_phong = true;
			phong_factor = shadow_ray.intersection.mat->transparency;
		}
		curLight->light->shade(ray,use_phong,phong_factor);
		curLight = curLight->next;
	}

}

void Raytracer::initPixelBuffer() {
	int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
	_rbuffer = new unsigned char[numbytes];
	_gbuffer = new unsigned char[numbytes];
	_bbuffer = new unsigned char[numbytes];
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			_rbuffer[i*_scrWidth+j] = 0;
			_gbuffer[i*_scrWidth+j] = 0;
			_bbuffer[i*_scrWidth+j] = 0;
		}
	}
}

void Raytracer::flushPixelBuffer( char *file_name ) {
	bmp_write( file_name, _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer );
	delete _rbuffer;
	delete _gbuffer;
	delete _bbuffer;
}

Colour Raytracer::shadeRay( Ray3D& ray ) {
	Colour col(0.0, 0.0, 0.0); 
	traverseScene(_root, ray); 
	
	// Don't bother shading if the ray didn't hit 
	// anything.
	if (!ray.intersection.none) {
		computeShading(ray); 
		col =  1.1*ray.col;
		
		//For the glossiness
		float random = 0.0;
		int i;

		for ( i = 0; i < MAX_GLOSSINESS_RAYS; i++)
		{	
			if(ray.intersection.mat->glossiness)
			{
				if( i == 0)
				col = ((1-ray.intersection.mat->reflectiveness))*col;
				random = ray.intersection.mat->glossiness * randomest_number()/50000.0;
			}
			else //don't shoot any glossiness rays because the material isn't glossy 
				i = MAX_GLOSSINESS_RAYS;

			//Could probably be done better
			Vector3D reflection_dir= ray.reflect_dir + ( Vector3D(random,random,random));
			reflection_dir.normalize();

			Ray3D reflection((ray.intersection.point + (.01*reflection_dir)), reflection_dir);
			reflection.reflect_bounce = ray.reflect_bounce + 1; //make sure we finish sometime
			if(reflection.reflect_bounce <= MAX_REFLECT_BOUNCES) {
				Colour colorReflect = 1.1*ray.intersection.mat->reflectiveness*shadeRay(reflection);
				colorReflect.clamp();
				if(!ray.intersection.mat->glossiness)
					col = ((1.-ray.intersection.mat->reflectiveness)*col + colorReflect);	
				else
				{
					col[0] += (colorReflect)[0]/MAX_GLOSSINESS_RAYS;	
					col[1] += (colorReflect)[1]/MAX_GLOSSINESS_RAYS;	
					col[2] += (colorReflect)[2]/MAX_GLOSSINESS_RAYS;	
				}
			}
			col.clamp();
		}
		if(ray.intersection.mat->refrac_ratio)
		{
			double n_mat_in;
			double n_mat_out;
			//snells law
			//We are going out of the material
			if ( ray.intersection.normal.dot(ray.dir))
			{
				n_mat_in = 1;
				n_mat_out = ray.intersection.mat->refrac_ratio;
			}
			else //We are going in the material
			{
				n_mat_in = ray.intersection.mat->refrac_ratio;
				n_mat_out = 1;	
			}
			
			double cosT1 = -ray.intersection.normal.dot(ray.dir);
			double sinT2 = n_mat_in / n_mat_out * std::sqrt(1 - std::pow(cosT1,2));
			double cosT2 = std::sqrt(1 - std::pow(sinT2,2));
			double T2 = std::asin(sinT2);

			Vector3D refrac_dir = (n_mat_in / n_mat_out) * ray.dir + ((n_mat_in / n_mat_out)*cosT1 - cosT2) * ray.intersection.normal;
			refrac_dir.normalize();

			Ray3D refraction(ray.intersection.point + (.01*refrac_dir), refrac_dir);
			refraction.refrac_bounce = ray.refrac_bounce + 1;
			refraction.dir.normalize();
		
			if(ray.intersection.mat->transparency && refraction.refrac_bounce <= MAX_REFRAC_BOUNCES)
			{
				Colour colorRefrac	 = shadeRay(refraction);
				colorRefrac.clamp();
				col = (1 - ray.intersection.mat->transparency) * col + ray.intersection.mat->transparency * colorRefrac;
			}
		}
		col.clamp();
	}

	// You'll want to call shadeRay recursively (with a different ray, 
	// of course) here to implement reflection/refraction effects.  
	return col; 
}	

void Raytracer::render( int width, int height, Point3D eye, Vector3D view, 
		Vector3D up, double fov, char* fileName ) {
	Matrix4x4 viewToWorld;
	_scrWidth = width;
	_scrHeight = height;
	double factor = (double(height)/2)/tan(fov*M_PI/360.0);
	if(frames_rendered == 1)
		initPixelBuffer();
	viewToWorld = initInvViewMatrix(eye, view, up);

	//Sample 4 times per pixel for Anti Aliasing at (.25,25),(.25,.75),(.75,.25),(.75,.75)
	enum Sampleplace { TopLeft=0, TopRight, BottomRight, BottomLeft };
	Sampleplace place = TopLeft;

	//conintue sampling
	bool continueloop;

	// Construct a ray for each pixel.
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			Colour col(0.0, 0.0, 0.0);
			continueloop = true;
			while(continueloop)
			{
				// Sets up ray origin and direction in view space, 
				// image plane is at z = -1.
				Point3D origin(0, 0, 0);
				Point3D imagePlane;
				short y = 0; //Sampling from top side of the pixel
				short x = 0; //Sampling from the right side of the pixel
				if(place == TopLeft || place == TopRight)
					y = 1;
				if(place == TopRight || place == BottomRight)
					x = 1;
			/*
				double rand_x = randomest_number()%5;
				double rand_y = randomest_number()%5;

		

				double DOF_distance = distance(DOF_point,eye);

				std::cout << rand_x << " " << rand_y << std::endl;
			*/
				

				imagePlane[0] = (-double(width)/2 + 0.25 + 0.5*x + j)/factor;
				imagePlane[1] = (-double(height)/2 + 0.25 + 0.5*y + i)/factor;
				imagePlane[2] = -1;
			
				//Shoot the ray
				Ray3D ray(origin, imagePlane - origin);
				ray.dir = viewToWorld * ray.dir;
				ray.origin = viewToWorld * ray.origin;
				ray.dir.normalize();

				#if AA
				//Temp result of the ray colour result
				Colour col_temp = shadeRay(ray); 
				col[0] += col_temp[0]/4;
				col[1] += col_temp[1]/4;
				col[2] += col_temp[2]/4;
	
							
				switch(place)
				{
					case TopLeft:
						place = TopRight;
						break;
					case TopRight:
						place = BottomRight;
						break;
					case BottomRight:
						place = BottomLeft;
						break;
					case BottomLeft:
						place = TopLeft;
						continueloop=false;
						//Save result
						_rbuffer[i*width+j] += int(col[0]*255)/NUM_FRAMES;
						_gbuffer[i*width+j] += int(col[1]*255)/NUM_FRAMES;
						_bbuffer[i*width+j] += int(col[2]*255)/NUM_FRAMES;
						break;
					default:
						std::cout << "what?" << std::endl;
						break;
				}
				#else
				col = shadeRay(ray);
				continueloop = false;
				_rbuffer[i*width+j] += int(col[0]*255)/NUM_FRAMES;
				_gbuffer[i*width+j] += int(col[1]*255)/NUM_FRAMES;
				_bbuffer[i*width+j] += int(col[2]*255)/NUM_FRAMES;
				#endif
			}
		}
	}
	if(frames_rendered == NUM_FRAMES)
	{
		std::cout << "writing to file" << std::endl;
		flushPixelBuffer(fileName);
	}
	frames_rendered++;
}

int main(int argc, char* argv[])
{	

	//For the glossy reflection
	std::srand(time(NULL));
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  

	Raytracer raytracer;
	int width = 320; 
	int height = 240; 

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	// Defines a material for shading.
	Material glass( Colour(0.3, 0.3, 0.3), Colour(0.9, 0.9, 0.9), 
			Colour(0.628281, 0.655802, 0.666065), 
			51.2, 0.85, 1.6, 0.9, 1.0 );
	Material jade( Colour(0.2, 0.2, 0.2), Colour(0.54, 0.89, 0.63), 
			Colour(0.316228, 0.316228, 0.316228), 
			12.8, 0.0, 1.0, 0.15, 0.0 );
	Material mirror( Colour(.5,.5,.5), Colour(0.5,0.5,0.5),
			Colour(1.0,1.0,1.0),
			10.0,0.0,1.0, .8, 0.0);
	
	Material red( Colour(.6, .1, .1), Colour(0.7, 0.1, 0.1), 
			Colour(0.7, 0.6, 0.6), 
			12.8, 0.0, 1.0, 0.0, 0.0 );
	Material blue( Colour(.4, .4, .6), Colour(0.05, 0.05, 0.8), 
			Colour(0.6, 0.6, 0.7), 
			12.8, 0.0, 1.0, 0.15, 1.0 );
	Material teal( Colour(.3, .5, .5), Colour(0.24, 0.69, 0.63), 
			Colour(0.416228, 0.416228, 0.416228), 
			12.8, 0.0, 1.0, 0.15, 0.0 );

	// Defines a point light source.
	raytracer.addLightSource( new PointLight(Point3D(0, 0, 5), 
				Colour(0.9, 0.9, 0.9) ) );

	// Add a unit square into the scene with material mat.
	SceneDagNode* sphere = raytracer.addObject( new UnitSphere(), &glass );
	SceneDagNode* plane = raytracer.addObject( new UnitSquare(), &jade );
	SceneDagNode* plane2 = raytracer.addObject( new UnitSquare(), &mirror );
	SceneDagNode* plane3 = raytracer.addObject( new UnitSquare(), &blue );
	SceneDagNode* circle = raytracer.addObject( new UnitCircle(), &red );
	SceneDagNode* circle2 = raytracer.addObject( new UnitCircle(), &red);
	SceneDagNode* cylinder = raytracer.addObject( new UnitCylinder(), &red);

	plane->obj->t_map("simple_texture.bmp");

	// Apply some transformations to the unit square.
	double factor1[3] = { 1.5, 1.5, 1.5 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	raytracer.translate(sphere, Vector3D(-.5, -.5, -4));

	raytracer.rotate(cylinder, 'y', 45);
	raytracer.rotate(circle, 'y', 45);
	raytracer.rotate(circle2, 'y', 45);
	
	raytracer.translate(cylinder, Vector3D(5, 1, -5.5));
	raytracer.translate(circle, Vector3D(5, 1, -5));
	raytracer.translate(circle2, Vector3D(5, 1, -6));
	raytracer.rotate(circle2, 'y', 180);

	raytracer.rotate(sphere, 'x', 45); 
	raytracer.rotate(sphere, 'z', 45); 
	raytracer.scale(sphere, Point3D(0, 0, 0), factor1);

	raytracer.translate(plane, Vector3D(0, 0, -8.9));	
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);

	raytracer.translate(plane2, Vector3D(0, -3, -6));	
	raytracer.rotate(plane2, 'x', 90); 
	raytracer.scale(plane2, Point3D(0, 0, 0), factor2);

	raytracer.translate(plane3, Vector3D(-3, 0, -6));	
	raytracer.rotate(plane3, 'y', 90); 
	raytracer.scale(plane3, Point3D(0, 0, 0), factor2);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	while(frames_rendered <= NUM_FRAMES)
	{
		std::cout << "rendering frame" << std::endl;
		raytracer.render(width, height, eye, view, up, fov, "view1.bmp");
		raytracer.translate(cylinder, Vector3D(0, .15, 0));
		raytracer.translate(circle, Vector3D(0, .15, 0));
		raytracer.translate(circle2, Vector3D(0, .15, 0));
	}


	raytracer.translate(cylinder, Vector3D(0, -.15*NUM_FRAMES, 0));
	raytracer.translate(circle, Vector3D(0, -.15*NUM_FRAMES, 0));
	raytracer.translate(circle2, Vector3D(0, -.15*NUM_FRAMES, 0));
	
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);

	frames_rendered = 1;

	while(frames_rendered <= NUM_FRAMES)
	{// Render it from a different point of view.
		std::cout << "Rendering second frame\n";
		raytracer.render(width, height, eye2, view2, up, fov, "view2.bmp");
		raytracer.translate(cylinder, Vector3D(0, .15, 0));
		raytracer.translate(circle, Vector3D(0, .15, 0));
		raytracer.translate(circle2, Vector3D(0, .15, 0)); 
	}
	
	frames_rendered = 1;

	raytracer.translate(cylinder, Vector3D(0, -.15*NUM_FRAMES, 0));
	raytracer.translate(circle, Vector3D(0, -.15*NUM_FRAMES, 0));
	raytracer.translate(circle2, Vector3D(0, -.15*NUM_FRAMES, 0));
	


	Point3D eye3(-1, 6, -2.5);
	Vector3D view3(0, -1.5, -1);

	while(frames_rendered <= NUM_FRAMES)
	{

		std::cout << "rendering third frame" << std::endl;
		raytracer.render(width, height, eye3, view3, up, fov, "view3.bmp");
		raytracer.translate(cylinder, Vector3D(0, .15, 0));
		raytracer.translate(circle, Vector3D(0, .15, 0));
		raytracer.translate(circle2, Vector3D(0, .15, 0));
	}


	return 0;
}

