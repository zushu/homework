#include "vector.h"

void Vec::equalizeCoord(Vector3f& left, Vector3f right)
{
    left.x = right.x;
    left.y = right.y;
    left.z = right.z;
}

/*void Vec::equalizeColor(Vector3f& left, Vector3f right)
{
    left.r = right.r;
    left.g = right.g;
    left.b = right.b;
}*/

Vector3f Vec::addCoord(Vector3f left, Vector3f right)
{
    Vector3f result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.z = left.z + right.z;
}