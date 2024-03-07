#if !defined GRAPHICS_H
#define GRAPHICS_H

BATCH_INLINE char*
load_shader_file(const char* file_path) {
	FILE*file;
	long size;
	char* shader_content;

	file = fopen(file_path, "rb");
	if(!file) {
		printf("Failed to load shader content\n");
		return "";
	}

	fseek(file, 0L, SEEK_END);
	size = ftell(file) + 1;
	fclose(file);

	file = fopen(file_path, "r");
	shader_content = memset(malloc(size), '\0', size);
	size_t _ = fread(shader_content, 1, size-1, file);
	fclose(file);

	return shader_content;
}

BATCH_INLINE shader_id
compile_shader(i32 type, const char* source) {
	shader_id id = glCreateShader(type);
	if(!id) {
		printf("Failed to create shader\n");
		return 0;
	}
	glShaderSource(id, 1, (const char**)&source, NULL);
	glCompileShader(id);
	i32 compile_status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);

	if(!compile_status) {
		char info[512];
		glGetShaderInfoLog(id, sizeof(info), NULL, info);
		printf("Shader failed to compile:\n%s\n", info);
		return 0;
	}

	return id;
}

BATCH_INLINE const shader_id
load_compile_shader(i32 type, const char* path) {
	const char* source = load_shader_file(path);
	const shader_id id = compile_shader(type, source);
	free((void*)source);

	return id;
}

BATCH_INLINE const shader_program_id
create_shader_program(shader_id vert_id, shader_id frag_id) {
	shader_program_id id = glCreateProgram();
	glAttachShader(id, vert_id);
	glAttachShader(id, frag_id);
	glLinkProgram(id);

	i32 sucess;
	glGetProgramiv(id, GL_LINK_STATUS, &sucess);
	if(!sucess) {
		char info[512];
		glGetProgramInfoLog(id, sizeof(info), NULL, info);
		printf("Program shader failed to link:\n%s\n", info);
		return 0;
	}

	return id;
}

BATCH_INLINE const shader_program_id
load_create_shader_program(const char* vert_path, const char* frag_path) {
	const shader_id vert_id = load_compile_shader(GL_VERTEX_SHADER, vert_path);
	const shader_id frag_id = load_compile_shader(GL_FRAGMENT_SHADER, frag_path);

	if(vert_id && frag_id) {
		const shader_program_id program_id = create_shader_program(vert_id, frag_id);
		glDeleteShader(vert_id);
		glDeleteShader(frag_id);
		if(program_id){
			return program_id;
		}
		else {
			return 0;
		}
	} else {
		return 0;
	}
}

BATCH_INLINE const texture
create_texture(image img, const i32 w, const i32 h) {
	texture texture_id;

	glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture_id;
}

BATCH_INLINE const texture
load_texture(const char* path) {
	i32 w, h, channels;

	stbi_set_flip_vertically_on_load(1);
	image img = stbi_load(path, &w, &h, &channels, STBI_rgb);
	if(img == NULL) {
		printf("Image could not be loaded");
		return 0;
	}

	const texture texture_id = create_texture(img, w, h);
	
	stbi_image_free(img);

	return texture_id;
}

BATCH_INLINE void
make_quad(quad data,
          const f32 x,
          const f32 y,
          const f32 w,
          const f32 h
) {
	const f32 hw = w * 0.5f;
	const f32 hh = h * 0.5f;

	data[0][0] = x - hw; data[0][1] = y - hh; data[0][2] = 0.0f; data[0][3] = 0.0f; data[0][4] = 0.0f;
	data[1][0] = x + hw; data[1][1] = y - hh; data[1][2] = 1.0f; data[1][3] = 0.0f; data[1][4] = 0.0f;
	data[2][0] = x + hw; data[2][1] = y + hh; data[2][2] = 1.0f; data[2][3] = 1.0f; data[2][4] = 0.0f;
	data[3][0] = x - hw; data[3][1] = y + hh; data[3][2] = 0.0f; data[3][3] = 1.0f; data[3][4] = 0.0f;
}

BATCH_INLINE const vertex_array
vao_init(void) {
	vertex_array vao;
    glCreateVertexArrays(1, &vao.id);
    glCreateBuffers     (1, &vao.vbo);
    glCreateBuffers     (1, &vao.ebo);

	return vao;
}

BATCH_INLINE void
vao_delete(const vertex_array vao) {
	glDeleteVertexArrays(1, &vao.id);
    glDeleteBuffers     (1, &vao.vbo);
    glDeleteBuffers     (1, &vao.ebo);
}

//I would have passed it by scene_vertex but that doesn't work
BATCH_INLINE void
fill_scene_vertex(cube data,
                  const i32 index,
                  const f32 x,
                  const f32 y,
                  const f32 z,
                  const f32 uvx,
                  const f32 uvz
) {
	data[index].v.pos[0] = x; data[index].v.pos[1] = y; data[index].v.pos[2] = z; data[index].v.uv[0] = uvx; data[index].v.uv[1] = uvz;
}

BATCH_INLINE void
fill_cube_elements(cube_elements data, const i32 offset) {
	const i32 i = offset * 24;

	data[ 0] =  0 + i; data[ 1] =  3 + i; data[ 2] =  2 + i;
	data[ 3] =  2 + i; data[ 4] =  1 + i; data[ 5] =  0 + i;
	data[ 6] =  4 + i; data[ 7] =  5 + i; data[ 8] =  6 + i;
	data[ 9] =  6 + i; data[10] =  7 + i; data[11] =  4 + i;

	data[12] = 11 + i; data[13] =  8 + i; data[14] =  9 + i;
	data[15] =  9 + i; data[16] = 10 + i; data[17] = 11 + i;
	data[18] = 12 + i; data[19] = 13 + i; data[20] = 14 + i;
	data[21] = 14 + i; data[22] = 15 + i; data[23] = 12 + i;

	data[24] = 16 + i; data[25] = 17 + i; data[26] = 18 + i;
	data[27] = 18 + i; data[28] = 19 + i; data[29] = 16 + i;
	data[30] = 20 + i; data[31] = 21 + i; data[32] = 22 + i;
	data[33] = 22 + i; data[34] = 23 + i; data[35] = 20 + i;
}

BATCH_INLINE void
make_cube(cube data) {
	//data[0].v.pos[0] = -0.5f; data[0].v.pos[1] = -0.5f; data[0].v.pos[2] = -0.5f; data[0].v.uv[0] = 0.0f; data[0].v.uv[1] = 0.0f;
	fill_scene_vertex(data,  0, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f);
	fill_scene_vertex(data,  1,  0.5f, -0.5f, -0.5f, 1.0f, 0.0f);
	fill_scene_vertex(data,  2,  0.5f,  0.5f, -0.5f, 1.0f, 1.0f);
	fill_scene_vertex(data,  3, -0.5f,  0.5f, -0.5f, 0.0f, 1.0f);
	fill_scene_vertex(data,  4, -0.5f, -0.5f,  0.5f, 0.0f, 0.0f);
	fill_scene_vertex(data,  5,  0.5f, -0.5f,  0.5f, 1.0f, 0.0f);
	fill_scene_vertex(data,  6,  0.5f,  0.5f,  0.5f, 1.0f, 1.0f);
	fill_scene_vertex(data,  7, -0.5f,  0.5f,  0.5f, 0.0f, 1.0f);

	fill_scene_vertex(data,  8, -0.5f,  0.5f, -0.5f, 0.0f, 0.0f);
	fill_scene_vertex(data,  9, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f);
	fill_scene_vertex(data, 10, -0.5f, -0.5f,  0.5f, 1.0f, 1.0f);
	fill_scene_vertex(data, 11, -0.5f,  0.5f,  0.5f, 0.0f, 1.0f);
	fill_scene_vertex(data, 12,  0.5f, -0.5f, -0.5f, 0.0f, 0.0f);
	fill_scene_vertex(data, 13,  0.5f,  0.5f, -0.5f, 1.0f, 0.0f);
	fill_scene_vertex(data, 14,  0.5f,  0.5f,  0.5f, 1.0f, 1.0f);
	fill_scene_vertex(data, 15,  0.5f, -0.5f,  0.5f, 0.0f, 1.0f);

	fill_scene_vertex(data, 16, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f);
	fill_scene_vertex(data, 17,  0.5f, -0.5f, -0.5f, 1.0f, 0.0f);
	fill_scene_vertex(data, 18,  0.5f, -0.5f,  0.5f, 1.0f, 1.0f);
	fill_scene_vertex(data, 19, -0.5f, -0.5f,  0.5f, 0.0f, 1.0f);
	fill_scene_vertex(data, 20,  0.5f,  0.5f, -0.5f, 0.0f, 0.0f);
	fill_scene_vertex(data, 21, -0.5f,  0.5f, -0.5f, 1.0f, 0.0f);
	fill_scene_vertex(data, 22, -0.5f,  0.5f,  0.5f, 1.0f, 1.0f);
	fill_scene_vertex(data, 23,  0.5f,  0.5f,  0.5f, 0.0f, 1.0f);
}

/*!Warn, this function is slow, in a real project transformations should be optimized as much as possible 
this function only works given the scope of the project*/
/*--Possible Optimizations--
  *Use an array of matrices on the gpu instead of calculating the model on the cpu and have an extra vertex float to indicate the index
  *Raw calculation similar to the one done in make_quad, maybe even removing matrices and unifying the cube creation process
*/
BATCH_INLINE void
transform_cube(cube data, const mat4 model) {
	for(i32 i=0; i<24; ++i) {
		mat4_mulv3(model, 1.0f, data[i].v.pos, data[i].v.pos);
	}
}
#endif
