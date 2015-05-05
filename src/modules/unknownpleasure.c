#include <stdio.h>
#include <math.h>
#include <string.h>

#include "unknownpleasure.h"
#include "global.h"
#include "mathematics.h"


#define MAX_GRID_WIDTH 80
#define MAX_GRID_HEIGHT 64

float vtx[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*3*2];
static GLuint idx[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*3*2];
static int nbidx = 0;
float morphvtx[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*3*2];
static GLfloat clr[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*4];
static GLfloat back[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*4*2];
static mat4 mortho;
static float ratio;

static int clrfade = FADE_NONE;


void unknownpleasureInit(struct unknownpleasure *up)
{
	int i, j, id;
	float orgx, orgy, orgz;
	float x, y, z;
	float w, h;

	if(up->gridw > MAX_GRID_WIDTH) {
		up->gridw = MAX_GRID_WIDTH;
	}
	if(up->gridh > MAX_GRID_HEIGHT) {
		up->gridh = MAX_GRID_HEIGHT;
	}

	w = up->scrw / up->gridw; /* width between two points */
	h = up->scrh / up->gridh;

	orgx = (up->scrw - w * up->gridw) / 2;
	orgy = (up->scrh - h * up->gridh) / 2; 
	orgz = -up->gridw;

printf("ORGX=%f, ORGY=%f\n", orgx, orgy);
printf("GRIDW=%d, GRIDH=%d, W=%f, H=%f\n", up->gridw, up->gridh, w, h);
printf("SCRW=%d, SCRH=%d\n", up->scrw, up->scrh);

	for(j = 0; j < up->gridh; j++) {
		for(i = 0; i < up->gridw; i++) {
			x = orgx + (float)i * w;
			y = orgy + (float)j * h;
			z = orgz + (float)j;
			id = (i + j * up->gridw) * 3;
			up->vtx[id] = x;
			up->vtx[id+1] = y;
			up->vtx[id+2] = z;
			id = (i + j * up->gridw) * 4;
			up->clr[id] = up->fc[0];
			up->clr[id+1] = up->fc[1];
			up->clr[id+2] = up->fc[2];
			up->clr[id+3] = up->fc[3];
		}
	}

	mat4Orthographic(mortho, 0.0, up->scrw, 0.0, up->scrh, -1.0, up->scrh);
	memcpy(&(up->morphvtx), &(up->vtx), up->gridw*up->gridh*6*sizeof(float));
	/* FIXME why 6? */
}

/*
void unknownpleasureInitOLD(struct unknownpleasure *up)
{	
	int i, j;
	int id; 
	float z;
	float tmpgridw = up->scrw * (440.0 / 838.0 * (9.0/16.0));
	float tmpgridh = up->scrh * (550.0 / 838.0);
	float w, h;
	float x, y;
	float posx, posy, posz;

	if(up->gridw > MAX_GRID_WIDTH)
		up->gridw = MAX_GRID_WIDTH;
	if(up->gridh > MAX_GRID_HEIGHT)
		up->gridh = MAX_GRID_HEIGHT;

	w = tmpgridw / (float)up->gridw;
	h = tmpgridh / (float)up->gridh;
	x = (up->scrw - tmpgridw) / 2.0;
	y = (up->scrh - tmpgridh) / 2.0;
	ratio = up->scrw / up->scrh;

	for(j = 0; j < up->gridh; j++) {
		for(i = 0; i < up->gridw; i++) {
			posx = x + (float)i * w;
			posy = y + (float)j * h;
			posz = -up->gridw + j;

			vtx[(i+j*up->gridw)*3] = posx;
			vtx[(i+j*up->gridw)*3+1] = posy;
			vtx[(i+j*up->gridw)*3+2] = posz;
			setColor(clr+(i+j*up->gridw)*4, 
				up->fc[0], up->fc[1], up->fc[2], up->fc[3]);
			setColor(back+(i+j*up->gridw)*4, up->bc[0],
				up->bc[1], up->bc[2], up->bc[3]);

			id = up->gridw * up->gridh * 3;
			vtx[(i+j*up->gridw)*3+id] = posx;
			vtx[(i+j*up->gridw)*3+1+id] = posy;
			vtx[(i+j*up->gridw)*3+2+id] = posz;

			id = up->gridw * up->gridh * 4;
			setColor(back+(i+j*up->gridw)*4+id, up->bc[0],
				up->bc[1], up->bc[2], up->bc[3]);
setColor(back+(i+j*up->gridw)*4+id, 1.0,0.0, 0.0, 1.0);
		}
	}

	nbidx = 0;
	for(j = 0; j < up->gridh; j++) {
		for(i = 0; i < up->gridw-1; i++) {
			id = j * up->gridw; 
			idx[nbidx++] = i + id;
			idx[nbidx++] = i + id + 1;
			idx[nbidx++] = i + id + up->gridw * up->gridh;
			idx[nbidx++] = i + id + up->gridw * up->gridh;
			idx[nbidx++] = i + id + up->gridw * up->gridh + 1;
			idx[nbidx++] = i + id + 1;
		}
	}

	mat4Orthographic(mortho, 0.0, up->scrw, 0.0, up->scrh, -1.0, up->scrh);
	memcpy(morphvtx, vtx, up->gridw*up->gridh*6*sizeof(GLfloat));
}
*/

void unknownpleasureMorph(struct unknownpleasure *up, float *data, int len)
{
	int i, j, k, m;
	int left, right, cleft, cright;
	int id;
	float mul, value;
	float color, maxcolor;
	float max;

	maxcolor = 0.0;
	k = 0;
// goto MORPH;
	for(j = 0; j < up->gridh; j++) {
		for(i = 0; i < up->gridw; i++) {
			mul = 720.0;
		mul = 0.01;
			id = (i + j * up->gridw) * 3 + 1; /* y pos */
			morphvtx[id] = vtx[id] - data[k++] * mul;
		morphvtx[id] = vtx[id];
			if(k > len) {
				k = 0;
			}
		}
	}

goto END;
MORPH:
	for(j = 0; j < up->gridh / 2; j++) {	
		color = 0.0;
		m = 0;
		max = 0.0;
		for(i = 0; i < up->gridw / 2; i++) {
			m = up->gridw / 2 - fabs(i - up->gridw/ 2);
			mul = m * m * 0.3 * ratio;
			left = (i + j * 2 * up->gridw) * 3;
			right = (i + (j * 2 + 1) * up->gridw) * 3;
			value = data[k];
			if(value > max) {
				max = value;
			}
			morphvtx[left+1] = vtx[left+1] - mul * value;
			k++;
			value = data[k];
			morphvtx[right+1] = vtx[right+1] - mul * value;
			color += fabs(data[k]);
			k++;

			if(k >= len) {
				k = 0;
			}
		}

		color = max*4.0;
color = 0.0;

//  printf("MAXCOLOR=%f, COLOR=%f\n", maxcolor, color);
		for(i = 0; i < up->gridw; i++) {
			cleft = (i + j * 2 * up->gridw) * 4;
			cright = (i + (j * 2 + 1) * up->gridw) * 4;
			clr[cleft] = color;
			clr[cleft+1] = color;
			clr[cleft+2] = color;
			clr[cright] = color;
			clr[cright+1] = color;
			clr[cright+2] = color;
		}
	}
END:
	color = 1;
}


void unknownpleasureStartFade(int fadetype)
{
	clrfade = fadetype;
printf("START FADE\n");
}



void unknownpleasureFade(void)
{
	int i;

	if(clrfade != FADE_NONE) {
		for(i = 0; i < MAX_GRID_WIDTH*MAX_GRID_HEIGHT; i++) {
			if(clrfade == FADE_IN) {
				if(clr[i*4+3] >= 1.0) {
					clrfade = FADE_NONE;
				} else {
					clr[i*4+3] += FADE_SPEED;
				}
			} else {
				if(clr[i*4+3] <= 0.0) {
					clrfade = FADE_NONE;
				} else {
					clr[i*4+3] -= FADE_SPEED;
				}
			}
		}
	}
}


void unknownpleasureDraw(struct unknownpleasure *up)
{
	int j;

/*
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); */

	glLineWidth(1.5);

	glUseProgram(prgup.id);
	glUniformMatrix4fv(prgup.mvp, 1, GL_FALSE, mortho);

	glEnableVertexAttribArray(prgup.pos);
	glEnableVertexAttribArray(prgup.clr);

	glVertexAttribPointer(prgup.pos, 3, GL_FLOAT, GL_FALSE, 0, up->vtx);
//	glVertexAttribPointer(prgup.clr, 4, GL_FLOAT, GL_FALSE, 0, back);

	// for(j = 0; j < up->gridh; j++) {
/*
	for(j = 0; j < 1; j++) {
		glDrawElements(GL_TRIANGLE_STRIP, up->gridw* 6, 
			GL_UNSIGNED_INT, 
			(char*)idx + j * up->gridw * 6 * sizeof(GLuint));
	}
*/
	glVertexAttribPointer(prgup.clr, 4, GL_FLOAT, GL_FALSE, 0, up->clr);

	// for(j = 0; j < up->gridh; j++) {
/*
	for(j = 0; j < 1; j++) {
		glDrawArrays(GL_LINE_STRIP, up->gridw*j, up->gridw);
	} */

glDrawArrays(GL_LINES, 0, up->gridw*up->gridh);

	glDisableVertexAttribArray(prgup.clr);
	glDisableVertexAttribArray(prgup.pos);
}
