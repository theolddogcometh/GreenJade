/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room getaddrinfo (numeric / localhost / soft services). Not GNU glibc.
 *
 * greppable: CGJ_GAI_SOFT_FLAGS
 * greppable: CGJ_GAI_SOFT_SERV
 * greppable: CGJ_GAI_SOFT_V6_NAMEINFO
 * greppable: CGJ_GAI_SOFT_CANON
 *
 * Soft deepen: AI_NUMERICHOST/SERV/CANONNAME, well-known service names,
 * socktype→protocol defaults, AF_INET6 getnameinfo, stricter flags.
 * /etc/hosts + lab DUT 10.200.125.50 + "*" wildcard. Dual DoD B OPEN.
 *
 * greppable: CGJ_GAI_SOFT_HOSTS
 * greppable: CGJ_GAI_SOFT_STAR
 * greppable: CGJ_GAI_SOFT_V4MAPPED
 * greppable: CGJ_GAI_SOFT_REV
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

/* Lab DUT wire (AGENTS.md): 10.200.125.50. Same as if.c eth0 bring-up. */
#define GJ_GAI_DUT_V4 0x0ac87d32u

const char *
gai_strerror(int nErr)
{
    switch (nErr) {
    case 0:
        return "Success";
    case EAI_NONAME:
        return "Name or service not known";
    case EAI_AGAIN:
        return "Temporary failure in name resolution";
    case EAI_FAIL:
        return "Non-recoverable failure in name resolution";
    case EAI_NODATA:
        return "No address associated with name";
    case EAI_FAMILY:
        return "ai_family not supported";
    case EAI_MEMORY:
        return "Memory allocation failure";
    case EAI_SYSTEM:
        return "System error";
    case EAI_SERVICE:
        return "Servname not supported for ai_socktype";
    case EAI_SOCKTYPE:
        return "ai_socktype not supported";
    case EAI_BADFLAGS:
        return "Bad value for ai_flags";
    case EAI_OVERFLOW:
        return "Argument buffer overflow";
    default:
        return "Unknown error";
    }
}

void
freeaddrinfo(struct addrinfo *pRes)
{
    struct addrinfo *p;

    while (pRes != NULL) {
        p = pRes->ai_next;
        free(pRes->ai_addr);
        free(pRes->ai_canonname);
        free(pRes);
        pRes = p;
    }
}

/* greppable: CGJ_GAI_SOFT_SERV */
static const struct {
    const char *szName;
    uint16_t    uPort;
} g_aSoftServ[] = {
    { "ftp", 21 },
    { "ssh", 22 },
    { "sshd", 22 },
    { "telnet", 23 },
    { "smtp", 25 },
    { "domain", 53 },
    { "tftp", 69 },
    { "http", 80 },
    { "pop3", 110 },
    { "nntp", 119 },
    { "ntp", 123 },
    { "imap", 143 },
    { "snmp", 161 },
    { "https", 443 },
    { "syslog", 514 },
    { "shell", 514 },
    { "login", 513 },
    { "exec", 512 },
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

/* greppable: CGJ_GAI_SOFT_ETC_SERVICES */
static int
services_lookup(const char *szName, uint16_t *pPort)
{
    FILE *pF;
    char aLine[128];

    if (szName == NULL || pPort == NULL) {
        return -1;
    }
    pF = fopen("/etc/services", "r");
    if (pF == NULL) {
        return -1;
    }
    while (fgets(aLine, (int)sizeof(aLine), pF) != NULL) {
        char *p;
        char *szTok;
        unsigned long v;
        char *pEnd;

        p = aLine;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '#' || *p == '\n') {
            continue;
        }
        szTok = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') {
            p++;
        }
        if (*p != '\0') {
            *p++ = '\0';
        }
        if (!soft_eq_ci(szTok, szName)) {
            continue;
        }
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        v = strtoul(p, &pEnd, 10);
        if (pEnd != p && v <= 65535ul && (*pEnd == '/' || *pEnd == '\0' ||
                                          *pEnd == '\n' || *pEnd == ' ' ||
                                          *pEnd == '\t')) {
            *pPort = (uint16_t)v;
            (void)fclose(pF);
            return 0;
        }
    }
    (void)fclose(pF);
    return -1;
}

static int
parse_port(const char *szService, uint16_t *pPort, int fNumericOnly)
{
    unsigned long v;
    char *pEnd;
    size_t i;

    if (szService == NULL || szService[0] == '\0') {
        *pPort = 0;
        return 0;
    }
    v = strtoul(szService, &pEnd, 10);
    if (pEnd != szService && *pEnd == '\0' && v <= 65535ul) {
        *pPort = (uint16_t)v;
        return 0;
    }
    if (fNumericOnly) {
        return -1;
    }
    /* greppable: CGJ_GAI_SOFT_SERV */
    for (i = 0; g_aSoftServ[i].szName != NULL; i++) {
        if (soft_eq_ci(szService, g_aSoftServ[i].szName)) {
            *pPort = g_aSoftServ[i].uPort;
            return 0;
        }
    }
    if (services_lookup(szService, pPort) == 0) {
        return 0;
    }
    return -1;
}

static int
hosts_lookup(const char *szNode, int *pf4, struct in_addr *p4,
             int *pf6, uint8_t a6[16])
{
    FILE *pF;
    char aLine[256];

    /* greppable: CGJ_GAI_SOFT_HOSTS */
    if (szNode == NULL || szNode[0] == '\0' || pf4 == NULL || p4 == NULL ||
        pf6 == NULL || a6 == NULL) {
        return 0;
    }
    pF = fopen("/etc/hosts", "r");
    if (pF == NULL) {
        return 0;
    }
    while (fgets(aLine, (int)sizeof(aLine), pF) != NULL) {
        char *p;
        char *szAddr;
        char *szTok;
        struct in_addr a4;
        uint8_t aTmp6[16];

        p = aLine;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '#' || *p == '\n') {
            continue;
        }
        szAddr = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') {
            p++;
        }
        if (*p != '\0') {
            *p++ = '\0';
        }
        if (inet_pton(AF_INET, szAddr, &a4) != 1 &&
            inet_pton(AF_INET6, szAddr, aTmp6) != 1) {
            continue;
        }
        while (*p != '\0') {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            if (*p == '\0' || *p == '#' || *p == '\n') {
                break;
            }
            szTok = p;
            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') {
                p++;
            }
            if (*p != '\0') {
                *p++ = '\0';
            }
            if (soft_eq_ci(szTok, szNode)) {
                if (inet_pton(AF_INET, szAddr, &a4) == 1) {
                    *p4 = a4;
                    *pf4 = 1;
                } else if (inet_pton(AF_INET6, szAddr, a6) == 1) {
                    *pf6 = 1;
                }
                (void)fclose(pF);
                return 1;
            }
        }
    }
    (void)fclose(pF);
    return 0;
}

static int
hosts_reverse(int nFamily, const void *pAddr, char *szHost, size_t cbHost)
{
    FILE *pF;
    char aLine[256];
    char aNum[INET6_ADDRSTRLEN];

    /* greppable: CGJ_GAI_SOFT_REV */
    if (pAddr == NULL || szHost == NULL || cbHost == 0) {
        return -1;
    }
    if (inet_ntop(nFamily, pAddr, aNum, sizeof(aNum)) == NULL) {
        return -1;
    }
    pF = fopen("/etc/hosts", "r");
    if (pF == NULL) {
        return -1;
    }
    while (fgets(aLine, (int)sizeof(aLine), pF) != NULL) {
        char *p;
        char *szAddr;
        char *szTok;

        p = aLine;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '#' || *p == '\n') {
            continue;
        }
        szAddr = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') {
            p++;
        }
        if (*p != '\0') {
            *p++ = '\0';
        }
        if (!soft_eq_ci(szAddr, aNum)) {
            continue;
        }
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '#' || *p == '\n') {
            continue;
        }
        szTok = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') {
            p++;
        }
        *p = '\0';
        {
            size_t n = strlen(szTok);

            if (n + 1 > cbHost) {
                (void)fclose(pF);
                return -1;
            }
            memcpy(szHost, szTok, n + 1);
        }
        (void)fclose(pF);
        return 0;
    }
    (void)fclose(pF);
    return -1;
}

static int
soft_is_local(const char *szNode)
{
    if (szNode == NULL) {
        return 0;
    }
    if (soft_eq_ci(szNode, "localhost") || soft_eq_ci(szNode, "localhost.") ||
        soft_eq_ci(szNode, "ip6-localhost") || soft_eq_ci(szNode, "ip6-loopback")) {
        return 1;
    }
    return 0;
}

static void
soft_proto_from_sock(int nSock, int *pProto)
{
    if (*pProto != 0) {
        return;
    }
    if (nSock == SOCK_STREAM) {
        *pProto = IPPROTO_TCP;
    } else if (nSock == SOCK_DGRAM) {
        *pProto = IPPROTO_UDP;
    }
}

static struct addrinfo *
soft_make_ai(int nFamily, int nSock, int nProto, uint16_t uPort,
             const void *pAddr, size_t cbAddr, const char *szCanon)
{
    struct addrinfo *pAi;
    void *pSa;

    pAi = (struct addrinfo *)calloc(1, sizeof(*pAi));
    pSa = calloc(1, cbAddr);
    if (pAi == NULL || pSa == NULL) {
        free(pAi);
        free(pSa);
        return NULL;
    }
    if (nFamily == AF_INET) {
        struct sockaddr_in *pSin = (struct sockaddr_in *)pSa;

        pSin->sin_family = AF_INET;
        pSin->sin_port = htons(uPort);
        memcpy(&pSin->sin_addr, pAddr, sizeof(struct in_addr));
    } else {
        struct sockaddr_in6 *p6 = (struct sockaddr_in6 *)pSa;

        p6->sin6_family = AF_INET6;
        p6->sin6_port = htons(uPort);
        memcpy(p6->sin6_addr.s6_addr, pAddr, 16);
    }
    pAi->ai_family = nFamily;
    pAi->ai_socktype = nSock;
    pAi->ai_protocol = nProto;
    pAi->ai_addrlen = (socklen_t)cbAddr;
    pAi->ai_addr = (struct sockaddr *)pSa;
    if (szCanon != NULL) {
        size_t n = strlen(szCanon);
        char *pC = (char *)malloc(n + 1);

        if (pC == NULL) {
            free(pAi->ai_addr);
            free(pAi);
            return NULL;
        }
        memcpy(pC, szCanon, n + 1);
        pAi->ai_canonname = pC; /* greppable: CGJ_GAI_SOFT_CANON */
    }
    return pAi;
}

int
getaddrinfo(const char *szNode, const char *szService,
            const struct addrinfo *pHints, struct addrinfo **ppRes)
{
    uint16_t uPort = 0;
    int nFamily = AF_UNSPEC;
    int nSock = SOCK_STREAM;
    int nProto = 0;
    int fPassive = 0;
    int fNumericHost = 0;
    int fNumericServ = 0;
    int fCanon = 0;
    int nFlags = 0;
    const char *szCanon = NULL;
    struct in_addr a4;
    uint8_t a6[16];
    int fHave4 = 0;
    int fHave6 = 0;
    struct addrinfo *pHead = NULL;
    struct addrinfo *pTail = NULL;

    /* Numeric, localhost, AI_PASSIVE wildcard — no DNS client in libc. */
    if (ppRes == NULL) {
        errno = EINVAL;
        return EAI_SYSTEM;
    }
    *ppRes = NULL;
    if (szNode == NULL && szService == NULL) {
        return EAI_NONAME;
    }
    if (pHints != NULL) {
        /* greppable: CGJ_GAI_SOFT_FLAGS */
        nFlags = pHints->ai_flags;
        if (nFlags & ~(AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST |
                       AI_NUMERICSERV | AI_V4MAPPED | AI_ALL |
                       AI_ADDRCONFIG)) {
            return EAI_BADFLAGS;
        }
        if (pHints->ai_family != AF_UNSPEC && pHints->ai_family != AF_INET &&
            pHints->ai_family != AF_INET6) {
            return EAI_FAMILY;
        }
        nFamily = pHints->ai_family;
        if (pHints->ai_socktype != 0) {
            nSock = pHints->ai_socktype;
        }
        if (pHints->ai_protocol != 0) {
            nProto = pHints->ai_protocol;
        }
        if (nFlags & AI_PASSIVE) {
            fPassive = 1;
        }
        if (nFlags & AI_NUMERICHOST) {
            fNumericHost = 1;
        }
        if (nFlags & AI_NUMERICSERV) {
            fNumericServ = 1;
        }
        if (nFlags & AI_CANONNAME) {
            fCanon = 1;
        }
    }
    if (nSock != 0 && nSock != SOCK_STREAM && nSock != SOCK_DGRAM &&
        nSock != SOCK_RAW) {
        return EAI_SOCKTYPE;
    }
    soft_proto_from_sock(nSock, &nProto);
    if (parse_port(szService, &uPort, fNumericServ) != 0) {
        return EAI_SERVICE;
    }

    memset(a6, 0, sizeof(a6));
    memset(&a4, 0, sizeof(a4));

    if (szNode == NULL) {
        if (fPassive) {
            a4.s_addr = htonl(INADDR_ANY);
            fHave4 = 1;
            /* :: */
            fHave6 = 1;
        } else {
            a4.s_addr = htonl(INADDR_LOOPBACK);
            fHave4 = 1;
            a6[15] = 1; /* ::1 */
            fHave6 = 1;
        }
        if (fCanon) {
            szCanon = "localhost";
        }
    } else if (inet_pton(AF_INET, szNode, &a4) == 1) {
        fHave4 = 1;
        if (fCanon) {
            szCanon = szNode;
        }
    } else if (inet_pton(AF_INET6, szNode, a6) == 1) {
        fHave6 = 1;
        if (fCanon) {
            szCanon = szNode;
        }
    } else if (!fNumericHost && szNode[0] == '*' && szNode[1] == '\0') {
        /* greppable: CGJ_GAI_SOFT_STAR */
        a4.s_addr = htonl(INADDR_ANY);
        fHave4 = 1;
        fHave6 = 1;
    } else if (!fNumericHost &&
               hosts_lookup(szNode, &fHave4, &a4, &fHave6, a6)) {
        if (fCanon) {
            szCanon = szNode;
        }
    } else if (!fNumericHost && soft_is_local(szNode)) {
        a4.s_addr = htonl(INADDR_LOOPBACK);
        fHave4 = 1;
        a6[15] = 1;
        fHave6 = 1;
        if (fCanon) {
            szCanon = "localhost";
        }
    } else if (!fNumericHost) {
        char aHost[256];

        if (gethostname(aHost, sizeof(aHost)) == 0 &&
            aHost[0] != '\0' && soft_eq_ci(szNode, aHost)) {
            a4.s_addr = htonl(GJ_GAI_DUT_V4);
            fHave4 = 1;
            if (fCanon) {
                szCanon = szNode;
            }
        } else {
            return EAI_NONAME; /* no DNS client in libc */
        }
    } else {
        return EAI_NONAME;
    }

    if (nFamily == AF_INET) {
        fHave6 = 0;
    } else if (nFamily == AF_INET6) {
        /* greppable: CGJ_GAI_SOFT_V4MAPPED */
        if ((nFlags & AI_V4MAPPED) != 0 && fHave4) {
            if (!fHave6 || (nFlags & AI_ALL) != 0) {
                uint32_t uV4 = a4.s_addr;

                memset(a6, 0, sizeof(a6));
                a6[10] = 0xff;
                a6[11] = 0xff;
                memcpy(a6 + 12, &uV4, 4);
                fHave6 = 1;
            }
            if ((nFlags & AI_ALL) == 0) {
                fHave4 = 0;
            }
        } else {
            fHave4 = 0;
        }
    }
    /* AI_ADDRCONFIG: drop families the host cannot open a datagram on. */
    if ((nFlags & AI_ADDRCONFIG) != 0) {
        int nProbe;

        if (fHave4) {
            nProbe = socket(AF_INET, SOCK_DGRAM, 0);
            if (nProbe < 0) {
                fHave4 = 0;
            } else {
                (void)close(nProbe);
            }
        }
        if (fHave6) {
            nProbe = socket(AF_INET6, SOCK_DGRAM, 0);
            if (nProbe < 0) {
                fHave6 = 0;
            } else {
                (void)close(nProbe);
            }
        }
    }

    if (!fHave4 && !fHave6) {
        return EAI_NONAME;
    }

    if (fHave4) {
        struct addrinfo *pAi = soft_make_ai(AF_INET, nSock, nProto, uPort, &a4,
                                            sizeof(struct sockaddr_in),
                                            fCanon ? szCanon : NULL);

        if (pAi == NULL) {
            freeaddrinfo(pHead);
            return EAI_MEMORY;
        }
        pHead = pTail = pAi;
        fCanon = 0; /* canon only on first */
        szCanon = NULL;
    }
    if (fHave6) {
        struct addrinfo *pAi = soft_make_ai(AF_INET6, nSock, nProto, uPort, a6,
                                            sizeof(struct sockaddr_in6),
                                            fCanon ? szCanon : NULL);

        if (pAi == NULL) {
            freeaddrinfo(pHead);
            return EAI_MEMORY;
        }
        if (pTail != NULL) {
            pTail->ai_next = pAi;
        } else {
            pHead = pAi;
        }
        pTail = pAi;
    }
    *ppRes = pHead;
    return 0;
}

static int
fmt_u16_dec(char *sz, size_t cb, unsigned u)
{
    char t[8];
    size_t n = 0;
    unsigned x = u;

    if (cb == 0) {
        return -1;
    }
    if (x == 0) {
        t[n++] = '0';
    } else {
        char r[8];
        int k = 0;

        while (x > 0 && k < 8) {
            r[k++] = (char)('0' + (x % 10u));
            x /= 10u;
        }
        while (k > 0) {
            t[n++] = r[--k];
        }
    }
    if (n + 1 > cb) {
        return -1;
    }
    memcpy(sz, t, n);
    sz[n] = '\0';
    return 0;
}

int
getnameinfo(const struct sockaddr *pSa, socklen_t cbSa, char *szHost,
            socklen_t cbHost, char *szServ, socklen_t cbServ, int nFlags)
{
    char aNum[INET6_ADDRSTRLEN];
    const void *pAddr = NULL;
    int nFamily = 0;
    uint16_t uPort = 0;
    int fRev = 0;

    if (pSa == NULL) {
        errno = EINVAL;
        return EAI_SYSTEM;
    }
    if (pSa->sa_family == AF_INET &&
        cbSa >= (socklen_t)sizeof(struct sockaddr_in)) {
        const struct sockaddr_in *pIn = (const struct sockaddr_in *)pSa;

        nFamily = AF_INET;
        pAddr = &pIn->sin_addr;
        uPort = ntohs(pIn->sin_port);
    } else if (pSa->sa_family == AF_INET6 &&
               cbSa >= (socklen_t)sizeof(struct sockaddr_in6)) {
        const struct sockaddr_in6 *p6 = (const struct sockaddr_in6 *)pSa;

        /* greppable: CGJ_GAI_SOFT_V6_NAMEINFO */
        nFamily = AF_INET6;
        pAddr = p6->sin6_addr.s6_addr;
        uPort = ntohs(p6->sin6_port);
    } else {
        nFamily = 0;
    }
    if (nFamily == AF_INET || nFamily == AF_INET6) {
        if (szHost != NULL && cbHost > 0) {
            if ((nFlags & NI_NUMERICHOST) == 0) {
                if (hosts_reverse(nFamily, pAddr, szHost,
                                  (size_t)cbHost) == 0) {
                    fRev = 1;
                } else if (nFamily == AF_INET) {
                    const struct in_addr *pIn =
                        (const struct in_addr *)pAddr;

                    if (pIn->s_addr == htonl(INADDR_LOOPBACK)) {
                        if ((size_t)cbHost < 10) {
                            return EAI_OVERFLOW;
                        }
                        memcpy(szHost, "localhost", 10);
                        fRev = 1;
                    } else if (pIn->s_addr == htonl(GJ_GAI_DUT_V4)) {
                        if (gethostname(szHost, (size_t)cbHost) == 0 &&
                            szHost[0] != '\0') {
                            fRev = 1;
                        }
                    }
                } else if (nFamily == AF_INET6) {
                    const uint8_t *p6 = (const uint8_t *)pAddr;
                    int i;
                    int fLb = 1;

                    for (i = 0; i < 15; i++) {
                        if (p6[i] != 0) {
                            fLb = 0;
                            break;
                        }
                    }
                    if (fLb && p6[15] == 1) {
                        if ((size_t)cbHost < 10) {
                            return EAI_OVERFLOW;
                        }
                        memcpy(szHost, "localhost", 10);
                        fRev = 1;
                    }
                }
            }
            if (!fRev) {
                if ((nFlags & NI_NAMEREQD) != 0) {
                    return EAI_NONAME;
                }
                if (inet_ntop(nFamily, pAddr, aNum, sizeof(aNum)) == NULL) {
                    return EAI_SYSTEM;
                }
                {
                    size_t n = strlen(aNum);

                    if (n + 1 > (size_t)cbHost) {
                        return EAI_OVERFLOW;
                    }
                    memcpy(szHost, aNum, n + 1);
                }
            }
        }
        if (szServ != NULL && cbServ > 0) {
            int fNamed = 0;

            if ((nFlags & NI_NUMERICSERV) == 0) {
                size_t iServ;

                for (iServ = 0; g_aSoftServ[iServ].szName != NULL; iServ++) {
                    if (g_aSoftServ[iServ].uPort == uPort) {
                        size_t nN = strlen(g_aSoftServ[iServ].szName);

                        if (nN + 1 > (size_t)cbServ) {
                            return EAI_OVERFLOW;
                        }
                        memcpy(szServ, g_aSoftServ[iServ].szName, nN + 1);
                        fNamed = 1;
                        break;
                    }
                }
            }
            if (!fNamed) {
                if (fmt_u16_dec(szServ, (size_t)cbServ, uPort) != 0) {
                    return EAI_OVERFLOW;
                }
            }
        }
        return 0;
    }
    /* OpenSSH privsep / agent / mux: AF_UNIX path, no reverse DNS. */
    if (pSa->sa_family == AF_UNIX &&
        cbSa >= (socklen_t)offsetof(struct sockaddr_un, sun_path)) {
        const struct sockaddr_un *pUn = (const struct sockaddr_un *)pSa;
        size_t cbPath;
        size_t cbMax;

        cbMax = (size_t)cbSa - offsetof(struct sockaddr_un, sun_path);
        if (cbMax > sizeof(pUn->sun_path)) {
            cbMax = sizeof(pUn->sun_path);
        }
        if (szHost != NULL && cbHost > 0) {
            if (cbMax == 0 || pUn->sun_path[0] == '\0') {
                if (cbHost < 2) {
                    return EAI_OVERFLOW;
                }
                szHost[0] = '@';
                szHost[1] = '\0';
            } else {
                cbPath = strnlen(pUn->sun_path, cbMax);
                if (cbPath + 1 > (size_t)cbHost) {
                    return EAI_OVERFLOW;
                }
                memcpy(szHost, pUn->sun_path, cbPath);
                szHost[cbPath] = '\0';
            }
        }
        if (szServ != NULL && cbServ > 0) {
            szServ[0] = '\0';
        }
        return 0;
    }
    return EAI_FAMILY;
}
