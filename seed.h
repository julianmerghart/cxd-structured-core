#ifndef CXD_NETWORK_CORE_H
#define CXD_NETWORK_CORE_H

#include <stdint.h>
#include "ledger.h" // To get the CxDBlock definition

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#define CXD_PORT 8888
#define MAX_PEERS 256

typedef struct {
    struct sockaddr_storage addr; 
    socklen_t addr_len;
    uint64_t last_seen;
    uint8_t hardware_verified; // 1 if TPM signature checked out
} CxDPeer;

// API
int cxd_net_init();
int cxd_net_listen(uint16_t port);
int cxd_add_peer_from_seed(const char *hostname);
int cxd_broadcast_block(int sock, CxDBlock *block);
void cxd_net_cleanup(int sock);

#endif
