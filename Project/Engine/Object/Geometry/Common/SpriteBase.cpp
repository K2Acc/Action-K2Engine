#include "SpriteBase.h"

#include "SpriteLoader.h"
#include "DescriptorHeap.h"

#include "BufferResource.h"
#include "BufferView.h"

#include "GlobalVariables.h"

void SpriteBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();

	texture_ = SpriteLoader::SearchTexture(texturePath_);
	textureSize_ = texture_.size;

	//パイプライン
	pipeline_ = new Pipeline();
	PipelineStateInitialize();	//パイプライン初期化

	//リソース
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();
}

void SpriteBase::Draw(Camera* camera)
{
	Matrix4x4 viewProjMatrix = MakeIdentityMatrix();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f,0.0f, (float)WindowsApp::kWindowWidth_,(float)WindowsApp::kWindowHeight_, 0.0f,100.0f);
	viewProjMatrix = projectionMatrixSprite;
	
	Matrix4x4 worldViewProjectionMatrixSprite = GetWorldMatrix() * viewProjMatrix;
	wvpData_->WVP = worldViewProjectionMatrixSprite;
	wvpData_->World = worldViewProjectionMatrixSprite;

	materialData_->uvTransform= MakeAffineMatrix(uvTransformSprite_.scale,uvTransformSprite_.rotation,uvTransformSprite_.translate);


	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology_);

	//マテリアルのconstBufferの場所を設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constResource_->GetGPUVirtualAddress());
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_.srvHandleGPU_);

	//描画
	dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0);
}


void SpriteBase::PipelineStateInitialize()
{
	//デスクリプタレンジ(SRV, CBVなどの情報をこれにまとめる)
	//例 :														Shaderでは
	//range[0]					range[1]						ConstBuffer<..>gMaterial0 : register(b0)
	//BaseRegister = 3;			BaseRegister = 0;				ConstBuffer<..>gMaterial1 : register(b1)
	//numDescriptor = 2;		NumDescritor = 3;				ConstBuffer<..>gMaterial2 : register(b2)
	//Type = SRV;				Type = CBV;						Texture2D<..> gTexture0 : register(t3)
	//															Texture2D<..> gTexture1 : register(t4)
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	//0から開始
	descriptorRange[0].NumDescriptors = 1;	//数は1
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//SRVを使用
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;		//Offsetを自動計算

	//ルートパラメータ設定
	rootParameters_.resize(3);
	//PS
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader使用
	rootParameters_[0].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0	
	//※RegisterとはShader上でのResource配置場所の情報　bというのは(ConstantBuffer)を意味
	//VS
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters_[1].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0
	//SRV(テクスチャ		シェーダでは各ピクセルのことをいう)
	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//DescriptorTableに使用
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange;	//tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//Tableで利用する数


	//Sampler設定(シェーダーのPS SamplerState　シェーダでは画像のことをいう)
	staticSamplers_.resize(1);
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//バイリニアフィルタ・
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//0 ~ 1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;	//ありったけのMipMapを使用
	staticSamplers_[0].ShaderRegister = 0;	//レジスタ番号0
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用

	
	//インプットレイアウト設定(頂点データでシェーダ内に送るデータたちのセマンティック名)
	inputElementDesc_.resize(3);
	inputElementDesc_[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDesc_[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//float4 型
	inputElementDesc_[0].InputSlot = 0;
	inputElementDesc_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDesc_[0].InstanceDataStepRate = 0;

	inputElementDesc_[1].SemanticName = "TEXCOORD";							//頂点シェーダーのセマンティック名
	inputElementDesc_[1].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc_[1].Format = DXGI_FORMAT_R32G32_FLOAT;					//float4 型
	inputElementDesc_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDesc_[2].SemanticName = "NORMAL";
	inputElementDesc_[2].SemanticIndex = 0;
	inputElementDesc_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//生成
	std::wstring vs = WindowsApp::ConvertString(VSPath_);
	std::wstring ps = WindowsApp::ConvertString(PSPath_);

	pipeline_->DepthStencilSet();
	pipeline_->Create(
		vs,
		ps,
		rootParameters_,
		staticSamplers_,
		inputElementDesc_,
		fillMode_,
		pipelinePrimitiveTopology_
	);
}

void SpriteBase::CreateVertex()
{
	vertexResource_ = CreateBufferResource(dxCommon->GetDevice() ,sizeof(VertexData)*4);
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*4, sizeof(VertexData));
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void SpriteBase::CreateIndex()
{
	indexResource_ = CreateBufferResource(dxCommon->GetDevice() ,sizeof(uint32_t)*6);
	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*6);
	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void SpriteBase::CreateMaterial()
{
	constResource_ = CreateBufferResource(dxCommon->GetDevice() ,sizeof(TextureMaterial));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	materialData_->color = color_;
	materialData_->uvTransform =materialData_->uvTransform.MakeIdentityMatrix();
}

void SpriteBase::CreateWVP()
{
	wvpResource_ = CreateBufferResource(dxCommon->GetDevice() ,sizeof(TransformationMatrix));
	wvpResource_->Map(0,nullptr,reinterpret_cast<void**>(&wvpData_));
	Matrix4x4 worldMatrixSprite = worldMatrixSprite.MakeIdentityMatrix();
	wvpData_->WVP = worldMatrixSprite;
	wvpData_->World = worldMatrixSprite;
}

void SpriteBase::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(texture_.name);
	globalVariables->AddItem(texture_.name, "0.translate", translate);
	globalVariables->AddItem(texture_.name, "1.rotate", rotation);
	globalVariables->AddItem(texture_.name, "2.scale", scale);
#endif // _DEBUG
}

void SpriteBase::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	translate = globalVariables->GetVector3Value(texture_.name, "0.translate");
	rotation = globalVariables->GetVector3Value(texture_.name, "1.rotate");
	scale = globalVariables->GetVector3Value(texture_.name, "2.scale");
#endif // _DEBUG
}
