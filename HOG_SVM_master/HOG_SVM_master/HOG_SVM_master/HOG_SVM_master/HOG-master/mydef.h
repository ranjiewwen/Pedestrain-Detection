#include <stdio.h>
#include <string.h>
#include "vec.h"

#define MYDEF_INCLUDED
#define MYDEF_ERROR -10
#define IMAGE_TYPE_ERROR -20
#define DESCRIPTOR_SIZE_ERROR -30
#define DETECT_FUNC_ERROR -40
#define GET_BLOCK_FUNC_ERROR -50
#define BORDER_INTERPOLATE_ERROR -60

#define mymax(a,b) (a > b ? a : b)
#define mymin(a,b) (a < b ? a : b)

#define myCV_CN_MAX     512
#define myCV_CN_SHIFT   3
#define myCV_DEPTH_MAX  (1 << myCV_CN_SHIFT)

#define myCV_8U   0
#define myCV_8S   1
#define myCV_16U  2
#define myCV_16S  3
#define myCV_32S  4
#define myCV_32F  5
#define myCV_64F  6
#define myCV_PI 3.141592653589732384626433832795

#define myCV_MAT_DEPTH_MASK       (myCV_DEPTH_MAX - 1)
#define myCV_MAT_DEPTH(flags)     ((flags) & myCV_MAT_DEPTH_MASK)
#define myCV_MAKETYPE(depth,cn) (myCV_MAT_DEPTH(depth) + (((cn)-1) << myCV_CN_SHIFT))
#define myCV_8UC2 myCV_MAKETYPE(myCV_8U,2)
#define myCV_8UC3 myCV_MAKETYPE(myCV_8U,3)
#define myCV_32FC2 myCV_MAKETYPE(myCV_32F,2)
#define myCV_CN_MAX     512
#define myCV_MAT_CN_MASK          ((myCV_CN_MAX - 1) << myCV_CN_SHIFT)
#define myCV_MAT_CN(flags)        ((((flags) & myCV_MAT_CN_MASK) >> myCV_CN_SHIFT) + 1)
#define myCV_MAT_TYPE_MASK        (myCV_DEPTH_MAX*myCV_CN_MAX - 1)
#define myCV_MAT_TYPE(flags)      ((flags) & myCV_MAT_TYPE_MASK)
#define myCV_ELEM_SIZE(type) \
    (myCV_MAT_CN(type) << ((((sizeof(size_t)/4+1)*16384|0x3a50) >> myCV_MAT_DEPTH(type)*2) & 3))

#define myBORDER_REPLICATE 1
#define myBORDER_REFLECT 2
#define myBORDER_WRAP 3
#define myBORDER_CONSTANT 0
#define myBORDER_REFLECT_101 4
#define MAX_ESIZE 16
#define INTER_LINEAR 1
#define INTER_AREA 3

typedef unsigned char uchar; 
typedef signed char schar;
typedef unsigned short ushort;
typedef enum BOOLEAN { false, true } bool;

#define T uchar
#define WT int
#define AT short

typedef struct myRect
{
	int x,y,width,height;
}rect_ ;

typedef struct mySize
{
	int width,height;
}size_ ;

typedef struct myPoint
{
	int x,y;
}point_ ;

typedef struct BlockData 
{
	int histOfs;
	point_ imgOffset;  //histOfs和imgOffset.其中histOfs表示为该block对整个滑动窗口内hog描述算子的贡献那部分向量的起始位置；imgOffset为该block在滑动窗口图片中的坐标(当然是指左上角坐标)。
}BData_ ; 

typedef struct PixData
{
	size_t gradOfs, qangleOfs;
	int histOfs[4];
	float histWeights[4];
	float gradWeight;
}PData_; 
//其中gradOfs表示该点的梯度幅度在滑动窗口图片梯度幅度图中的位置坐标；qangleOfs表示该点的梯度角度在滑动窗口图片梯度角度图中的位置坐标；
//histOfs[]表示该像素点对1个或2个或4个cell贡献的hog描述子向量的起始位置坐标（比较抽象，需要看源码才懂）。histWeight[]表示该像素点对1个或2个或4个cell贡献的权重。
//gradWeight表示该点本身由于处在block中位置的不同因而对梯度直方图贡献也不同，其权值按照二维高斯分布(以block中心为二维高斯的中心)来决定。

typedef struct myAutoBuffer
{
	enum { buffer_padding = (int)((16+sizeof(int) - 1)/sizeof(int)) } buffer_pad;
	void* ptr;	
	size_t size;
}AB_ ;

typedef struct {
	union
	{
		int x;
		int cols;
		int width;
	};
	union
	{
		int y;
		int rows;
		int height;
	};
	union
	{
		int type;
		int depth;
	};
	union
	{
		uchar *data, *datastart;
		float *dataf, *datastartf;
	};
	union
	{
		uchar *dataend, *datalimit;
		float *dataendf, *datalimitf;
	};
	int step, dims, channels;
	size_t totalsize;
} myMat;

typedef struct myNAryMatIterator
{
	const myMat** arrays;
	myMat* planes;
	uchar** ptrs;
	int narrays;
	size_t nplanes;
	size_t size;
	int iterdepth;
	size_t idx;
}NMIt_ ;

typedef struct myHOGDescriptor
{
	size_ winSize;
	size_ blockSize;
	size_ blockStride;
	size_ cellSize;
	int nbins;
	int derivAperture;
	double winSigma;
	int histogramNormType;
	double L2HysThreshold;
	bool gammaCorrection;
	float *svmDetector;
	int nlevels;
	int svmSize;
}hog_ ;

typedef struct HOGCache
{
	bool useCache;
	PData_* pixData;
	BData_* blockData;
	int* ymaxCached;
	size_ winSize, cacheStride;
	size_ nblocks,ncells;
	int blockHistogramSize;
	int count1, count2, count4;
	point_ imgoffset;		
	myMat blockCache;	//float
	myMat blockCacheFlags;	//char
	myMat grad, qangle;	
}HCache_ ;

int myround (double val);

void nmitalc (NMIt_* obj, const myMat** arrays, uchar** ptrs);

void sizealc(size_ *szobj, int w, int h);

void ABalc (AB_ *abobj, size_t _size, void* _type, int type);

void hogalc (hog_ *obj);

void detectMultiScale(hog_* obj, myMat* img, rect_** foundLocations,double hitThreshold, 
					  size_ winStride, size_ padding,double scale0, double finalThreshold, bool useMeanshiftGrouping);

void parallel(int start, int end, hog_* obj, myMat* img, double hitThreshold, size_ winStride, size_ padding, double** levelScale, rect_** allCandidates, 
			  double** weights, double** scales);

void detect(hog_* obj, myMat* img, point_** hits, double** weights, double hitThreshold, size_ winStride, size_ padding);

double gcd(double a, double b);

__inline size_t myalignSize(size_t sz, int n);

__inline int clip(int x, int a, int b);

void cacheinit(hog_* descriptor, HCache_* cache, myMat* img, size_ paddingTL, size_ paddingBR, bool useCache, size_ cacheStride);

void computeGradient(hog_* obj, myMat* img, myMat* grad, myMat* qangle, size_ paddingTL, size_ paddingBR);

const float* getBlock(hog_* descriptor, HCache_* cache, point_ pt, float* buf);

void normalizeBlockHistogram(hog_* descriptor, HCache_* cache, float* _hist);

void groupRectangles(rect_** rectList, int groupThreshold, double eps, int* weights, double* levelWeights);

int mypartition(rect_* _vec, int** labels, double eps);

bool predicate(const rect_* r1, const rect_* r2, double eps);

int myborderInterpolate( int p, int len, int borderType );

void cartToPolar( myMat* src1, myMat* src2, myMat* dst1, myMat* dst2, bool angleInDegrees );

void myFastAtan2_32f(const float *Y, const float *X, float *angle, int len, bool angleInDegrees );

void myMagnitude_32f(const float* x, const float* y, float* mag, int len);

void veccopydouble(double** vs, double** vd);

void veccopyrect_(rect_** vs, rect_** vd);

void createMat(myMat* img, size_* sz, int _channels, int _type, void* refdata);

void resizeMat( myMat* _src, myMat* _dst, size_ dsize,
                 double inv_scale_x, double inv_scale_y, int interpolation );

void resizeGeneric_( myMat* src, myMat* dst, int* xofs, void* _alpha, int* yofs, void* _beta, int xmin, int xmax, int ksize );

void hresize(T** src, WT** dst, int count, int* xofs, AT* alpha, int swidth, int dwidth, int cn, int xmin, int xmax );

void vresize(WT** src, T* dst, AT* beta, int width );
