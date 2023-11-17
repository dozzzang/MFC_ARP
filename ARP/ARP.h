
// ARP.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#if !defined(AFX_ARP_H__3D24E705_A889_431A_B388_17290947B99B__INCLUDED_)
#define AFX_ARP_H__3D24E705_A889_431A_B388_17290947B99B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CARPApp:
// �� Ŭ������ ������ ���ؼ��� ARP.cpp�� �����Ͻʽÿ�.
//

class CARPApp : public CWinApp
{
public:
	CARPApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CARPApp theApp;
#endif