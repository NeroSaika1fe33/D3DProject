#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
struct StaticMeshComponentVertexData {
	float mPosition[4];
	float mTexcoord[4];
	float mNormal[4];
	float mTangent[4];
};
struct SubMesh {
	ID3D12Resource* mIBO;
	D3D12_INDEX_BUFFER_VIEW mIBView;
	int mIndexCount;
};
class StaticMeshComponent{
public:
	ID3D12Resource* mVBO;
	D3D12_VERTEX_BUFFER_VIEW mVBOView;
	StaticMeshComponentVertexData* mVertexData;
	int mVertexCount;
	std::unordered_map<std::string, SubMesh*> mSubMeshes;
	void SetVertexCount(int inVertexCount);
	void SetVertexPosition(int inIndex, float inX, float inY, float inZ, float inW = 1.0f);
	void SetVertexTexcoord(int inIndex, float inX, float inY, float inZ, float inW = 1.0f);
	void SetVertexNormal(int inIndex, float inX, float inY, float inZ, float inW = 1.0f);
	void SetVertexTangent(int inIndex, float inX, float inY, float inZ, float inW = 1.0f);
	void InitFromFile(ID3D12GraphicsCommandList*inCommandList,const char* inFilePath);
	void Render(ID3D12GraphicsCommandList* inCommandList);
};

