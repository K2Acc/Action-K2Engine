#include "Player.h"
#include "Input.h"

#include "FollowCamera.h"
#include <MathUtility.h>
#include "Easing.h"
#include "BlendSetting.h"

#include "Geometry/SphereCollider.h"

#include <imgui.h>
#include "GlobalVariables.h"

Player::Player(std::string filePath, Transform transform):
	ObjModel(filePath, transform, BlendSetting::kBlendModeNormal)
{
	ObjModelLoad();
	Initialize(false);
	ObjModelVertexData();

	weapon_ = ObjModel::Create("weapon");

	collider_ = SphereCollider::Create(this);
	collider_->SetShapeType(COLLISIONSHAPE_SPHERE);

	ApplyGlobalVariablesInitialize();
}

void Player::Update()
{
	ApplyGlobalVariablesUpdate();

	//状態遷移
	if(behaviorRequest_){
		behavior_ = behaviorRequest_.value();

		switch(behavior_){
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;

		case Behavior::kDush:
			BehaviorDushInitialize();
			break;

		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;

		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	//状態更新
	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;

	case Behavior::kDush:
		BehaviorDushUpdate();
		break;

	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;

	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}
	ObjModel::Update();
}

void Player::Draw(FollowCamera* camera)
{
	this->camera = camera;

	switch (behavior_)
	{
	case Player::Behavior::kAttack:
		weapon_->Draw(camera);
		break;
	}

	ObjModel::Draw(camera);
}

void Player::OnCollision(const CollisionInfo &info)
{
	WindowsApp::Log("Hit\n");
}


void Player::BehaviorRootInitialize()
{
}
void Player::BehaviorRootUpdate()
{
	RootInput();

	Move();
}

void Player::BehaviorDushInitialize()
{
	isCameraChange = true;
	cameraChangeFrame = 0;
	//カメラ情報の保存
	normalValue.cameraOffset = camera->GetCameraOffset();
	normalValue.targetOffset = camera->GetTargetOffset();
}
void Player::BehaviorDushUpdate()
{
	DushInput();

	Dush();

	//カメラ遷移
	if(!isCameraChange) return;

	if(cameraChangeFrame >= CameraChangeMaxSecond) isCameraChange = false;
	Time_OneWay(cameraChangeFrame, CameraChangeMaxSecond);
	camera->SetCameraOffset((Vector3)Easing_Point2_Linear(camera->GetCameraOffset(), dushValue.cameraOffset, cameraChangeFrame));
	camera->SetTargetOffset((Vector3)Easing_Point2_Linear(camera->GetTargetOffset(), dushValue.targetOffset, cameraChangeFrame));
}

void Player::BehaviorAttackInitialize()
{
	weaponAngle_ = 0.0f;
	weaponAnimFrame_ = 0.0f;
}
void Player::BehaviorAttackUpdate()
{
	Attack();
}

void Player::BehaviorJumpInitialize()
{
	velocity_.y = kJumpVelocity;
}
void Player::BehaviorJumpUpdate()
{
	Jump();
}


void Player::RootInput()
{
	if(Input::GetInstance()->PadButtonTrigger(XINPUT_GAMEPAD_A)){
		behaviorRequest_ = Behavior::kJump;
	}
	else if(Input::GetInstance()->PadButtonTrigger(XINPUT_GAMEPAD_B)){
		behaviorRequest_ = Behavior::kAttack;
	}
	else if(Input::GetInstance()->PadButtonPush(XINPUT_GAMEPAD_RIGHT_SHOULDER)){
		behaviorRequest_ = Behavior::kDush;
	}
}

void Player::DushInput()
{
	if(!Input::GetInstance()->PadButtonPush(XINPUT_GAMEPAD_RIGHT_SHOULDER)){
		behaviorRequest_ = Behavior::kRoot;
	}
}


void Player::Move()
{
	if(!Input::GetInstance()->GetIsPadConnect()) return;
	const float threshold = 0.7f;
	bool isMoving = false;

	velocity_ = {
		Input::GetInstance()->PadLStick().x,
		0.0f,
		Input::GetInstance()->PadLStick().y
	};
	
	if(velocity_.length() > threshold){
		isMoving = true;
	}

	if(!isMoving) return;

	velocity_ *= kMoveVelocity;

	//カメラの方向へと動く
	Matrix4x4 matRot;
	matRot = MakeIdentityMatrix();
	matRot *= MakeRotationYMatrix(FollowCamera::GetInstance()->rotation.y);
	velocity_ = Multiplication(velocity_, matRot);

	//回転
	rotation.y = std::atan2(velocity_.x, velocity_.z);

	//移動
	translate += velocity_;
}

void Player::Dush()
{
	if(!Input::GetInstance()->GetIsPadConnect()) return;
	const float threshold = 0.7f;
	bool isMoving = false;

	velocity_ = {
		Input::GetInstance()->PadLStick().x,
		0.0f,
		Input::GetInstance()->PadLStick().y
	};
	
	if(velocity_.length() > threshold){
		isMoving = true;
	}

	if(!isMoving) return;

	velocity_ *= kDushVelocity;

	//カメラの方向へと動く
	Matrix4x4 matRot;
	matRot = MakeIdentityMatrix();
	matRot *= MakeRotationYMatrix(FollowCamera::GetInstance()->rotation.y);
	velocity_ = Multiplication(velocity_, matRot);

	//回転
	rotation.y = std::atan2(velocity_.x, velocity_.z);

	//移動
	translate += velocity_;
}

void Player::Attack()
{
	weaponAngle_ = Easing_Point2_EaseOutBounce(0.0f, 1.8f, Time_OneWay(weaponAnimFrame_, 1.0f));

	weapon_->rotation.x = weaponAngle_;
	weapon_->rotation.y = rotation.y;
	weapon_->translate = translate;

	if(weaponAnimFrame_ >= 1.f){
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::Jump()
{
	translate += velocity_;
	Vector3 accelerationVector = {0, -kGravityAcceleration, 0};
	velocity_ += accelerationVector;

	if(translate.y <= 0.0f){
		translate.y = 0.0f;
		behaviorRequest_ = Behavior::kRoot;
	}
}


void Player::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Player";
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(name);
	globalVariables->AddItem(name, "0.moveVelocity", kMoveVelocity);
	globalVariables->AddItem(name, "1.dushVelocity", kDushVelocity);
	globalVariables->AddItem(name, "2.jumpVelocity", kJumpVelocity);
	globalVariables->AddItem(name, "3.gravityVelocity", kGravityAcceleration);
#endif // _DEBUG
}

void Player::ApplyGlobalVariablesUpdate()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Player";
#ifdef _DEBUG
	ImGui::Text("Player - Pos X: %f, Y: %f, Z:%f", translate.x,translate.y,translate.z);
	ImGui::Text("Player - Rot X: %f, Y: %f, Z:%f", rotation.x,rotation.y,rotation.z);
	ImGui::ColorEdit4("Player - Color", &color_.x);
#endif // _DEBUG
	kMoveVelocity = globalVariables->GetFloatValue(name, "0.moveVelocity");
	kDushVelocity = globalVariables->GetFloatValue(name, "1.dushVelocity");
	kJumpVelocity = globalVariables->GetFloatValue(name, "2.jumpVelocity");
	kGravityAcceleration = globalVariables->GetFloatValue(name, "3.gravityVelocity");
}
