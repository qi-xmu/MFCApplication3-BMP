
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

	int h = dib->bheight;
	int w = dib->bwidth;
	BYTE* ph = dib->ph;
	double* kernel = dib->conv3_3; // 卷积核

	/* Coded by qi 2021.11.22
	 * 24位图像实现 */
	if (dib->bih->biBitCount == 24) {
		// 原图绘制
		printBmp_24(pDC, ph, w, h, 0, 0);

		BYTE* nip = (BYTE*)malloc(3 * w * h * sizeof(UINT));
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				for (int k = 0; k < 3; k++) {
					nip[k + 3 * j + i * w * 3] = (BYTE)dib->conv(i, k + 3 * j, kernel, 3);
				}
			}
		}
		printBmp_24(pDC, nip, w, h, w, 0);
	}
	/* Coded by qi
	 * 8位图像实现 */
	if (dib->bih->biBitCount == 8) {
		// 原图绘制
		printBmp_8(pDC, ph, w, h, 0, 0);
		
		dib->gauConvKerlGen(1.5);
		// 高斯滤波
		BYTE* nip = (BYTE*)malloc(w * h * sizeof(UINT));
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				// 卷积核卷积
				nip[j + i * w] = (BYTE)dib->conv(i, j, kernel, 1);
			}
		}
		printBmp_8(pDC, nip, w, h, w, 0);

		// 中值滤波
		BYTE* mip3 = (BYTE*)malloc(w * h * sizeof(UINT));
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				// 卷积核卷积
				mip3[j + i * w] = (BYTE)dib->median(i, j, 3, 1);
			}
		}
		printBmp_8(pDC, mip3, w, h, 2*w, 0);

		BYTE* mip5 = (BYTE*)malloc(w * h * sizeof(UINT));
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				// 卷积核卷积
				mip5[j + i * w] = (BYTE)dib->median(i, j, 5, 1);
			}
		}
		printBmp_8(pDC, mip5, w, h, 3*w, 0);

		//dib->ph = nip;
		//delete ph;
	}

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


