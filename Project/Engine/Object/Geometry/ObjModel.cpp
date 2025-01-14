#include "ObjModel.h"
#include <imgui.h>
#include "SpriteLoader.h"

ObjModel *ObjModel::Create(std::string filePath, Transform transform, BlendSetting::BlendMode mode)
{
	ObjModel *instance = new ObjModel(filePath, transform,mode);
	instance->ObjModelLoad();
	instance->Initialize(false);
	instance->ObjModelVertexData();
	return instance;
}

void ObjModel::ObjModelLoad()
{
	//モデル読み込み
	modelData_ = LoadObjFile(filePath_);
	//頂点数決め
	vertNum_ = (UINT)modelData_.vertices.size();
	//画像パス
	texturePath_ = modelData_.material.textureFilePath;
	SpriteLoader::LoadTexture(DirectXCommon::GetInstance(), texturePath_);
}

void ObjModel::ObjModelVertexData()
{
	//データ送信
	std::memcpy(vertData_, modelData_.vertices.data(), sizeof(VertexData) * vertNum_);
}
