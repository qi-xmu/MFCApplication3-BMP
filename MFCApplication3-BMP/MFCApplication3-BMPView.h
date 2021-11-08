
// MFCApplication3-BMPView.h: CMFCApplication3BMPView 类的接口
//

#pragma once

#include "DIB.h"
#include "CImageZFTDlg.h"
#include "fftw3.h"
#include <cmath>

class CMFCApplication3BMPView : public CView
{
private:
	DIB *dib;

public:
	// dft
	void FDFT(unsigned char* channel, DIB::complex* output);
	void FDFT(unsigned char* channel, DIB::complex* output), int limit);
	void FIDFT(DIB::complex* input, unsigned cahr* channel);





protected: // 仅从序列化创建
	CMFCApplication3BMPView() noexcept;
	DECLARE_DYNCREATE(CMFCApplication3BMPView)

// 特性
public:
	CMFCApplication3BMPDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCApplication3BMPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
private:

};

#ifndef _DEBUG  // MFCApplication3-BMPView.cpp 中的调试版本
inline CMFCApplication3BMPDoc* CMFCApplication3BMPView::GetDocument() const
   { return reinterpret_cast<CMFCApplication3BMPDoc*>(m_pDocument); }
#endif

