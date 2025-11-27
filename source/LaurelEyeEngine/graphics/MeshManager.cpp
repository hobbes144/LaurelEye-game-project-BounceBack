#include "LaurelEyeEngine/graphics/MeshManager.h"
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
#include "LaurelEyeEngine/graphics/resources/RenderMeshResource.h"

namespace LaurelEye::Graphics {

    MeshManager::MeshManager(RenderResources& _renderResources) : renderResources(_renderResources) {
    }

    MeshManager::~MeshManager() {
        destroyAllMeshes();
    }

    MeshHandle MeshManager::allocateMeshHandle(const std::string& name) {
        MeshHandle handle = static_cast<MeshHandle>(meshes.size());
        meshes.push_back(RenderMeshResource{});
        meshes.back().handle = handle;
        meshes.back().name = name;
        meshNames[name] = handle;

        return handle;
    }

    MeshHandle MeshManager::getHandle(const std::string& name) const {
        auto it = meshNames.find(name);
        if ( it == meshNames.end() ) {
            return RenderMesh::InvalidMesh;
        }
        return it->second;
    }

    const RenderMeshResource* MeshManager::getMesh(MeshHandle h) const {
        if ( !isValidMesh(h) || h >= meshes.size() ) return nullptr;
        return &meshes[h];
    }

    RenderMeshResource* MeshManager::getMesh(MeshHandle h) {
        if ( !isValidMesh(h) || h >= meshes.size() ) return nullptr;
        return &meshes[h];
    }

    MeshHandle MeshManager::createMesh(
        const std::string& name,
        const MeshVertex* vertices,
        uint32_t vertexCount,
        const uint32_t* indices,
        uint32_t indexCount) {

        MeshHandle handle = allocateMeshHandle(name);
        RenderMeshResource& mesh = meshes[handle];

        mesh.gpu = createGpuMeshForStatic(name, vertices, vertexCount, indices, indexCount);
        // mesh.localBounds = computeLocalBounds(vertices, vertexCount);

        mesh.alive = true;

        return handle;
    }

    MeshHandle MeshManager::createSkinnedMesh(
        const std::string& name,
        const SkinnedMeshVertex* vertices,
        uint32_t vertexCount,
        const uint32_t* indices,
        uint32_t indexCount,
        // SkeletonHandle skeleton,
        // const Matrix4* inverseBindMatrices,
        uint32_t boneCount,
        uint32_t maxBonesPerVertex) {
        MeshHandle handle = allocateMeshHandle(name);
        RenderMeshResource& mesh = meshes[handle];

        // GPU resources for this skinned mesh
        mesh.gpu = createGpuMeshForSkinned(name, vertices, vertexCount, indices, indexCount, maxBonesPerVertex);
        // mesh.localBounds = computeLocalBounds(vertices, vertexCount);

        // Skin data stored directly on the mesh
        // mesh.skeleton = skeleton;
        // mesh.inverseBindMatrices.assign(inverseBindMatrices, inverseBindMatrices + boneCount);
        mesh.maxBonesPerVertex = maxBonesPerVertex;

        mesh.alive = true;

        return handle;
    }

    // MeshHandle MeshManager::createSkinnedMesh(
    //     const std::string& name,
    //     const SkinnedMeshVertex* vertices,
    //     uint32_t vertexCount,
    //     const uint32_t* indices,
    //     uint32_t indexCount,
    //     SkeletonHandle skeletonHandle,
    //     const std::vector<uint32_t>& meshBoneIndices // mapping from mesh bones -> skeleton bones
    // ) {
    //     MeshHandle handle = allocateMeshHandle(name);
    //     RenderMeshResource& mesh = meshes[handle];
    //
    //     // Skeleton* skeleton = skeletonManager.getSkeleton(skeletonHandle);
    //     // assume skeleton != nullptr
    //
    //     // 1. GPU stuff
    //     mesh.gpu = createGpuMeshForSkinned(name, vertices, vertexCount, indices, indexCount, /*maxBonesPerVertex*/ 4);
    //     // mesh.localBounds = computeLocalBounds(vertices, vertexCount);
    //
    //     // 2. Skin data stored on the mesh
    //     mesh.skeleton = skeletonHandle;
    //     mesh.inverseBindMatrices.clear();
    //     mesh.inverseBindMatrices.reserve(meshBoneIndices.size());
    //
    //     for ( uint32_t meshBoneIdx = 0; meshBoneIdx < meshBoneIndices.size(); ++meshBoneIdx ) {
    //         uint32_t skelBoneIdx = meshBoneIndices[meshBoneIdx];
    //         // const Matrix4& globalBind = skeleton->bones[skelBoneIdx].globalBindPose;
    //         // mesh.inverseBindMatrices.push_back(globalBind.inverse());
    //     }
    //
    //     mesh.maxBonesPerVertex = 4;
    //
    //     return handle;
    // }

    void MeshManager::destroyMesh(MeshHandle h) {
        if ( !isValidMesh(h) || h >= meshes.size() ) return;

        RenderMeshResource& mesh = meshes[h];

        if ( !mesh.alive ) return;

        destroyGpuMesh(mesh.gpu);

        if ( mesh.isSkinned() ) {
            mesh.skeleton = Skeleton::InvalidSkeleton;
            // mesh.inverseBindMatrices.clear();
            mesh.maxBonesPerVertex = 0;
        }

        mesh.alive = false;
    }

    void MeshManager::destroyAllMeshes() {
        for ( size_t h = 0; h < meshes.size(); ++h ) {
            destroyMesh(h);
        }

        meshes.clear();
        meshNames.clear();
        primitiveMeshes.clear();
    }

    GpuMesh MeshManager::createGpuMeshForStatic(
        const std::string& meshName,
        const MeshVertex* vertices,
        uint32_t vertexCount,
        const uint32_t* indices,
        uint32_t indexCount) {

        GpuMesh gpu{};

        const uint64_t vertexDataSize = static_cast<uint64_t>(vertexCount) * sizeof(MeshVertex);
        const uint64_t indexDataSize = static_cast<uint64_t>(indexCount) * sizeof(uint32_t);

        const std::string vbName = meshName + ":VB";
        const std::string ibName = meshName + ":IB";
        const std::string vaoName = meshName + ":VAO";

        // 1. Buffers
        auto vbDesc = makeVertexBufferDesc(vertexDataSize);
        gpu.vertexBuffer = renderResources.createDataBuffer(vbName, vbDesc, std::string{}, vertices);

        auto ibDesc = makeIndexBufferDesc(indexDataSize);
        gpu.indexBuffer = renderResources.createDataBuffer(ibName, ibDesc, std::string{}, indices);

        gpu.indexCount = indexCount;
        gpu.indexType = IndexType::Uint32;

        // 2. VAO
        VertexArrayDesc vaoDesc;

        vaoDesc.vertexBindings.push_back(VertexBufferBindingDesc{
            .buffer = gpu.vertexBuffer,
            .bindingIndex = 0,
            .stride = sizeof(MeshVertex),
            .offset = 0});

        // Attributes: adjust locations to match your shader

        // layout(location = 0) in vec3 aPosition;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 0,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(MeshVertex, position)),
            .format = VertexAttribFormat::Float32x3});

        // layout(location = 1) in vec3 aNormal;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 1,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(MeshVertex, normal)),
            .format = VertexAttribFormat::Float32x3});

        // layout(location = 2) in vec2 aUV;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 2,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(MeshVertex, uv)),
            .format = VertexAttribFormat::Float32x2});

        // layout(location = 3) in vec3 aTangents;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 3,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(MeshVertex, tangent)),
            .format = VertexAttribFormat::Float32x3});

        vaoDesc.indexBuffer = gpu.indexBuffer;

        gpu.vao = renderResources.createVertexArray(vaoName, vaoDesc);

        return gpu;
    }

    GpuMesh MeshManager::createGpuMeshForSkinned(
        const std::string& meshName,
        const SkinnedMeshVertex* vertices,
        uint32_t vertexCount,
        const uint32_t* indices,
        uint32_t indexCount,
        uint32_t maxBonesPerVertex) {
        (void)maxBonesPerVertex; // for now, we assume 4; can assert if needed

        GpuMesh gpu{};

        const uint64_t vertexDataSize = static_cast<uint64_t>(vertexCount) * sizeof(SkinnedMeshVertex);
        const uint64_t indexDataSize = static_cast<uint64_t>(indexCount) * sizeof(uint32_t);

        const std::string vbName = meshName + ":VB";
        const std::string ibName = meshName + ":IB";
        const std::string vaoName = meshName + ":VAO";

        // 1. Buffers
        auto vbDesc = makeVertexBufferDesc(vertexDataSize);
        gpu.vertexBuffer = renderResources.createDataBuffer(vbName, vbDesc, std::string{}, vertices);

        auto ibDesc = makeIndexBufferDesc(indexDataSize);
        gpu.indexBuffer = renderResources.createDataBuffer(ibName, ibDesc, std::string{}, indices);

        gpu.indexCount = indexCount;
        gpu.indexType = IndexType::Uint32;

        // 2. VAO
        VertexArrayDesc vaoDesc;

        vaoDesc.vertexBindings.push_back(VertexBufferBindingDesc{
            .buffer = gpu.vertexBuffer,
            .bindingIndex = 0,
            .stride = sizeof(SkinnedMeshVertex),
            .offset = 0});

        // Attributes: adjust locations to match your shader

        // layout(location = 0) in vec3 aPosition;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 0,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(SkinnedMeshVertex, position)),
            .format = VertexAttribFormat::Float32x3});

        // layout(location = 1) in vec3 aNormal;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 1,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(SkinnedMeshVertex, normal)),
            .format = VertexAttribFormat::Float32x3});

        // layout(location = 2) in vec2 aUV;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 2,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(SkinnedMeshVertex, uv)),
            .format = VertexAttribFormat::Float32x2});

        // layout(location = 3) in vec3 aTangents;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 3,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(MeshVertex, tangent)),
            .format = VertexAttribFormat::Float32x3});

        // layout(location = 3) in vec4 aBoneIndicesEncoded;  // we'll decode ints in shader
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 4,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(SkinnedMeshVertex, boneIndices)),
            .format = VertexAttribFormat::Uint8x4Norm});

        // layout(location = 4) in vec4 aBoneWeights;
        vaoDesc.attributes.push_back(VertexAttributeDesc{
            .location = 5,
            .bindingIndex = 0,
            .relativeOffset = static_cast<uint32_t>(offsetof(SkinnedMeshVertex, boneWeights)),
            .format = VertexAttribFormat::Float32x4});

        vaoDesc.indexBuffer = gpu.indexBuffer;

        gpu.vao = renderResources.createVertexArray(vaoName, vaoDesc);

        return gpu;
    }

    void MeshManager::destroyGpuMesh(const GpuMesh& r) {
        renderResources.destroyVertexArray(r.vao);
        renderResources.destroyDataBuffer(r.vertexBuffer);
        renderResources.destroyDataBuffer(r.indexBuffer);
    }

} // namespace LaurelEye::Graphics
