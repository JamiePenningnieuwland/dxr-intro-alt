#include "Structures.h"
#include <Utils.h>
#include <Graphics.h>

Model::Model(std::string filepath, Material& material)
{
	Utils::LoadModel(filepath, vertices, indices, material);

}
/**
* Create the bottom level acceleration structure.
*/
void Model::Create_Bottom_Level_AS(D3D12Global& d3d)
{
	
	// Describe the geometry that goes in the bottom acceleration structure(s)
	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc;
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexBufferView.StrideInBytes;
	geometryDesc.Triangles.VertexCount = static_cast<UINT>(vertices.size());
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.IndexBuffer = indexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexFormat = indexBufferView.Format;
	geometryDesc.Triangles.IndexCount = static_cast<UINT>(indices.size());
	geometryDesc.Triangles.Transform3x4 = 0;
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// Get the size requirements for the BLAS buffers
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS ASInputs = {};
	ASInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	ASInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	ASInputs.pGeometryDescs = &geometryDesc;
	ASInputs.NumDescs = 1;
	ASInputs.Flags = buildFlags;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO ASPreBuildInfo = {};
	d3d.device->GetRaytracingAccelerationStructurePrebuildInfo(&ASInputs, &ASPreBuildInfo);

	ASPreBuildInfo.ScratchDataSizeInBytes = ALIGN(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT, ASPreBuildInfo.ScratchDataSizeInBytes);
	ASPreBuildInfo.ResultDataMaxSizeInBytes = ALIGN(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT, ASPreBuildInfo.ResultDataMaxSizeInBytes);

	// Create the BLAS scratch buffer
	D3D12BufferCreateInfo bufferInfo(ASPreBuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	bufferInfo.alignment = max(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	D3DResources::Create_Buffer(d3d, bufferInfo, &BLAS.pScratch);
#if NAME_D3D_RESOURCES
	BLAS.pScratch->SetName(L"DXR BLAS Scratch");
#endif

	// Create the BLAS buffer
	bufferInfo.size = ASPreBuildInfo.ResultDataMaxSizeInBytes;
	bufferInfo.state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	D3DResources::Create_Buffer(d3d, bufferInfo, &BLAS.pResult);
#if NAME_D3D_RESOURCES
	BLAS.pResult->SetName(L"DXR BLAS");
#endif

	// Describe and build the bottom level acceleration structure
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
	buildDesc.Inputs = ASInputs;
	buildDesc.ScratchAccelerationStructureData = BLAS.pScratch->GetGPUVirtualAddress();
	buildDesc.DestAccelerationStructureData = BLAS.pResult->GetGPUVirtualAddress();

	d3d.cmdList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

	// Wait for the BLAS build to complete
	D3D12_RESOURCE_BARRIER uavBarrier;
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = BLAS.pResult;
	uavBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d.cmdList->ResourceBarrier(1, &uavBarrier);
}
/*
* Create the vertex buffer.
*/
void Model::Create_Vertex_Buffer(D3D12Global& d3d)
{
	// Create the vertex buffer resource
	D3D12BufferCreateInfo info(((UINT)vertices.size() * sizeof(Vertex)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
	D3DResources::Create_Buffer(d3d, info, &vertexBuffer);
#if NAME_D3D_RESOURCES
	vertexBuffer->SetName(L"Vertex Buffer");
#endif

	// Copy the vertex data to the vertex buffer
	UINT8* pVertexDataBegin;
	D3D12_RANGE readRange = {};
	HRESULT hr = vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	Utils::Validate(hr, L"Error: failed to map vertex buffer!");

	memcpy(pVertexDataBegin, vertices.data(), info.size);
	vertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view
	vertexBufferView.BufferLocation =vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = static_cast<UINT>(info.size);
}

/**
* Create the index buffer.
*/
void Model::Create_Index_Buffer(D3D12Global& d3d)
{
	// Create the index buffer resource
	D3D12BufferCreateInfo info((UINT)indices.size() * sizeof(UINT), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
	D3DResources::Create_Buffer(d3d, info, &indexBuffer);
#if NAME_D3D_RESOURCES
	indexBuffer->SetName(L"Index Buffer");
#endif

	// Copy the index data to the index buffer
	UINT8* pIndexDataBegin;
	D3D12_RANGE readRange = {};
	HRESULT hr = indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
	Utils::Validate(hr, L"Error: failed to map index buffer!");

	memcpy(pIndexDataBegin,indices.data(), info.size);
	indexBuffer->Unmap(0, nullptr);

	// Initialize the index buffer view
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = static_cast<UINT>(info.size);
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}
void Model::Create_DescriptorHeaps(D3D12Global& d3d, D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
	int descriptorSize = d3d.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Create the index buffer SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC indexSRVDesc;
	indexSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	indexSRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	indexSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
	indexSRVDesc.Buffer.StructureByteStride = 0;
	indexSRVDesc.Buffer.FirstElement = 0;
	indexSRVDesc.Buffer.NumElements = (static_cast<UINT>(indices.size()) * sizeof(UINT)) / sizeof(float);
	indexSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	UINT handleIncrement = d3d.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	handle.ptr +=  descriptorSize;
	d3d.device->CreateShaderResourceView(indexBuffer, &indexSRVDesc, handle);
	

	// Create the vertex buffer SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC vertexSRVDesc;
	vertexSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	vertexSRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	vertexSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
	vertexSRVDesc.Buffer.StructureByteStride = 0;
	vertexSRVDesc.Buffer.FirstElement = 0;
	vertexSRVDesc.Buffer.NumElements = (static_cast<UINT>(vertices.size()) * sizeof(Vertex)) / sizeof(float);
	vertexSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	handle.ptr +=  descriptorSize;
	d3d.device->CreateShaderResourceView(vertexBuffer, &vertexSRVDesc, handle);;

}