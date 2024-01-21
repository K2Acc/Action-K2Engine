#pragma once
#include <wrl.h>
#include <vector>
#include <list>

#include "Transform.h"
#include "Pipeline.h"
#include "DirectXCommon.h"
#include "Camera.h"


#include "ComputeCommon.h"

//コンピュートシェーダを使用するうえで
/*
* DirectXCommonでCompute用のやつを用意する?
*/

class GPUParticleBase
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
	template <class T> using list = std::list<T>;
public:
	static GPUParticleBase* Create();

	~GPUParticleBase(){
		delete pipeline_;
	}
	void Initialize(bool isIndexEnable = true);
	void Draw(Camera* camera);

private:
	//パイプライン
	void PipelineStateInitialize();

	//頂点リソース/ビュー
	void CreateVertex();
	//定数リソース/ビュー
	void CreateMaterial();
	//行列リソース/ビュー
	void CreateWVP();
	//コンピュートリソース/ビュー
	void CreateCompute();

private:
	//Instance
	DirectXCommon* dxCommon = nullptr;
	ComputeCommon* compute = nullptr;

	//パイプライン関係
	Pipeline* pipeline_ = nullptr;

	vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;		//インプットレイアウト

	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;		//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ComPtr<ID3D12Resource> constResource_;		//定数
	Vector4* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;		//行列
	Matrix4x4* wvpData_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	ComPtr<ID3D12Resource> computeResource_;
	//受信用
	Sample* computeData_ = nullptr;

	//描画方法
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;				//コマンドリスト


protected:
	//頂点データ
	Vector4* vertData_ = nullptr;
	UINT vertNum_ = 1;

	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_WIREFRAME;	//塗りつぶし

	//パラメータ
	const int kNumMaxInstance = 10000;
	list<Transform> transfrom_;
	Vector4 color_ = {0.1f, 0.5f, 0.1f, 1.0f};


	std::string VSPath_ = "Particle/GPUParticle.VS.hlsl";
	std::string PSPath_ = "Particle/Emitter/EmitterFrame.PS.hlsl";
};
