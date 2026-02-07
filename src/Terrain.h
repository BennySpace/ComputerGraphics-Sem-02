#pragma once

#include "Common/d3dUtil.h"
#include "Common/MathHelper.h"
#include "QuadTree.h"

#include <vector>
#include <memory>

struct TerrainVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 TexC;
};

class Terrain
{
public:
    Terrain(ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        float terrainSize,
        float minHeight,
        float maxHeight);

    ~Terrain() = default;

    // Load heightmap from raw binary file (8-bit or 16-bit)
    bool LoadHeightmap(const std::wstring& filename,
        UINT width,
        UINT height,
        bool is16Bit = true);

    // Load heightmap from DDS texture and optionally generate procedural data
    bool LoadHeightmapDDS(const std::wstring& filename,
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList);

    // Generate fractal Brownian motion (fBm) based heightmap using Perlin noise
    void GenerateProceduralHeightmap(UINT width,
        UINT height,
        float frequency,
        int octaves);

    // Creates vertex and index buffers for all LOD levels
    void BuildGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    // Height at world position (x,z) — bilinear interpolated
    float GetHeight(float x, float z) const;

    // Normal at world position (x,z) — computed using finite differences
    DirectX::XMFLOAT3 GetNormal(float x, float z) const;

    float GetTerrainSize() const { return mTerrainSize; }
    float GetMinHeight() const { return mMinHeight; }
    float GetMaxHeight() const { return mMaxHeight; }
    UINT  GetHeightmapWidth() const { return mHeightmapWidth; }
    UINT  GetHeightmapHeight() const { return mHeightmapHeight; }

    MeshGeometry* GetGeometry() { return mGeometry.get(); }
    ID3D12Resource* GetHeightmapResource() { return mHeightmapTexture.Get(); }

    static const char* GetLODMeshName(int lod);

private:
    // Builds vertex/index data for one specific LOD level
    void BuildLODMesh(int lodLevel, UINT gridSize);

    // Currently not used — left for possible future normal map generation
    void CalculateNormals();

    float SampleHeight(int x, int z) const;

    // Classic Perlin noise 2D implementation
    float PerlinNoise(float x, float z) const;

    float Fade(float t) const;
    float Lerp(float a, float b, float t) const;
    float Grad(int hash, float x, float z) const;

private:
    ID3D12Device* md3dDevice = nullptr;

    float mTerrainSize = 0.0f;
    float mMinHeight = 0.0f;
    float mMaxHeight = 0.0f;

    UINT mHeightmapWidth = 0;
    UINT mHeightmapHeight = 0;

    // Normalized height values [0..1]
    std::vector<float> mHeightmap;

    std::unique_ptr<MeshGeometry> mGeometry;

    // GPU texture containing height data (can be used in shaders)
    Microsoft::WRL::ComPtr<ID3D12Resource> mHeightmapTexture;
    Microsoft::WRL::ComPtr<ID3D12Resource> mHeightmapUploadBuffer;

    // Permutation table for Perlin noise (doubled for convenient indexing)
    std::vector<int> mPermutation;
};