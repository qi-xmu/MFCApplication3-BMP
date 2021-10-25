#pragma once
#pragma pack(2) // 对齐
struct BMPFileHeader {
	UINT16 bfType;		 // 说明位图类型  2字节
	DWORD bfSize;		 // 说明位图大小  4字节
	UINT16 bfReserved1;  // 保留字，必须为0  2字节
	UINT16 bfReserved2;  // 保留字，必须为0  2字节
	DWORD bfOffBits;	 // 从文件头到实际的图像数据的偏移量是多少  4字节
};

struct BMPInfoHeader {
	DWORD biSize;		// 说明该结构一共需要的字节数 2字节
	LONG biWidth;		// 说明图片的宽度，以像素为单位 4字节
	LONG biHeight;		// 说明图片的高度，以像素为单位 4字节
	WORD biPlanes;		// 颜色板，总是设为1  2个字节
	WORD biBitCount;		// 说明每个比特占多少bit位，可以通过这个字段知道图片类型  2个字节
	DWORD biCompression;	// 说明使用的压缩算法 2个字节 （BMP无压缩算法）
	DWORD biSizeImage;		// 说明图像大小   2个字节
	LONG biXPelsPerMeter;	// 水平分辨率 4字节  单位：像素/米
	LONG biYPelsPerMeter;	// 垂直分辨率4字节
	DWORD biClrUsed;		// 说明位图使用的颜色索引数 4字节
	DWORD biClrImportant;	// 4字节
};

struct RGBQuad {
	BYTE    rgbBlue;		// 蓝色 1字节
	BYTE    rgbGreen;		//绿色 1字节
	BYTE    rgbRed;			// 红色 1字节
	BYTE    rgbReserved;	// 保留字 1字节
};

// DIB类
class DIB
{
public:
	BMPFileHeader* bfh;		// 文件头
	BMPInfoHeader* bih;		// 信息头
	RGBQuad* quad;			// 调色板
	unsigned char *bdata;	// 标记
	int bwidth;		// 宽
	int bheight;	// 高
	int real_size;	// 真实大小

public:
	DIB();	// 构造函数
	~DIB(); // 析构函数

	void read(const CString& fileName);		// 读取位图文件
	void write(const CString& fileName);	// 写入位图文件
};

