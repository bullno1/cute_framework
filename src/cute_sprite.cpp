/*
	Cute Framework
	Copyright (C) 2019 Randy Gaul https://randygaul.net

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#define SPRITEBATCH_IMPLEMENTATION
#include <cute/cute_spritebatch.h>

#define CUTE_PNG_IMPLEMENTATION
#include <cute/cute_png.h>

#include <cute_sprite.h>
#include <cute_alloc.h>
#include <cute_array.h>
#include <cute_file_system.h>

namespace cute
{

struct sprite_vertex_t
{
	v2 pos;
	v2 uv;
};

enum sprite_batch_mode_t
{
	SPRITE_BATCH_MODE_UNINITIALIZED,
	SPRITE_BATCH_MODE_LRU,
	SPRITE_BATCH_MODE_CUSTOM,
};

struct sprite_batch_t
{
	spritebatch_t sb;
	gfx_t* gfx;
	sprite_batch_mode_t mode = SPRITE_BATCH_MODE_UNINITIALIZED;
	float w = 0, h = 0;

	array<sprite_t> sprites;
	array<sprite_vertex_t> verts;
	gfx_vertex_buffer_t* sprite_buffer = NULL;
	gfx_shader_t* default_shader = 0;
	gfx_shader_t* outline_shader = 0;
	gfx_shader_t* tint_shader = 0;
	gfx_shader_t* active_shader = 0;
	sprite_shader_type_t shader_type = SPRITE_SHADER_TYPE_DEFAULT;
	aabb_t scissor;
	int use_scissor = 0;
	float outline_use_border = 0;
	gfx_matrix_t mvp;

	const char** image_paths = NULL;
	int image_paths_count = 0;
	size_t cache_size_in_bytes = 0;

	void* mem_ctx = NULL;
};

//--------------------------------------------------------------------------------------------------

sprite_batch_t* sprite_batch_make(gfx_t* gfx, int screen_w, int screen_h, void* mem_ctx)
{
	sprite_batch_t* sb = CUTE_NEW(sprite_batch_t, mem_ctx);
	if (!sb) return NULL;

	sb->w = (float)screen_w;
	sb->h = (float)screen_h;
	sb->gfx = gfx;
	sb->mem_ctx = mem_ctx;
	return sb;
}

void sprite_batch_destroy(sprite_batch_t* sb)
{
	spritebatch_term(&sb->sb);
	sb->~sprite_batch_t();
	CUTE_FREE(sb, sb->mem_ctx);
}

void sprite_batch_push(sprite_batch_t* sb, sprite_t sprite)
{
	sb->sprites.add(sprite);
}

error_t sprite_batch_flush(sprite_batch_t* sb)
{
	if (spritebatch_defrag(&sb->sb)) {
		return error_failure("`spritebatch_defrag` failed.");
	}
	spritebatch_tick(&sb->sb);
	if (spritebatch_flush(&sb->sb)) {
		return error_failure("`spritebatch_flush` failed.");
	}
	return error_success();
}

//--------------------------------------------------------------------------------------------------
// Internal functions.

static gfx_scissor_t s_scissor_from_aabb(aabb_t aabb, float w, float h)
{
	gfx_scissor_t scissor;
	scissor.left = (int)aabb.min.x;
	scissor.right = (int)aabb.max.x;
	scissor.top = (int)aabb.max.y;
	scissor.bottom = (int)aabb.min.y;

	// Transform to screen space (origin top left, downward y).
	scissor.left = (int)(scissor.left + w / 2);
	scissor.right = (int)(scissor.right + w / 2);
	scissor.top = (int)(-scissor.top + h / 2);
	scissor.bottom = (int)(-scissor.bottom + h / 2);

	return scissor;
}

static gfx_shader_t* s_load_shader(sprite_batch_t* sb, sprite_shader_type_t type)
{
	const char* default_vs = CUTE_STRINGIZE(
		struct vertex_t
		{
			float2 pos : POSITION0;
			float2 uv  : TEXCOORD0;
		};

		struct interp_t
		{
			float4 posH : POSITION0;
			float2 uv   : TEXCOORD0;
		};

		float4x4 u_mvp;
		float2 u_inv_screen_wh;

		interp_t main(vertex_t vtx)
		{
			float4 posH = mul(float4(round(vtx.pos), 0, 1), u_mvp);

			posH.xy += u_inv_screen_wh;
			interp_t interp;
			interp.posH = posH;
			interp.uv = vtx.uv;
			return interp;
		}
	);

	const char* default_ps = CUTE_STRINGIZE(
		struct interp_t
		{
			float4 posH : POSITION;
			float2 uv   : TEXCOORD0;
		};

		sampler2D u_image;

		float4 main(interp_t interp) : COLOR
		{
			float2 uv = interp.uv;
			float4 color = tex2D(u_image, uv);
			return color;
		}
	);

	const char* outline_vs = CUTE_STRINGIZE(
		struct vertex_t
		{
			float2 pos : POSITION0;
			float2 uv  : TEXCOORD0;
		};

		struct interp_t
		{
			float4 posH : POSITION0;
			float2 uv   : TEXCOORD0;
		};

		float4x4 u_mvp;
		float2 u_inv_screen_wh;

		interp_t main(vertex_t vtx)
		{
			float4 posH = mul(float4(round(vtx.pos), 0, 1), u_mvp);

			posH.xy += u_inv_screen_wh;
			interp_t interp;
			interp.posH = posH;
			interp.uv = vtx.uv;
			return interp;
		}
	);

	const char* outline_ps = CUTE_STRINGIZE(
		struct interp_t
		{
			float4 posH : POSITION;
			float2 uv   : TEXCOORD0;
		};

		sampler2D u_image;
		float2 u_texel_size;
		float u_use_border;

		float4 main(interp_t interp) : COLOR
		{
			float2 uv = interp.uv;

			// Border detection for pixel outlines.
			float a = (float)any(tex2D(u_image, uv + float2(0,  u_texel_size.y)).rgb);
			float b = (float)any(tex2D(u_image, uv + float2(0, -u_texel_size.y)).rgb);
			float c = (float)any(tex2D(u_image, uv + float2(u_texel_size.x,  0)).rgb);
			float d = (float)any(tex2D(u_image, uv + float2(-u_texel_size.x, 0)).rgb);
			float e = (float)any(tex2D(u_image, uv + float2(-u_texel_size.x, -u_texel_size.y)).rgb);
			float f = (float)any(tex2D(u_image, uv + float2(-u_texel_size.x,  u_texel_size.y)).rgb);
			float g = (float)any(tex2D(u_image, uv + float2( u_texel_size.x, -u_texel_size.y)).rgb);
			float h = (float)any(tex2D(u_image, uv + float2( u_texel_size.x,  u_texel_size.y)).rgb);

			float4 image_color = tex2D(u_image, uv);
			float image_mask = (float)any(image_color.rgb);

			float border = max(a, max(b, max(c, max(d, max(e, max(f, max(g, h))))))) * (1 - image_mask);
			border *= u_use_border;

			// Pick between white border or sprite color.
			float4 border_color = float4(1, 1, 1, 1);
			float4 color = lerp(image_color * image_mask, border_color, border);

			return color;
		}
	);

	const char* tint_vs = CUTE_STRINGIZE(
		struct vertex_t
		{
			float2 pos : POSITION0;
			float2 uv  : TEXCOORD0;
		};

		struct interp_t
		{
			float4 posH : POSITION0;
			float2 uv   : TEXCOORD0;
		};

		float4x4 u_mvp;
		float2 u_inv_screen_wh;

		interp_t main(vertex_t vtx)
		{
			float4 posH = mul(float4(round(vtx.pos), 0, 1), u_mvp);

			posH.xy += u_inv_screen_wh;
			interp_t interp;
			interp.posH = posH;
			interp.uv = vtx.uv;
			return interp;
		}
	);

	const char* tint_ps = CUTE_STRINGIZE(
		struct interp_t
		{
			float4 posH : POSITION;
			float2 uv   : TEXCOORD0;
		};

		sampler2D u_image;

		float4 main(interp_t interp) : COLOR
		{
			float2 uv = interp.uv;
			float4 color = tex2D(u_image, uv);
			return color;
		}
	);

	// Grab the shader strings.
	const char* vs = NULL;
	const char* ps = NULL;

	switch (type)
	{
	case SPRITE_SHADER_TYPE_DEFAULT:
		vs = default_vs;
		ps = default_ps;
		break;

	case SPRITE_SHADER_TYPE_OUTLINE:
		vs = outline_vs;
		ps = outline_ps;
		break;

	case SPRITE_SHADER_TYPE_TINT:
		vs = tint_vs;
		ps = tint_ps;
		break;
	}

	gfx_t* gfx = sb->gfx;

	// Compile the shader and remove the old shader.
	gfx_matrix_t projection;
	matrix_ortho_2d(&projection, sb->w, sb->h, 0, 0);

	// Set common uniforms.
	gfx_shader_t* shader = gfx_shader_new(gfx, sb->sprite_buffer, vs, ps);
	gfx_shader_set_screen_wh(gfx, shader, sb->w, sb->h);

	return shader;
}

//--------------------------------------------------------------------------------------------------
// spritebatch_t callbacks.

static void s_batch_report(spritebatch_sprite_t* sprites, int count, int texture_w, int texture_h, void* udata)
{
	sprite_batch_t* sb = (sprite_batch_t*)udata;

	// Build draw call.
	gfx_draw_call_t draw_call;
	gfx_draw_call_add_texture(&draw_call, (gfx_texture_t*)sprites->texture_id, "u_image");
	draw_call.buffer = sb->sprite_buffer;
	draw_call.shader = sb->active_shader;

	if (sb->use_scissor) {
		gfx_scissor_t scissor = s_scissor_from_aabb(sb->scissor, sb->w, sb->h);
		gfx_draw_call_set_scissor_box(&draw_call, &scissor);
	}

	// Set shader-specific uniforms.
	switch (sb->shader_type)
	{
	case SPRITE_SHADER_TYPE_DEFAULT:
		break;

	case SPRITE_SHADER_TYPE_OUTLINE:
	{
		v2 texel_size = v2(1.0f / (float)texture_w, 1.0f / (float)texture_h);
		gfx_draw_call_add_uniform(&draw_call, "u_texel_size", &texel_size, GFX_UNIFORM_TYPE_FLOAT2);
		gfx_draw_call_add_uniform(&draw_call, "u_use_border", &sb->outline_use_border, GFX_UNIFORM_TYPE_FLOAT);
	}	break;

	case SPRITE_SHADER_TYPE_TINT:
		break;
	}

	// Build vertex buffer of all quads for each sprite.
	int vert_count = count * 6;
	sb->verts.ensure_count(vert_count);
	sprite_vertex_t* verts = sb->verts.data();

	for (int i = 0; i < count; ++i)
	{
		spritebatch_sprite_t* s = sprites + i;

		typedef struct v2
		{
			float x;
			float y;
		} v2;

		v2 quad[] = {
			{ -0.5f,  0.5f },
			{  0.5f,  0.5f },
			{  0.5f, -0.5f },
			{ -0.5f, -0.5f },
		};

		for (int j = 0; j < 4; ++j)
		{
			float x = quad[j].x;
			float y = quad[j].y;

			// rotate sprite about origin
			float x0 = s->c * x - s->s * y;
			float y0 = s->s * x + s->c * y;
			x = x0;
			y = y0;

			// scale sprite about origin
			x *= s->sx;
			y *= s->sy;

			// translate sprite into the world
			x += s->x;
			y += s->y;

			quad[j].x = x;
			quad[j].y = y;
		}

		// output transformed quad into CPU buffer
		sprite_vertex_t* out_verts = verts + i * 6;

		out_verts[0].pos.x = quad[0].x;
		out_verts[0].pos.y = quad[0].y;
		out_verts[0].uv.x = s->minx;
		out_verts[0].uv.y = s->maxy;

		out_verts[1].pos.x = quad[3].x;
		out_verts[1].pos.y = quad[3].y;
		out_verts[1].uv.x = s->minx;
		out_verts[1].uv.y = s->miny;

		out_verts[2].pos.x = quad[1].x;
		out_verts[2].pos.y = quad[1].y;
		out_verts[2].uv.x = s->maxx;
		out_verts[2].uv.y = s->maxy;

		out_verts[3].pos.x = quad[1].x;
		out_verts[3].pos.y = quad[1].y;
		out_verts[3].uv.x = s->maxx;
		out_verts[3].uv.y = s->maxy;

		out_verts[4].pos.x = quad[3].x;
		out_verts[4].pos.y = quad[3].y;
		out_verts[4].uv.x = s->minx;
		out_verts[4].uv.y = s->miny;

		out_verts[5].pos.x = quad[2].x;
		out_verts[5].pos.y = quad[2].y;
		out_verts[5].uv.x = s->maxx;
		out_verts[5].uv.y = s->miny;
	}

	// Map verts onto GPU buffer.
	gfx_draw_call_add_verts(sb->gfx, &draw_call, verts, vert_count);

	// Push draw call onto the gfx stack.
	gfx_draw_call_set_mvp(&draw_call, &sb->mvp);
	gfx_push_draw_call(sb->gfx, &draw_call);
}

static void s_get_pixels(SPRITEBATCH_U64 image_id, void* buffer, int bytes_to_fill, void* udata)
{
	sprite_batch_t* sb = (sprite_batch_t*)udata;
	if (image_id) {
		const char* path = sb->image_paths[image_id];
		void* data;
		size_t sz;
		error_t err = file_system_read_entire_file_to_memory(path, &data, &sz, sb->mem_ctx);
		if (err.is_error()) return;

		cp_image_t img = cp_load_png_mem(data, (int)sz);
		CUTE_FREE(data, sb->mem_ctx);

		CUTE_ASSERT(img.w * img.h * sizeof(cp_pixel_t) == bytes_to_fill);
		CUTE_MEMCPY(buffer, img.pix, bytes_to_fill);
		CUTE_FREE(img.pix, NULL); // TODO - Adjust cute_png for custom alloc ctx.
	} else {
		CUTE_MEMSET(buffer, 0, bytes_to_fill);
	}
}

static SPRITEBATCH_U64 s_generate_texture_handle(void* pixels, int w, int h, void* udata)
{
	sprite_batch_t* sb = (sprite_batch_t*)udata;
	return (SPRITEBATCH_U64)gfx_texture_create(sb->gfx, w, h, pixels, GFX_PIXEL_FORMAT_R8B8G8A8, GFX_WRAP_MODE_CLAMP_BORDER);
}

static void s_destroy_texture_handle(SPRITEBATCH_U64 texture_id, void* udata)
{
	sprite_batch_t* sb = (sprite_batch_t*)udata;
}

//--------------------------------------------------------------------------------------------------

void sprite_batch_set_shader_type(sprite_batch_t* sb, sprite_shader_type_t type)
{
	sb->shader_type = type;

	// Load the shader, if needed, and set the active sprite system shader.
	switch (type)
	{
	case SPRITE_SHADER_TYPE_DEFAULT:
		if (!sb->default_shader) {
			sb->default_shader = s_load_shader(sb, type);
		}
		sb->active_shader = sb->default_shader;
		break;

	case SPRITE_SHADER_TYPE_OUTLINE:
		if (!sb->outline_shader) {
			sb->outline_shader = s_load_shader(sb, type);
		}
		sb->active_shader = sb->outline_shader;
		break;

	case SPRITE_SHADER_TYPE_TINT:
		if (!sb->tint_shader) {
			sb->tint_shader = s_load_shader(sb, type);
		}
		sb->active_shader = sb->tint_shader;
		break;
	}
}

void sprite_batch_set_mvp(sprite_batch_t* sb, gfx_matrix_t mvp)
{
	sb->mvp = mvp;
}

void sprite_batch_set_scissor_box(sprite_batch_t* sb, aabb_t scissor)
{
	sb->scissor = scissor;
	sb->use_scissor = 1;
}

void sprite_batch_no_scissor_box(sprite_batch_t* sb)
{
	sb->use_scissor = 0;
}

void sprite_batch_outlines_use_border(sprite_batch_t* sb, int use_border)
{
	sb->outline_use_border = use_border ? 1.0f : 0;
}

error_t sprite_batch_enable_disk_LRU_cache(sprite_batch_t* sb, const char** image_paths, int image_paths_count, size_t cache_size_in_bytes)
{
	spritebatch_config_t config;
	spritebatch_set_default_config(&config);
	config.atlas_use_border_pixels = 1;
	config.batch_callback = s_batch_report;
	config.get_pixels_callback = s_get_pixels;
	config.generate_texture_callback = s_generate_texture_handle;
	config.delete_texture_callback = s_destroy_texture_handle;
	config.allocator_context = sb->mem_ctx;

	if (spritebatch_init(&sb->sb, &config, sb)) {
		return error_failure("Unable to initialize `spritebatch_t`.");
	}

	sb->mode = SPRITE_BATCH_MODE_LRU;
	sb->image_paths = image_paths;
	sb->image_paths_count = image_paths_count;
	sb->cache_size_in_bytes = cache_size_in_bytes;

	return error_success();
}

error_t sprite_batch_LRU_cache_prefetch(sprite_batch_t* sb, uint64_t id)
{
	if (sb->mode != SPRITE_BATCH_MODE_LRU) {
		return error_failure("Sprite batch is not in `SPRITE_BATCH_MODE_LRU` mode -- please call `sprite_batch_enable_disk_LRU_cache` to use this function.");
	}

	return error_success();
}

error_t sprite_batch_enable_custom_pixel_loader(sprite_batch_t* sb, void (*get_pixels_fn)(uint64_t image_id, void* buffer, int bytes_to_fill, void* udata), void* udata)
{
	spritebatch_config_t config;
	spritebatch_set_default_config(&config);
	config.atlas_use_border_pixels = 1;
	config.batch_callback = s_batch_report;
	config.get_pixels_callback = get_pixels_fn;
	config.generate_texture_callback = s_generate_texture_handle;
	config.delete_texture_callback = s_destroy_texture_handle;
	config.allocator_context = sb->mem_ctx;

	if (spritebatch_init(&sb->sb, &config, udata)) {
		return error_failure("Unable to initialize `spritebatch_t`.");
	}

	sb->mode = SPRITE_BATCH_MODE_CUSTOM;

	return error_success();
}

}