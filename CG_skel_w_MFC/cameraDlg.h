#pragma once


// cameraDlg dialog

class cameraDlg : public CDialogEx
{
	DECLARE_DYNAMIC(cameraDlg)

public:
	cameraDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~cameraDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
