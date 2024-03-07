#include "text.h"

BATCH_INLINE font
create_font(u8* buffer, const i32 w, const i32 h, const i32 size, const i32 num_glyphs) {
	font f = {};
	f.num_glyphs = num_glyphs;
	f.w = w;
	f.h = h;
	f.bitmap = (u8*)malloc(w*h);
	f.cdata = (stbtt_bakedchar*)malloc(num_glyphs*sizeof(stbtt_bakedchar));
	stbtt_BakeFontBitmap(buffer,
	                     0,
	                     size,
	                     f.bitmap,
	                     w,
	                     h,
	                     32,
	                     num_glyphs,
	                     f.cdata);

	return f;
}

BATCH_INLINE const texture
create_font_atlas(const font f) {
	texture atlas;
	glCreateTextures(GL_TEXTURE_2D, 1, &atlas);
	glBindTexture(GL_TEXTURE_2D, atlas);
	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             GL_R8,
	             f.w,
	             f.h,
	             0,
	             GL_RED,
	             GL_UNSIGNED_BYTE,
	             f.bitmap);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	return atlas;
}

BATCH_INLINE glyph_quad
get_glyph_quad(unsigned char c, vec2 txt_pos, const font f, const f32 size) {
	glyph_quad g;

	if(c>=32 && c<128) {
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(f.cdata, f.w, f.h, c - 32, &txt_pos[0], &txt_pos[1], &q, 1);

		g.bbmin[0] =  q.x0*size;
		g.bbmin[1] = -q.y1*size;

		g.bbmax[0] =  q.x1*size;
		g.bbmax[1] = -q.y0*size;

		g.uvmin[0] =  q.s0;
		g.uvmin[1] =  q.t0;

		g.uvmax[0] =  q.s1;
		g.uvmax[1] =  q.t1;
	}
	return g;
}

BATCH_INLINE void
make_glyph_quad(quad data, const glyph_quad g) {
	//Vertices
	data[0][0] = g.bbmin[0]; data[0][1] = g.bbmin[1];
	data[1][0] = g.bbmax[0]; data[1][1] = g.bbmin[1];
	data[2][0] = g.bbmax[0]; data[2][1] = g.bbmax[1];
	data[3][0] = g.bbmin[0]; data[3][1] = g.bbmax[1];

	//Uvs
	data[0][2] = g.uvmin[0]; data[0][3] = g.uvmax[1];
	data[1][2] = g.uvmax[0]; data[1][3] = g.uvmax[1];
	data[2][2] = g.uvmax[0]; data[2][3] = g.uvmin[1];
	data[3][2] = g.uvmin[0]; data[3][3] = g.uvmin[1];

	//Texture id
	data[0][4] = 0.0f;
	data[1][4] = 0.0f;
	data[2][4] = 0.0f;
	data[3][4] = 0.0f;
}

BATCH_INLINE void
free_font(font* f) {
	free(f->cdata);
	stbtt_FreeBitmap(f->bitmap, NULL);
}

static inline void
make_text_row(font f, vec2 txt_pos, i32* av_vertice, u32* index_count, quad quad_arr[], const char* msg, ...) {
	__builtin_va_list arg_ptr;
	va_start(arg_ptr, msg);

	char message[100];
	memset(message, 0, sizeof(message));

	vsnprintf(message, 100, msg, arg_ptr);
	va_end(arg_ptr);

	for(i32 i=0; i < 100; ++i) {
		const char c = message[i];
		if(!c) { break; };
		const glyph_quad g = get_glyph_quad(c, txt_pos, f, 1.0f);
		if(c != ' ') {
			make_glyph_quad(quad_arr[*av_vertice], g);
			*index_count+=6;
			++(*av_vertice);
		}
	}
}
