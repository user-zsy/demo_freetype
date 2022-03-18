/* 向JPEG图片中插入图片 */

#include <stdio.h>
#include "freetypedemo.h"
#include "bmp_utils.h"
#include "jpeg_utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma comment(lib,"freetype.lib")
#pragma comment(lib,"libjpeg.lib")
/* Replace this function with something useful. */


FT_Library    library;
FT_Face       face;
FT_GlyphSlot  slot;
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
	error = FT_Init_FreeType(&library);              /* initialize library */
	error = FT_New_Face(library, filename, 0, &face);/* create face object */
	/* use 50pt at 100dpi */
	error = FT_Set_Char_Size(face, fontsize* 64, 0,200, 0);                /* set character size */
	/* error handling omitted */
	slot = face->glyph;
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

//插入单个字符
int FT_AddText(wchar_t  text, FT_Face face, FT_Bitmap*  bitmap, unsigned char* imgbuf, int* widthLen, int x, int y, int width, int height)
{
	FT_Error      error;
	int i, j, MaxPoint;
	error = FT_Load_Char(face, text, FT_LOAD_RENDER);
	*widthLen = bitmap->width;
	if (error)
		return -1;
	for (i = 0; i <bitmap->rows; i++)
	{
		for (j = 0; j < bitmap->width; j++)
		{
			if (bitmap->buffer[i*bitmap->width + j])
			{   
				if ((((i + y)*width + j + x) * 3 + 2)>width*height * 3)
				{
					continue;
				}
				imgbuf[((i + y)*width + j + x) * 3] = textcol.R;
				imgbuf[((i + y)*width + j + x) * 3 + 1] = textcol.G;
				imgbuf[((i + y)*width + j + x) * 3 + 2] = textcol.B;
			}

		}
	}
	return 0;
}


//插入一行字符
int FT_AddString(wchar_t*  text, FT_Face face, FT_Bitmap*  bitmap, unsigned char* imgbuf, int x, int y, int width, int height)
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
		FT_AddText(text[i], face, bitmap, imgbuf, &textwidth, x + allwidth, y, width, height);
		allwidth += textwidth + textwidth / 6;
	}
}

int ImgWriteText(const char* in_jpg_file, const char* out_jpg_file,wchar_t* insertString)
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
	FT_AddString(insertString, face, &slot->bitmap, buffer, 10, 20, width, height);
	write_jpg_file(out_jpg_file,buffer, width, height, 50);
	free(buffer);
	if (jpg_buffer != NULL)
	{
	free(jpg_buffer);
	}
	return 0;
}
int main()
{
	wchar_t*  text = L"这是FREETYPE_DEMO";
	TEXTCOLOR color;
	color.R = 0;
	color.G = 255;
	color.B = 0;
	FT_INIT("simhei.ttf",20, &color);
	ImgWriteText("1.jpg", "2.jpg", text);
}

