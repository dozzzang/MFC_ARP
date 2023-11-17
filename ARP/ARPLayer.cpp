#include "stdafx.h"
#include "ARP.h"
#include "ARPLayer.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CARPLayer::CARPLayer( char* pName)
: CBaseLayer( pName )
{
	ResetHeader();
	arp_table.clear();
}

CARPLayer::~CARPLayer()
{
}

void CARPLayer::ResetHeader()
{
	m_sHeader.arp_hardType = 0x0000;
	m_sHeader.arp_protocolType = 0x0000;
	m_sHeader.arp_hardLength = 0x06;
	m_sHeader.arp_protocolLength = 0x04;
	m_sHeader.arp_option = 0x0000;
	memset(m_sHeader.arp_srcEtherAddress.addrs, 0, 6);
	memset(m_sHeader.arp_srcIPAddress.addrs, 0, 4);
	memset(m_sHeader.arp_dstEtherAddress.addrs, 0, 6);
	memset(m_sHeader.arp_dstIPAddress.addrs, 0, 4);
}

BOOL CARPLayer::Send(unsigned char* ppayload, int nlength)
{

	BOOL found = false;
	for(unsigned int i=0; i<arp_table.size(); i++) {
		if(get<1>(arp_table[i]) == GetDstIPAddress()) {
			SetEnetDstAddress(get<0>(arp_table[i]));
			found = true;
			break;
		}
	}

	BOOL bSuccess = FALSE;
	unsigned char ARP_BROADCAST[6] = {0xff, };
	if(!found) { // Send ARP Request
		tempPayload = make_pair(ppayload,nlength);

		// Cache table �� �߰�.
		arp_table.push_back(make_tuple( ARP_BROADCAST, GetDstIPAddress() , 3, FALSE));

		SetOption(ntohs(ARP_OPCODE_REQUEST));
		SetEnetDstAddress(ARP_BROADCAST);
		bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, nlength);
	} else {
		((CEthernetLayer *)mp_UnderLayer)->SetEnetDstAddress(GetEnetDstAddress());
		bSuccess = mp_UnderLayer->Send(ppayload, nlength);
	}

    return true;
}

BOOL CARPLayer::Receive(unsigned char* ppayload) {
	PARPLayer_HEADER pFrame = (PARPLayer_HEADER)ppayload;
	BOOL bSuccess = FALSE;

	if (ntohs(pFrame->arp_option) == ARP_OPCODE_REQUEST) {
		if (memcmp(pFrame->arp_dstIPAddress.addrs, m_sHeader.arp_srcIPAddress.addrs, 4) == 0) {
			// 대상 주소가 로컬 주소와 일치
			SwapAddresses();
			bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, sizeof(m_sHeader));
		}
		// 대상 주소가 로컬 주소와 일치하지 않으면 패킷 폐기
	}

	return bSuccess;
}

void CARPLayer::SetEnetSrcAddress(unsigned char *pAddress)
{
	memcpy(&m_sHeader.arp_srcEtherAddress.addrs, pAddress, 6);
}

void CARPLayer::SetEnetDstAddress(unsigned char *pAddress)
{
	memcpy(&m_sHeader.arp_dstEtherAddress.addrs, pAddress, 6);
}

void CARPLayer::SetSrcIPAddress(unsigned char* src_ip)
{
	memcpy(m_sHeader.arp_srcIPAddress.addrs, src_ip, 4);
}

void CARPLayer::SetDstIPAddress(unsigned char* dst_ip)
{
	memcpy(m_sHeader.arp_dstIPAddress.addrs, dst_ip, 4);
}

void CARPLayer::SetOption(unsigned int arp_option)
{
	m_sHeader.arp_option = arp_option;
}

unsigned char* CARPLayer::GetEnetSrcAddress()
{
	return m_sHeader.arp_srcEtherAddress.addrs;
}

unsigned char* CARPLayer::GetEnetDstAddress()
{
	return m_sHeader.arp_dstEtherAddress.addrs;
}

unsigned char* CARPLayer::GetSrcIPAddress()
{
	return m_sHeader.arp_srcIPAddress.addrs;
}

unsigned char* CARPLayer::GetDstIPAddress()
{
	return m_sHeader.arp_dstIPAddress.addrs;
}

unsigned int CARPLayer::GetOption()
{
	return m_sHeader.arp_option;
}
void CARPLayer::SwapAddresses() {
	// 교환: 송신자 및 수신자의 하드웨어 주소
	unsigned char tempHardwareAddr[6];
	memcpy(tempHardwareAddr, m_sHeader.arp_srcEtherAddress.addrs, 6);
	memcpy(m_sHeader.arp_srcEtherAddress.addrs, m_sHeader.arp_dstEtherAddress.addrs, 6);
	memcpy(m_sHeader.arp_dstEtherAddress.addrs, tempHardwareAddr, 6);

	// 교환: 송신자 및 수신자의 프로토콜 주소
	unsigned char tempProtocolAddr[4];
	memcpy(tempProtocolAddr, m_sHeader.arp_srcIPAddress.addrs, 4);
	memcpy(m_sHeader.arp_srcIPAddress.addrs, m_sHeader.arp_dstIPAddress.addrs, 4);
	memcpy(m_sHeader.arp_dstIPAddress.addrs, tempProtocolAddr, 4);

	// Opcode를 ARP 응답으로 설정
	SetOption(htons(ARP_OPCODE_REPLY));
}