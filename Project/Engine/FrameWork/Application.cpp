#include "Application.h"

#include "WindowsApp.h"
#include <imgui.h>
#include "Geometry/SphereCollider.h"

Application *Application::Create()
{
	Application* instance = new Application();
	instance->Initialize();
	return instance;
}

void Application::Initialize()
{
	camera_ = FollowCamera::Create();
	light_ = LightingGroup::Create();

	SpriteLoader::LoadTexture(DirectXCommon::GetInstance());
	//collisionsManager = CollisionsManager::GetInstance();


	////シーンオブジェクト
	levelLoader_ = std::make_unique<LevelLoader>();
	levelLoader_->Load("levelSample");
	levelLoader_->Initialize();

	player_ = std::make_unique<Player>("Human");
	player_.get()->translate = {0,0,0};
	player_.get()->scale = {1.0f,1.0f,1.0f};
	//collisionsManager->AddCollider(player_->GetCollider());

	//box_ = ObjModel::Create("fence", {{0,0,0},{0,0,0},{1,1,1}}, BlendSetting::kBlendModeNone);
	//box_->translate = {-5.f,0,0};
	//box_->rotation = DegreesToRadians({0,180.f,0});
	//box_->scale = {0.8f,0.8f,0.8f};
	//box_->collider_ = SphereCollider::Create(box_);
	//box_->collider_->SetShapeType(COLLISIONSHAPE_SPHERE);
	//collisionsManager->AddCollider(box_->GetCollider());

	//particle_ = ParticleObject::Create();
	//emitter_ = ParticleEmitterObject::Create();

	//sp_ = Sprite2D::Create();

	gpu_ = GPUParticleBase::Create();
}

void Application::Update()
{
	//シーンオブジェクト
	levelLoader_->Update();
	player_->Update();
	/*box_->Update();

	sp_->Update();

	particle_->Add(particlePos_);
	particle_->Update();

	emitter_->Add(emitterPos_);
	emitter_->Update();*/

	camera_->Update(player_->translate);
	light_->Update();

	CollisionCheck();
}

void Application::GeometryDraw()
{
	//シーンオブジェクト
	levelLoader_->Draw(camera_);
	player_->Draw(camera_);
	//box_->Draw(camera_);

	gpu_->Draw(camera_);
}

void Application::SpriteDraw()
{
	//sp_->Draw(camera_);
}

void Application::ParticleDraw()
{
	/*particle_->Draw(camera_);
	emitter_->Draw(camera_);*/
}

void Application::CollisionCheck()
{
	//collisionsManager->CheckAllCollisions();
}
