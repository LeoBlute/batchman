#include <stdbool.h>
#include "gl.c"
#include <SDL.h>
#include <SDL_opengl.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "generics.h"
#include "bmath.h"

#include "graphics_generics.h"
#include "graphics.h"

#include "text.c"

#define TEXT_VERT_PATH  "shaders/text_vert.glsl"
#define TEXT_FRAG_PATH  "shaders/text_frag.glsl"
#define SCENE_VERT_PATH "shaders/scene_vert.glsl"
#define SCENE_FRAG_PATH "shaders/scene_frag.glsl"

#define HUD_MAX_QUAD_COUNT  1000
#define HUD_MAX_INDEX_COUNT HUD_MAX_QUAD_COUNT * 6

#define SCENE_MAX_CUBES 2050

typedef struct {
	vec3 pos;
	vec3 front;
	vec3 up;
	f32 yaw;
	f32 pitch;
} camera_data;

static void
gl_debug_output(GLenum source,
                GLenum type,
                u32 id,
                GLenum severity,
                GLsizei length,
                const char* msg,
                const void* user_param) {
	printf("GL Error:%s\n", msg);
}

typedef struct {
	i32 xrel;
	i32 yrel;
	u32 requests;
} events_data;

#define EVENT_CLOSE       (1 << 0)
#define EVENT_HOT_RELOAD  (1 << 1)
#define EVENT_UP          (1 << 2)
#define EVENT_DOWN        (1 << 3)
#define EVENT_LEFT        (1 << 4)
#define EVENT_RIGHT       (1 << 5)
#define EVENT_MODE_CHANGE (1 << 6)
#define EVENT_MODE_TEXT   (1 << 7)

static const events_data
handle_events(const SDL_Event* event, const events_data previous_data) {
	events_data new_data = previous_data;

	switch(event->type)
	{
		case SDL_QUIT:
		{
			new_data.requests |= EVENT_CLOSE;
		} break;

		case SDL_MOUSEMOTION:
		{
			new_data.xrel = event->motion.xrel;
			new_data.yrel = event->motion.yrel;
		} break;

		case SDL_WINDOWEVENT:
		{
			switch(event->window.event)
			{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					glViewport(0, 0, event->window.data1, event->window.data2);
				} break;
			}
		} break;

		case SDL_KEYUP:
		{
			SDL_Keycode keycode = event->key.keysym.sym;

			if(event->key.state == SDL_RELEASED) {
				if(keycode == SDLK_w) {
					new_data.requests &= ~EVENT_UP;
				}
				if(keycode == SDLK_s) {
					new_data.requests &= ~EVENT_DOWN;
				}
				if(keycode == SDLK_a) {
					new_data.requests &= ~EVENT_LEFT;
				}
				if(keycode == SDLK_d) {
					new_data.requests &= ~EVENT_RIGHT;
				}
			}
		} break;

		case SDL_KEYDOWN:
		{
			SDL_Keycode keycode = event->key.keysym.sym;

			if(event->key.repeat == 0) {
				//Hot reload shaders request
				if(keycode == SDLK_r) {
					new_data.requests |= EVENT_HOT_RELOAD;
				}

				if(keycode == SDLK_SPACE) {
					new_data.requests |= EVENT_MODE_CHANGE;
				}

				if(keycode == SDLK_f) {
					new_data.requests |= EVENT_MODE_TEXT;
				}

				if(keycode == SDLK_ESCAPE) {
					new_data.requests |= EVENT_CLOSE;
				}

				//Camera controls
				{
					if(keycode == SDLK_w) {
						new_data.requests |= EVENT_UP;
					}
					if(keycode == SDLK_s) {
						new_data.requests |= EVENT_DOWN;
					}
					if(keycode == SDLK_a) {
						new_data.requests |= EVENT_LEFT;
					}
					if(keycode == SDLK_d) {
						new_data.requests |= EVENT_RIGHT;
					}
				}
			}
		} break;
	}

	return new_data;
}

int main(int argc, char* argv[]) {
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL failed to initialize\n");
		return 1;
	}

	//SDL Window creation
	SDL_Window* window = SDL_CreateWindow("Batchman", 0, 0, 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if(!window) {
		printf("Window has not been created\n");
		return 1;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_ENABLE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	//If you're having cap fps, try disabling vsync or running it with vblank_mode=0
	SDL_GL_SetSwapInterval(0);

	//SDL Opengl context creation
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if(!gl_context) {
		printf("GL Context has not been created\n");
		return 1;
	}
	SDL_GL_MakeCurrent(window, gl_context);

	//GLAD initialization
	if(!gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress)) {
		printf("Glad failed to initialize\n");
		return 1;
	}

	//OpenGL Debug context
	{
		i32 gl_flags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &gl_flags);

		if(gl_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
			printf("GL Debug context created\n");
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(gl_debug_output, NULL);
			glDebugMessageControl(GL_DONT_CARE,
                                  GL_DONT_CARE,
                                  GL_DONT_CARE,
                                  0,
                                  NULL,
                                  GL_TRUE);

		}
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const vertex_array text_vao  = vao_init();
	const vertex_array scene_vao = vao_init();

	shader_program_id text_program  = load_create_shader_program(TEXT_VERT_PATH, TEXT_FRAG_PATH);
	shader_program_id scene_program = load_create_shader_program(SCENE_VERT_PATH, SCENE_FRAG_PATH);
	if(!text_program || !scene_program) {
		printf("Shader programs could not be created\n");
		return 1;
	}

	const u32 u_text_proj = glGetUniformLocation(text_program, "u_proj");
	const u32 u_proj_view  = glGetUniformLocation(scene_program, "u_proj_view");

	quad quad_array[HUD_MAX_QUAD_COUNT];
	cube cube_array[SCENE_MAX_CUBES];

	//Text buffers
	{
    	glBindVertexArray(text_vao.id);

    	glBindBuffer(GL_ARRAY_BUFFER, text_vao.vbo);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(quad) * HUD_MAX_QUAD_COUNT, NULL, GL_DYNAMIC_DRAW);

		u32 offset = 0;
		u32 indices[HUD_MAX_INDEX_COUNT];
		for(i32 i =0; i < HUD_MAX_INDEX_COUNT; i += 6) {
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text_vao.ebo);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//2D Positions
	    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(hud_vertex), (void*)0);
	    glEnableVertexAttribArray(0);
	
		//UVs
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(hud_vertex), (void*)(2*sizeof(f32)));
	    glEnableVertexAttribArray(1);
	
		//Atlas id
	    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(hud_vertex), (void*)(4*sizeof(f32)));
	    glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	    glBindBuffer(GL_ARRAY_BUFFER, 0); 
	    glBindVertexArray(0);
		glUseProgram(0);
	}

	{
		glBindVertexArray(scene_vao.id);

		glBindBuffer(GL_ARRAY_BUFFER, scene_vao.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_array), cube_array, GL_DYNAMIC_DRAW);

		cube_elements elements_array[SCENE_MAX_CUBES];
		for(i32 i; i<SCENE_MAX_CUBES; ++i) {
			fill_cube_elements(elements_array[i], i);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene_vao.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements_array), elements_array, GL_STATIC_DRAW);

		// positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(scene_vertex), (void*)0);
		glEnableVertexAttribArray(0);

		// uvs
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(scene_vertex), (void*)offsetof(scene_vertex, v.uv));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//Font atlases setup here, can be expanded to include multiple fonts
	{
		glUseProgram(text_program);
		u32 u_atlases = glGetUniformLocation(text_program, "u_atlases");
		i32 samplers[1] = { 0 };
		glUniform1iv(u_atlases, 1, samplers);
		glUseProgram(0);
	}

	//Main Scene texture
	{
		glUseProgram(scene_program);
		const u32 u_texture = glGetUniformLocation(scene_program, "u_texture");
		glUniform1i(u_texture, 0);
		glUseProgram(0);
	}

	//Texture init
	texture main_texture = load_texture("res/imgs/fsdlsfad[.png");

	//Font init
	font main_font;
	texture main_atlas;
	{
		FILE* font_file = fopen("res/fonts/dogicapixel.ttf", "rb");
		if(!font_file) {
			printf("Failed to load font file\n");
		}

		fseek(font_file, 0, SEEK_END);
		long font_file_size = ftell(font_file);
		rewind(font_file);

		u8* font_buffer = (u8*)malloc(font_file_size);
		if(!font_buffer) {
			printf("Failed\n");
		}

		size_t _ = fread(font_buffer, 1, font_file_size, font_file);
		fclose(font_file);

		main_font = create_font(font_buffer, 1024, 768, 64.0f, 96);
		main_atlas = create_font_atlas(main_font);
	}

	events_data evs_data = { 0, 0, 0 };
	f64 dt               = 0.0;
	f32 fps              = 0.0f;
	f32 rot              = 0.0f;
	bool draw_text       = true;
	bool running         = true;

	camera_data cam_data;
	{
		vec3 pos   =  { 0.0f,  0.0f,  10.0f };
		vec3 front =  { 0.0f,  0.0f, -1.0f };
		vec3 up    =  { 0.0f,  1.0f,  0.0f };
		f32 yaw    = -90.0f;
		f32 pitch  =  0.0f;

		vec3_copy(pos, cam_data.pos);
		vec3_copy(up, cam_data.up);
		vec3_copy(front, cam_data.front);
		cam_data.yaw = yaw;
		cam_data.pitch = pitch;
	}

	const u64 perf_frequency = SDL_GetPerformanceFrequency();
	u64 last_counter = SDL_GetPerformanceCounter();

	do {
		SDL_Event event;
		evs_data.xrel = 0;
		evs_data.yrel = 0;
		while(SDL_PollEvent(&event))
		{
			evs_data = handle_events(&event, evs_data);
		}

		if(evs_data.requests & EVENT_CLOSE) {
			running = false;
			break;
		}

		if(evs_data.requests & EVENT_HOT_RELOAD) {
			shader_program_id temp_text_program_id;
			shader_program_id temp_scene_program_id;

			temp_text_program_id  = load_create_shader_program(TEXT_VERT_PATH, TEXT_FRAG_PATH);
			temp_scene_program_id = load_create_shader_program(SCENE_VERT_PATH, SCENE_FRAG_PATH);

			if(temp_text_program_id && temp_scene_program_id) {
				text_program = temp_text_program_id;
				scene_program = temp_scene_program_id;
				printf("Shaders reload sucessfully\n");
			} else {
				printf("Shaders could not be reloaded\n");
			}

			evs_data.requests &= ~EVENT_HOT_RELOAD;
		}

		if(evs_data.requests & EVENT_MODE_TEXT) {
			draw_text = !draw_text;

			evs_data.requests &= ~EVENT_MODE_TEXT;
		}

		//Camera controlling
		{
			const f32 cam_speed = 0.01f * dt;
			if(evs_data.requests & EVENT_UP) {
				vec3 tmp;
				vec3_scale(cam_data.front, cam_speed, tmp);
				vec3_add(tmp, cam_data.pos);
			}

			if(evs_data.requests & EVENT_DOWN) {
				vec3 tmp;
				vec3_scale(cam_data.front, cam_speed, tmp);
				vec3_sub(tmp, cam_data.pos);
			}

			if(evs_data.requests & EVENT_LEFT) {
				vec3 tmp;
				vec3_cross(cam_data.front, cam_data.up, tmp);
				vec3_normalize(tmp);
				vec3_scale(tmp, cam_speed, tmp);
				vec3_add(tmp, cam_data.pos);
			}

			if(evs_data.requests & EVENT_RIGHT) {
				vec3 tmp;
				vec3_cross(cam_data.front, cam_data.up, tmp);
				vec3_normalize(tmp);
				vec3_scale(tmp, cam_speed, tmp);
				vec3_sub(tmp, cam_data.pos);
			}
		}

		{
			const f32 sen = 0.1f;
			const f32 xrel = sen * evs_data.xrel;
			const f32 yrel = sen * evs_data.yrel;

			cam_data.yaw   -= xrel;
			cam_data.pitch -= yrel;

			if(cam_data.pitch > 89.0f) {
				cam_data.pitch = 89.0f;
			}
			if(cam_data.pitch < -89.0f) {
				cam_data.pitch = -89.0f;
			}

			vec3 front;
			front[0] = cos(to_radians_32(cam_data.yaw)) * cos(to_radians_32(cam_data.pitch));
			front[1] = sin(to_radians_32(cam_data.pitch));
			front[2] = sin(to_radians_32(cam_data.yaw)) * cos(to_radians_32(cam_data.pitch));
			vec3_copy(front, cam_data.front);
		}

		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Get window width and height for projections
		i32 viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		const f32 w = (f32)viewport[2];
		const f32 h = (f32)viewport[3];

		//Scene
		{
			mat4 view = MAT4_IDENTITY;
			mat4 scene_proj = MAT4_IDENTITY;

			glEnable(GL_DEPTH_TEST);

			//Bind everything needed
        	glBindVertexArray(scene_vao.id);
			glUseProgram(scene_program);
			glBindTextureUnit(0, main_texture);

			mat4_perspective(to_radians_32(45.0f), (f32)w / (f32)h, 0.1f, 1000.0f, scene_proj);

			{
				vec3 tmp;
				vec3_copy(cam_data.pos, tmp);
				vec3_add(cam_data.front, tmp);
				mat4_look_at(cam_data.pos, tmp, cam_data.up, view);
			}


			mat4 proj_view;
			mat4_mul(scene_proj, view, proj_view);

			const f32 x_begin = -50.0f;
			      vec3 m_pos   = { x_begin, 0.0f, 0.0f };
			const vec3 m_axis  = { 1.0f, 0.3f, 0.5f };
			const vec3 m_scale = { 1.0f, 1.0f, 1.0f };
			mat4 model;

			const u32 index_count = SCENE_MAX_CUBES * 36;
			const f32 offset = 1.5f;
			for(i32 i=0; i<SCENE_MAX_CUBES; ++i) {
				mat4_identity(model);

				m_pos[0] += offset;

				if(!(i % 10)) {
					m_pos[2] += offset;
					m_pos[0] = x_begin;
				}

				if(!(i % 100)) {
					m_pos[2] = 0.0f;
					m_pos[0] = x_begin;
					m_pos[1] += offset;
				}

				const f32 rot_offset = (f32)i * 1.0f;
				mat4_translate(model, m_pos);
				mat4_scale(model, m_scale);
				mat4_rotate(model, to_radians_32(rot + rot_offset), m_axis);

				make_cube(cube_array[i]);
				transform_cube(cube_array[i], model);
			}
			rot+=dt * 0.1f;


			glBindBuffer(GL_ARRAY_BUFFER, scene_vao.vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_array), cube_array);

			glUniformMatrix4fv(u_proj_view,  1, GL_FALSE, &proj_view[0][0]);

			glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
		}

		if(draw_text) {
			u32 index_count = 0;
			mat4 hud_proj;

			//Bind everything needed
        	glBindVertexArray(text_vao.id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text_vao.ebo);
			glUseProgram(text_program);
			glBindTextureUnit(0, main_atlas);

			//Enabling depth_test will break the exclusive 2D rendering
			glDisable(GL_DEPTH_TEST);

			//!no uniform needs to be get every frame it was done this way for simplicity and it's not optimal
			mat4_ortho(-w, w, -h, h, -1.0f, 1.0f, hud_proj);
			glUniformMatrix4fv(u_text_proj, 1, GL_FALSE, &hud_proj[0][0]);

			//Create text on screen
			{ 
				vec2 txt_pos = { -w, -h + 100.0f };
				i32 current_vertice = 0;

				make_text_row(main_font, txt_pos, &current_vertice, &index_count, quad_array, "FPS:%.f", fps);

				txt_pos[0] = -w; txt_pos[1] = h - 16.0f;
				make_text_row(main_font, txt_pos, &current_vertice, &index_count, quad_array, "Press f to disable/enable text");
				txt_pos[0] = -w; txt_pos[1] = h - 116.0f;
				make_text_row(main_font, txt_pos, &current_vertice, &index_count, quad_array, "Press space to change mode");
			}


			//Update data on gpu
			glBindBuffer(GL_ARRAY_BUFFER, text_vao.vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad_array), quad_array);

        	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
		}

		SDL_GL_SwapWindow(window);

		//Performance monitoring
		const u64 end_counter = SDL_GetPerformanceCounter();
		const u64 counter_elapsed = end_counter - last_counter;

		fps = (f64)perf_frequency / (f64)counter_elapsed;
		dt = (f64)(counter_elapsed*1000 / (f64)perf_frequency);

		last_counter = end_counter;
	} while(running);

	free_font(&main_font);
	vao_delete(text_vao);
	vao_delete(scene_vao);
	glDeleteTextures(1, &main_atlas);
	glDeleteTextures(1, &main_texture);
    glDeleteProgram(text_program);
    glDeleteProgram(scene_program);

	SDL_DestroyWindow(window);
	SDL_Quit();

	printf("App closed properly\n");

	return 0;
}
