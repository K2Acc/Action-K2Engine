#pragma once
#include "Camera.h"

class FollowCamera : public Camera
{
public:
	static FollowCamera* Create(Transform transform = {{0,0,-5}, {0,0,0}, {1,1,1}});
	static FollowCamera* GetInstance();

public:
	FollowCamera() = default;
	FollowCamera(Transform transform);

public:
	void Update(Vector3 target);

	//Getter/Setter
	Vector3 GetTargetOffset()	{return targetOffset_;}
	Vector3 GetCameraOffset()	{return cameraOffset_;}

	void SetTargetOffset(Vector3 value)	{targetOffset_ = value;}
	void SetCameraOffset(Vector3 value)	{cameraOffset_ = value;}

private:
	void Rot();
	void Move();

	void ApplyGlobalVariablesInitialize() override;
	void ApplyGlobalVariablesUpdate() override;

private:
	static FollowCamera* instance_;

private:
	float speed_ = 0.01f;
	Vector2 RotMinMax_ = {-45.f,45.f};

	Vector3 targetOffset_ = {0,0.4f,7.5f};
	Vector3 cameraOffset_ = {0,10,-20};
};

