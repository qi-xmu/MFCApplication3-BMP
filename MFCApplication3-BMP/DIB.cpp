#include "pch.h"
#include "DIB.h"



DIB::DIB() {
	real_size = 0;
	bwidth = 0;
	bheight = 0;
	maxp = 0;
	minp = 0;
	threshold = 0;
	arr = new int[256];

	ph = NULL;
	bfh = NULL;
	bih = NULL;
	quad = NULL;
	bdata = NULL;
}
DIB::~DIB() {
	delete[] bdata;
	ph = NULL;
	bfh = NULL;
	bih = NULL;
	quad = NULL;
	bdata = NULL;
}

void DIB::read(const CString& fileName)
{
	// 文件读取
	CFile file;
	file.Open(fileName, CFile::modeRead);
	real_size = (DWORD)file.GetLength();
	bdata = new unsigned char[real_size];

	file.Read(bdata, real_size);
	file.Close();

	// 头信息
	bfh = (BMPFileHeader*)bdata;
	bih = (BMPInfoHeader*)(bdata + sizeof(BMPFileHeader));

	// 调色板
	quad = (RGBQuad*)(bdata + sizeof(BMPFileHeader) + sizeof(BMPInfoHeader));

	//图片高度 宽度
	bheight = bih->biHeight; 
	bwidth = bih->biWidth;

	//图片指针
	ph = (BYTE*)(bdata + bfh->bfOffBits);

}

void DIB::write(const CString& fileName)
{
	CFile new_file;
	new_file.Open(fileName, CFile::modeCreate | CFile::modeWrite);
	try {
		new_file.Write(bdata, real_size);
	}
	catch (CException* pe) {
		pe->Delete();
		AfxMessageBox(_T("写入错误！"));
		new_file.Close();
	}
	new_file.Close();
}

void DIB::equalizated()
{

}

void DIB::standardized()
{
}

void DIB::getExtVal()
{
	UINT16 h = bheight;
	UINT16 w = bwidth;
	maxp = 0;
	minp = 255;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			UINT8 index = *(UINT8*)(ph + x + y * w);
			//RGBQuad* pix = &dib->quad[index];
			//index = *(UINT8*)(pix);
			if (index > maxp) maxp = index;
			if (index < minp) minp = index;
			arr[index]++;
		}
	}
	// 归一化
	int max = arr[0], min = arr[0];
	for (int i = 1; i < 256; i++) {
		if (arr[i] > max) max = arr[i];
		if (arr[i] < min) min = arr[i];
	}
	double area = (double)max - min;
	for (int i = 0; i < 256; i++) {
		arr[i] = ((double)arr[i] - min) / area * 200;
	}
}