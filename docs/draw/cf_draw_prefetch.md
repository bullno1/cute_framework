[//]: # (This file is automatically generated by Cute Framework's docs parser.)
[//]: # (Do not edit this file by hand!)
[//]: # (See: https://github.com/RandyGaul/cute_framework/blob/master/samples/docs_parser.cpp)
[](../header.md ':include')

# cf_draw_prefetch

Category: [draw](/api_reference?id=draw)  
GitHub: [cute_draw.h](https://github.com/RandyGaul/cute_framework/blob/master/include/cute_draw.h)  
---

Prefetches a sprite.

```cpp
void cf_draw_prefetch(const CF_Sprite* sprite);
```

Parameters | Description
--- | ---
sprite | The sprite.

## Remarks

This function ensures the sprite is fully loaded into memory without actually rendering anything.
This is a good way to avoid disk io at inconvenient times.

## Related Pages

[cf_draw_sprite](/draw/cf_draw_sprite.md)  
[cf_draw_quad](/draw/cf_draw_quad.md)  
draw_look_at  
cf_draw_to  
[cf_app_draw_onto_screen](/app/cf_app_draw_onto_screen.md)  