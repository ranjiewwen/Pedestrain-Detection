
#include "readbmp.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"

//typedef unsigned short WORD;
//typedef unsigned int DWORD;
//typedef int LONG;
//typedef struct tagBITMAPFILEHEADER {
//	WORD    bfType;
//	DWORD   bfSize;
//	WORD    bfReserved1;
//	WORD    bfReserved2;
//	DWORD   bfOffBits;
//} BITMAPFILEHEADER;
//typedef struct tagBITMAPINFOHEADER{
//	DWORD      biSize;
//	LONG        biWidth;
//	LONG        biHeight;
//	WORD       biPlanes;
//	WORD       biBitCount;
//	DWORD      biCompression;
//	DWORD      biSizeImage;
//	LONG        biXPelsPerMeter;
//	LONG        biYPelsPerMeter;
//	DWORD      biClrUsed;
//	DWORD      biClrImportant;
//} BITMAPINFOHEADER;


#include "windows.h"  //BITMAPFILEHEADER包含文件

myMat *loadBitmapFromFile24(const char *filePath, U8 **bits)     //24
{
	myMat *img;
	img = (myMat*)malloc(sizeof(myMat));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	FILE *fp = fopen(filePath, "rb");
	if (fp == NULL) {
		exit(-1);
	}
	BITMAPFILEHEADER bfh;
	if (fread(&bfh, sizeof(bfh), 1, fp) != 1) {
		fclose(fp);
		exit(-1);
	}
	BITMAPINFOHEADER bih;
	if (fread(&bih, sizeof(bih), 1, fp) != 1) {
		fclose(fp);
		exit(-1);
	}
	if (bih.biBitCount != 24) {
		fclose(fp);
		printf("unsupported bitmap format.\n");
		exit(-1);
	}
	int imageSize = (bih.biWidth*3 + 3) / 4 * 4 * bih.biHeight;

	img->width = bih.biWidth;
	img->height = bih.biHeight;
	img->type = myCV_8U;
	img->dims = 2;
	img->channels = 3;
	img->step = imageSize / bih.biHeight;
	img->totalsize = imageSize;
	img->data = (uchar*)malloc(img->totalsize);
	img->dataend = img->datalimit = (uchar*)img->data + img->totalsize;

	fseek(fp, bfh.bfOffBits - sizeof(bfh) - sizeof(bih), SEEK_CUR);  //138-54?感觉应该没有138才对啊

	if (fread(img->data, 1, imageSize, fp) != imageSize) {
		fclose(fp);
		exit(-1);
	}
	fclose(fp);

	U8 *buffer = (U8 *)malloc(imageSize);
	buffer = img->data;
	*bits = buffer;

	return img;
}

myMat *loadBitmapFromFile8(const char *filePath, U8 **bits)     //----8
{
	myMat *img;
	img = (myMat*)malloc(sizeof(myMat));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	FILE *fp = fopen(filePath, "rb");
	if (fp == NULL) {
		exit(-1);
	}
	BITMAPFILEHEADER bfh;
	if (fread(&bfh, sizeof(bfh), 1, fp) != 1) {
		fclose(fp);
		exit(-1);
	}
	BITMAPINFOHEADER bih;
	if (fread(&bih, sizeof(bih), 1, fp) != 1) {
		fclose(fp);
		exit(-1);
	}
	if (bih.biBitCount != 8) {
		fclose(fp);
		printf("unsupported bitmap format.\n");
		exit(-1);
	}
	int imageSize = (bih.biWidth + 3) / 4 * 4 * bih.biHeight;

	img->width = bih.biWidth;
	img->height = bih.biHeight;
	img->type = myCV_8U;
	img->dims = 2;
	img->channels = 1;
	img->step = imageSize / bih.biHeight;
	img->totalsize = imageSize ;
	img->data = (uchar*)malloc(img->totalsize);
	img->dataend = img->datalimit = (uchar*)img->data + img->totalsize;

	fseek(fp, bfh.bfOffBits - sizeof(bfh) - sizeof(bih), SEEK_CUR);

	U8 *buffer = (U8 *)malloc(imageSize );

	if (fread(img->data, 1, imageSize, fp) != imageSize) {
		fclose(fp);
		exit(-1);
	}
	
	fclose(fp);

	*bits = buffer;

	return img;
}

int CreateImage(IplImage_ *_img, int _width, int _height, U8 *_image_data)
{
	if (_img->valid)
	{
		return -1;	//这个图像结构体已经有用了
	}
	if (_width & 3)	//是否是4的整数倍
	{
		_img->width_step = (_width & (~3)) + 4;
	}
	else
	{
		_img->width_step = _width;
	}
	_img->width = _width;
	_img->height = _height;
	_img->image_data = _image_data;
	_img->valid = 1;
	return 0;
}

void ReleaseImage(IplImage_ *_img)
{
	_img->width_step = 0;
	_img->width = 0;
	_img->height = 0;
	_img->image_data = 0;
	_img->valid = 0;
}

void SaveGrayBitmap24(const char *fileName, const U8 *imageData, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	DWORD palette[256];
	int i, imageSize;
	FILE *fp = fopen(fileName, "wb");
	int widthS = (width*3 + 3) / 4 * 4;
	imageSize = widthS* height;

	if (fp == NULL)
	{
		return;
	}

	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 0x4d42;
	bfh.bfSize = imageSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	fwrite(&bfh, sizeof(bfh), 1, fp);

	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0;
	bih.biSizeImage = imageSize;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	fwrite(&bih, sizeof(bih), 1, fp);

	for (i = 0; i < height; i++)
	{
		fwrite(imageData + (height - i - 1)*widthS, 1, widthS, fp);
	}

	fclose(fp);
}

void SaveGrayBitmap8(const char *fileName, const U8 *imageData, int width, int height)  //8
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	DWORD palette[256];
	int i, imageSize;

	FILE *fp = fopen(fileName, "wb");
	int widthS = (width + 3) / 4 * 4;
	imageSize = widthS* height;

	if (fp == NULL)
	{
		return;
	}

	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 0x4d42;
	bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4 + imageSize;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	fwrite(&bfh, sizeof(bfh), 1, fp);

	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 8;
	bih.biCompression = 0;
	bih.biSizeImage = imageSize;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	fwrite(&bih, sizeof(bih), 1, fp);

	for (i = 0; i < 256; i++)
	{
		palette[i] = (i << 16) | (i << 8) | i;
	}
	fwrite(palette, sizeof(palette), 1, fp);

	for (i = 0; i < height; i++)
	{
		fwrite(imageData + (height - i - 1)*widthS, 1, widthS, fp);
	}

	fclose(fp);
}
