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
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

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
    { "smtp", 25 },
    { "domain", 53 },
    { "http", 80 },
    { "pop3", 110 },
    { "nntp", 119 },
    { "imap", 143 },
    { "https", 443 },
    { "syslog", 514 },
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
                       AI_NUMERICSERV)) {
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
    } else if (!fNumericHost && soft_is_local(szNode)) {
        a4.s_addr = htonl(INADDR_LOOPBACK);
        fHave4 = 1;
        a6[15] = 1;
        fHave6 = 1;
        if (fCanon) {
            szCanon = "localhost";
        }
    } else {
        return fNumericHost ? EAI_NONAME : EAI_NONAME; /* no DNS soft path */
    }

    if (nFamily == AF_INET) {
        fHave6 = 0;
    } else if (nFamily == AF_INET6) {
        fHave4 = 0;
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
    (void)nFlags;
    if (pSa == NULL) {
        errno = EINVAL;
        return EAI_SYSTEM;
    }
    if (pSa->sa_family == AF_INET &&
        cbSa >= (socklen_t)sizeof(struct sockaddr_in)) {
        const struct sockaddr_in *pIn = (const struct sockaddr_in *)pSa;

        if (szHost != NULL && cbHost > 0) {
            if (inet_ntop(AF_INET, &pIn->sin_addr, szHost, cbHost) == NULL) {
                return EAI_OVERFLOW;
            }
        }
        if (szServ != NULL && cbServ > 0) {
            if (fmt_u16_dec(szServ, (size_t)cbServ, ntohs(pIn->sin_port)) !=
                0) {
                return EAI_OVERFLOW;
            }
        }
        return 0;
    }
    /* greppable: CGJ_GAI_SOFT_V6_NAMEINFO */
    if (pSa->sa_family == AF_INET6 &&
        cbSa >= (socklen_t)sizeof(struct sockaddr_in6)) {
        const struct sockaddr_in6 *p6 = (const struct sockaddr_in6 *)pSa;

        if (szHost != NULL && cbHost > 0) {
            if (inet_ntop(AF_INET6, p6->sin6_addr.s6_addr, szHost, cbHost) ==
                NULL) {
                return EAI_OVERFLOW;
            }
        }
        if (szServ != NULL && cbServ > 0) {
            if (fmt_u16_dec(szServ, (size_t)cbServ, ntohs(p6->sin6_port)) !=
                0) {
                return EAI_OVERFLOW;
            }
        }
        return 0;
    }
    return EAI_FAMILY;
}
