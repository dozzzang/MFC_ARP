// NILayer.cpp: implementation of the CNILayer class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "stdafx.h"
#include "ARP.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNILayer::CNILayer( char *pName, LPADAPTER *pAdapterObject, int iNumAdapter )
	: CBaseLayer( pName )
{
	m_AdapterObject = NULL;
	m_iNumAdapter = iNumAdapter;
	m_thrdSwitch = TRUE;
	SetAdapterList(NULL);
}

CNILayer::~CNILayer()
{
}

void CNILayer::PacketStartDriver()
{
	char errbuf[PCAP_ERRBUF_SIZE];

	if(m_iNumAdapter == -1){
		AfxMessageBox("Not exist NICard");
		return;
	}
	
	m_AdapterObject = pcap_open_live(m_pAdapterList[m_iNumAdapter]->name,1500,PCAP_OPENFLAG_PROMISCUOUS,2000,errbuf);
	if(!m_AdapterObject){
		AfxMessageBox(errbuf);
		return;
	}
	AfxBeginThread(ReadingThread, this);
}

pcap_if_t *CNILayer::GetAdapterObject(int iIndex)
{
	return m_pAdapterList[iIndex];
}

void CNILayer::SetAdapterNumber(int iNum)
{
	m_iNumAdapter = iNum;
}

void CNILayer::SetAdapterList(LPADAPTER *plist)
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i=0;
	
	char errbuf[PCAP_ERRBUF_SIZE];

	for(int j=0;j<NI_COUNT_NIC;j++)
	{
		m_pAdapterList[j] = NULL;
	}

	if(pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		AfxMessageBox("Not exist NICard");
		return;
	}
	if(!alldevs)
	{
		AfxMessageBox("Not exist NICard");
		return;
	}

	for(d=alldevs; d; d=d->next)
	{
		m_pAdapterList[i++] = d;
	}
}

CString CNILayer::GetNICardAddress(char* adapter_name)
{
	PPACKET_OID_DATA OidData;
	LPADAPTER lpAdapter;

	OidData = (PPACKET_OID_DATA)malloc(6+sizeof(PACKET_OID_DATA));
	if(OidData == NULL)
	{
		return "None";
	}

	OidData->Oid = OID_802_3_CURRENT_ADDRESS;
	OidData->Length = 6;
	ZeroMemory(OidData->Data, 6);

	lpAdapter = PacketOpenAdapter(adapter_name);

	CString NICardAddress;
	
	if(PacketRequest(lpAdapter, FALSE, OidData))
	{
		NICardAddress.Format("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", 
			(OidData->Data)[0],
			(OidData->Data)[1],
			(OidData->Data)[2],
			(OidData->Data)[3],
			(OidData->Data)[4],
			(OidData->Data)[5]);
	}

	PacketCloseAdapter(lpAdapter);
	free(OidData);
	return NICardAddress;
}

BOOL CNILayer::Send(unsigned char *ppayload, int nlength)
{
	if(pcap_sendpacket(m_AdapterObject,ppayload,nlength))
	{
		AfxMessageBox("qqq");
		return FALSE;
	}
	return TRUE;
}

BOOL CNILayer::Receive( unsigned char* ppayload )
{
	BOOL bSuccess = FALSE;

	bSuccess = mp_aUpperLayer[0]->Receive(ppayload);
	return bSuccess;
}

UINT CNILayer::ReadingThread(LPVOID pParam)
{
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	int result;

	CNILayer *pNI = (CNILayer *)pParam;

	while(pNI->m_thrdSwitch) {
		result = pcap_next_ex(pNI->m_AdapterObject,&header,&pkt_data);

		if(result == 0) {

		} else if(result == 1) {
			pNI->Receive((u_char *)pkt_data);
		} else if(result < 0) {
		}
	}

	return 0;
}