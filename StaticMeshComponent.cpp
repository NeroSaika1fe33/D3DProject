#include "StaticMeshComponent.h"
#include "BattleFireDirect.h"
#include <stdio.h>

void StaticMeshComponent::SetVertexCount(int inVertexCount) {
	mVertexCount = inVertexCount;
	mVertexData = new StaticMeshComponentVertexData[inVertexCount];
	memset(mVertexData, 0, sizeof(StaticMeshComponentVertexData)*inVertexCount);
}
void StaticMeshComponent::SetVertexPosition(int inIndex, float inX, float inY, float inZ, float inW /* = 1.0f */) {
	mVertexData[inIndex].mPosition[0] = inX;
	mVertexData[inIndex].mPosition[1] = inY;
	mVertexData[inIndex].mPosition[2] = inZ;
	mVertexData[inIndex].mPosition[3] = inW;
}
void StaticMeshComponent::SetVertexTexcoord(int inIndex, float inX, float inY, float inZ, float inW /* = 1.0f */) {
	mVertexData[inIndex].mTexcoord[0] = inX;
	mVertexData[inIndex].mTexcoord[1] = inY;
	mVertexData[inIndex].mTexcoord[2] = inZ;
	mVertexData[inIndex].mTexcoord[3] = inW;
}
void StaticMeshComponent::SetVertexNormal(int inIndex, float inX, float inY, float inZ, float inW /* = 1.0f */) {
	mVertexData[inIndex].mNormal[0] = inX;
	mVertexData[inIndex].mNormal[1] = inY;
	mVertexData[inIndex].mNormal[2] = inZ;
	mVertexData[inIndex].mNormal[3] = inW;
}
void StaticMeshComponent::SetVertexTangent(int inIndex, float inX, float inY, float inZ, float inW /* = 1.0f */) {
	mVertexData[inIndex].mTangent[0] = inX;
	mVertexData[inIndex].mTangent[1] = inY;
	mVertexData[inIndex].mTangent[2] = inZ;
	mVertexData[inIndex].mTangent[3] = inW;
}
void StaticMeshComponent::InitFromFile(ID3D12GraphicsCommandList* inCommandList, const char* inFilePath) {
	FILE* pFile = nullptr;
	errno_t err = fopen_s(&pFile, inFilePath, "rb");
	if (err == 0) {
		int temp = 0;
		fread(&temp, 4, 1, pFile);
		mVertexCount = temp;
		mVertexData = new StaticMeshComponentVertexData[mVertexCount];
		fread(mVertexData, 1, sizeof(StaticMeshComponentVertexData) * mVertexCount, pFile);
		mVBO=CreateBufferObject(inCommandList,mVertexData,
			sizeof(StaticMeshComponentVertexData) * mVertexCount,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		mVBOView.BufferLocation = mVBO->GetGPUVirtualAddress();
		mVBOView.SizeInBytes = sizeof(StaticMeshComponentVertexData) * mVertexCount;
		mVBOView.StrideInBytes = sizeof(StaticMeshComponentVertexData);

		while (!feof(pFile)) {
			fread(&temp, 4, 1, pFile);
			if (feof(pFile)) {
				break;
			}
			char name[256] = {0};
			fread(name, 1, temp, pFile);
			fread(&temp, 4, 1, pFile);
			SubMesh* submesh = new SubMesh;
			submesh->mIndexCount = temp;
			unsigned int *indexes = new unsigned int[temp];
			fread(indexes, 1, sizeof(unsigned int) * temp, pFile);
			submesh->mIBO = CreateBufferObject(inCommandList, indexes,
				sizeof(unsigned int) * temp,
				D3D12_RESOURCE_STATE_INDEX_BUFFER);

			submesh->mIBView.BufferLocation = submesh->mIBO->GetGPUVirtualAddress();
			submesh->mIBView.SizeInBytes = sizeof(unsigned int) * temp;
			submesh->mIBView.Format = DXGI_FORMAT_R32_UINT;
			mSubMeshes.insert(std::pair<std::string, SubMesh*>(name,submesh));
			delete[]indexes;
		}
		fclose(pFile);
	}
}
void StaticMeshComponent::Render(ID3D12GraphicsCommandList* inCommandList) {
	D3D12_VERTEX_BUFFER_VIEW vbos[] = {
		mVBOView
	};
	inCommandList->IASetVertexBuffers(0, 1, vbos);
	if (mSubMeshes.empty()) {
		inCommandList->DrawInstanced(mVertexCount, 1, 0, 0);
	}
	else {
		for (auto iter = mSubMeshes.begin();
			iter != mSubMeshes.end(); iter++) {
			inCommandList->IASetIndexBuffer(&iter->second->mIBView);
			inCommandList->DrawIndexedInstanced(iter->second->mIndexCount, 1, 0, 0, 0);
		}
	}
}