#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include <algorithm>
#include <vector>
#include <tuple>

class CARPLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader( );


	// Cache table data structure


public:
	CARPLayer( char* pName );
	virtual ~CARPLayer();

	void SetSrcIPAddress(unsigned char* src_ip);
	void SetDstIPAddress(unsigned char* dst_ip);
	void SetEnetSrcAddress(unsigned char* pAddress);
	void SetEnetDstAddress(unsigned char* pAddress);
	void SetOption(unsigned int arp_option);
	void SwapAddresses();

	unsigned char* GetSrcIPAddress();
	unsigned char* GetDstIPAddress();
	unsigned char* GetEnetSrcAddress();
	unsigned char* GetEnetDstAddress();
	unsigned int GetOption();

	BOOL Send(unsigned char* ppayload, int nlength);
	BOOL Receive(unsigned char* ppayload);

	typedef struct _ARPLayer_HEADER {
		unsigned short arp_hardType;
		unsigned short arp_protocolType;
		unsigned char arp_hardLength;
		unsigned char arp_protocolLength;
		unsigned short arp_option;
		ETHERNET_ADDR arp_srcEtherAddress;
		IP_ADDR arp_srcIPAddress;
		ETHERNET_ADDR arp_dstEtherAddress;
		IP_ADDR arp_dstIPAddress;
	} ARPLayer_HEADER, *PARPLayer_HEADER;

	// MAC, IP, timer, complete/incomplete
	vector<tuple<unsigned char*, unsigned char*, int, BOOL> > arp_table;

protected:
	ARPLayer_HEADER m_sHeader;
	
	pair<unsigned char*, int> tempPayload;
};