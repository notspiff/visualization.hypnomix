#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "opengl.h"
#include "shader.h"
#include "font.h"
#include "texture.h"
#include "mathematics.h"


#ifdef X11 
	#define glGenBuffers glGenBuffersEXT
	#define glBindFramebuffer glBindFramebufferEXT
	#define glGenFramebuffers glGenFramebuffersEXT
	#define glFramebufferTexture2D glFramebufferTexture2DEXT
	#define glCheckFramebufferStatus glCheckFramebufferStatusEXT
	#ifndef GL_FRAMEBUFFER 
		#define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT 
	#endif
	#ifndef GL_FRAMEBUFFER_COMPLETE 
		#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT 
	#endif
	#ifndef GL_COLOR_ATTACHMENT0 
		#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT 
	#endif
#endif


static FT_Library ft;
static FT_Face face;
static int ftw;
static int fth;
static mat4 mortho;
static struct program fontpg;


static const char fontvtx[] = 
	"#version 120\n"
	"attribute vec4 pos;\n"
	"attribute vec2 texpos;\n"
	"uniform mat4 mvp;\n"
	"varying vec2 vtexpos;\n"
	"void main(void) {\n"
	"	vtexpos = texpos;\n"
	"	gl_Position = mvp * pos;\n"
	"}";

static const char fontfsh[] = 
	"#version 120\n"
	"varying vec2 vtexpos;\n"
	"uniform sampler2D texture;\n"
	"void main(void) {\n"
	"	vec4 clr = texture2D(texture, vtexpos);\n"
	"	gl_FragColor = clr;\n"
	"}";

enum {
	FRAMEBUFFER_FREETYPE,
	FRAMEBUFFER_NUM
};

GLuint freetypetex;
GLuint framebuffers[FRAMEBUFFER_NUM];


int fontInit(struct hypnomix *hyp, const char *ftname, int w, int h)
{
	int err;
	// GLenum drawbuffers[2] = { GL_COLOR_ATTACHMENT0 };
	// GLuint status;

	shaderInit(&fontpg.id, fontvtx, fontfsh, NULL);
	fontpg.pos = shaderGetAttribute(fontpg.id, "pos");
	fontpg.texpos = shaderGetAttribute(fontpg.id, "texpos");
	fontpg.texture0 = shaderGetUniform(fontpg.id, "texture");
	fontpg.mvp = shaderGetUniform(fontpg.id, "mvp");

	err = FT_Init_FreeType(&ft);
	if(err) {
		(*hyp->log)(HYP_ERROR, "fontInit(): FT_Init_Freetype()\n");
		return -1;
	}

	err = FT_New_Face(ft, ftname, 0, &face);
	if(err == FT_Err_Unknown_File_Format) {
		(*hyp->log)(HYP_ERROR, "fontInit(): %s format unsupported\n", 
			ftname);
		return -1;
	} else if(err) {
		(*hyp->log)(HYP_ERROR, "fontInit(): opening %s\n", ftname);
		return -1;
	}

	err = FT_Set_Pixel_Sizes(face, w, h);
	if(err) {
		(*hyp->log)(HYP_ERROR, "fontInit(): setting pixel size\n");
		return -1;
	}

	ftw = w;
	fth = h;

	(*hyp->log)(HYP_SUCCESS, "fontInit(): loading %s\n", ftname);

	mat4Orthographic(mortho, 0.0, hyp->w, 0.0, hyp->h, -1.0, hyp->h);

	/* create the framebuffer to render the texture */
/* FIXME: framebuffer does not work... */
	/* glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FREETYPE_RENDER]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hyp->w, hyp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenFramebuffers(FRAMEBUFFER_NUM, framebuffers);
printf("glGetError(main) = %d\n", glGetError());
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[FRAMEBUFFER_FREETYPE]);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, textures[TEXTURE_FREETYPE_RENDER], 0);
	glDrawBuffers(1, drawbuffers);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "ERROR: fontInit(): create framebuffer: 0x%x\n",
			status);
		exit(1);
	} 

	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 

	fprintf(stderr, "SUCCESS: fontInit(): creating framebuffer\n");  */
	return 0;
}


void fontDestroy(void)
{
//	glDeleteFramebuffers(1, &ft_framebuffer);
	glDeleteTextures(1, &freetypetex);
	shaderDestroy(fontpg.id);
}


void fontPrint(struct hypnomix *hyp, const char *txt, float x, float y, 
	int r, int g, int b, int a)
{
	int i, j;
	int len = strlen(txt);
	int err;
	FT_GlyphSlot slot = face->glyph;
	FT_UInt id;
	unsigned char rgba_glyph[ftw * fth * 4];
/*
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[FRAMEBUFFER_FREETYPE]); */
	glViewport(0, 0, hyp->w, hyp->h);

//	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glUseProgram(fontpg.id);
	glUniformMatrix4fv(fontpg.mvp, 1, GL_FALSE, mortho);

	glGenTextures(1, &freetypetex);
	glBindTexture(GL_TEXTURE_2D, freetypetex);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(fontpg.texture0, 0);
	/* glyph are 1 grayscale texture */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for(i = 0; i < len; i++) {
		id = FT_Get_Char_Index(face, txt[i]);
		err = FT_Load_Glyph(face, id, FT_LOAD_DEFAULT);
		if(err) {
			fprintf(stderr, "WARNING: fontPrint(): "
				"load glyph %c\n", txt[i]);
		}
		err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if(err) {
			fprintf(stderr, "WARNING: fontPrint(): "
				"render glyph %c\n", txt[i]);
		}

		for(j = 0; j < slot->bitmap.width * slot->bitmap.rows; j++) {
			rgba_glyph[j*4] = r;
			rgba_glyph[j*4+1] = g; 
			rgba_glyph[j*4+2] = b;
			rgba_glyph[j*4+3] = slot->bitmap.buffer[j]
				* (float)a / 255.0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, slot->bitmap.width,
			slot->bitmap.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			rgba_glyph); 
	/*	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, slot->bitmap.width,
			slot->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
			rgba_glyph); */

		fontDrawChar(slot->bitmap.buffer, x+slot->bitmap_left, 
			y, slot->bitmap.width,
			slot->bitmap.rows);

		x += slot->advance.x >> 6;
		y += slot->advance.y >> 6;
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
/*
	glBindFramebuffer(GL_FRAMEBUFFER, 0); */

//	fontDrawFramebuffer();
}


void fontDrawChar(unsigned char *buf, float x, float y, float w, float h)
{
	struct point2D v[4];

	v[0].x = x; v[0].y = y; v[0].s = 0.0; v[0].t = 0.0;
	v[1].x = x + w; v[1].y = y; v[1].s = 1.0; v[1].t = 0.0;
	v[2].x = x; v[2].y = y + h; v[2].s = 0.0; v[2].t = 1.0;
	v[3].x = x + w; v[3].y = y + h; v[3].s = 1.0; v[3].t = 1.0;

	glEnableVertexAttribArray(fontpg.pos);	
	glVertexAttribPointer(fontpg.pos, 2, GL_FLOAT, GL_FALSE, 
		sizeof(struct point2D), v);

	glEnableVertexAttribArray(fontpg.texpos);
	glVertexAttribPointer(fontpg.texpos, 2, GL_FLOAT, GL_FALSE,
		 sizeof(struct point2D), 
		(unsigned char *)v + 2 * sizeof(float));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(fontpg.texpos);
	glDisableVertexAttribArray(fontpg.pos);
}


void fontDrawFramebuffer(struct hypnomix *hyp) 
{
	struct point2D v[4];

	float x = 0.0;
	float y = 0.0;
	float w = hyp->w;
	float h = hyp->h;

	v[0].x = x; v[0].y = y; v[0].s = 0.0; v[0].t = 1.0;
	v[1].x = x + w; v[1].y = y; v[1].s = 1.0; v[1].t = 1.0;
	v[2].x = x; v[2].y = y + h; v[2].s = 0.0; v[2].t = 0.0;
	v[3].x = x + w; v[3].y = y + h; v[3].s = 1.0; v[3].t = 0.0;

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(fontpg.id); /* FIXME: put that elsewhere ? */
	glUniformMatrix4fv(fontpg.mvp, 1, GL_FALSE, mortho);

	glEnableVertexAttribArray(fontpg.pos);	
	glVertexAttribPointer(fontpg.pos, 2, GL_FLOAT, GL_FALSE, sizeof(struct point2D), v);

	glEnableVertexAttribArray(fontpg.texpos);
	glVertexAttribPointer(fontpg.texpos, 2, GL_FLOAT, GL_FALSE,
		 sizeof(struct point2D), 
		(unsigned char *)v + 2 * sizeof(float));

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(fontpg.texture0, 0);
	glBindTexture(GL_TEXTURE_2D, freetypetex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(fontpg.texpos);
	glDisableVertexAttribArray(fontpg.pos);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}
