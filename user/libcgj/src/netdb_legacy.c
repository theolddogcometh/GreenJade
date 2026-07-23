/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Legacy netdb: gethostbyname / getservbyname / getprotobyname bring-up.
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

static struct protoent g_pe;
static char g_szPeName[32];
static char *g_aPeAliases[1];

static const struct {
    const char *szName;
    int         nPort;
    const char *szProto;
} g_aServ[] = {
    { "ftp", 21, "tcp" },
    { "ssh", 22, "tcp" },
    { "smtp", 25, "tcp" },
    { "domain", 53, "udp" },
    { "domain", 53, "tcp" },
    { "http", 80, "tcp" },
    { "pop3", 110, "tcp" },
    { "imap", 143, "tcp" },
    { "https", 443, "tcp" },
    { "nntp", 119, "tcp" },
    { NULL, 0, NULL }
};

static const struct {
    const char *szName;
    int         nProto;
} g_aProto[] = {
    { "ip", 0 },
    { "icmp", 1 },
    { "tcp", 6 },
    { "udp", 17 },
    { "ipv6", 41 },
    { NULL, 0 }
};

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
    if (pRes->ai_addr == NULL || pRes->ai_addrlen < sizeof(struct sockaddr_in)) {
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

    if (pAddr == NULL || nType != AF_INET || cbLen < (socklen_t)sizeof(struct in_addr)) {
        h_errno = HOST_NOT_FOUND;
        errno = EINVAL;
        return NULL;
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
}

void
endservent(void)
{
}

struct servent *
getservent(void)
{
    return NULL;
}

struct servent *
getservbyname(const char *szName, const char *szProto)
{
    size_t i;
    size_t n;

    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (i = 0; g_aServ[i].szName != NULL; i++) {
        if (strcmp(g_aServ[i].szName, szName) != 0) {
            continue;
        }
        if (szProto != NULL && strcmp(g_aServ[i].szProto, szProto) != 0) {
            continue;
        }
        n = strlen(g_aServ[i].szName);
        if (n + 1 > sizeof(g_szSeName)) {
            n = sizeof(g_szSeName) - 1;
        }
        memcpy(g_szSeName, g_aServ[i].szName, n);
        g_szSeName[n] = '\0';
        n = strlen(g_aServ[i].szProto);
        if (n + 1 > sizeof(g_szSeProto)) {
            n = sizeof(g_szSeProto) - 1;
        }
        memcpy(g_szSeProto, g_aServ[i].szProto, n);
        g_szSeProto[n] = '\0';
        g_aSeAliases[0] = NULL;
        g_se.s_name = g_szSeName;
        g_se.s_aliases = g_aSeAliases;
        g_se.s_port = (int)htons((uint16_t)g_aServ[i].nPort);
        g_se.s_proto = g_szSeProto;
        return &g_se;
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
        if (szProto != NULL && strcmp(g_aServ[i].szProto, szProto) != 0) {
            continue;
        }
        return getservbyname(g_aServ[i].szName, g_aServ[i].szProto);
    }
    return NULL;
}

void
setprotoent(int fStayopen)
{
    (void)fStayopen;
}

void
endprotoent(void)
{
}

struct protoent *
getprotoent(void)
{
    return NULL;
}

struct protoent *
getprotobyname(const char *szName)
{
    size_t i;
    size_t n;

    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (i = 0; g_aProto[i].szName != NULL; i++) {
        if (strcmp(g_aProto[i].szName, szName) != 0) {
            continue;
        }
        n = strlen(g_aProto[i].szName);
        if (n + 1 > sizeof(g_szPeName)) {
            n = sizeof(g_szPeName) - 1;
        }
        memcpy(g_szPeName, g_aProto[i].szName, n);
        g_szPeName[n] = '\0';
        g_aPeAliases[0] = NULL;
        g_pe.p_name = g_szPeName;
        g_pe.p_aliases = g_aPeAliases;
        g_pe.p_proto = g_aProto[i].nProto;
        return &g_pe;
    }
    return NULL;
}

struct protoent *
getprotobynumber(int nProto)
{
    size_t i;

    for (i = 0; g_aProto[i].szName != NULL; i++) {
        if (g_aProto[i].nProto == nProto) {
            return getprotobyname(g_aProto[i].szName);
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
