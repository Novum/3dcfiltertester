#include "Camera.h"
#include <cmath>

namespace engine
{
	const float pi = 3.141592f;

	Camera::Camera()
	{		
		rotation[0] = 0.0f;		
		rotation[1] = 0.0f;
		rotation[2] = 0.0f;
		aspect_ratio = 0.75f;
		fov = 30.0f*(pi/360.0f)*2.0f;
		znear = 0.1f;
		zfar = 1000.0f;	
	}

	void Camera::SetViewportSize(int x,int y)
	{ 
		viewportx = x; 
		viewporty = y; 

		const float left = (float)x * -0.5f;
		const float right = (float)x * 0.5f;
		const float top = (float)y * -0.5f;
		const float bottom = (float)y * 0.5f;

		hud_projection_matrix = MatrixIdentity<4,float>();

		// x = 2.0f/(right-left)*x' - 1.0f
		hud_projection_matrix[0][0] = 2.0f/(right-left);
		hud_projection_matrix[3][0] = -1.0f;

		// y = 2.0f/(top-bottom)*y' + 1.0f
		hud_projection_matrix[1][1] = 2.0f/(top-bottom);
		hud_projection_matrix[3][1] = 1.0f;

		// z = 0.0f;
		hud_projection_matrix[2][2] = 0.0f;
	}

	void Camera::Move(Vector3 &translation) 
	{ 
		position += translation;	
	}

	void Camera::Rotate(Vector3 &rot)
	{
		rotation += rot;

		// Map rotation to 0.0f-2.0f*pi range
		rotation[0] = fmod(rotation[0]+2.0f*pi,2.0f*pi);
		rotation[1] = fmod(rotation[1]+2.0f*pi,2.0f*pi);
		rotation[2] = fmod(rotation[2]+2.0f*pi,2.0f*pi);		
	}

	void Camera::Update()
	{
		CalculateProjectionMatrix();
		CalculateViewMatrix();		
		view_projection_matrix = projection_matrix * view_matrix;
		CalculateFrustumPlanes();
	}

	Vector3 Camera::ProjectNoScale(Vector3 point)
	{
		Vector3 ret; 			
		Vector4 temp_point(point); temp_point[3] = 1.0f;
		Vector4 homogenous_coords = view_projection_matrix * temp_point;
		float homogenous_rcp = 1.0f / homogenous_coords[3];
		ret[0] = homogenous_coords[0] * homogenous_rcp;
		ret[1] = homogenous_coords[1] * homogenous_rcp;
		ret[2] = homogenous_coords[2];
		return ret;
	}

	Vector3 Camera::Project(Vector3 point)
	{
		Vector3 ret = ProjectNoScale(point);
		ret[0] = ((ret[0] + 1.0f) * 0.5f) * viewportx;
		ret[1] = ((ret[1] - 1.0f) * -0.5f) * viewporty;		
		return ret;
	}

	Ray Camera::GetRayThrough(int x,int y)
	{
		Vector3 direction;	

		float scaley = tan(fov*0.5f) * znear;
		float scalex = scaley * aspect_ratio;

		scaley *= ((float)y/(float)viewporty)*-2.0f + 1.0f;
		scalex *= ((float)x/(float)viewportx)*2.0f - 1.0f;

		direction = forward*znear + left*scalex + up*scaley;		
		direction.Normalize();

		return Ray(position,direction);		
	}

	void Camera::CalculateViewMatrix() 
	{
		Matrix44 rotation_matrix = XRotationMatrix44<float>(-rotation[0]);
		rotation_matrix *= YRotationMatrix44<float>(-rotation[1]);
		rotation_matrix *= ZRotationMatrix44<float>(-rotation[2]);

		left = Vector3(rotation_matrix[0][0],rotation_matrix[1][0],rotation_matrix[2][0]);
		up = Vector3(rotation_matrix[0][1],rotation_matrix[1][1],rotation_matrix[2][1]);
		forward = Vector3(rotation_matrix[0][2],rotation_matrix[1][2],rotation_matrix[2][2]);

		Matrix44 translation_matrix = MatrixIdentity<4,float>();
		translation_matrix[3][0] = -position[0];
		translation_matrix[3][1] = -position[1];
		translation_matrix[3][2] = -position[2];

		view_matrix = rotation_matrix * translation_matrix;		
	}

	void Camera::CalculateProjectionMatrix()
	{
		projection_matrix = MatrixIdentity<4,float>();
		
		const float scaley = 1.0f/tan(fov*0.5f);
		const float scalex = scaley / aspect_ratio;

		// x = scalex
		projection_matrix[0][0] = scalex;

		// y = scaley
		projection_matrix[1][1] = scaley;

        // z = (zfar+znear)/(znear-zfar)*z' + (2*zfar*znear)/(znear-zfar)*w'		
		projection_matrix[2][2] = (zfar+znear)/(znear-zfar);
		projection_matrix[3][2] = (2*zfar*znear)/(znear-zfar);

		// w = -z'
		projection_matrix[2][3]	= -1.0f;			
		projection_matrix[3][3] = 0.0f;
	}

	void Camera::CalculateFrustumPlanes()
	{
		Matrix44 matrix = view_projection_matrix;

		Vector4 row1 = Vector4(matrix[0][0],matrix[1][0],matrix[2][0],matrix[3][0]);
		Vector4 row2 = Vector4(matrix[0][1],matrix[1][1],matrix[2][1],matrix[3][1]);
		Vector4 row3 = Vector4(matrix[0][2],matrix[1][2],matrix[2][2],matrix[3][2]);
		Vector4 row4 = Vector4(matrix[0][3],matrix[1][3],matrix[2][3],matrix[3][3]);

		frustum_planes[0] = row4 + row3;
		frustum_planes[1] = row4 - row3;
		frustum_planes[2] = row4 + row1;
		frustum_planes[3] = row4 - row1;
		frustum_planes[4] = row4 + row2;
		frustum_planes[5] = row4 - row2;

		for(unsigned int i=0;i<6;++i)
			frustum_planes[i].Normalize();		
	}
}