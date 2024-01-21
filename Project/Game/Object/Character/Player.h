#pragma once
#include <optional>
#include "../Geometry/ObjModel.h"
#include "FollowCamera.h"

class Player : public ObjModel
{
private:
	enum class Behavior{
		kRoot,
		kDush,
		kAttack,
		kJump,
	};

	struct CameraChangeValue{
		Vector3 cameraOffset;
		Vector3 targetOffset;
	};

public:
	Player(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {0.8f,0.8f,0.8f}});

public:
	void Update()override;
	void Draw(FollowCamera* camera);

	void OnCollision(const CollisionInfo& info) override;

private:
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();

	void BehaviorDushInitialize();
	void BehaviorDushUpdate();

	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	void BehaviorJumpInitialize();
	void BehaviorJumpUpdate();


	void RootInput();
	void DushInput();
	
	
	void Move();
	void Dush();
	void Attack();
	void Jump();


	void ApplyGlobalVariablesInitialize() override;
	void ApplyGlobalVariablesUpdate() override;

private:
	//ダッシュ時のカメラ遷移
	bool isCameraChange = false;
	CameraChangeValue normalValue;
	CameraChangeValue dushValue = {{5,5,-15},{0,0,9}};
	
	const float CameraChangeMaxSecond = 2;
	float cameraChangeFrame = 0;

	//各遷移の値
	float kMoveVelocity = 0.1f;
	float kDushVelocity = 0.1f;
	float kGravityAcceleration = 0.05f;
	float kJumpVelocity = 1.0f;

private:
	FollowCamera* camera = nullptr;

	//状態
	Behavior behavior_ = Behavior::kRoot;
	//次の状態
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//武器
	ObjModel* weapon_ = nullptr;
	float weaponAngle_ = 0.0f;
	float weaponAnimFrame_ = 0.f;

	//速度
	Vector3 velocity_ = {};
};

