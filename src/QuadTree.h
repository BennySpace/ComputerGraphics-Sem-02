#pragma once

#include "Common/d3dUtil.h"
#include "Common/MathHelper.h"

#include <vector>
#include <memory>

struct BoundingBoxAABB
{
    DirectX::XMFLOAT3 Center{};
    DirectX::XMFLOAT3 Extents{};

    bool Intersects(const DirectX::XMFLOAT4* frustumPlanes) const;
};

struct TerrainNode
{
    float X = 0.0f;
    float Z = 0.0f;
    float Size = 0.0f;

    int LODLevel = 0;
    int MaxLOD = 0;

    BoundingBoxAABB Bounds{};
    float MinY = 0.0f;
    float MaxY = 0.0f;

    // Tree topology
    bool IsLeaf = true;
    std::unique_ptr<TerrainNode> Children[4];

    bool IsVisible = false;
    UINT ObjectCBIndex = 0;

    TerrainNode() = default;
};

class QuadTree
{
public:
    QuadTree();
    ~QuadTree() = default;

    void Initialize(float terrainSize, float minNodeSize, int maxLODLevels);

    void Update(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT4* frustumPlanes);

    void GetVisibleNodes(std::vector<TerrainNode*>& outNodes);

    void SetHeightRange(float x, float z, float size, float minY, float maxY);

    int GetVisibleNodeCount() const { return mVisibleNodeCount; }
    int GetTotalNodeCount() const { return mTotalNodeCount; }

    void SetLODDistances(const std::vector<float>& distances) { mLODDistances = distances; }

private:
    void BuildTree(TerrainNode* node, float x, float z, float size, int depth);
    void UpdateNode(TerrainNode* node,
        const DirectX::XMFLOAT3& cameraPos,
        const DirectX::XMFLOAT4* frustumPlanes);

    void CollectVisibleNodes(TerrainNode* node, std::vector<TerrainNode*>& outNodes);

    int  CalculateLOD(const TerrainNode* node, const DirectX::XMFLOAT3& cameraPos) const;
    bool ShouldSubdivide(const TerrainNode* node, const DirectX::XMFLOAT3& cameraPos) const;

private:
    std::unique_ptr<TerrainNode> mRoot;

    float mTerrainSize = 0.0f;
    float mMinNodeSize = 0.0f;
    int   mMaxLODLevels = 0;

    std::vector<float> mLODDistances;

    int  mVisibleNodeCount = 0;
    int  mTotalNodeCount = 0;
    UINT mNextObjectCBIndex = 0;
};