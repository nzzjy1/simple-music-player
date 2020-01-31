
// MFCApplication3Dlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MFCApplication3.h"
#include "MFCApplication3Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication3Dlg 对话框



CMFCApplication3Dlg::CMFCApplication3Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION3_DIALOG, pParent)
	, m_nVolume(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_DeviceID = 0;
}

void CMFCApplication3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
	DDX_Text(pDX, IDC_EDIT1, m_nVolume);
}

BEGIN_MESSAGE_MAP(CMFCApplication3Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(open, &CMFCApplication3Dlg::OnBnClickedopen)
	ON_BN_CLICKED(play, &CMFCApplication3Dlg::OnBnClickedplay)
	ON_BN_CLICKED(pause, &CMFCApplication3Dlg::OnBnClickedpause)
	ON_BN_CLICKED(end, &CMFCApplication3Dlg::OnBnClickedend)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMFCApplication3Dlg::OnCustomdrawSlider1)
END_MESSAGE_MAP()


// CMFCApplication3Dlg 消息处理程序

BOOL CMFCApplication3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_slider.SetRange(0, 1000);
	m_slider.SetPos(1000);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		//获取客户区大小
		CRect rect;
		GetClientRect(&rect);
		//
		dc.FillSolidRect(&rect,RGB(123, 104, 238));
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication3Dlg::OnBnClickedopen()
{
	//MessageBox(L"打开了", L"试调", MB_OK);
	CFileDialog dlg(TRUE,NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,L"MP3文件(*.mp3)|*.mp3||，L"全部文件(*.all)");
	if (IDCANCEL == dlg.DoModal())
		return;

	//获取路径
	CString strMusicFile = dlg.GetPathName();
	//打开
	MCI_OPEN_PARMS mciOpenParams;
	mciOpenParams.lpstrElementName = strMusicFile;
	MCIERROR mciErro = mciSendCommand(NULL , MCI_OPEN,MCI_OPEN_ELEMENT|MCI_WAIT,(DWORD)&mciOpenParams);
	if (mciErro)
	{
		wchar_t szErrorMsg[256];
		//获取错误信息
		mciGetErrorString(mciErro, szErrorMsg, 256);
		MessageBox(szErrorMsg, L"音乐播放器");
			return;
	}

	m_DeviceID = mciOpenParams.wDeviceID;
}


void CMFCApplication3Dlg::OnBnClickedplay()
{
	MCI_PLAY_PARMS mciPlayParms;
	mciPlayParms.dwCallback = NULL;
	mciPlayParms.dwFrom = 0;

	mciSendCommand(m_DeviceID, MCI_PLAY, MCI_FROM | MCI_NOTIFY, (DWORD)&mciPlayParms);
}


void CMFCApplication3Dlg::OnBnClickedpause()
{
	CString str;
	GetDlgItemText(pause, str);

	if (str == L"暂停")
	{
		mciSendCommand(m_DeviceID, MCI_PAUSE, 0, 0);
		SetDlgItemText(pause, L"继续");

	}
	else if (str == "继续")
	{
		mciSendCommand(m_DeviceID, MCI_RESUME, 0, 0);
		SetDlgItemText(pause, L"暂停");
	}
}


void CMFCApplication3Dlg::OnBnClickedend()
{
	mciSendCommand(m_DeviceID, MCI_STOP, 0, 0);
	mciSendCommand(m_DeviceID, MCI_CLOSE, 0, 0);
}


void CMFCApplication3Dlg::OnCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	//获取当前滑动控件位置
	m_nVolume = m_slider.GetPos()/10;
	UpdateData(FALSE);
	
	//发送命令
	MCI_DGV_SETAUDIO_PARMS mciSetVolume;
	mciSetVolume.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciSetVolume.dwValue = m_slider.GetPos();
	mciSendCommand(m_DeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD)&mciSetVolume);
	*pResult = 0;
}
