#pragma once


// CImageZFTDlg 对话框

class CImageZFTDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CImageZFTDlg)

public:
	CImageZFTDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CImageZFTDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ZFT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
protected:

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	int init_value = 0;
};
