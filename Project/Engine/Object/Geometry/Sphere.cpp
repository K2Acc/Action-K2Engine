#include "Sphere.h"
#include <imgui.h>

Sphere *Sphere::Create()
{
	Sphere* instance = new Sphere();
	instance->Initialize();
	instance->SphereVertexData();
	return instance;
}

void Sphere::Update()
{
	ImGui::DragFloat3("Pos", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("Rot", &transform_.rotation.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
}

void Sphere::SphereVertexData()
{
	//球体
	//PI円周率
	const float pi = 3.14159265f;
	//経度分割1つ分の角度 φ
	const float kLonEvery = pi * 2.0f / (float)kSubdivision;
	//緯度分割1つ分の角度 Θ
	const float kLatEvert = pi / (float)kSubdivision;

	//経度インデックス
	uint32_t lonIndex = 0;
	//緯度インデックス
	uint32_t latIndex = 0;

	//緯度の方向に分割
	for(latIndex = 0; latIndex < kSubdivision; ++latIndex){
		float lat = -pi / 2.0f + kLatEvert * latIndex;	//Θ
		//経度の方向に分割しながら絵を書く
		for(lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;	//φ

			float u = (float)lonIndex / (float)kSubdivision;
			float v = 1.0f - (float)latIndex / (float)kSubdivision;
			float uvLength = 1.0f / (float)kSubdivision;

			//頂点データを入力
			//一枚目
			vertData_[start].position.x = cos(lat) * cos(lon);
			vertData_[start].position.y = sin(lat);
			vertData_[start].position.z = cos(lat) * sin(lon);
			vertData_[start].position.w = 1.0f;
			vertData_[start].texcoord.x = u;
			vertData_[start].texcoord.y = v;
			vertData_[start].normal.x = vertData_[start].position.x;
			vertData_[start].normal.y = vertData_[start].position.y;
			vertData_[start].normal.z = vertData_[start].position.z;
			
			vertData_[start+1].position.x = cos(lat+kLatEvert) * cos(lon);
			vertData_[start+1].position.y = sin(lat+kLatEvert);
			vertData_[start+1].position.z = cos(lat+kLatEvert) * sin(lon);
			vertData_[start+1].position.w = 1.0f;
			vertData_[start+1].texcoord.x = u;
			vertData_[start+1].texcoord.y = v - uvLength;
			vertData_[start+1].normal.x = vertData_[start+1].position.x;
			vertData_[start+1].normal.y = vertData_[start+1].position.y;
			vertData_[start+1].normal.z = vertData_[start+1].position.z;

			vertData_[start+2].position.x = cos(lat) * cos(lon+kLonEvery);
			vertData_[start+2].position.y = sin(lat);
			vertData_[start+2].position.z = cos(lat) * sin(lon+kLonEvery);
			vertData_[start+2].position.w = 1.0f;
			vertData_[start+2].texcoord.x = u + uvLength;
			vertData_[start+2].texcoord.y = v;
			vertData_[start+2].normal.x = vertData_[start+2].position.x;
			vertData_[start+2].normal.y = vertData_[start+2].position.y;
			vertData_[start+2].normal.z = vertData_[start+2].position.z;

			//二枚目
			vertData_[start+3].position.x = cos(lat+kLatEvert) * cos(lon);
			vertData_[start+3].position.y = sin(lat+kLatEvert);
			vertData_[start+3].position.z = cos(lat+kLatEvert) * sin(lon);
			vertData_[start+3].position.w = 1.0f;
			vertData_[start+3].texcoord.x = u;
			vertData_[start+3].texcoord.y = v - uvLength;
			vertData_[start+3].normal.x = vertData_[start+3].position.x;
			vertData_[start+3].normal.y = vertData_[start+3].position.y;
			vertData_[start+3].normal.z = vertData_[start+3].position.z;
			
			vertData_[start+4].position.x = cos(lat+kLatEvert) * cos(lon+kLonEvery);
			vertData_[start+4].position.y = sin(lat+kLatEvert);
			vertData_[start+4].position.z = cos(lat+kLatEvert) * sin(lon+kLonEvery);
			vertData_[start+4].position.w = 1.0f;
			vertData_[start+4].texcoord.x = u + uvLength;
			vertData_[start+4].texcoord.y = v - uvLength;
			vertData_[start+4].normal.x = vertData_[start+4].position.x;
			vertData_[start+4].normal.y = vertData_[start+4].position.y;
			vertData_[start+4].normal.z = vertData_[start+4].position.z;

			vertData_[start+5].position.x = cos(lat) * cos(lon+kLonEvery);
			vertData_[start+5].position.y = sin(lat);
			vertData_[start+5].position.z = cos(lat) * sin(lon+kLonEvery);
			vertData_[start+5].position.w = 1.0f;
			vertData_[start+5].texcoord.x = u + uvLength;
			vertData_[start+5].texcoord.y = v;
			vertData_[start+5].normal.x = vertData_[start+5].position.x;
			vertData_[start+5].normal.y = vertData_[start+5].position.y;
			vertData_[start+5].normal.z = vertData_[start+5].position.z;

		}
	}
}
