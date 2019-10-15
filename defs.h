#ifndef _DEFS_H_
#define _DEFS_H_

#include <cmath> 
#include <stdexcept>

class Scene;

/* Structure to hold return value from ray intersection routine. 
This should hold information related to the intersection point, 
for example, coordinate of the intersection point, surface normal at the intersection point etc. 
Think about the variables you will need for this purpose and declare them here inside of this structure. */
typedef struct ReturnVal
{
	/***********************************************
     *                                             *
	 * TODO: Implement this structure              *
     *                                             *
     ***********************************************
	 */
} ReturnVal;

/* 3 dimensional vector holding floating point numbers.
Used for both coordinates and color. 
Use x, y, z for coordinate computations, and use r, g, b for color computations. 
Note that you do not have to use this one if you use any vector computation library like Eigen. */
/*typedef struct Vector3f
{
	union 
	{
		float x;
		float r;
	};
	union
	{
		float y;
		float g;
	};
	union
	{
		float z;
		float b;
	};
} Vector3f;*/

typedef struct Vector3f
{
	float x;
	float y;
	float z;

	// default constructor
	Vector3f() {}
	// constructor
	Vector3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	// copy constructor
	Vector3f(const Vector3f& right)
	{
		this->x = right.x;
		this->y = right.y;
		this->z = right.z;
	}

	// overloaded assignment operator
	Vector3f& operator=(const Vector3f& right)
	{
		this->x = right.x;
		this->y = right.y;
		this->z = right.z;
		return *this;
	}

	float length() const
	{
		float result = sqrt(x * x + y * y + z * z);
		return result;
	}

	Vector3f normalize() const
	{
		float len = this->length();
		Vector3f result(x/len, y/len, z/len);
		return result;
	}

	bool operator==(const Vector3f right) const
	{
		if (x == right.x &&
			y == right.y &&
			z == right.z)
			return true;
		return false;
	}

	Vector3f operator+(const Vector3f right) const
	{
		Vector3f result(x + right.x, y + right.y, z + right.z);
		return result;
	}

	Vector3f operator-(const Vector3f right) const
	{
		Vector3f result(x - right.x, y - right.y, z - right.z);
		return result;
	}

	// dot product
	float operator*(const Vector3f right) const
	{
		float result = x * right.x + y * right.y + z * right.z;
		return result;
	}

	// scalar multiplication
	Vector3f operator*(float k) const
	{
		Vector3f result(k*x, k*y, k*z);
		return result;
	}

	// scalar division
	Vector3f operator/(float k) const
	{
		Vector3f result(x/k, y/k, z/k);
		return result;
	}

	Vector3f cross_product(const Vector3f right) const
	{
		Vector3f result(y * right.z - z * right.y, 
						z * right.x - x * right.z, 
						x * right.y - y * right.x);
		return result;
	}

	Vector3f pointwise_multiplication(const Vector3f right) const
	{
		Vector3f result(x * right.x, y * right.y, z * right.z);
		return result;
	}

	Vector3f pointwise_division(const Vector3f right) const
	{
		Vector3f result(x / right.x, y / right.y, z / right.z);
		return result;
	}

} Vector3f;

//
// The global variable through which you can access the scene data
//
extern Scene* pScene;

#endif
