[](../header.md ':include')

# cf_ray_to_aabb

Category: [collision](/api_reference?id=collision)  
GitHub: [cute_math.h](https://github.com/RandyGaul/cute_framework/blob/master/include/cute_math.h)  
---

Returns true if a ray hits an Aabb.

```cpp
bool cf_ray_to_aabb(CF_Ray A, CF_Aabb B, CF_Raycast* out);
```

Parameters | Description
--- | ---
A | The ray.
B | The Aabb.
out | Can be `NULL`. [CF_Raycast](/math/cf_raycast.md) results are placed here (contains normal + time of impact).

## Related Pages

[CF_Ray](/math/cf_ray.md)  
[CF_Aabb](/math/cf_aabb.md)  
[CF_Raycast](/math/cf_raycast.md)  
[cf_ray_to_circle](/collision/cf_ray_to_circle.md)  
[cf_ray_to_poly](/collision/cf_ray_to_poly.md)  
[cf_ray_to_capsule](/collision/cf_ray_to_capsule.md)  
