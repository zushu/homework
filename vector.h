#ifndef _VECTORZ_H_
#define _VECTORZ_H_

#include "defs.h"
namespace Vec {

void equalizeCoord(Vector3f& left, Vector3f right);

//void equalizeColor(Vector3f& left, Vector3f right);

Vector3f addCoord(Vector3f left, Vector3f right);
} // namespace Vec ends

#endif