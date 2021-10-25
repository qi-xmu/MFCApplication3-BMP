// CImageZFTDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication3-BMP.h"
#include "CImageZFTDlg.h"
#include "afxdialogex.h"


// CImageZFTDlg 对话框

IMPLEMENT_DYNAMIC(CImageZFTDlg, CDialogEx)

CImageZFTDlg::CImageZFTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ZFT, pParent)
{

}

CImageZFTDlg::~CImageZFTDlg()
{
}

void CImageZFTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImageZFTDlg, CDialogEx)
ON_WM_CREATE()
END_MESSAGE_MAP()



int CImageZFTDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	return 0;
}


