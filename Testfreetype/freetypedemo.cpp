/* 向JPEG图片中插入图片 */

#include <stdio.h>
#include <stdlib.h>
#include "freetypedemo.h"
#include "bmp_utils.h"
#include "jpeg_utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#pragma comment(lib,"freetype.lib")
#pragma comment(lib,"libjpeg.lib")
#include <locale.h>
/* Replace this function with something useful. */

//_CRT_SECURE_NO_WARNINGS     unsafe提示时可以在预处理器中加入这些
FT_Library    library;
FT_Face       face;
int Afontsize;
typedef struct T_color
{
	char R;
	char G;
	char B;
}TEXTCOLOR;
TEXTCOLOR   textcol;
int FT_INIT(char* filename, int fontsize, TEXTCOLOR* tcolor)
{

	FT_Matrix     matrix;                 /* transformation matrix */
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Error      error;
	double        angle;
	int           target_height = 0;
	int           n, num_chars;
	textcol.R = tcolor->R;
	textcol.G = tcolor->G;
	textcol.B = tcolor->B;
	angle = (0.0 / 360) * 3.14159 * 2;      /* use 0 degrees  旋转     */
	setlocale(LC_ALL, "chs");
	error = FT_Init_FreeType(&library);              /* initialize library */
	error = FT_New_Face(library, filename, 0, &face);/* create face object */
	/* use 50pt at 100dpi */
	Afontsize = fontsize;
	error = FT_Set_Char_Size(face, fontsize* 64, 0,300, 0);                /* set character size */
	/* error handling omitted */
	/* set up matrix */
	matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);
	pen.x = 0 * 64;
	pen.y = (target_height - 40) * 64;
	FT_Set_Transform(face, &matrix, &pen);
	return  0;
}
//释放字库
void FT_Deinit()
{

	if (face != NULL)
	{
		FT_Done_Face(face);
		face = NULL;
	}
	if (library != NULL)
	{
		FT_Done_FreeType(library);
		library = NULL;
	}
}


//插入单个字符
int FT_AddText(wchar_t  text, FT_Face face, unsigned char* imgbuf, int* widthLen, int x, int y, int width, int height)
{
	FT_Error      error;
	int i, j, MaxPoint, top;
	int upperBound;
	int lowerBound;
	FT_Glyph glyph=NULL;
	error = FT_Load_Char(face, text, FT_LOAD_DEFAULT);
	error = FT_Get_Glyph(face->glyph, &glyph);
	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
	FT_Bitmap  *bitmap = &bitmap_glyph->bitmap;
	*widthLen = bitmap->width;
	if (error)
		return -1;
	if ((bitmap->rows < 1) || (bitmap->width < 1))
	{
		return -1;
	}
	//top = bitmap->rows - bitmap_glyph->top;
	for (i = 0; i< bitmap->rows; i++)
	{
		for (j = 0; j<bitmap->width; j++)
		{

			if (bitmap->buffer[i*bitmap->width + j])
			{
				upperBound = ((Afontsize + y - bitmap->rows + i)*width + j + x) * 3;
				lowerBound = Afontsize + y - bitmap->rows + i;
				if ((upperBound>width*height * 3) || (lowerBound<0))
				{
					continue;
				}
				if (lowerBound< 0)
				{
					continue;
				}
				imgbuf[upperBound] = textcol.R;
				imgbuf[upperBound + 1] = textcol.G;
				imgbuf[upperBound + 2] = textcol.B;
			}
		}
	}
	FT_Done_Glyph(glyph);
	glyph = NULL;
	return 0;
}


//插入一行字符
int FT_AddString(wchar_t*  text, FT_Face face, unsigned char* imgbuf, int x, int y, int width, int height)
{
	int i, wlen, allwidth, textwidth;
	if (text == NULL)
	{
		return -1;
	}
	wlen = wcslen(text);
	allwidth = 0;
	for (i = 0; i < wlen; i++)
	{
		FT_AddText(text[i], face, imgbuf, &textwidth, x + allwidth, y, width, height);
		allwidth += textwidth + textwidth / 6;
	}
}

int ImgWriteText(const char* in_jpg_file, const char* out_jpg_file, char* insertString, int x, int y)
{
	unsigned char* buffer = NULL;
	int size;
	int width, height;
	FILE* fp1;
	unsigned char* jpg_buffer = NULL;
	int jpg_size = 0;
	fopen_s(&fp1, in_jpg_file, "rb");
	if (fp1 == NULL)
	{
		return -1;
	}
	fseek(fp1, 0, SEEK_END);	// 文件尾
	jpg_size = ftell(fp1);
	fseek(fp1, 0, SEEK_SET);	// 指回文件头
	jpg_buffer = (unsigned char *)malloc(sizeof(char)* jpg_size);
	if (jpg_buffer == NULL)
	{
		return -1;
	}
	memset(jpg_buffer, 0, jpg_size);
	fread(jpg_buffer, 1, jpg_size, fp1);
	fclose(fp1);
	read_jpg_buffer(jpg_buffer, jpg_size, &buffer, &size, &width, &height);
	if (buffer == NULL)
	{
		return -1;
	}
	/******************************/
	char *val1, *val2, *buf;
	char seps[2] = "\n";
	char *pc = new char[strlen(insertString) + 1];
	strcpy_s(pc, strlen(insertString) + 1, insertString);
	val1 = strtok_s(pc, seps, &buf);

	wchar_t  wc[100];
	size_t cSize;
	int ii = 0;
	while (1)
	{
		cSize = strlen(val1) + 1;
		if (cSize - 2 >= 0)
		{
			if (val1[cSize - 2] == '\r')
			{
				val1[cSize - 2] =0;
				//cSize = cSize - 2;
			}
		}
		size_t wlen = 0;
		mbstowcs_s(&wlen,wc, cSize,val1,_TRUNCATE);
		
		FT_AddString(wc, face, buffer, x, y + (Afontsize * 4)*ii, width, height);
		val1 = strtok_s(NULL, seps, &buf);
		if (val1 == NULL)
		{
			break;
		}
		ii++;
	}
	delete[]pc;
	/************************************/
	write_jpg_file(out_jpg_file, buffer, width, height, 50);
	free(buffer);
	if (jpg_buffer != NULL)
	{
		free(jpg_buffer);
	}
	return 0;
}

int jpegStringAddText(const unsigned char* inputJpgString,int inputJpgSize, const char* outjpgfile, char* insertString, int x, int y)
{
	unsigned char* buffer = NULL;
	int width, height,size;
	read_jpg_buffer((unsigned char*)inputJpgString,inputJpgSize, &buffer, &size, &width, &height);
	if (buffer == NULL)
	{
		return -1;
	}
	/******************************/
	char *val1, *val2, *buf;
	char seps[2] = "\n";
	char *pc = new char[strlen(insertString) + 1];
	strcpy_s(pc, strlen(insertString) + 1, insertString);
	val1 = strtok_s(pc, seps, &buf);

	wchar_t  wc[100];
	size_t cSize;
	int ii = 0;
	while (1)
	{
		cSize = strlen(val1) + 1;
		if (cSize - 2 >= 0)
		{
			if (val1[cSize - 2] == '\r')
			{
				val1[cSize - 2] =0;
				//cSize = cSize - 2;
			}
		}
		size_t wlen = 0;
		mbstowcs_s(&wlen,wc, cSize,val1,_TRUNCATE);
		
		FT_AddString(wc, face, buffer, x, y + (Afontsize * 4)*ii, width, height);
		val1 = strtok_s(NULL, seps, &buf);
		if (val1 == NULL)
		{
			break;
		}
		ii++;
	}
	delete[]pc;
	write_jpg_file(outjpgfile, buffer, width, height, 50);
	/************************************/
	free(buffer);
	return 0;
}




int main()
{
	char*  text = "这是FREETYPE_DEMO\r\n123\r\n789\r\n";
	FILE* fp;
	fp = fopen("1.jpg", "r");
	if (!fp)
	{
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	int jpgSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* jpgbuf = (char*)malloc(jpgSize);
	fread(jpgbuf,jpgSize,1,fp);
	fclose(fp);
	
	TEXTCOLOR color;
	color.R = 255;
	color.G = 0;
	color.B = 0;
	FT_INIT("simhei.ttf",10, &color);
	ImgWriteText("1.jpg", "2.jpg", text,50,50);
	jpegStringAddText((const unsigned char*)jpgbuf, jpgSize, "3.jpg", text, 50, 200);
	free(jpgbuf);
	FT_Deinit();
}

