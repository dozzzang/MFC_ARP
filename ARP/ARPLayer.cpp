#include "pch.h"
#include "stdafx.h"
#include "ARP.h"
#include "ARPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CARPLayer::CARPLayer(char* pName)
    : CBaseLayer(pName)
{
    ResetHeader();
    memset(m_myIp, 0, IP_ADDR_SIZE);
    memset(m_myMac, 0, ENET_ADDR_SIZE);
}

CARPLayer::~CARPLayer() {}

std::unordered_map<CString, CARPLayer::LARP_NODE> CARPLayer::m_arpTable;

void CARPLayer::ResetHeader() {
    m_sHeader.arp_hardType = 0x0000;    //Ethernet 0x0001
    m_sHeader.arp_protocolType = 0x0000;    //IP 0x0800
    m_sHeader.arp_hardLength = ENET_ADDR_SIZE;
    m_sHeader.arp_protocolLength = IP_ADDR_SIZE;
    m_sHeader.arp_option = 0x0000;  //Request 1,Reply 2
    memset(m_sHeader.arp_HardSrcAddr, 0, ENET_ADDR_SIZE);
    memset(m_sHeader.arp_ProtcolSrcAddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.arp_HardDstaddr, 0, ENET_ADDR_SIZE);
    memset(m_sHeader.arp_PorotocolDstAddr, 0, IP_ADDR_SIZE);
}

CARPLayer::_LARP_NODE::_LARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char incomplete = false) {
    memcpy(prot_addr, cipaddr, IP_ADDR_SIZE);
    memcpy(hard_addr, cenetaddr, ENET_ADDR_SIZE);
    status = incomplete;
    lifeTime = CTime::GetCurrentTime();
}


BOOL CARPLayer::Send(unsigned char* ppayload, int nlength) {
    PIP_HEADER ip_header = (PIP_HEADER)ppayload;  // C-style cast
    CString dst_ip_str;
    byte2Str(ip_header->ip_DstAddr, dst_ip_str, ARP_IP_TYPE); // Using ARP_IP_TYPE for clarity

    auto iter = m_arpTable.find(dst_ip_str);
    if (iter != m_arpTable.end()) {
        if (iter->second.status == FALSE) { //incomplete
            AfxMessageBox(_T("plz wait"));
            return TRUE;
        }
    }
    else {
        unsigned char broadcastAddr[ENET_ADDR_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
        LARP_NODE newNode(ip_header->ip_DstAddr, broadcastAddr, FALSE);
        m_arpTable[dst_ip_str] = newNode;
        SetSrcAddress(m_myMac, ip_header->ip_SrcAddr);
        SetDstAddress(broadcastAddr, ip_header->ip_DstAddr);  // Broadcast address
        SetOption(ARP_OPCODE_REQUEST);

        return mp_UnderLayer->Send((unsigned char*)&m_sHeader, sizeof(ARP_HEADER));
    }
}


BOOL CARPLayer::Receive(unsigned char* ppayload) {
    PARP_HEADER arp_header = (PARP_HEADER)ppayload; // C-style cast for the ARP header

    // Process ARP request
    if (arp_header->arp_option == ARP_OPCODE_REQUEST) {  
        // Check if the destination IP is ours
        if (checkAddressWithMyIp(arp_header->arp_PorotocolDstAddr) == 0) { //Perhaps In Dlg
            // Respond to the ARP request swap!
            SetDstAddress(arp_header->arp_HardSrcAddr, arp_header->arp_ProtcolSrcAddr);
            SetSrcAddress(m_myMac, m_myIp); // Perhaps In Dlg
            SetOption(ARP_OPCODE_REPLY);
        }
        return mp_UnderLayer->Send((unsigned char*)arp_header, sizeof(ARP_HEADER));
    }
    // Process ARP reply
    else if (arp_header->arp_option == ARP_OPCODE_REPLY) {
        CString src_ip_str;
        byte2Str(arp_header->arp_ProtcolSrcAddr, src_ip_str, ARP_IP_TYPE); // Convert IP to CString

        auto iter = m_arpTable.find(src_ip_str);
        if (iter != m_arpTable.end()) {
            // Update the ARP cache entry
            memcpy(iter->second.hard_addr, arp_header->arp_HardSrcAddr, ENET_ADDR_SIZE);
            iter->second.status = TRUE;
            iter->second.lifeTime = CTime::GetCurrentTime();
        }
    }

    return TRUE;
}
void CARPLayer::SetSrcAddress(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
    memcpy(m_sHeader.arp_HardSrcAddr, enetAddr, ENET_ADDR_SIZE);
    memcpy(m_sHeader.arp_ProtcolSrcAddr, ipAddr, IP_ADDR_SIZE);
}

void CARPLayer::SetDstAddress(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
    memcpy(m_sHeader.arp_HardDstaddr, enetAddr, ENET_ADDR_SIZE);
    memcpy(m_sHeader.arp_PorotocolDstAddr, ipAddr, IP_ADDR_SIZE);
}

BOOL CARPLayer::checkAddressWithMyIp(unsigned char dstip[]) {
    return memcmp(dstip, m_myIp, IP_ADDR_SIZE) == 0;
}
std::vector<CARPLayer::LARP_NODE> CARPLayer::getTable() {
    std::vector<LARP_NODE> table;
    for (const auto& entry : m_arpTable) {
        table.push_back(entry.second);
    }
    return table;
}

void CARPLayer::addARPEntry(const CString& ip_key, const LARP_NODE& node) {
    // Add or update the ARP entry in the ARP table
    m_arpTable[ip_key] = node;
    // Assuming you want to update the timestamp of the entry as well
    m_arpTable[ip_key].lifeTime = CTime::GetCurrentTime();
}

void CARPLayer::deleteAllARPEntry() {
    // Clear the entire ARP table
    m_arpTable.clear();
}

void CARPLayer::deleteARPEntry(const CString& target) {
    // Delete a specific entry from the ARP table
    m_arpTable.erase(target);
}
void CARPLayer::byte2Str(unsigned char* src, CString& dst, unsigned short type) {
    if (type == ARP_IP_TYPE) { // IP Address
        dst.Format(_T("%d.%d.%d.%d"), src[0], src[1], src[2], src[3]);
    }
    else if (type == ARP_ENET_TYPE) { // ENET Address
        dst.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"), src[0], src[1], src[2], src[3], src[4], src[5]);
    }
    else {
        dst.Empty(); // Unknown type
    }
}

void CARPLayer::str2Byte(unsigned char* dst, CString& src, unsigned short type) {
    int values[6]; // Array to hold the bytes
    if (type == ARP_ENET_TYPE) { // IP Address
        if (sscanf((LPCTSTR)src, "%d.%d.%d.%d", &values[0], &values[1], &values[2], &values[3]) == 4) {
            for (int i = 0; i < 4; ++i) {
                dst[i] = (unsigned char)values[i];
            }
        }
    }
    else if (type == ARP_ENET_TYPE) { // ENET Address
        if (sscanf((LPCTSTR)src, "%02X:%02X:%02X:%02X:%02X:%02X", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) == 6) {
            for (int i = 0; i < 6; ++i) {
                dst[i] = (unsigned char)values[i];
            }
        }
    }
}


void CARPLayer::SetOption(unsigned int arp_option) {
    m_sHeader.arp_option = htons(static_cast<unsigned short>(arp_option));
}

void CARPLayer::setDlgIp(CString ip) {
    str2Byte(m_myIp, ip, ARP_IP_TYPE);
}

void CARPLayer::setDlgMac(CString enet) {
    str2Byte(m_myMac, enet, ARP_ENET_TYPE);
}
