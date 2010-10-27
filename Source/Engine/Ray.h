#pragma once

#include "LinearMath.h"
#include "Plane.h"

namespace engine
{
	class Ray
	{
	public:
		Ray() {}
		Ray(Vector3 origin,Vector3 direction) 
			: origin(origin), direction(direction) {}
		bool IntersectSphere(Vector3 &position,float radius);
		bool IntersectSphere(Vector3 &position,float radius,float &l);
		bool IntersectSphere(Vector3 &position,float radius,float &l1,float &l2);		

		Vector3 origin;
		Vector3 direction;
	};
}