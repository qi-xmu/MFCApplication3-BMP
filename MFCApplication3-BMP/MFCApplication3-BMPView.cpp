
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
	BYTE* ph = dib->ph;
	UINT16 h = dib->bheight; //图片高度
	UINT16 w = dib->bwidth;  //图片宽度
	DOUBLE arr[256] = { 0.0 };

	int start_x = w+50;
	int height = 240, width = 3;

	int iw24 = 3 * w;
	int iw8 = w;

	switch (dib->bih->biBitCount)
	{
	case 24: // 24位
		iw24 = iw24 + 3;
		iw24 -= iw24 % 4; // 换算
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				// 指针指向一个色彩的头部*4
				BYTE* pixel = (ph + 3 * x +  y * iw24);
				pDC->SetPixelV(x, h - y - 1,
					RGB(pixel[2], pixel[1], pixel[0]));
			}
		}
		break;
	case 8: // 8位 256色
		iw8 += 3;
		iw8 -= iw8 % 4; // window读取按照4字节读取，这里进行一个简单换算
		// 原图绘制
		dib->getExtVal(arr);
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				UINT8 index = *(UINT8*)(ph + x + y * iw8);
				RGBQuad* pix = &dib->quad[index]; // 读取一个像素
				pDC->SetPixelV(x,h - y - 1,
					RGB(pix->rgbRed, pix->rgbGreen, pix->rgbBlue));
			}
		}
		break;
	case 4: // 4位 16色
		w /= 2;
		w = w + 3;
		w -= w % 4; // 转化成4的倍数
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < dib->bwidth / 2; x++) {
				UINT8 index = *(UINT8*)(ph + x + y * w);
				// 将一个八位分成两部分读取，从高位向地位读取。
				RGBQuad* pix0 = &dib->quad[(index & 0xf0) >> 4];
				RGBQuad* pix1 = &dib->quad[index & 0x0f];
				pDC->SetPixelV(
					2 * x,
					h - y - 1,
					RGB(pix0->rgbRed, pix0->rgbGreen, pix0->rgbBlue)
				);
				pDC->SetPixelV(
					2 * x + 1,
					h - y - 1,
					RGB(pix1->rgbRed, pix1->rgbGreen, pix1->rgbBlue)
				);
			}
		}
		break;
	case 1: // 1位
		w /= 8;
		w += 3;
		w -= w % 4; // 转化成4的倍数
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < dib->bwidth / 8; x++) {
				UINT8 index = *(UINT8*)(ph + x + w * y);
				// 一个字节，从地位向高位读取，渲染的时候反过来渲染。
				for (int k = 0; k < 8; k++) {
					UINT8 bit = (index & (1 << k)) >> k; // 读取一个bit
					RGBQuad* pix = &dib->quad[bit];
					pDC->SetPixelV(
						8 * x + 8 - k,
						h - y - 1,
						RGB(pix->rgbRed, pix->rgbGreen, pix->rgbBlue)
					);
				}
			}
		}
		break;
	default:
		break;
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
		dib = NULL;
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


