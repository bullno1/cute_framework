[//]: # (This file is automatically generated by Cute Framework's docs parser.)
[//]: # (Do not edit this file by hand!)
[//]: # (See: https://github.com/RandyGaul/cute_framework/blob/master/samples/docs_parser.cpp)
[](../header.md ':include')

# cf_shader_directory

Category: [graphics](/api_reference?id=graphics)  
GitHub: [cute_graphics.h](https://github.com/RandyGaul/cute_framework/blob/master/include/cute_graphics.h)  
---

Sets up the app's shader directory.

```cpp
void cf_shader_directory(const char* path);
```

Parameters | Description
--- | ---
path | A virtual path to the folder with your shaders (subfolders supported). See [Virtual File System](https://randygaul.github.io/cute_framework/#/topics/virtual_file_system).

## Remarks

Shaders can `#include` each other as long as they exist in this directory. Changes to shaders on disk
may also be watched via [cf_shader_on_changed](/graphics/cf_shader_on_changed.md) to support shader reloading during development.

## Related Pages

[CF_Shader](/graphics/cf_shader.md)  
[cf_make_shader](/graphics/cf_make_shader.md)  
[cf_destroy_shader](/graphics/cf_destroy_shader.md)  
[cf_apply_shader](/graphics/cf_apply_shader.md)  
[CF_Material](/graphics/cf_material.md)  