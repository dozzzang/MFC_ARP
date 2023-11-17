
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#if !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#define AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN     
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  
#define _AFX_ALL_WARNINGS

#include <afxwin.h>   
#include <afxext.h>        


#include <afxdisp.h> 



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>   
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

//{{AFX_INSERT_LOCATION}}

#define MAX_LAYER_NUMBER		0xff

#define ETHER_MAX_SIZE			1514
#define ETHER_HEADER_SIZE		14
#define ETHER_MAX_DATA_SIZE		( ETHER_MAX_SIZE - ETHER_HEADER_SIZE )
#define ETHER_PROTO_TYPE_IP		0x0800
#define ETHER_PROTO_TYPE_ARP	0x0806
#define ETHER_PROTO_TYPE_RARP	0x0835

#define IP_HEADER_SIZE			20
#define IP_DATA_SIZE			1500

#define APP_HEADER_SIZE			28			

#define ARP_HARDTYPE			0x0001	// Ethernet frame
#define ARP_PROTOTYPE_IP		0x0800
#define ARP_PROTOTYPE_ARP		0x0806
#define ARP_PROTOTYPE_RARP		0x0835
#define ARP_OPCODE_REQUEST		0x0001
#define ARP_OPCODE_REPLY		0x0002
#define ARP_HEADER_SIZE			28

#define NI_COUNT_NIC	10


typedef struct _ETHERNET_ADDR
{
	union {
		struct { unsigned char e0, e1, e2, e3, e4, e5; } s_un_byte;
		unsigned char s_ether_addr[6];
	} S_un;

	#define addr0 S_un.s_un_byte.e0
	#define addr1 S_un.s_un_byte.e1
	#define addr2 S_un.s_un_byte.e2
	#define addr3 S_un.s_un_byte.e3
	#define addr4 S_un.s_un_byte.e4
	#define addr5 S_un.s_un_byte.e5

	#define addrs  S_un.s_ether_addr

} ETHERNET_ADDR, *LPETHERNET_ADDR;

typedef struct _IP_ADDR
{
	union {
		struct { unsigned char e0, e1, e2, e3; } s_un_byte;
		unsigned char s_ether_addr[4];
	} S_un;

	#define addr0 S_un.s_un_byte.e0
	#define addr1 S_un.s_un_byte.e1
	#define addr2 S_un.s_un_byte.e2
	#define addr3 S_un.s_un_byte.e3

	#define addrs  S_un.s_ether_addr

} IP_ADDR, *LPIP_ADDR;

#endif // !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
