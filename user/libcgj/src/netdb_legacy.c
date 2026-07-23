/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Legacy netdb: gethostbyname / getservbyname / getprotobyname bring-up.
 *
 * greppable: CGJ_NETDB_SOFT_CASEFOLD
 * greppable: CGJ_NETDB_SOFT_SERV_ITER
 * greppable: CGJ_NETDB_SOFT_PROTO_TABLE
 * greppable: CGJ_NETDB_SOFT_LOOPBACK_REV
 *
 * Soft deepen: larger well-known tables, case-insensitive match, soft
 * getservent/getprotoent cursors, loopback reverse → "localhost".
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int h_errno;

static struct hostent g_he;
static char g_szHeName[256];
static char *g_aHeAliases[1];
static char *g_aHeAddrList[2];
static struct in_addr g_heAddr;

static struct servent g_se;
static char g_szSeName[64];
static char *g_aSeAliases[1];
static char g_szSeProto[16];
static size_t g_iServCur;

static struct protoent g_pe;
static char g_szPeName[32];
static char *g_aPeAliases[1];
static size_t g_iProtoCur;

static const struct {
    const char *szName;
    int         nPort;
    const char *szProto;
} g_aServ[] = {
    { "ftp", 21, "tcp" },
    { "ssh", 22, "tcp" },
    { "telnet", 23, "tcp" },
    { "smtp", 25, "tcp" },
    { "domain", 53, "udp" },
    { "domain", 53, "tcp" },
    { "tftp", 69, "udp" },
    { "http", 80, "tcp" },
    { "pop3", 110, "tcp" },
    { "nntp", 119, "tcp" },
    { "ntp", 123, "udp" },
    { "imap", 143, "tcp" },
    { "snmp", 161, "udp" },
    { "ldap", 389, "tcp" },
    { "https", 443, "tcp" },
    { "syslog", 514, "udp" },
    { "ldaps", 636, "tcp" },
    { "imaps", 993, "tcp" },
    { "pop3s", 995, "tcp" },
    { NULL, 0, NULL }
};

/* greppable: CGJ_NETDB_SOFT_PROTO_TABLE */
static const struct {
    const char *szName;
    int         nProto;
} g_aProto[] = {
    { "ip", 0 },
    { "icmp", 1 },
    { "igmp", 2 },
    { "tcp", 6 },
    { "udp", 17 },
    { "ipv6", 41 },
    { "rsvp", 46 },
    { "gre", 47 },
    { "esp", 50 },
    { "ah", 51 },
    { "icmpv6", 58 },
    { "sctp", 132 },
    { NULL, 0 }
};

static int
soft_eq_ci(const char *szA, const char *szB)
{
    size_t i;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (i = 0;; i++) {
        unsigned char ca = (unsigned char)szA[i];
        unsigned char cb = (unsigned char)szB[i];

        if (ca >= 'A' && ca <= 'Z') {
            ca = (unsigned char)(ca - 'A' + 'a');
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = (unsigned char)(cb - 'A' + 'a');
        }
        if (ca != cb) {
            return 0;
        }
        if (ca == '\0') {
            return 1;
        }
    }
}

static void
soft_copy_name(char *szDst, size_t cbDst, const char *szSrc)
{
    size_t n;

    if (szDst == NULL || cbDst == 0) {
        return;
    }
    if (szSrc == NULL) {
        szDst[0] = '\0';
        return;
    }
    n = strlen(szSrc);
    if (n + 1 > cbDst) {
        n = cbDst - 1;
    }
    memcpy(szDst, szSrc, n);
    szDst[n] = '\0';
}

void
sethostent(int fStayopen)
{
    (void)fStayopen;
}

void
endhostent(void)
{
}

struct hostent *
gethostent(void)
{
    h_errno = NO_RECOVERY;
    return NULL;
}

struct hostent *
gethostbyname(const char *szName)
{
    struct addrinfo hints;
    struct addrinfo *pRes = NULL;
    struct sockaddr_in *pSin;
    size_t n;

    if (szName == NULL) {
        h_errno = HOST_NOT_FOUND;
        errno = EINVAL;
        return NULL;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(szName, NULL, &hints, &pRes) != 0 || pRes == NULL) {
        h_errno = HOST_NOT_FOUND;
        return NULL;
    }
    if (pRes->ai_addr == NULL ||
        pRes->ai_addrlen < (socklen_t)sizeof(struct sockaddr_in)) {
        freeaddrinfo(pRes);
        h_errno = NO_DATA;
        return NULL;
    }
    pSin = (struct sockaddr_in *)(void *)pRes->ai_addr;
    g_heAddr = pSin->sin_addr;
    n = strlen(szName);
    if (n + 1 > sizeof(g_szHeName)) {
        n = sizeof(g_szHeName) - 1;
    }
    memcpy(g_szHeName, szName, n);
    g_szHeName[n] = '\0';
    g_aHeAliases[0] = NULL;
    g_aHeAddrList[0] = (char *)&g_heAddr;
    g_aHeAddrList[1] = NULL;
    g_he.h_name = g_szHeName;
    g_he.h_aliases = g_aHeAliases;
    g_he.h_addrtype = AF_INET;
    g_he.h_length = (int)sizeof(struct in_addr);
    g_he.h_addr_list = g_aHeAddrList;
    freeaddrinfo(pRes);
    h_errno = 0;
    return &g_he;
}

struct hostent *
gethostbyaddr(const void *pAddr, socklen_t cbLen, int nType)
{
    char aName[INET_ADDRSTRLEN];
    const struct in_addr *pIn;

    if (pAddr == NULL || nType != AF_INET ||
        cbLen < (socklen_t)sizeof(struct in_addr)) {
        h_errno = HOST_NOT_FOUND;
        errno = EINVAL;
        return NULL;
    }
    pIn = (const struct in_addr *)pAddr;
    /* greppable: CGJ_NETDB_SOFT_LOOPBACK_REV */
    if (pIn->s_addr == htonl(INADDR_LOOPBACK)) {
        soft_copy_name(g_szHeName, sizeof(g_szHeName), "localhost");
        g_heAddr = *pIn;
        g_aHeAliases[0] = NULL;
        g_aHeAddrList[0] = (char *)&g_heAddr;
        g_aHeAddrList[1] = NULL;
        g_he.h_name = g_szHeName;
        g_he.h_aliases = g_aHeAliases;
        g_he.h_addrtype = AF_INET;
        g_he.h_length = (int)sizeof(struct in_addr);
        g_he.h_addr_list = g_aHeAddrList;
        h_errno = 0;
        return &g_he;
    }
    if (inet_ntop(AF_INET, pAddr, aName, sizeof(aName)) == NULL) {
        h_errno = NO_RECOVERY;
        return NULL;
    }
    return gethostbyname(aName);
}

void
setservent(int fStayopen)
{
    (void)fStayopen;
    g_iServCur = 0; /* greppable: CGJ_NETDB_SOFT_SERV_ITER */
}

void
endservent(void)
{
    g_iServCur = 0;
}

static struct servent *
soft_fill_serv(size_t i)
{
    if (g_aServ[i].szName == NULL) {
        return NULL;
    }
    soft_copy_name(g_szSeName, sizeof(g_szSeName), g_aServ[i].szName);
    soft_copy_name(g_szSeProto, sizeof(g_szSeProto), g_aServ[i].szProto);
    g_aSeAliases[0] = NULL;
    g_se.s_name = g_szSeName;
    g_se.s_aliases = g_aSeAliases;
    g_se.s_port = (int)htons((uint16_t)g_aServ[i].nPort);
    g_se.s_proto = g_szSeProto;
    return &g_se;
}

struct servent *
getservent(void)
{
    struct servent *p;

    p = soft_fill_serv(g_iServCur);
    if (p != NULL) {
        g_iServCur++;
    }
    return p;
}

struct servent *
getservbyname(const char *szName, const char *szProto)
{
    size_t i;

    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (i = 0; g_aServ[i].szName != NULL; i++) {
        /* greppable: CGJ_NETDB_SOFT_CASEFOLD */
        if (!soft_eq_ci(g_aServ[i].szName, szName)) {
            continue;
        }
        if (szProto != NULL && !soft_eq_ci(g_aServ[i].szProto, szProto)) {
            continue;
        }
        return soft_fill_serv(i);
    }
    return NULL;
}

struct servent *
getservbyport(int nPort, const char *szProto)
{
    size_t i;
    int nHost = (int)ntohs((uint16_t)nPort);

    for (i = 0; g_aServ[i].szName != NULL; i++) {
        if (g_aServ[i].nPort != nHost) {
            continue;
        }
        if (szProto != NULL && !soft_eq_ci(g_aServ[i].szProto, szProto)) {
            continue;
        }
        return soft_fill_serv(i);
    }
    return NULL;
}

void
setprotoent(int fStayopen)
{
    (void)fStayopen;
    g_iProtoCur = 0;
}

void
endprotoent(void)
{
    g_iProtoCur = 0;
}

static struct protoent *
soft_fill_proto(size_t i)
{
    if (g_aProto[i].szName == NULL) {
        return NULL;
    }
    soft_copy_name(g_szPeName, sizeof(g_szPeName), g_aProto[i].szName);
    g_aPeAliases[0] = NULL;
    g_pe.p_name = g_szPeName;
    g_pe.p_aliases = g_aPeAliases;
    g_pe.p_proto = g_aProto[i].nProto;
    return &g_pe;
}

struct protoent *
getprotoent(void)
{
    struct protoent *p;

    p = soft_fill_proto(g_iProtoCur);
    if (p != NULL) {
        g_iProtoCur++;
    }
    return p;
}

struct protoent *
getprotobyname(const char *szName)
{
    size_t i;

    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (i = 0; g_aProto[i].szName != NULL; i++) {
        if (!soft_eq_ci(g_aProto[i].szName, szName)) {
            continue;
        }
        return soft_fill_proto(i);
    }
    return NULL;
}

struct protoent *
getprotobynumber(int nProto)
{
    size_t i;

    for (i = 0; g_aProto[i].szName != NULL; i++) {
        if (g_aProto[i].nProto == nProto) {
            return soft_fill_proto(i);
        }
    }
    return NULL;
}

void
herror(const char *szStr)
{
    const char *pMsg;

    switch (h_errno) {
    case HOST_NOT_FOUND:
        pMsg = "Unknown host";
        break;
    case TRY_AGAIN:
        pMsg = "Temporary failure in name resolution";
        break;
    case NO_RECOVERY:
        pMsg = "Non-recoverable failure in name resolution";
        break;
    case NO_DATA:
        pMsg = "No address associated with name";
        break;
    default:
        pMsg = "Unknown resolver error";
        break;
    }
    if (szStr != NULL && szStr[0] != '\0') {
        (void)write(2, szStr, strlen(szStr));
        (void)write(2, ": ", 2);
    }
    (void)write(2, pMsg, strlen(pMsg));
    (void)write(2, "\n", 1);
}

const char *
hstrerror(int nErr)
{
    switch (nErr) {
    case 0:
        return "Success";
    case HOST_NOT_FOUND:
        return "Unknown host";
    case TRY_AGAIN:
        return "Temporary failure in name resolution";
    case NO_RECOVERY:
        return "Non-recoverable failure in name resolution";
    case NO_DATA:
        return "No address associated with name";
    default:
        return "Unknown resolver error";
    }
}
