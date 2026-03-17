#pragma once
namespace Andromeda {
	struct MeshGPUHandle;
	struct Mesh;
	void createMesh(MeshGPUHandle& gpuMesh, const Mesh& mesh);
} 