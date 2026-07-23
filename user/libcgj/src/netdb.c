/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room getaddrinfo (numeric / localhost bring-up). Not GNU glibc.
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

static int
parse_port(const char *szService, uint16_t *pPort)
{
    unsigned long v;
    char *pEnd;

    if (szService == NULL || szService[0] == '\0') {
        *pPort = 0;
        return 0;
    }
    v = strtoul(szService, &pEnd, 10);
    if (pEnd == szService || *pEnd != '\0' || v > 65535ul) {
        return -1;
    }
    *pPort = (uint16_t)v;
    return 0;
}

int
getaddrinfo(const char *szNode, const char *szService,
            const struct addrinfo *pHints, struct addrinfo **ppRes)
{
    struct addrinfo *pAi;
    struct sockaddr_in *pSin;
    struct in_addr a;
    uint16_t uPort = 0;
    int nFamily = AF_INET;
    int nSock = SOCK_STREAM;
    int nProto = 0;
    int fPassive = 0;

    if (ppRes == NULL) {
        return EAI_SYSTEM;
    }
    *ppRes = NULL;
    if (szNode == NULL && szService == NULL) {
        return EAI_NONAME;
    }
    if (pHints != NULL) {
        if (pHints->ai_family != AF_UNSPEC && pHints->ai_family != AF_INET &&
            pHints->ai_family != AF_INET6) {
            return EAI_FAMILY;
        }
        if (pHints->ai_family == AF_INET6) {
            /* bring-up: IPv6 only for numeric :: / ::1 via inet_pton path later */
            nFamily = AF_INET6;
        } else if (pHints->ai_family == AF_INET) {
            nFamily = AF_INET;
        }
        if (pHints->ai_socktype != 0) {
            nSock = pHints->ai_socktype;
        }
        if (pHints->ai_protocol != 0) {
            nProto = pHints->ai_protocol;
        }
        if (pHints->ai_flags & AI_PASSIVE) {
            fPassive = 1;
        }
    }
    if (parse_port(szService, &uPort) != 0) {
        return EAI_SERVICE;
    }

    /* Resolve node: numeric, localhost, or passive ANY */
    if (nFamily == AF_INET6) {
        uint8_t a6[16];
        struct sockaddr_in6 *p6;

        memset(a6, 0, sizeof(a6));
        if (szNode == NULL) {
            if (!fPassive) {
                a6[15] = 1; /* ::1 */
            }
        } else if (inet_pton(AF_INET6, szNode, a6) != 1) {
            if (strcmp(szNode, "localhost") == 0) {
                a6[15] = 1;
            } else {
                return EAI_NONAME; /* no DNS in bring-up */
            }
        }
        pAi = (struct addrinfo *)calloc(1, sizeof(*pAi));
        p6 = (struct sockaddr_in6 *)calloc(1, sizeof(*p6));
        if (pAi == NULL || p6 == NULL) {
            free(pAi);
            free(p6);
            return EAI_MEMORY;
        }
        p6->sin6_family = AF_INET6;
        p6->sin6_port = htons(uPort);
        memcpy(p6->sin6_addr.s6_addr, a6, 16);
        pAi->ai_family = AF_INET6;
        pAi->ai_socktype = nSock;
        pAi->ai_protocol = nProto;
        pAi->ai_addrlen = sizeof(*p6);
        pAi->ai_addr = (struct sockaddr *)p6;
        *ppRes = pAi;
        return 0;
    }

    if (szNode == NULL) {
        a.s_addr = fPassive ? htonl(INADDR_ANY) : htonl(INADDR_LOOPBACK);
    } else if (inet_aton(szNode, &a) == 0) {
        if (strcmp(szNode, "localhost") == 0) {
            a.s_addr = htonl(INADDR_LOOPBACK);
        } else {
            return EAI_NONAME;
        }
    }

    pAi = (struct addrinfo *)calloc(1, sizeof(*pAi));
    pSin = (struct sockaddr_in *)calloc(1, sizeof(*pSin));
    if (pAi == NULL || pSin == NULL) {
        free(pAi);
        free(pSin);
        return EAI_MEMORY;
    }
    pSin->sin_family = AF_INET;
    pSin->sin_port = htons(uPort);
    pSin->sin_addr = a;
    pAi->ai_family = AF_INET;
    pAi->ai_socktype = nSock;
    pAi->ai_protocol = nProto;
    pAi->ai_addrlen = sizeof(*pSin);
    pAi->ai_addr = (struct sockaddr *)pSin;
    *ppRes = pAi;
    return 0;
}

int
getnameinfo(const struct sockaddr *pSa, socklen_t cbSa, char *szHost,
            socklen_t cbHost, char *szServ, socklen_t cbServ, int nFlags)
{
    (void)nFlags;
    if (pSa == NULL) {
        return EAI_SYSTEM;
    }
    if (pSa->sa_family == AF_INET && cbSa >= (socklen_t)sizeof(struct sockaddr_in)) {
        const struct sockaddr_in *pIn = (const struct sockaddr_in *)pSa;

        if (szHost != NULL && cbHost > 0) {
            if (inet_ntop(AF_INET, &pIn->sin_addr, szHost, cbHost) == NULL) {
                return EAI_OVERFLOW;
            }
        }
        if (szServ != NULL && cbServ > 0) {
            unsigned p = ntohs(pIn->sin_port);
            char a[8];
            size_t n = 0;
            unsigned x = p;

            if (x == 0) {
                a[n++] = '0';
            } else {
                char t[8];
                int k = 0;

                while (x > 0) {
                    t[k++] = (char)('0' + (x % 10u));
                    x /= 10u;
                }
                while (k > 0) {
                    a[n++] = t[--k];
                }
            }
            a[n] = '\0';
            if (n + 1 > (size_t)cbServ) {
                return EAI_OVERFLOW;
            }
            memcpy(szServ, a, n + 1);
        }
        return 0;
    }
    return EAI_FAMILY;
}
