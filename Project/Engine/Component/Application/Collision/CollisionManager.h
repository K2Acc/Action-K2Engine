#pragma once
#include "Collider/BaseCollider.h"
#include <forward_list>

class CollisionManager
{
public:
	static CollisionManager* GetInstance();

public:

	//総当たりcheck
	void CheckAllCollisions();

	//総当たり用のコライダー追加
	void AddCollider(BaseCollider* collider);

private:
	static CollisionManager* instance_;

private:
	std::forward_list<BaseCollider*> colliders_;
};

