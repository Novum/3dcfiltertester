#include "Plane.h"

namespace engine
{
	Plane::Plane()
	{
		normal[0] = normal[1] = normal[2] = d = 0.0f;
	}

	Plane::Plane(const Plane &p) 
	{
		normal = p.normal;
		d = p.d;
	}

	Plane::Plane(Vector4 vector,bool normalize)
	{
		normal[0] = vector[0];
		normal[1] = vector[1];
		normal[2] = vector[2];
		d = vector[3];
		if(normalize) Normalize();
	}

	Plane::Plane(Vector3 point,Vector3 normal)
	{
		this->normal[0] = normal[0]; this->normal[1] = normal[1]; this->normal[2] = normal[2];
		d = -(normal * point);
	}	

	float Plane::Distance(Vector3 point)
	{
		return normal[0]*point[0] + normal[1]*point[1] + normal[2]*point[2] + d;		
	}

	void Plane::Normalize()
	{
		float one_over_l = 1.0f/normal.Length();		
		normal *= one_over_l;
		d *= one_over_l;
	}
}