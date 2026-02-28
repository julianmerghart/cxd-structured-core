#include "network_core.h"
#include <stdio.h>
#include <string.h>

static CxDPeer peer_list[MAX_PEERS];
static int peer_count = 0;

int cxd_net_init() {
#ifdef _WIN32
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2,2), &wsa) == 0 ? 0 : -1;
#endif
    return 0;
}

int cxd_net_listen(uint16_t port) {
    int sock;
    struct addrinfo hints, *res;
    char port_str[6];
    sprintf(port_str, "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // IPv4 & IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port_str, &hints, &res) != 0) return -1;

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) return -1;

    // Enable Dual-Stack (IPv6 socket handles IPv4 too)
    int no = 0;
    setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no));

    if (bind(sock, res->ai_addr, res->ai_addrlen) < 0) {
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return sock;
}

int cxd_add_peer_from_seed(const char *hostname) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(hostname, "8888", &hints, &res) != 0) return -1;
    if (peer_count >= MAX_PEERS) return -1;

    // Add the seed node to our peer list
    memcpy(&peer_list[peer_count].addr, res->ai_addr, res->ai_addrlen);
    peer_list[peer_count].addr_len = res->ai_addrlen;
    peer_list[peer_count].last_seen = (uint64_t)time(NULL);
    peer_count++;

    freeaddrinfo(res);
    return 0;
}

int cxd_broadcast_block(int sock, CxDBlock *block) {
    int sent_count = 0;
    for (int i = 0; i < peer_count; i++) {
        ssize_t s = sendto(sock, (char*)block, sizeof(CxDBlock), 0, 
                           (struct sockaddr *)&peer_list[i].addr, 
                           peer_list[i].addr_len);
        if (s > 0) sent_count++;
    }
    return sent_count;
}

void cxd_net_cleanup(int sock) {
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
}
