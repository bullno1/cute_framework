# CF_VertexFormat | [graphics](https://github.com/RandyGaul/cute_framework/blob/master/docs/graphics_readme.md) | [cute_graphics.h](https://github.com/RandyGaul/cute_framework/blob/master/include/cute_graphics.h)

The possible formats for vertex attributes (inputs to vertex shader, coming from [CF_Mesh](https://github.com/RandyGaul/cute_framework/blob/master/docs/graphics/cf_mesh.md)).

## Values

Enum | Description
--- | ---
VERTEX_FORMAT_INVALID | Invalid.
VERTEX_FORMAT_FLOAT | A single 32-bit float.
VERTEX_FORMAT_FLOAT2 | Two 32-bit floats.
VERTEX_FORMAT_FLOAT3 | Three 32-bit floats.
VERTEX_FORMAT_FLOAT4 | Four 32-bit floats.
VERTEX_FORMAT_BYTE4N | Four 8-bit signed bytes, in normalized form.
VERTEX_FORMAT_UBYTE4N | Four 8-bit unsigned bytes, in normalized form.
VERTEX_FORMAT_SHORT2N | Two 16-bit signed bytes, in normalized form.
VERTEX_FORMAT_USHORT2N | Two 16-bit unsigned bytes, in normalized form.
VERTEX_FORMAT_SHORT4N | A single 32-bit signed byte, in normalized form.
VERTEX_FORMAT_USHORT4N | A single 32-bit unsigned byte, in normalized form.

## Remarks

To help understand the notation see [CF_PixelFormat](https://github.com/RandyGaul/cute_framework/blob/master/docs/graphics/cf_pixelformat.md).

## Related Pages

[cf_mesh_set_attributes](https://github.com/RandyGaul/cute_framework/blob/master/docs/graphics/cf_mesh_set_attributes.md)  
[cf_vertex_format_string](https://github.com/RandyGaul/cute_framework/blob/master/docs/graphics/cf_vertex_format_string.md)  
[CF_VertexAttribute](https://github.com/RandyGaul/cute_framework/blob/master/docs/graphics/cf_vertexattribute.md)  