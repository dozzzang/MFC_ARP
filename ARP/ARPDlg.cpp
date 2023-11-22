
// ARPDlg.cpp : ���� ����
//
#include "pch.h"
#include "stdafx.h"
#include "ARP.h"
#include "ARPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()





CARPDlg::CARPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CARPDlg::IDD, pParent),
	CBaseLayer("ARPDlg")
	, m_ARP(nullptr)
	, m_Ether(nullptr)
	, m_NI(nullptr)
	, m_IP(nullptr)
{
	mEdit_GARPEther = _T("");
	mEdit_MyEther = _T("");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ARP = new CARPLayer("ARP");
	m_IP = new CIPLayer("Network");
	m_Ether = new CEthernetLayer("Ethernet");
	m_NI = new CNILayer("NI");

	if (m_ARP == nullptr || m_IP == nullptr || m_Ether == nullptr || m_NI == nullptr) {
		AfxMessageBox(_T("Fail : Layer Link"));
		return;
	}

	m_LayerMgr.AddLayer(m_ARP);
	m_LayerMgr.AddLayer(m_IP);
	m_LayerMgr.AddLayer(m_Ether);
	m_LayerMgr.AddLayer(m_NI);
	m_LayerMgr.AddLayer(this);
		
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *IP ( *ARPDlg  -ARP ) *ARP ) )");
}

void CARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// Control�� ������ �̾��ִ� �۾�
	
	// My Device
	DDX_Control(pDX, IDC_MYDEV_LIST, mCombo_MyDev);
	DDX_Text(pDX, IDC_MYDEV_ETHERNET, mEdit_MyEther);
	DDX_Control(pDX, IDC_MYDEV_IP, mIP_MyIP);

	// ARP Cache Table
	DDX_Control(pDX, IDC_ARP_CACHE_TABLE, mList_ARPCacheTable);
	DDX_Control(pDX, IDC_ARP_DSTIP, mIP_ARPDSTIP);

	// Proxy ARP Entry
	DDX_Control(pDX, IDC_PARP_ENTRY, mList_PARPEntry);

	// Gratuituos ARP
	DDX_Text(pDX, IDC_GARP_ETHERNET, mEdit_GARPEther);
}

BEGIN_MESSAGE_MAP(CARPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MYDEV_SELECT, &CARPDlg::OnBnClickedMydevSelect)
	ON_BN_CLICKED(IDC_ARP_SEND, &CARPDlg::OnBnClickedArpSend)
	ON_CBN_SELCHANGE(IDC_MYDEV_LIST, &CARPDlg::OnCbnSelchangeMydevList)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_MYDEV_IP, &CARPDlg::OnIpnFieldchangedMydevIp)
END_MESSAGE_MAP()


// CARPDlg �޽��� ó����

BOOL CARPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	SetDlgState(ARP_INITIALIZING);
	SetDlgState(ARP_DEVICE);
	
	// List Control�� 3���� Column �߰�
	// Arp Cache Table
	((CListCtrl*)GetDlgItem(IDC_ARP_CACHE_TABLE))->InsertColumn(0,"IP Address",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_ARP_CACHE_TABLE))->InsertColumn(1,"Ethernet Address",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_ARP_CACHE_TABLE))->InsertColumn(2,"Timer",LVCFMT_LEFT,50);
	((CListCtrl*)GetDlgItem(IDC_ARP_CACHE_TABLE))->InsertColumn(3,"Status",LVCFMT_LEFT,130);

	// Proxy Cache Table
	((CListCtrl*)GetDlgItem(IDC_PARP_ENTRY))->InsertColumn(0,"Device",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_PARP_ENTRY))->InsertColumn(1,"IP Address",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_PARP_ENTRY))->InsertColumn(2,"Ethernet Address",LVCFMT_LEFT,180);


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.s
}

void CARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CARPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ��� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CARPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CARPDlg::SetDlgState(int state)
{
	UpdateData( TRUE ) ;

	CString		device_description;

	CButton*	pMYDEV_SelectButton = (CButton*) GetDlgItem(IDC_MYDEV_SELECT);

	CButton*	pARP_ItemDeleteButton = (CButton*) GetDlgItem(IDC_ARP_ITEM_DELETE);
	CButton*	pARP_AllDeleteButton = (CButton*) GetDlgItem(IDC_ARP_ALL_DELETE);
	CButton*	pARP_SendButton = (CButton*) GetDlgItem(IDC_ARP_SEND);

	CButton*	pPARP_AddButton = (CButton*) GetDlgItem(IDC_PARP_ADD);
	CButton*	pPARP_DeleteButton = (CButton*) GetDlgItem(IDC_PARP_DELETE);

	CButton*	pGARP_SendButton = (CButton*) GetDlgItem(IDC_GARP_SEND);

	CEdit*		pGARP_EtherEdit = (CEdit*) GetDlgItem(IDC_GARP_ETHERNET);
	CEdit*		pMYDEV_EtherEdit = (CEdit*) GetDlgItem(IDC_MYDEV_ETHERNET);

	CComboBox*	pMYDEV_EtherComboBox = (CComboBox*) GetDlgItem(IDC_MYDEV_LIST);

	CIPAddressCtrl* pMYDEV_IPAddrCtrl = (CIPAddressCtrl*) GetDlgItem(IDC_MYDEV_IP);
	CIPAddressCtrl* pARP_IPAddrCtrl = (CIPAddressCtrl*) GetDlgItem(IDC_ARP_DSTIP);

	switch(state)
	{
	case ARP_INITIALIZING:
		pARP_ItemDeleteButton->EnableWindow(FALSE);
		pARP_AllDeleteButton->EnableWindow(FALSE);
		pARP_SendButton->EnableWindow(FALSE);
		pPARP_AddButton->EnableWindow(FALSE);
		pPARP_DeleteButton->EnableWindow(FALSE);
		pGARP_SendButton->EnableWindow(FALSE);
		break;

	case ARP_OPERATING:
		pARP_ItemDeleteButton->EnableWindow(TRUE);
		pARP_AllDeleteButton->EnableWindow(TRUE);
		pARP_SendButton->EnableWindow(TRUE);
		pPARP_AddButton->EnableWindow(TRUE);
		pPARP_DeleteButton->EnableWindow(TRUE);
		pGARP_SendButton->EnableWindow(TRUE);

		pGARP_EtherEdit->EnableWindow(TRUE);
		pARP_IPAddrCtrl->EnableWindow(TRUE);

		pMYDEV_EtherComboBox->EnableWindow(FALSE);
		pMYDEV_IPAddrCtrl->EnableWindow(FALSE);
		pMYDEV_SelectButton->EnableWindow(FALSE);

		break;

	case ARP_ENDPROCESSING:
		break;

	case ARP_DEVICE:
		for(int i=0; i<NI_COUNT_NIC; i++) {
			if(!m_NI->GetAdapterObject(i))
				break;
			device_description = m_NI->GetAdapterObject(i)->description;
			device_description.Trim();
			pMYDEV_EtherComboBox->AddString(device_description);
		}

		pMYDEV_EtherComboBox->SetCurSel(0);
		CString macAddr = m_NI->GetAdapterObject(0)->name;

		mEdit_MyEther = m_NI->GetNICardAddress((char *)macAddr.GetString());
		break;
	}
}

/* My Device Select ��ư Ŭ���� */
void CARPDlg::OnBnClickedMydevSelect()
{
	UpdateData(TRUE);

	// �Էµ� IP �ּҸ� ����
	unsigned char mydev_ip[4];
	mIP_MyIP.GetAddress(mydev_ip[0],mydev_ip[1],mydev_ip[2],mydev_ip[3]);

	// ������ IP, MAC(16������ ��ȯ�Ͽ�) �ּҸ� ARP Header�� ����.
	m_IP->SetSrcIPAddress(mydev_ip);
	m_ARP->SetEnetSrcAddress(MacAddrToHexInt(mEdit_MyEther));
	m_ARP->SetSrcIPAddress(mydev_ip);
	m_Ether->SetEnetSrcAddress(MacAddrToHexInt(mEdit_MyEther));


	// ������ NICard�� Adapter ���
	int nIndex = mCombo_MyDev.GetCurSel();
	m_NI->SetAdapterNumber(nIndex);

	m_NI->PacketStartDriver();

	// Dialog ���� ����
	SetDlgState(ARP_OPERATING);

	UpdateData(FALSE);
}

unsigned char* CARPDlg::MacAddrToHexInt(CString ether)
{
	CString noColStr;
	unsigned char *arp_ether = (u_char *)malloc(sizeof(u_char)*6);

	for(int i=0; i<6; i++) {
		// ":"���� ���еǾ��� MAC �ּҸ� Substring�Ͽ�, 1Byte�� �迭�� �־� ��
		AfxExtractSubString(noColStr,ether,i,':');
		// �޾ƿ� MAC �ּҸ� 16������ ��ȯ������
		arp_ether[i] = (unsigned char)strtoul(noColStr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';

	return arp_ether;
}

/* ARP Request ��ư Ŭ���� */
void CARPDlg::OnBnClickedArpSend()
{
	UpdateData(TRUE);

	// �Էµ� ������ �ּҸ� IP Header�� ����
	unsigned char dst_ip[4];
	mIP_ARPDSTIP.GetAddress(dst_ip[0], dst_ip[1],dst_ip[2],dst_ip[3]);
	m_IP->SetDstIPAddress(dst_ip);
	m_ARP->SetDstIPAddress(dst_ip);

	// UnderLayer Send
	BOOL bSuccess = FALSE ;

	unsigned char a = '0';
	bSuccess = mp_UnderLayer->Send(&a,1);


	// Dialog cache table update
	if(bSuccess) {

	} else {

	}


	// Change dialog setting 

	

	UpdateData(FALSE);
}


void CARPDlg::OnCbnSelchangeMydevList()
{
	UpdateData ( TRUE );

	// ã�� Device List�� ComboBox�� ����
	CComboBox*	pMY_EtherComboBox = (CComboBox*)GetDlgItem(IDC_MYDEV_LIST);
	
	// ComboBox���� ���õ� Item���� NIC�� Adapter �̸��� ������
	int sIndex = pMY_EtherComboBox->GetCurSel();
	CString nicName = m_NI->GetAdapterObject(sIndex)->name;
	
	// Adapter �̸����� Mac Address�� ������
	mEdit_MyEther = m_NI->GetNICardAddress((char *)nicName.GetString());

	// 1�ʸ��� timer ����
	SetTimer(1, 1000, NULL);

	UpdateData ( FALSE );
}

void CARPDlg::OnTimer(UINT nIDEvent)
{
    switch(nIDEvent)
    {
        case 1:
			CacheTableUpdate();
            break;
		default:
			break;
    }
}

void CARPDlg::CacheTableUpdate()
{
	mList_ARPCacheTable.DeleteAllItems();

	int length = m_ARP->arp_table.size();
	for(int i=0; i<length; i++)
	{
		LV_ITEM lvItem;
		lvItem.iItem = mList_ARPCacheTable.GetItemCount();
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;

		int time = get<2>(m_ARP->arp_table[i]);
		if(time > 0) {

			unsigned char* currentIP = get<1>(m_ARP->arp_table[i]);
			CString ip;
			ip.Format("%d.%d.%d.%d", currentIP,currentIP+1,currentIP+2,currentIP+3);
			mList_ARPCacheTable.SetItemText(lvItem.iItem, 0, (LPSTR)(LPCTSTR)ip);


			unsigned char* currentMAC = get<0>(m_ARP->arp_table[i]);
			CString mac;
			mac.Format("%d:%d:%d:%d:%d:%d",currentMAC,currentMAC+1,currentMAC+2
										,currentMAC+3,currentMAC+4,currentMAC+5);
			mList_ARPCacheTable.SetItemText(lvItem.iItem, 1, (LPSTR)(LPCTSTR)mac);


			CString timer;
			timer.Format("%d",time);
			mList_ARPCacheTable.SetItemText(lvItem.iItem, 2, (LPSTR)(LPCTSTR)timer);

			CString status; 
			status.Format("%s",get<3>(m_ARP->arp_table[i])? "complete":"incomplete");
			mList_ARPCacheTable.SetItemText(lvItem.iItem, 3, (LPSTR)(LPCTSTR)status);

		} else {
			m_ARP->arp_table.erase(m_ARP->arp_table.begin() + i);
			continue;
		}

		// 1�� ���̱�
		get<2>(m_ARP->arp_table[i]) -= 1;
	}
}

void CARPDlg::OnIpnFieldchangedMydevIp(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
