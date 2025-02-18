#pragma once
#include <d3d12.h>
#include <cstdint>

#include "DescriptorData.h"

//ディスクリプタヒープ作成
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heaptype, UINT numDescriptors, bool shaderVisible);

//ディスクリプタヒープハンドル取得
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorSRVData& descriptorHeap, uint32_t descriptorSize);
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorRTVData& descriptorHeap, uint32_t descriptorSize);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(DescriptorSRVData& descriptorHeap, uint32_t descriptorSize);