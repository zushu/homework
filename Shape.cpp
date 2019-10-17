#include "Shape.h"
#include "Scene.h"
#include <cstdio>

Shape::Shape(void)
{
}

Shape::Shape(int id, int matIndex)
    : id(id), matIndex(matIndex)
{
}

Sphere::Sphere(void)
{}

/* Constructor for sphere. You will implement this. */
Sphere::Sphere(int id, int matIndex, int cIndex, float R)
    : Shape(id, matIndex), cIndex(cIndex), R(R)
{
    this->center = pScene->vertices[id - 1];
}

/* Sphere-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Sphere::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */

    ReturnVal result;

    float t0, t1;
    Vector3f o_minus_c = ray.origin - center;
    float a = ray.direction * ray.direction;
    float b = (ray.direction * o_minus_c) * 2;
    float c = o_minus_c * o_minus_c - R*R;
    float discriminant = b * b - 4 * a * c;
    float sqrt_disc = sqrt(discriminant);
    if (discriminant < 0)
        result.intersects = false;
    else
        t0 = (-b - sqrt_disc) / (2.0 * a);
        t1 = (-b + sqrt_disc) / (2.0 * a);
        
        //if (t0 > t1)



}

Triangle::Triangle(void)
{}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index)
    : Shape(id, matIndex), p1Index(p1Index), p2Index(p2Index), p3Index(p3Index)
{
    this->vertex1 = pScene->vertices[p1Index - 1];
    this->vertex2 = pScene->vertices[p2Index - 1];
    this->vertex3 = pScene->vertices[p3Index - 1];
}

/* Triangle-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Triangle::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
}

Mesh::Mesh()
{}

/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, const vector<Triangle>& faces)
    : Shape(id, matIndex), faces(faces)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
}

/* Mesh-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Mesh::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
}
