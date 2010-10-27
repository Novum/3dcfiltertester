#include "Ray.h"

namespace engine
{
	bool Ray::IntersectSphere(Vector3 &position,float radius)
	{
		Vector3 origin = position - this->origin;

		float a = direction*direction;
		float b = 2.0f * (origin * direction);
		float c = (origin*origin) - radius*radius;

		float det = b*b - 4.0f*c;

		if(det > 0.0f) return true;

		return false;
	}

	bool Ray::IntersectSphere(Vector3 &position,float radius,float &l)
	{
		Vector3 origin = position - this->origin;

		float a = direction*direction;
		float b = 2.0f * (origin * direction);
		float c = (origin*origin) - radius*radius;

		float det = b*b - 4.0f*c;

		if(det > 0.0f) {
			l = (-b - sqrt(det))/(2.0f*a);

			return true;
		}

		return false;
	}

	bool Ray::IntersectSphere(Vector3 &position,float radius,float &l1,float &l2)
	{
		Vector3 origin = position - this->origin;

		float a = direction*direction;
		float b = 2.0f * (origin * direction);
		float c = (origin*origin) - radius*radius;

		float det = b*b - 4.0f*c;

		if(det > 0.0f) {			
			l1 = (-b - sqrt(det))/(2.0f*a);
			l2 = (-b + sqrt(det))/(2.0f*a);

			return true;
		}
		
		return false;
	}
}