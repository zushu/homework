#include "Ray.h"

Ray::Ray() {}

Ray::Ray(const Vector3f& origin, const Vector3f& direction)
    : origin(origin), direction(direction) {}

/* Takes a parameter t and returns the point accoring to t. t is the parametric variable in the ray equation o+t*d.*/
Vector3f Ray::getPoint(float t) const 
{

    const Vector3f result = this->origin + ((this->direction) * t);
    return result;
}

/* Takes a point p and returns the parameter t according to p such that p = o+t*d. */
float Ray::gett(const Vector3f & p) const
{

    // TODO: First check if point p is on the ray 
    Vector3f p_minus_o = p - origin;

    Vector3f p_minus_o_normalized = p_minus_o.normalize();
    Vector3f d_normalized = direction.normalize();
    
    if (p_minus_o_normalized == d_normalized)
    {
        float len1 = p_minus_o.length();
        float len2 = direction.length();
        float result = len1/len2;
        return result;
    }

    else if (p_minus_o_normalized == d_normalized * (-1))
    {
        float len1 = p_minus_o.length();
        float len2 = direction.length();
        float result = (-1) * len1/len2;
        return result;
    }

    else
        throw std::invalid_argument("point not on the ray");
    //return 0;
    

    //float len1 = p_minus_o.length();
    //float len2 = direction.length();
    //float result = len1/len2;
    //return result;

}

