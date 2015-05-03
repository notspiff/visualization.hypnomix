#define _GNU_SOURCE /* need for strcasestr */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <jpeglib.h>
#include <math.h>
#include "texture.h"


static int tex_width;
static int tex_height;

static int maxtexturesize;


static void powerOfTwoScaling(unsigned char **buf, int *w, int *h)
{
	int potw;
	int poth;
	int i, j;
	unsigned char *tmp;
	float scalew, scaleh;
	int scalei, scalej;
	int tmpid, bufid;

	i = 0;
	potw = 0;
	while(potw < (*w)) {
		potw = pow(2.0, i++);		
	}

	i = 0;
	poth = 0;
	while(poth < (*h)) {
		poth = pow(2.0, i++);
	}

	if((*h) != poth && (*w) != potw) {
		fprintf(stdout, "INFO: powerOfTwoScaling(): %dx%d -> %dx%d\n",
			(*w), (*h), potw, poth);
		tmp = (unsigned char *)malloc(potw * poth * 4);
		if(tmp == NULL) {
			fprintf(stderr, "ERROR: powerOfTwoScaling(): "
				"tmp malloc\n");
			exit(1);
		}

		/* crappy scaling */
		scalew = (float)(*w) / (float)potw;
		scaleh = (float)(*h) / (float)poth;
		scalei = 0;
		for(i = 0; i < potw; i++) {
			scalei = scalew * i;
			scalej = 0;
			for(j = 0; j < poth; j++) {
				scalej = scaleh * j;
				tmpid = (i + j * potw) * 4;
				bufid = (scalei + scalej * (*w)) * 4;
				tmp[tmpid] = (*buf)[bufid];
				tmp[tmpid+1] = (*buf)[bufid+1];
				tmp[tmpid+2] = (*buf)[bufid+2];
				tmp[tmpid+3] = (*buf)[bufid+3];
			}
		} 
		
		free((*buf)); 
		(*buf) = tmp;
		(*w) = potw;
		(*h) = poth;
	}
}


/*
 * http://paulbourke.net/libraries/jpeg.c
 */
static unsigned char *readJPEG(const char *filename)
{
	unsigned char *buf;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *fp;
	JSAMPLE *scanlines;
	int row_stride;
	int i, j;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	if((fp = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "ERROR: readJPEG(): fopen %s\n", filename);
		return NULL;
	}

	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	if(cinfo.output_components != 3) {
		fprintf(stderr, "ERROR: readJPEG(): %s is not RGB\n", filename);
		return NULL;
	}

	/* alloc RGBA image */
	buf = (unsigned char*)malloc(
		cinfo.output_width * cinfo.output_height * 4);
	if(buf == NULL) {
		fprintf(stderr, "ERROR: readJPEG(): malloc(buf)\n");
		return NULL;
	}

	row_stride = cinfo.output_width * cinfo.output_components;
	scanlines = calloc(row_stride, sizeof(JSAMPLE)); /* FIXME test calloc */
	if(scanlines == NULL) {
		fprintf(stderr, "ERROR: readJPEG(): malloc(scanlines)\n");
		return NULL;
	}

	j = 0;
	while(cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, &scanlines, 1);
		for(i = 0; i < cinfo.output_width; i++) {
			/* scanlines rgba */
			buf[(i+j*cinfo.output_width)*4] = scanlines[3*i];
			buf[(i+j*cinfo.output_width)*4+1] = scanlines[3*i+1];
			buf[(i+j*cinfo.output_width)*4+2] = scanlines[3*i+2];
			buf[(i+j*cinfo.output_width)*4+3] = 0xff;
		}
		j++;
	}

	tex_width = cinfo.output_width;
	tex_height = cinfo.output_height;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(scanlines);

	fclose(fp);

	return buf;
}

/*
 * http://www.piko3d.com/tutorials/libpng-tutorial-loading-png-files-from-streams
 */
static unsigned char *readPNG(const char *filename) {
	unsigned char *buf;
	FILE *fp;
	png_byte magic[8];
	png_structp png_ptr;
	png_infop info_ptr;
	int bit_depth;
	int color_type;
	png_bytep *row_ptr;
	// int row_bytes;
	int i;

	if((fp = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "ERROR: readPng(): fopen %s\n", filename);
		return NULL;
	}

	/* check if the file is a .PNG */
	fread(magic, 1, sizeof(magic), fp);
	if(png_sig_cmp(magic, 0, sizeof(magic))) {
		printf("ERROR: readPNG(): %s is not a .png\n", filename);
		fclose(fp);
		return NULL;
	}

	png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) {
		printf("ERROR: readPNG(): png_create_read_struct\n");
		fclose(fp);
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		printf("ERROR: readPNG(): png_create_info_struct\n");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		return NULL;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		printf("ERROR: readPNG(): setjmp\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		return NULL;
	}

	/* read PNG info */
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sizeof(magic));

	png_read_info(png_ptr, info_ptr);

	tex_width = png_get_image_width(png_ptr, info_ptr);
	tex_height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	/* convert to RGBA */
	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	/* background to alpha */
	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	}

	if(bit_depth == 16) {
		png_set_strip_16(png_ptr);
	} else if(bit_depth < 8) {
		png_set_packing(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);
	// row_bytes = png_get_rowbytes(png_ptr, info_ptr);

	buf = (unsigned char*)malloc(tex_width * tex_height * 4);
	row_ptr = (png_bytep*)malloc(sizeof(png_bytep) * tex_height);

	/* pointer indexes */
	for(i=0; i<tex_height; i++) {
		row_ptr[i] = (png_bytep)(buf + i * tex_width * 4);
	}	
	png_read_image(png_ptr, row_ptr);

	free(row_ptr);
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);

	return buf;	
}


static void scalePixel(unsigned char *data, 
	int r, int g, int b, int a,
	int width, int height, int zoom)
{
	int i, j;

	for(j = 0; j < zoom; j++) {
		for(i = 0; i < zoom; i++) {
			data[(i+j*width*4)] = r;
			data[(i+j*width*4+1)] = g;
			data[(i+j*width*4+2)] = b;
			data[(i+j*width*4+3)] = a;
		}
	}
}


void scale4x(unsigned char *data, int width, int height)
{
	int i, j;
	int r, g, b, a;
	unsigned char buf[width*4*height*4*4];

/* FIXME to test */

	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			r = data[(i+j*width)*4];
			g = data[(i+j*width)*4+1];
			b = data[(i+j*width)*4+2];
			a = data[(i+j*width)*4+3];
			scalePixel(buf+(i+j*width*4)*4, 
				r, g, b, a, width*4, height*4, 4);	
		}
	}

	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			data[(i+j*width)*4] = buf[(i+j*width*4)*4];
			data[(i+j*width)*4+1] = buf[(i+j*width*4)*4+1];
			data[(i+j*width)*4+2] = buf[(i+j*width*4)*4+2];
			data[(i+j*width)*4+3] = buf[(i+j*width*4)*4+3];
		}
	}
}


void textureLoadWithFilename(const char *filename, GLuint *texturename) 
{
	unsigned char *image_data;

	/* FIXME: test mime instead */	
	if(strcasestr(filename, ".png") != NULL) {
		image_data = readPNG(filename);
	} else {
		image_data = readJPEG(filename);
	}

	powerOfTwoScaling(&image_data, &tex_width, &tex_height);
// scale4x(&image_data, tex_width, tex_height);

	if(image_data != NULL) {
		glBindTexture(GL_TEXTURE_2D, (*texturename));
		glTexParameteri(GL_TEXTURE_2D, 
			GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, 
			GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 
			0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

		free(image_data);

		printf("SUCCESS: loadTextureWithFilename():" 
			" %s\n", filename);		
	}
}


void textureInit(GLuint *textures, int nbtex)
{
	glGenTextures(nbtex, textures);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexturesize);
	printf("INFO: MAX TEXTURE SIZE: %dx%d\n", 
		maxtexturesize, maxtexturesize);
}


void textureDestroy(GLuint *textures, int nbtex)
{
	glDeleteTextures(nbtex, textures);
}
