#if !defined(GRAPHICS_GENERICS_H)
#define GRAPHICS_GENERICS_H
typedef u32 shader_id;
typedef u32 shader_program_id;
typedef u8* image;
typedef u32 texture;

typedef struct {
	u32 id;
	u32 vbo;
	u32 ebo;
} vertex_array;

typedef union {
	f32 data[5];
	struct {
		vec3 pos;
		vec2 uv;
	} v;
} scene_vertex;

typedef scene_vertex cube[24];

typedef i32 cube_elements[36];

/*
  [0-1] = Position x,y
  [2-3] = Texture coordinate
  [ 4 ] = Texture id

  The reason it's not called text_vertex it's because it is a generic hud vertex
*/
typedef f32 hud_vertex[5];
typedef hud_vertex quad[4];

#define CUBE_VERTICES { {-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, }, \
                        { 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  }, \
                        { 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  }, \
                        {-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, }, \
                        {-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, }, \
                        { 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  }, \
                        { 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  }, \
                        {-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, }, \
                                                              \
                        {-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, }, \
                        {-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, }, \
                        {-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, }, \
                        {-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, }, \
                        { 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  }, \
                        { 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  }, \
                        { 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  }, \
                        { 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  }, \
                                                              \
                        {-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, }, \
                        { 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  }, \
                        { 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  }, \
                        {-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, }, \
                        { 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, }, \
                        {-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, }, \
                        {-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, }, \
                        { 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, } }  

#endif
