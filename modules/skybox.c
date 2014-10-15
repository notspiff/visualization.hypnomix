#include <stdio.h>

#include "hypnomix.h"
#include "shader.h"
#include "skybox.h"


static struct program pg;
static struct transform tr;

static const char skyboxvsh[] = 
	"#version 120\n"
	"attribute vec4 pos;\n"
	"//attribute vec4 clr;\n"
	"attribute vec2 texpos;\n"
	"uniform mat4 mvp;\n"
	"varying vec4 v_color;\n"
	"varying vec2 v_texpos;\n"
	"void main(void) {\n"
	"	//v_color = clr;\n"
	"	v_texpos = texpos;\n"
	"	gl_Position = mvp * pos;\n"
	"}";

static const char skyboxfsh[] = 
	"#version 120\n"
	"//varying vec4 v_color;\n"
	"varying vec2 v_texpos;\n"
	"uniform sampler2D texture0;\n"
	"void main(void) {\n"
	"	vec4 clr = texture2D(texture0, v_texpos);\n"
	"	gl_FragColor = clr;\n"
	"	// gl_FragColor = v_color * clr;\n"
	"	// gl_FragColor = vec4(v_texpos.x, v_texpos.y, 0.0, 1.0);\n"
	"}";
/*
static float vtx[] = {
	-1.0, -1.0, 1.0,
	1.0, -1.0, 1.0,
	-1.0, 1.0, 1.0,
	1.0, 1.0, 1.0,

	-1.0, 1.0, -1.0,
	1.0, 1.0, -1.0,
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0
};


static float texpos[] = {
	0.0, 0.0,
	1.0, 0.0,
	0.0, 1.0,
	1.0, 1.0, 

	0.0, 1.0,
	1.0, 1.0,
	0.0, 0.0,
	1.0, 0.0
};


static unsigned int idx[] = {
	0, 2, 1,
	2, 3, 1,
	2, 4, 3,
	4, 5, 3,
	4, 6, 5,
	6, 7, 5,
	6, 0, 7,
	0, 1, 7,
	0, 6, 4,
	0, 4, 2, 
	1, 3, 7,
	3, 5, 7
};
*/

static float vtx[] = {
	-1.0, -1.0, 1.0, 	/* A */
	1.0, -1.0, 1.0, 	/* B */
	-1.0, 1.0, 1.0, 	/* C */
	1.0, 1.0, 1.0, 		/* D */

	-1.0, 1.0, 1.0, 	/* E */
	1.0, 1.0, 1.0, 		/* F */
	-1.0, 1.0, -1.0, 	/* G */
	1.0, 1.0, -1.0, 	/* H */

	-1.0, 1.0, -1.0, 	/* I */
	1.0, 1.0, -1.0, 	/* J */
	-1.0, -1.0, -1.0, 	/* K */
	1.0, -1.0, -1.0, 	/* L */

	-1.0, -1.0, -1.0, 	/* M */
	1.0, -1.0, -1.0, 	/* N */
	-1.0, -1.0, 1.0, 	/* O */
	1.0, -1.0, 1.0, 	/* P */

	-1.0, -1.0, -1.0, 	/* Q */
	-1.0, -1.0, 1.0, 	/* R */
	-1.0, 1.0, -1.0, 	/* S */
	-1.0, 1.0, 1.0, 	/* T */

	1.0, -1.0, 1.0, 	/* U */
	1.0, -1.0, -1.0, 	/* V */
	1.0, 1.0, 1.0, 		/* W */
	1.0, 1.0, -1.0 		/* X */
};


static unsigned int idx[] = {
	0, 2, 1, 	/* ACB */
	1, 2, 3, 	/* BCD */
	20, 22, 21, 	/* UWV */
	21, 22, 23,	/* VWX */
	16, 18, 17,	/* QSR */
	17, 18, 19,	/* RST */
	4, 6, 5,	/* EGF */
	5, 6, 7,	/* FGH */
	8, 10, 9, 	/* IKJ */
	9, 10, 11,	/* JKL */
	12, 13, 14,	/* MNO */
	13, 14, 15	/* NOP */
};


static float texpos[] = {
	0.0, 0.0, 	/* ABCD */
	1.0, 0.0,
	0.0, 1.0,
	1.0, 1.0, 

	0.0, 0.0, 	/* EFGH */
	1.0, 0.0,
	0.0, 1.0,
	1.0, 1.0, 

	0.0, 1.0, 	/* IJKL */
	1.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,

	0.0, 0.0, 	/* MNOP */
	1.0, 0.0,
	0.0, 1.0,
	1.0, 1.0, 

	0.0, 0.0, 	/* QRST */
	1.0, 0.0,
	0.0, 1.0,
	1.0, 1.0, 

	0.0, 0.0, 	/* UVWX */
	1.0, 0.0,
	0.0, 1.0,
	1.0, 1.0, 

};


static GLuint skyboxtex;

#define TEX_WIDTH 256
#define TEX_HEIGHT TEX_WIDTH
unsigned char data[TEX_WIDTH*TEX_HEIGHT*4];


/* plasma: http://lodev.org/cgtutor/plasma.html */
void skyboxCompute(struct hypnomix *hyp)
{
	int i, j;
	int id;

	for(j = 0; j < TEX_HEIGHT; j++) {
		for(i = 0; i < TEX_WIDTH; i++) {
			id = (i + j * TEX_HEIGHT) * 4;
			//data[id] = 128.0 + 128.0 * sinf(i/8.0);
			//data[id+1] = 128.0 + 128.0 * sinf(j/8.0);
			data[id] = (128.0 + 128.0 * sinf((i)/16.0) 
				+ 128.0 + 128.0 * sinf(j/4.0)
				+ 128.0 + 128.0 * sinf(j+j/8.0)
				+ sinf(sqrt(i*i + j*j) / 8.0)) / 4.0;
			data[id+1] = (128.0 + 128.0 * sinf((j+j)/16.0) 
				+ 128.0 + 128.0 * sinf(i/8.0)
				+ 128.0 + 128.0 * sinf(j/8.0)
				+ sinf(sqrt(i*i + j*j) / 4.0)) / 4.0;
			data[id+2] = (128.0 + 128.0 * sinf((i+j)/16.0) 
				+ 128.0 + 128.0 * sinf(i/16.0)
				+ 128.0 + 128.0 * sinf(j/8.0)
				+ sinf(sqrt(i*i + j*j) / 8.0)) / 4.0;
			data[id+3] = 0xff;
		}
	}

	glGenTextures(1, &skyboxtex);
	glBindTexture(GL_TEXTURE_2D, skyboxtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);
}


void skyboxInit(struct hypnomix *hyp)
{
	transformProjection(&tr, hyp->pmatrix);
	shaderInit(&pg.id, skyboxvsh, skyboxfsh, NULL);
fprintf(stderr, "AFTER SHADER INIT\n");
	pg.pos = shaderGetAttribute(pg.id, "pos");
	pg.texpos = shaderGetAttribute(pg.id, "texpos");
	pg.mvp = shaderGetUniform(pg.id, "mvp");
	pg.texture0 = shaderGetUniform(pg.id, "texture0");

	skyboxCompute(hyp);
}


void skyboxDestroy(struct hypnomix *hyp)
{
	glDeleteTextures(1, &skyboxtex);
	shaderDestroy(pg.id);
}


void skyboxDraw(struct hypnomix *hyp, float ax, float ay, float az)
{	
	GLint pgid;

	glGetIntegerv(GL_CURRENT_PROGRAM, &pgid);
	glUseProgram(pg.id);

	transformIdentity(&tr);
	transformModelScale(&tr, 50.0, 50.0, 50.0);
	transformModelRotate(&tr, ax, ay, az); 
//	transformViewRotate(&tr, 0.0, angle, 0.0);
// transformViewTranslate(&tr, 0.0, 0.0, -19.0);

	transformMVP(&tr);

	// glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);

	glUniformMatrix4fv(pg.mvp, 1, GL_FALSE, tr.mvp);
	glEnableVertexAttribArray(pg.pos);
	glEnableVertexAttribArray(pg.texpos);

	glVertexAttribPointer(pg.pos, 3, GL_FLOAT, GL_FALSE, 0, vtx);
	glVertexAttribPointer(pg.texpos, 2, GL_FLOAT, GL_FALSE, 0, texpos);

	glBindTexture(GL_TEXTURE_2D, skyboxtex);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(pg.texture0, 0);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, idx);

	glDisableVertexAttribArray(pg.texpos);
	glDisableVertexAttribArray(pg.pos);

//	glDisable(GL_CULL_FACE);

	glUseProgram(pgid);
}
