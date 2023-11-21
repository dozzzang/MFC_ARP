#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "pch.h"
#include "BaseLayer.h"
#include <unordered_map>
#include <string>

class CARPLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader( );
	void  byte2Str(unsigned char* src, CString& dst, unsigned short t);
	void  str2byte(unsigned char* dst, CString& src, unsigned short t);

public:
	CARPLayer( char* pName );
	virtual ~CARPLayer();

	typedef struct _ARP_HEADER {
		unsigned short  arp_hardType;
		unsigned short  arp_protocolType;
		unsigned char   arp_hardLength;
		unsigned char   arp_protocolLength;
		unsigned short  arp_option;
		unsigned char   arp_HardSrcAddr[MAC_ADDR_SIZE];
		unsigned char   arp_ProtcolSrcAddr[IP_ADDR_SIZE];
		unsigned char   arp_HardDstaddr[MAC_ADDR_SIZE];
		unsigned char   arp_PorotocolDstAddr[IP_ADDR_SIZE];
		struct _ARP_HEADER();
		struct _ARP_HEADER(const struct _ARP_HEADER& ot);
	} ARP_HEADER, * PARP_HEADER;

	typedef struct _LARP_NODE {
		unsigned char prot_addr[IP_ADDR_SIZE];
		unsigned char hard_addr[MAC_ADDR_SIZE];
		unsigned char status;
		time_t Time;

		struct _LARP_NODE(unsigned int ipaddr, unsigned int enetaddr, unsigned char incomplete);
		struct _LARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete);
		struct _LARP_NODE(const _LARP_NODE& other);
	} LARP_NODE, * PLARP_NODE;

	typedef struct _IP_HEADER {	//ARP Layer에 payload casting위해 IP Header가 필요.
		unsigned char   ip_VersionHeaderLegnth;
		unsigned char   ip_Tos;
		unsigned short  ip_Tlength;

		unsigned short  ip_Id;
		unsigned char   ip_Offset;

		unsigned char   ip_Ttl;
		unsigned char   ip_Ptype;
		unsigned short  ip_Checksum;

		unsigned char   ip_SrcAddr[IP_ADDR_SIZE];
		unsigned char   ip_DstAddr[IP_ADDR_SIZE];
		unsigned char   ip_Data[MAC_MAX_SIZE - MAC_HEADER_SIZE - 20];
	}IP_HEADER, * PIP_HEADER;

	// 현재 ARP cache table 반환 함수
	std::vector<LARP_NODE> getTable();

	void SetSrcAddress(const unsigned char macAddr[], const unsigned char ipAddr[]);
	void SetDstAddress(const unsigned char macAddr[], const unsigned char ipAddr[]);
	void SetOption(unsigned int arp_option);
	void SwapAddresses();
	BOOL checkAddressWithMyIp(const unsigned char dstip[]); //인자로 들어온 dstip와 현재 Host의 id가 다르면 False 반환
	static void addARPEntry(const CString& ip_key, const LARP_NODE& node);// arp cache entry를 해시테이블에 추가하는 함수 + table에 update도 같이
	static void deleteAllARPEntry(); //clear사용?
	static void deleteARPEntry(const CString& target);
	unsigned int GetOption();

	BOOL Send(unsigned char* ppayload, int nlength);
	BOOL Receive(unsigned char* ppayload);

	// MAC, IP, timer, complete/incomplete

protected:
	unsigned char myIp[IP_ADDR_SIZE];
	unsigned char myMac[MAC_ADDR_SIZE];
	_ARP_HEADER m_sHeader;
	std::unordered_map<CString, LARP_NODE> m_arpTable;
};