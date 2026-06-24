#pragma once

/**
 * @file a_primitiveGenerator.hpp
 * @brief Procedural generation of built-in primitive meshes (cube, plane, UV sphere).
 */

#include <vector>
#include "a_primitives.hpp"
#include "a_geometry.hpp"

namespace Andromeda {
    /**
     * @class PrimitiveGenerator
     * @brief Stateless utility that fills @c Mesh objects with the geometry of standard primitives.
     *
     * @details All generators write directly into a caller-provided @c Mesh (clearing it first),
     *          producing per-vertex position, normal, UV and color, plus a triangle index buffer.
     *          Primitives are unit-sized and centered on the origin. Used by the @c ResourceManager
     *          to register the default Cube/Plane/Sphere meshes.
     */
    class PrimitiveGenerator {
    public:
        /**
         * @brief Generates a unit cube (side length 1, centered on the origin) with per-face normals.
         * @details Builds each of the 6 faces from 4 vertices and 2 triangles, so face normals and
         *          UVs are sharp rather than shared/smoothed across edges.
         * @param outMesh The mesh to populate; its existing vertex/index data is cleared first.
         */
        static void generateCube(Mesh& outMesh) {
            outMesh.vertexbuffer.clear();
            outMesh.indexBuffer.clear();

            struct Face { vec3 normal; vec3 up; vec3 right; };

            for (int i = 0; i < 6; i++) {
                Face faces[6] = {
                    { { 0, 0, 1}, {0, 1, 0}, { 1, 0, 0} }, // Front
                    { { 0, 0,-1}, {0, 1, 0}, {-1, 0, 0} }, // Back
                    { { 0, 1, 0}, {0, 0,-1}, { 1, 0, 0} }, // Top
                    { { 0,-1, 0}, {0, 0, 1}, { 1, 0, 0} }, // Bottom
                    { { 1, 0, 0}, {0, 1, 0}, { 0, 0,-1} }, // Right
                    { {-1, 0, 0}, {0, 1, 0}, { 0, 0, 1} }  // Left
                };
                u32 offset = static_cast<u32>(outMesh.vertexbuffer.size());
                vec3 n = faces[i].normal;
                vec3 r = faces[i].right * 0.5f;
                vec3 u = faces[i].up * 0.5f;

                outMesh.vertexbuffer.push_back({ n*0.5f - r + u, n, {0, 1}, {1,1,1} }); // TL
                outMesh.vertexbuffer.push_back({ n*0.5f + r + u, n, {1, 1}, {1,1,1} }); // TR
                outMesh.vertexbuffer.push_back({ n*0.5f + r - u, n, {1, 0}, {1,1,1} }); // BR
                outMesh.vertexbuffer.push_back({ n*0.5f - r - u, n, {0, 0}, {1,1,1} }); // BL

                outMesh.indexBuffer.push_back(offset + 0); outMesh.indexBuffer.push_back(offset + 2); outMesh.indexBuffer.push_back(offset + 1);
                outMesh.indexBuffer.push_back(offset + 0); outMesh.indexBuffer.push_back(offset + 3); outMesh.indexBuffer.push_back(offset + 2);
            }
        }

        /**
         * @brief Generates a flat square plane on the XZ axis (facing +Y), spanning -1..1 in X and Z.
         * @param outMesh The mesh to populate; overwritten with the plane's 4 vertices and 2 triangles.
         */
        static void generatePlane(Mesh& outMesh) {
            float h = 1;
            outMesh.vertexbuffer = {
                { {-h, 0,  h}, {0, 1, 0}, {0, 1}, {1,1,1} },
                { { h, 0,  h}, {0, 1, 0}, {1, 1}, {1,1,1} },
                { { h, 0, -h}, {0, 1, 0}, {1, 0}, {1,1,1} },
                { {-h, 0, -h}, {0, 1, 0}, {0, 0}, {1,1,1} }
            };
            outMesh.indexBuffer = { 0, 1, 2, 0, 2, 3 };
        }


        /**
         * @brief Generates a UV sphere of radius 0.5 centered on the origin.
         * @details Uses a latitude/longitude tessellation with a fixed resolution of 32 stacks and
         *          32 sectors. Vertices carry normalized normals and spherical UV coordinates.
         * @param outMesh The mesh to populate; its existing vertex/index data is cleared first.
         */
        static void generateSphere(Mesh& outMesh) {
            outMesh.vertexbuffer.clear();
            outMesh.indexBuffer.clear();
            constexpr u32 sectors = 32; ///< Number of longitudinal subdivisions.
            constexpr u32 stacks = 32;  ///< Number of latitudinal subdivisions.
            constexpr float PI = 3.14159265359f;
            float sectorStep = 2 * PI / sectors;
            float stackStep = PI / stacks;

            for (u32 i = 0; i <= stacks; ++i) {
                float radius = 0.5f;
                float stackAngle = PI / 2 - i * stackStep;
                float xy = radius * cosf(stackAngle);
                float z = radius * sinf(stackAngle);

                for (u32 j = 0; j <= sectors; ++j) {
                    float sectorAngle = j * sectorStep;

                    Vertex vertex;
                    vertex.pos.x = xy * cosf(sectorAngle);
                    vertex.pos.y = xy * sinf(sectorAngle);
                    vertex.pos.z = z;

                    vertex.normal = glm::normalize(vertex.pos);

                    vertex.uv.x = static_cast<float>(j) / sectors;
                    vertex.uv.y = static_cast<float>(i) / stacks;

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    outMesh.vertexbuffer.push_back(vertex);
                }
            }

            for (u32 i = 0; i < stacks; ++i) {
                u32 k1 = i * (sectors + 1);
                u32 k2 = k1 + sectors + 1;

                for (u32 j = 0; j < sectors; ++j, ++k1, ++k2) {
                    if (i != 0) {
                        outMesh.indexBuffer.push_back(k1);
                        outMesh.indexBuffer.push_back(k2);
                        outMesh.indexBuffer.push_back(k1 + 1);
                    }

                    if (i != stacks - 1) {
                        outMesh.indexBuffer.push_back(k1 + 1);
                        outMesh.indexBuffer.push_back(k2);
                        outMesh.indexBuffer.push_back(k2 + 1);
                    }
                }
            }
        }
    };
}