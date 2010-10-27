#pragma once

#include "LinearMath.h"
#include "Ray.h"
#include "Plane.h"

namespace engine
{
	class Camera
	{
	public:
		Camera();

		void Update();

		void SetAspectRatio(float ratio) { aspect_ratio = ratio; CalculateFrustumPlanes(); }
		void SetFOV(float fov) { this->fov = fov; CalculateFrustumPlanes(); }
		float GetFov() const { return fov; }
		float GetZNear() const { return znear; }
		float GetZFar() const { return zfar; }
		void SetViewportSize(int x,int y);

		void Move(Vector3 &translation);
		void Rotate(Vector3 &rot);		

		Vector3 Project(Vector3 point);
		Vector3 ProjectNoScale(Vector3 point);

		const float GetAspectRatio() const { return aspect_ratio; }

		void SetPosition(Vector3 pos) { position = pos; }
		Vector3 GetPosition() const { return position; }
		void SetRotation(Vector3 rot) { rotation = rot; }
		Vector3 GetRotation() const { return rotation; }

		Vector3 GetUpVector() const { return up; }
		Vector3 GetLeftVector() const { return left; }
		Vector3 GetForwardVector() const { return forward; }

		Ray GetRayThrough(int x,int y);

		const int GetViewportX() const { return viewportx; }
		const int GetViewportY() const { return viewporty; }

		Matrix44 &GetViewMatrix() { return view_matrix; }
		Matrix44 &GetProjectionMatrix() { return projection_matrix; }
		Matrix44 &GetViewProjectionMatrix() { return view_projection_matrix; }
		
		Matrix44 &GetHudProjectionMatrix() { return hud_projection_matrix; }

		Plane *GetFrustumPlanes() { return frustum_planes; }
	private:
		void CalculateViewMatrix();
		void CalculateProjectionMatrix();
		void CalculateFrustumPlanes();

		Matrix44 view_matrix;
		Matrix44 projection_matrix;
		Matrix44 view_projection_matrix;
		Matrix44 hud_projection_matrix;

		Vector3 position;
		Vector3 rotation;	

		Vector3 up,left,forward;

		Plane frustum_planes[6];		

		int viewportx,viewporty;	
		
		float aspect_ratio;
		float fov;
		float znear,zfar;		
	};
}