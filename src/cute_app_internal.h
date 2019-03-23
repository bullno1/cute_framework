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

#ifndef CUTE_INTERNAL_H
#define CUTE_INTERNAL_H

#include <cute_buffer.h>
#include <cute_audio.h>

#define CUTE_CHECK(x) do { if (x) goto cute_error; } while (0)
#define CUTE_CHECK_POINTER(x) do { if (!(x)) goto cute_error; } while (0)

struct SDL_Window;
struct cs_context_t;

namespace cute
{

struct mouse_state_t
{
	int left_button = 0;
	int right_button = 0;
	int middle_button = 0;
	int wheel_motion = 0;
	int x = 0;
	int y = 0;
	int xrel = 0;
	int yrel = 0;
	int click_type = 0;
};

struct app_t
{
	int running = 1;
	SDL_Window* window = NULL;
	buffer_t playing_sounds = buffer_t(internal::sound_instance_size());
	audio_t* playing_music = NULL;
	audio_t* next_music = NULL;
	cs_context_t* cs = NULL;
	threadpool_t* threadpool = NULL;
	buffer_t input_text = buffer_t(sizeof(int));
	int keys[512];
	int keys_prev[512];
	float keys_duration[512];
	mouse_state_t mouse, mouse_prev;
	void* mem_ctx = NULL;
};

}

#endif // CUTE_INTERNAL_H