[//]: # (This file is automatically generated by Cute Framework's docs parser.)
[//]: # (Do not edit this file by hand!)
[//]: # (See: https://github.com/RandyGaul/cute_framework/blob/master/samples/docs_parser.cpp)
[](../header.md ':include')

# CF_ShaderInfo

Category: [graphics](/api_reference?id=graphics)  
GitHub: [cute_shader_bytecode.h](https://github.com/RandyGaul/cute_framework/blob/master/include/cute_shader_bytecode.h)  
---

Reflection info for a shader.

Struct Members | Description
--- | ---
`int num_samplers` | Number of samplers.
`int num_storage_textures` | Number of storage textures.
`int num_storage_buffers` | Number of storage buffers.
`int num_images` | Number of images.
`const char** image_names` | Name of each images.
`int num_uniforms` | Number of uniform blocks.
`CF_ShaderUniformInfo* uniforms` | Information about each uniform block.
`int num_uniform_members` | Number of uniform block members.
`CF_ShaderUniformMemberInfo* uniform_members` | Members of all uniform blocks tightly packed (see [CF_ShaderUniformInfo](/graphics/cf_shaderuniforminfo.md) for more details).
`int num_inputs` | Number of inputs for vertex shader.
`CF_ShaderInputInfo* inputs` | Information about each vertex shader input.

## Related Pages

[CF_ShaderBytecode](/graphics/cf_shaderbytecode.md)  