[//]: # (This file is automatically generated by Cute Framework's docs parser.)
[//]: # (Do not edit this file by hand!)
[//]: # (See: https://github.com/RandyGaul/cute_framework/blob/master/samples/docs_parser.cpp)
[](../header.md ':include')

# cf_mesh_set_instance_buffer

Category: [graphics](/api_reference?id=graphics)  
GitHub: [cute_graphics.h](https://github.com/RandyGaul/cute_framework/blob/master/include/cute_graphics.h)  
---

Sets up an instance buffer on the mesh, for instanced style rendering.

```cpp
void cf_mesh_set_instance_buffer(CF_Mesh mesh, int instance_buffer_size_in_bytes, int instance_stride);
```

Parameters | Description
--- | ---
mesh | The mesh.
instance_buffer_size_in_bytes | The size of the mesh's index buffer.
instance_stride | The number of bytes for each instance data.

## Related Pages

[CF_Mesh](/graphics/cf_mesh.md)  
[cf_make_mesh](/graphics/cf_make_mesh.md)  
[cf_mesh_update_instance_data](/graphics/cf_mesh_update_instance_data.md)  