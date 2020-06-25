#include "Shape.h"
#include "Scene.h"
#include <cstdio>
#include <algorithm>

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
    //this->center = pScene->vertices[id - 1];
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

    float infty = std::numeric_limits<float>::infinity();
    float eps = pScene->intTestEps;
    ReturnVal result;

    Vector3f center = pScene->vertices[cIndex - 1];

    float t0, t1;
    Vector3f e_minus_c = ray.origin - center;
    float a = ray.direction * ray.direction;
    float b = (ray.direction * e_minus_c) * 2;
    float c = e_minus_c * e_minus_c - R*R;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < -eps)
    {
        result.intersects = false;
        result.t = infty;
        return result;
    }

    else
    {
        float sqrt_disc = sqrt(discriminant);
        t0 = (-b - sqrt_disc) / (2.0 * a);
        t1 = (-b + sqrt_disc) / (2.0 * a);
        
        if (t0 > t1) 
        {
            std::swap(t0, t1);
        }

        if (t0 < -eps)
        {
            t0 = t1;
            if (t0 < -eps)
            {
                result.intersects = false;
                result.t = infty;
                return result;
            }
        }

        result.intersects = true;
        result.t = t0;
        result.intersection_point = ray.origin + (ray.direction * result.t);
        result.material_index = matIndex;
        result.normal = (result.intersection_point - center) / R;
        result.shape_type = 1; // 1 for sphere, 2 for triangle
        result.shape_id = id;
        //result.obj = this;
        return result;
    }

}

Triangle::Triangle(void)
{}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index)
    : Shape(id, matIndex), p1Index(p1Index), p2Index(p2Index), p3Index(p3Index)
{
    /*
    this->vertex1 = pScene->vertices[p1Index - 1];
    this->vertex2 = pScene->vertices[p2Index - 1];
    this->vertex3 = pScene->vertices[p3Index - 1];   
    this->normal = (vertex3 - vertex2).cross_product(vertex1 - vertex2).normalize();
    */
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

    float infty = std::numeric_limits<float>::infinity();
    float eps = pScene->intTestEps;

    Vector3f vertex1 = pScene->vertices[p1Index - 1];
    Vector3f vertex2 = pScene->vertices[p2Index - 1];
    Vector3f vertex3 = pScene->vertices[p3Index - 1];   
    Vector3f normal = (vertex3 - vertex2).cross_product(vertex1 - vertex2).normalize();

    ReturnVal result;
    result.t = infty;

    float normal_dot_ray_dir = normal * ray.direction;
    if (normal_dot_ray_dir > .0 || fabs(normal * ray.direction) < eps)
    {
        result.intersects = false;
        return result;
    }

    float a = vertex1.x - vertex2.x;
    float b = vertex1.y - vertex2.y;
    float c = vertex1.z - vertex2.z;
    float d = vertex1.x - vertex3.x;
    float e = vertex1.y - vertex3.y;
    float f = vertex1.z - vertex3.z;
    float g = ray.direction.x;
    float h = ray.direction.y;
    float i = ray.direction.z;
    float j = vertex1.x - ray.origin.x;
    float k = vertex1.y - ray.origin.y;
    float l = vertex1.z - ray.origin.z;
    float ei_minus_hf = e * i - h * f;
    float gf_minus_di = g * f - d * i;
    float dh_minus_eg = d * h - e * g;
    float ak_minus_jb = a * k - j * b;
    float jc_minus_al = j * c - a * l;
    float bl_minus_kc = b * l - k * c;

    float detA = a * ei_minus_hf + b * gf_minus_di + c * dh_minus_eg;

    float beta = (j * ei_minus_hf + k * gf_minus_di + l * dh_minus_eg) / detA;

    if (beta < -eps)
    {
        result.intersects = false;
        return result;
    }

    float gamma = (i * ak_minus_jb + h * jc_minus_al + g * bl_minus_kc) / detA;

    if (gamma < -eps || (gamma + beta) > (1.0 + eps))
    {
        result.intersects = false;
        return result;
    }

    float t = (-1) * (f * ak_minus_jb + e * jc_minus_al + d * bl_minus_kc) / detA;

    if (t <= -eps) 
    {
        result.intersects = false;
        return result;
    }
    else
    {
        result.intersects = true;
        result.t = t;
        result.intersection_point = ray.origin + (ray.direction * result.t);
        result.normal = normal;
        result.material_index = matIndex;
        result.shape_type = 2;
        result.shape_id = id;
        //result.obj = this;
        result.beta = beta;
        result.gamma = gamma;
    }
    return result;
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

    float tNear = std::numeric_limits<float>::infinity();
    int closest_tri_id;
    ReturnVal result;
    result.intersects = false;
    int num_tris = faces.size();

    for (int i = 0; i < num_tris; i++)
    {
        Triangle tri = faces[i];
        const ReturnVal tri_res = tri.intersect(ray);
        if (tri_res.intersects && tri_res.t < tNear)
        {
            tNear = tri_res.t;
            result = tri_res;
        }
    }

    return result;

}
