#include "FollowCamera.h"
#include "Input.h"
#include "MathUtility.h"
#include "WindowsApp.h"

#include "GlobalVariables.h"
#include <imgui.h>

FollowCamera* FollowCamera::instance_ = nullptr;

FollowCamera *FollowCamera::Create(Transform transform)
{
	if(!instance_){
		instance_ = new FollowCamera(transform);
	}
	return instance_;
}

FollowCamera *FollowCamera::GetInstance()
{
	return instance_;
}

FollowCamera::FollowCamera(Transform transform)
{
	translate = transform.translate;
	rotation = transform.rotation;

	ApplyGlobalVariablesInitialize();
}

void FollowCamera::Update(Vector3 target)
{
	target_ = target + Multiplication(targetOffset_, GetRotMatrix());

	Rot();
	Move();

	Camera::Update();
}

void FollowCamera::Rot()
{
	if(!Input::GetInstance()->GetIsPadConnect()) return;
	
	rotation.y += Input::GetInstance()->PadRStick().x * speed_;
	rotation.x -= Input::GetInstance()->PadRStick().y * speed_;

	//上限
	rotation.x = max(rotation.x*(180.f/3.141592f), RotMinMax_.x) * (3.141592f/180.f);
	rotation.x = min(rotation.x*(180.f/3.141592f), RotMinMax_.y) * (3.141592f/180.f);

	//回転行列
	Matrix4x4 matRot;
	matRot = MakeIdentityMatrix();
	//matRot *= MakeRotationZMatrix(lRot.z);
	matRot *= MakeRotationXMatrix(rotation.x);
	matRot *= MakeRotationYMatrix(rotation.y);

	cameraOffset_ = Multiplication(cameraOffset_, matRot);
}

void FollowCamera::Move()
{
	translate = target_ + cameraOffset_;
}

void FollowCamera::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Camera";
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(name);
	globalVariables->AddItem(name, "0.aspect", aspect_);
	globalVariables->AddItem(name, "1.RotSp", speed_);
	globalVariables->AddItem(name, "2.RotX Min,Max", RotMinMax_);
#endif // _DEBUG
}

void FollowCamera::ApplyGlobalVariablesUpdate()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Camera";
#ifdef _DEBUG
	ImGui::Text("Camera - Pos X: %f, Y: %f, Z:%f", translate.x,translate.y,translate.z);
	ImGui::Text("Camera - Rot X: %f, Y: %f, Z:%f", rotation.x,rotation.y,rotation.z);
#endif // _DEBUG
	aspect_ = globalVariables->GetFloatValue(name, "0.aspect");
	speed_ = globalVariables->GetFloatValue(name, "1.RotSp");
	RotMinMax_ = globalVariables->GetVector2Value(name, "2.RotX Min,Max");
}