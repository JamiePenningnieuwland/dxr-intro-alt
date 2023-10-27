/* Copyright (c) 2018-2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Common.h"
#include "../DXRstructures.h"
#include <winnt.h>

//--------------------------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------------------------

static bool CompareVector3WithEpsilon(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	const DirectX::XMFLOAT3 vector3Epsilon = DirectX::XMFLOAT3(0.00001f, 0.00001f, 0.00001f);
	return DirectX::XMVector3NearEqual(DirectX::XMLoadFloat3(&lhs), DirectX::XMLoadFloat3(&rhs), DirectX::XMLoadFloat3(&vector3Epsilon)) == TRUE;
}

static bool CompareVector2WithEpsilon(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	const DirectX::XMFLOAT2 vector2Epsilon = DirectX::XMFLOAT2(0.00001f, 0.00001f);
	return DirectX::XMVector3NearEqual(DirectX::XMLoadFloat2(&lhs), DirectX::XMLoadFloat2(&rhs), DirectX::XMLoadFloat2(&vector2Epsilon)) == TRUE;
}

//--------------------------------------------------------------------------------------
// Global Structures
//--------------------------------------------------------------------------------------

struct ConfigInfo 
{
	int				width = 640;
	int				height = 360;
	bool			vsync = false;
	HINSTANCE		instance = NULL;
};

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;

	bool operator==(const Vertex &v) const 
	{
		if (CompareVector3WithEpsilon(position, v.position)) 
		{
			if (CompareVector2WithEpsilon(uv, v.uv)) return true;
			return true;
		}
		return false;
	}

	Vertex& operator=(const Vertex& v) 
	{
		position = v.position;
		uv = v.uv;
		return *this;
	}
};

struct Material 
{
	std::string name = "defaultMaterial";
	std::string texturePath = "";
	float  textureResolution = 512;
};


struct TextureInfo
{
	std::vector<UINT8> pixels;
	int width = 0;
	int height = 0;
	int stride = 0;
	int offset = 0;
};

struct MaterialCB 
{
	DirectX::XMFLOAT4 resolution;
};

struct ViewCB
{
	DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT4 viewOriginAndTanHalfFovY = DirectX::XMFLOAT4(0, 0.f, 0.f, 0.f);
	DirectX::XMFLOAT2 resolution = DirectX::XMFLOAT2(1280, 720);
};

//--------------------------------------------------------------------------------------
// D3D12
//--------------------------------------------------------------------------------------

struct D3D12BufferCreateInfo
{
	UINT64 size = 0;
	UINT64 alignment = 0;
	D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	D3D12BufferCreateInfo() {}

	D3D12BufferCreateInfo(UINT64 InSize, D3D12_RESOURCE_FLAGS InFlags) : size(InSize), flags(InFlags) {}

	D3D12BufferCreateInfo(UINT64 InSize, D3D12_HEAP_TYPE InHeapType, D3D12_RESOURCE_STATES InState) :
		size(InSize),
		heapType(InHeapType),
		state(InState) {}

	D3D12BufferCreateInfo(UINT64 InSize, D3D12_RESOURCE_FLAGS InFlags, D3D12_RESOURCE_STATES InState) :
		size(InSize),
		flags(InFlags),
		state(InState) {}

	D3D12BufferCreateInfo(UINT64 InSize, UINT64 InAlignment, D3D12_HEAP_TYPE InHeapType, D3D12_RESOURCE_FLAGS InFlags, D3D12_RESOURCE_STATES InState) :
		size(InSize),
		alignment(InAlignment),
		heapType(InHeapType),
		flags(InFlags),
		state(InState) {}
};


struct BlasInstance
{
	AccelerationStructureBuffer BLAS;
	DirectX::XMMATRIX transform;
};
struct D3D12Resources 
{
	std::vector<BlasInstance>                       instances;
	ID3D12Resource*									DXROutput;

	ID3D12Resource*									viewCB = nullptr;
	ViewCB											viewCBData;
	UINT8*											viewCBStart = nullptr;

	ID3D12Resource*									materialCB = nullptr;
	MaterialCB										materialCBData;	
	UINT8*											materialCBStart = nullptr;

	ID3D12DescriptorHeap*							rtvHeap = nullptr;
	ID3D12DescriptorHeap*							descriptorHeap = nullptr;

	ID3D12Resource*									texture = nullptr;
	ID3D12Resource*									textureUploadResource = nullptr;

	UINT											rtvDescSize = 0;

};

struct D3D12Global
{
	IDXGIFactory4*									factory = nullptr;
	IDXGIAdapter1*									adapter = nullptr;
	ID3D12Device5*									device = nullptr;
	ID3D12GraphicsCommandList4*						cmdList = nullptr;
	ID3D12CommandQueue*								cmdQueue = nullptr;
	ID3D12CommandAllocator*							cmdAlloc[2] = { nullptr, nullptr };

	IDXGISwapChain3*								swapChain = nullptr;
	ID3D12Resource*									backBuffer[2] = { nullptr, nullptr };

	ID3D12Fence*									fence = nullptr;
	UINT64											fenceValues[2] = { 0, 0 };
	HANDLE											fenceEvent;
	UINT											frameIndex = 0;

	int												width = 640;
	int												height = 360;
	bool											vsync = false;
};


class Model
{
public:
	Model() = default;
	Model(std::string filepath, Material& material);
	AccelerationStructureBuffer& GetBLAS() { return BLAS; }
	~Model()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		//SAFE_RELEASE(BLAS.pScratch);
		//SAFE_RELEASE(BLAS.pResult);
		//SAFE_RELEASE(BLAS.pInstanceDesc);
	}
	void Create_DescriptorHeaps(D3D12Global& d3d, D3D12_CPU_DESCRIPTOR_HANDLE& handleStart);
	void Create_Bottom_Level_AS(D3D12Global& d3d);
	void Create_Vertex_Buffer(D3D12Global& d3d);
	void Create_Index_Buffer(D3D12Global& d3d);

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	ID3D12Resource* vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW						vertexBufferView;
	ID3D12Resource* indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW							indexBufferView;

private:
	
	AccelerationStructureBuffer						BLAS;
};
