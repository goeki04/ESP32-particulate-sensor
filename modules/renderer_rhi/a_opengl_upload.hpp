#pragma once
namespace Andromeda {
	struct MeshGPUHandle;
	struct Mesh;
	void createMesh(MeshGPUHandle& glMesh, const Mesh& mesh);
} 