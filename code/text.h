#if !defined(TEXT_H)
#define TEXT_H

typedef struct {
	vec2 bbmin;
	vec2 bbmax;
	vec2 uvmin;
	vec2 uvmax;
} glyph_quad;

typedef struct {
	i32              num_glyphs;
	i32              size;
	i32              w;
	i32              h;
	u8*              bitmap;
	stbtt_bakedchar* cdata;
} font;

#endif
