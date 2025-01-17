[](../header.md ':include')

# cf_parallel2

Category: [math](/api_reference?id=math)  
GitHub: [cute_math.h](https://github.com/RandyGaul/cute_framework/blob/master/include/cute_math.h)  
---

Returns true the planes a-b and b-c are parallel with a distance tolerance.

```cpp
bool cf_parallel2(CF_V2 a, CF_V2 b, CF_V2 c, float tol)
```

## Remarks

You should experiment to find a good `tol` value, such as commonly used values like 1.0e-3f, 1.0e-6f, or 1.0e-8f.
Different orders of magnitude are suitable for different tasks, so it may take some experience to figure out
what a good tolerance is for your situation.

## Related Pages

[CF_V2](/math/cf_v2.md)  
[cf_lesser_v2](/math/cf_lesser_v2.md)  
[cf_greater_v2](/math/cf_greater_v2.md)  
[cf_lesser_equal_v2](/math/cf_lesser_equal_v2.md)  
[cf_greater_equal_v2](/math/cf_greater_equal_v2.md)  
[cf_parallel](/math/cf_parallel.md)  
