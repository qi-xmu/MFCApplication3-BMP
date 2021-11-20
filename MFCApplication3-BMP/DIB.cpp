#include "pch.h"
#include "DIB.h"



DIB::DIB() {
	real_size = 0;
	bwidth = 0;
	bheight = 0;
	maxp = 0;
	minp = 255;

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

// 计算最大最小值
void DIB::getExtVal()
{
	UINT16 h = bheight;
	UINT16 w = bwidth;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			UINT8 index = *(UINT8*)(ph + x + y * w);
			if (index > maxp) maxp = index;
			if (index < minp) minp = index;
		}
	}
}


// 离散傅里叶变换代码
void DIB::FDFT(fftw_complex* in, fftw_complex* out)
{
	int size = bheight * bwidth;
	for (int i = 0; i < size; i++) {
		in[i][0] = (double)ph[i];
	}
	fftw_plan p = fftw_plan_dft_2d(bheight, bwidth, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);
}

void DIB::FIDFT(fftw_complex* rin, fftw_complex* rout)
{
	int size = bheight * bwidth;
	fftw_plan p = fftw_plan_dft_2d(bheight, bwidth, rin, rout, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(p);
}

// fft shift 
void DIB::DFTShift(fftw_complex* out, fftw_complex* out_shift)
{
	int a = bheight >> 1;
	int b = bwidth >> 1;
	int x, y;
	for (int i = 0; i < bheight; i++) {
		for (int j = 0; j < bwidth; j++) {
			(i < a) ? (y = i + a) : (y = i - a);
			(j < b) ? (x = j + b) : (x = j - b);
			memcpy(out_shift[j + i * bwidth], out[x + y * bwidth], sizeof(fftw_complex));
		}
	}
}

void DIB::Magnitude(fftw_complex* out, BYTE* mag)
{
	DOUBLE dmag;
	for (int i = 0; i < bheight * bwidth; i++) {
		dmag = sqrt(pow(out[i][0], 2) + pow(out[i][1], 2));
		dmag = 20 * log(dmag);
		if (dmag > 255) dmag = 255;
		mag[i] = (BYTE)dmag;
	}
}

void DIB::RectFilter(fftw_complex* out, int len, int flag)
{
	int wc = bwidth >> 1;
	int hc = bheight >> 1;
	int lc = len >> 1;
	if (flag == 0) {
		// 高通过滤
		for (int i = hc - lc; i < hc + lc; i++) {
			for (int j = wc - lc; j < wc + lc; j++) {
				memset(out[j + i * bwidth], 0, sizeof(fftw_complex));
			}
		}
	}
	else {
		// 反向低通过滤
		for (int i = 0; i < bheight ; i++) {
			for (int j = 0; j < bwidth; j++) {
				if (((i >= hc - lc) && (i <= hc + lc)) 
					&& ((j >= wc - lc) && (j <= wc + lc))) continue;
				memset(out[j + i * bwidth], 0, sizeof(fftw_complex));
			}
		}
	}
}



