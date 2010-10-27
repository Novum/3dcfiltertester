#pragma once

#include "LinearMath.h"

namespace engine
{
	class Plane
	{
	public:
		Plane();
		Plane(const Plane &p);
		Plane(Vector4 vector,bool normalize=false);
		Plane(Vector3 point,Vector3 normal);
		float Distance(Vector3 point);
		void Normalize();

		Vector3 normal;
		float d;
	};
}