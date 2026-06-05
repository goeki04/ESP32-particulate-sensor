#pragma once
#include "a_Primitives.hpp"
#include <string>
namespace Andromeda {
    /**
    * @enum ShaderDataType
    * @brief Supported uniform and input data types for shader communication.
    */
    enum class ShaderDataType {
        Mat4, Mat3, Vec4, Vec3, Vec2, Float, Int, Texture2D, TextureCube
    };

    /**
     * @enum CullMode
     * @brief Defines the polygon culling behavior for the rasterizer.
     */
    enum class CullMode {
        None, Back, Front
    };

    /**
     * @enum DepthFunc
     * @brief Defines the comparison function for the depth test.
     */
    enum class DepthFunc { Less, Equal, LEqual };

    /**
     * @enum BlendMode
     * @brief Defines the blending equation used for transparent objects.
     */
    enum class BlendMode { None, AlphaBlend };

    /**
     * @enum RasterizerMode
     * @brief Determines how polygons are rasterized (e.g., solid or wireframe).
     */
    enum class RasterizerMode { Fill, Wireframe };

    /**
     * @struct RenderPassSpecs
     * @brief Encapsulates the global state configuration for a render pass.
     * * Used by the IGraphicsContext to update pipeline state (culling, depth test, etc.)
     * before issuing draw commands.
     */
    struct RenderPassSpecs {
        RasterizerMode rasterizerMode = RasterizerMode::Fill;
        CullMode cullMode = CullMode::Back;
        bool depthTest = true;
        DepthFunc depthFunction = DepthFunc::Less;
        BlendMode blendMode = BlendMode::None;
    };

    /**
     * @struct ShaderProgramHandle
     * @brief An opaque handle to an underlying GPU shader program.
     */
    struct ShaderProgramHandle {
        u32 apiID = 0;
    };

    /**
     * @struct TextureBinding
     * @brief Associates a texture with a specific GPU binding slot.
     */
    struct TextureBinding {
        u32 apiID = 0;
        u32 slot = 0;
    };

    /**
     * @struct ReflectedUniform
     * @brief Metadata for a uniform variable discovered via shader reflection.
     */
    struct ReflectedUniform {
        std::string name;
        u32 location;
        ShaderDataType type;
    };

    /**
     * @struct UniformData
     * @brief Encapsulates uniform data to be uploaded to the GPU.
     */
    struct UniformData {
        u32 location;
        u32 size;
        const void* dataPtr;
        ShaderDataType type;
    };
}