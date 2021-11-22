
// MFCApplication3-BMPView.cpp: CMFCApplication3BMPView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCApplication3-BMP.h"
#endif

#include "MFCApplication3-BMPDoc.h"
#include "MFCApplication3-BMPView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication3BMPView

IMPLEMENT_DYNCREATE(CMFCApplication3BMPView, CView)

BEGIN_MESSAGE_MAP(CMFCApplication3BMPView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CMFCApplication3BMPView::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_AS, &CMFCApplication3BMPView::OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, &CMFCApplication3BMPView::OnFileSave)
END_MESSAGE_MAP()


// 显示 图像指针 图像宽度 图像长度 绘制位置x y
void CMFCApplication3BMPView::printBmp_24(CDC* pDC, BYTE* p, int w, int h, int x, int y)
{
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			UINT8* head = &p[j * 3 + 3 * i * w];
			pDC->SetPixelV(x + j, h + y - i - 1, RGB(head[2], head[1], head[0]));
		}
	}
}

// 显示 图像指针 图像宽度 图像长度 绘制位置x y
void CMFCApplication3BMPView::printBmp_8(CDC* pDC, BYTE* p, int w, int h, int x , int y)
{
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			BYTE index = p[j + i * w];
			//4 * w + x, h - y - 1,
			pDC->SetPixelV(x + j, h + y - i - 1, RGB(index, index, index));
		}
	}
}

// CMFCApplication3BMPView 构造/析构
CMFCApplication3BMPView::CMFCApplication3BMPView() noexcept
{
	// TODO: 在此处添加构造代码
	dib = NULL;
}

CMFCApplication3BMPView::~CMFCApplication3BMPView()
{
	if (dib == NULL) {
		delete dib;
		dib = NULL;
	}
}

BOOL CMFCApplication3BMPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCApplication3BMPView 绘图


void CMFCApplication3BMPView::OnDraw(CDC* pDC)
{
	CMFCApplication3BMPDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	if (dib == NULL) {
		return;
	}
	/* Coded by qi 2021.11.22
	 * 24位图像实现 */
	if (dib->bih->biBitCount == 24) {
		int size = dib->bwidth * dib->bheight;
		// 绘制原图
		printBmp_24(pDC, dib->ph, dib->bwidth, dib->bheight, 0, 0);
		// 傅里叶变换尝试
		fftw_complex *in, *out;
		in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size * 3);
		out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size * 3);
		// 将值放入in中
		for (int i = 0; i < 3 * size; i++) {
			in[i][0] = dib->ph[i];
		}
		fftw_plan p = fftw_plan_dft_3d(dib->bheight, dib->bwidth, 3, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(p);

		// fft3 shift
		int w = dib->bwidth, h = dib->bheight;
		int hc = h >> 1, wc = w >> 1;
		int x, y;
		fftw_complex* out_shift = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size * 3);
		for (int k = 0; k < 3; k++) {
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					(i < hc) ? (y = i + hc) : (y = i - hc);
					(j < wc) ? (x = j + wc) : (x = j - wc);
					memcpy(out_shift[3 * j + k + 3 * i * w], out[3 * x + k + 3 * y * w], sizeof(fftw_complex));
				}
			}
		}
		
		// 归一
		BYTE* mag3 = (BYTE*)fftw_malloc(sizeof(BYTE) * size * 3);
		for (int i = 0; i < 3 * size; i++) {
			double tmp = sqrt(pow(out_shift[i][0], 2) + pow(out_shift[i][1], 2));
			tmp = 20 * log(tmp);
			if (tmp > 255) tmp = 255;
			mag3[i] = (BYTE)tmp;
		}
		printBmp_24(pDC, mag3, dib->bwidth, dib->bheight, 0, 0);
		
		// 滤波
		int lc = 30 >> 1;
		for (int k = 0; k < 3; k++) {
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					if (((i >= hc - lc) && (i <= hc + lc))
						&& ((j >= wc - lc) && (j <= wc + lc))) continue;
					memset(out_shift[3 * j + k + 3 * i * w], 0, sizeof(fftw_complex));
				}
			}
		}
		for (int i = 0; i < 3 * size; i++) {
			double tmp = sqrt(pow(out_shift[i][0], 2) + pow(out_shift[i][1], 2));
			tmp = 20 * log(tmp);
			if (tmp > 255) tmp = 255;
			mag3[i] = (BYTE)tmp;
		}
		printBmp_24(pDC, mag3, dib->bwidth, dib->bheight, 0, 0);


		// 傅里叶逆变换
		fftw_complex* fout= (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size * 3);
		fftw_plan ip = fftw_plan_dft_3d(dib->bheight, dib->bwidth, 3, out_shift, fout, FFTW_BACKWARD, FFTW_ESTIMATE);
		fftw_execute(ip);
		for (int i = 0; i < 3 * size; i++) {
			double tmp = sqrt(pow(fout[i][0], 2) + pow(fout[i][1], 2));
			tmp = tmp / size;
			mag3[i] = (BYTE)tmp;
		}
		printBmp_24(pDC, mag3, dib->bwidth, dib->bheight, 0, 0);
		return;
	}


	/* Coded by qi
	 * 8位图像实现 */

	// 原图绘制
	printBmp_8(pDC, dib->ph, dib->bwidth, dib->bheight, 10, 10);

	// 输出傅里叶变换结果
	int size = dib->bheight * dib->bwidth; // 图像大小
	fftw_complex* in, * out;
	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
	dib->FDFT(in, out);
	// 这里需要进行fftshift才能正常显示最终的傅里叶图像,重新开辟一个区域进行
	fftw_complex *out_shift = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);

	dib->DFTShift(out, out_shift);
	
	// 这里转换成图像格式
	BYTE* mag = (BYTE*)malloc(sizeof(BYTE) * size);
	dib->Magnitude(out_shift, mag);
	// 显示变换后的频谱图
	printBmp_8(pDC, mag,
		dib->bwidth, dib->bheight, 
		20+dib->bwidth, 10);

	// 这里进行傅里叶反变换
	fftw_complex *rout = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
	dib->FIDFT(out, rout);
	BYTE* new_img = (BYTE*)malloc(sizeof(BYTE) * size);
	// 显示重新变化生成的图像
	for (int i = 0; i < size; i++) {
		new_img[i] = rout[i][0] / size;
	}
	printBmp_8(pDC, new_img,
		dib->bwidth, dib->bheight,
		30 + 2*dib->bwidth, 10);

	// 下面进行滤波， 这里按照矩形进行截取out_shift
	// 滤波说明：当最后一位为0，对应高通滤波， 最后一位为1时对应低通滤波。
	dib->RectFilter(out_shift,20, 1);  

	// 显示滤波效果
	BYTE* fmag = (BYTE*)malloc(sizeof(BYTE) * size);
	dib->Magnitude(out_shift, fmag);
	printBmp_8(pDC, fmag,
		dib->bwidth, dib->bheight,
		40 + 3 * dib->bwidth, 10);

	// 滤波后变换回来 fout
	fftw_complex* fout = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
	dib->DFTShift(out_shift, fout);

	dib->getExtVal();
	// 进行反变换rfout
	fftw_complex* rfout = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
	dib->FIDFT(fout, rfout);

	// 重新生成图形fnew_img
	BYTE* fnew_img = (BYTE*)malloc(sizeof(BYTE) * size);
	//dib->Magnitude(rfout, fnew_img);
	for (int i = 0; i < size; i++) {
		double tmp = sqrt(pow(rfout[i][0], 2) + pow(rfout[i][1], 2));
		fnew_img[i] = tmp / size;
	}
	// 显示重新变化生成的图像
	printBmp_8(pDC, fnew_img,
		dib->bwidth, dib->bheight,                    
		50 + 4 * dib->bwidth, 10);
}


// CMFCApplication3BMPView 打印

BOOL CMFCApplication3BMPView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCApplication3BMPView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCApplication3BMPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMFCApplication3BMPView 诊断

#ifdef _DEBUG
void CMFCApplication3BMPView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication3BMPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication3BMPDoc* CMFCApplication3BMPView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3BMPDoc)));
	return (CMFCApplication3BMPDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplication3BMPView 消息处理程序

void CMFCApplication3BMPView::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	if (dib) {
		delete dib;
	}
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();

		dib = new DIB;
		dib->read(filename);
	}

	Invalidate();
}


void CMFCApplication3BMPView::OnFileSaveAs()
{
	// TODO: 在此添加命令处理程序代码
	if (dib == NULL) {
		return;
	}
	CFileDialog dlg(FALSE, _T("bmp"), _T(".bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("(*.bmp)|*.bmp||"));
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		dib->write(filename);
	}
}

void CMFCApplication3BMPView::OnFileSave()
{
	// TODO: 在此添加命令处理程序代码
	if (dib == NULL) {
		return;
	}

	CFileDialog dlg(FALSE, _T("bmp"), _T(".bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("(*.bmp)|*.bmp||"));
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		dib->write(filename);
	}
}


