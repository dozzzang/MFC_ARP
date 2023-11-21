#include "pch.h"
#include "stdafx.h"
#include "ARP.h"
#include "ARPLayer.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CARPLayer::CARPLayer(char* pName)
    : CBaseLayer(pName)
{
    ResetHeader();
}

CARPLayer::~CARPLayer() {}

void CARPLayer::ResetHeader() {
    m_sHeader.arp_hardType = 0x0000;
    m_sHeader.arp_protocolType = 0x0000;
    m_sHeader.arp_hardLength = MAC_ADDR_SIZE;
    m_sHeader.arp_protocolLength = IP_ADDR_SIZE;
    m_sHeader.arp_option = 0x0000;
    memset(m_sHeader.arp_HardSrcAddr, 0, MAC_ADDR_SIZE);
    memset(m_sHeader.arp_ProtcolSrcAddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.arp_HardDstaddr, 0, MAC_ADDR_SIZE);
    memset(m_sHeader.arp_PorotocolDstAddr, 0, IP_ADDR_SIZE);
}

BOOL CARPLayer::Send(unsigned char* ppayload, int nlength) {
    // Cast payload to IP_HEADER to access IP addresses
    PIP_HEADER ip_header = reinterpret_cast<PIP_HEADER>(ppayload);
    std::string dst_ip_str(reinterpret_cast<char*>(ip_header->ip_DstAddr), IP_ADDR_SIZE);

    // Check if the destination IP is already in the ARP cache
    auto iter = m_arpTable.find(dst_ip_str);
    if (iter != m_arpTable.end()) {
        // If it's incomplete, we don't process it further
        if (iter->second.status == FALSE) {
            AfxMessageBox(_T("Already In Cache!"));
            return TRUE;
        }
    }
    else {
        // Add a new ARP cache entry if it does not exist
        LARP_NODE newNode;
        memcpy(newNode.prot_addr, ip_header->ip_DstAddr, IP_ADDR_SIZE);
        memset(newNode.hard_addr, 0xff, MAC_ADDR_SIZE); // Broadcast address
        newNode.status = FALSE; // Incomplete entry
        newNode.Time = time(nullptr); // Current time
        m_arpTable[dst_ip_str] = newNode;
    }

    // Set ARP header
    SetSrcAddress(m_myMac, ip_header->ip_SrcAddr);
    SetDstAddress(broadcastAddr, ip_header->ip_DstAddr);
    SetOption(ARP_OP_REQUEST);

    // Send the ARP request
    return mp_UnderLayer->Send(reinterpret_cast<unsigned char*>(&m_sHeader), sizeof(ARP_HEADER));
}

BOOL CARPLayer::Receive(unsigned char* ppayload) {
    PARP_HEADER arp_header = reinterpret_cast<PARP_HEADER>(ppayload);

    // Process ARP request
    if (arp_header->arp_option == ARP_OP_REQUEST) {
        // Check if the destination IP is ours
        if (memcmp(arp_header->arp_ProtocolDstAddr, myIp, IP_ADDR_SIZE) == 0) {
            // Respond to the ARP request
            SetDstAddress(arp_header->arp_HardSrcAddr, arp_header->arp_ProtocolSrcAddr);
            SetSrcAddress(myMac, myIp);
            SetOption(ARP_OP_REPLY);
            SwapAddresses();

            return mp_UnderLayer->Send(reinterpret_cast<unsigned char*>(arp_header), sizeof(ARP_HEADER));
        }
    }
    // Process ARP reply
    else if (arp_header->arp_option == ARP_OP_REPLY) {
        std::string src_ip_str(reinterpret_cast<char*>(arp_header->arp_ProtocolSrcAddr), IP_ADDR_SIZE);
        auto iter = m_arpTable.find(src_ip_str);
        if (iter != m_arpTable.end()) {
            // Update the ARP cache entry
            memcpy(iter->second.hard_addr, arp_header->arp_HardSrcAddr, MAC_ADDR_SIZE);
            iter->second.status = TRUE;
            iter->second.Time = time(nullptr);
        }
    }

    return TRUE;
}
void CARPLayer::SetSrcAddress(const unsigned char macAddr[], const unsigned char ipAddr[]) {
    memcpy(m_sHeader.arp_HardSrcAddr, macAddr, MAC_ADDR_SIZE);
    memcpy(m_sHeader.arp_ProtcolSrcAddr, ipAddr, IP_ADDR_SIZE);
}

void CARPLayer::SetDstAddress(const unsigned char macAddr[], const unsigned char ipAddr[]) {
    memcpy(m_sHeader.arp_HardDstaddr, macAddr, MAC_ADDR_SIZE);
    memcpy(m_sHeader.arp_PorotocolDstAddr, ipAddr, IP_ADDR_SIZE);
}
void CARPLayer::SwapAddresses() {
    unsigned char tempMac[MAC_ADDR_SIZE];
    unsigned char tempIp[IP_ADDR_SIZE];

    // Swap MAC addresses
    memcpy(tempMac, m_sHeader.arp_HardSrcAddr, MAC_ADDR_SIZE);
    memcpy(m_sHeader.arp_HardSrcAddr, m_sHeader.arp_HardDstaddr, MAC_ADDR_SIZE);
    memcpy(m_sHeader.arp_HardDstaddr, tempMac, MAC_ADDR_SIZE);

    // Swap IP addresses
    memcpy(tempIp, m_sHeader.arp_ProtcolSrcAddr, IP_ADDR_SIZE);
    memcpy(m_sHeader.arp_ProtcolSrcAddr, m_sHeader.arp_PorotocolDstAddr, IP_ADDR_SIZE);
    memcpy(m_sHeader.arp_PorotocolDstAddr, tempIp, IP_ADDR_SIZE);
}
BOOL CARPLayer::checkAddressWithMyIp(const unsigned char dstip[]) {
    return memcmp(dstip, myIp, IP_ADDR_SIZE) == 0;
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
    m_arpTable[ip_key].Time = CTime::GetCurrentTime();
}

void CARPLayer::deleteAllARPEntry() {
    // Clear the entire ARP table
    m_arpTable.clear();
}

void CARPLayer::deleteARPEntry(const CString& target) {
    // Delete a specific entry from the ARP table
    m_arpTable.erase(target);
}



void CARPLayer::SetOption(unsigned int arp_option) {
    m_sHeader.arp_option = htons(static_cast<unsigned short>(arp_option));
}

unsigned int CARPLayer::GetOption() {
    return ntohs(m_sHeader.arp_option);
}
