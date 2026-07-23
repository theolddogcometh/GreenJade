/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped netdb.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AI_PASSIVE     0x0001
#define AI_CANONNAME   0x0002
#define AI_NUMERICHOST 0x0004
#define AI_NUMERICSERV 0x0400

#define EAI_BADFLAGS  -1
#define EAI_NONAME    -2
#define EAI_AGAIN     -3
#define EAI_FAIL      -4
#define EAI_FAMILY    -6
#define EAI_MEMORY    -10
#define EAI_SYSTEM    -11
#define EAI_OVERFLOW  -12
#define EAI_NODATA    -5
#define EAI_SERVICE   -8
#define EAI_SOCKTYPE  -7

struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};

int getaddrinfo(const char *szNode, const char *szService,
                const struct addrinfo *pHints, struct addrinfo **ppRes);
void freeaddrinfo(struct addrinfo *pRes);
const char *gai_strerror(int nErr);
int getnameinfo(const struct sockaddr *pSa, socklen_t cbSa, char *szHost,
                socklen_t cbHost, char *szServ, socklen_t cbServ, int nFlags);

/* Legacy resolver surface (glibc-shaped) */
#define HOST_NOT_FOUND 1
#define TRY_AGAIN      2
#define NO_RECOVERY    3
#define NO_DATA        4
#define NO_ADDRESS     NO_DATA

extern int h_errno;
int *__h_errno_location(void);

struct hostent {
    char  *h_name;
    char **h_aliases;
    int    h_addrtype;
    int    h_length;
    char **h_addr_list;
};
#define h_addr h_addr_list[0]

struct servent {
    char  *s_name;
    char **s_aliases;
    int    s_port;
    char  *s_proto;
};

struct protoent {
    char  *p_name;
    char **p_aliases;
    int    p_proto;
};

struct netent {
    char          *n_name;
    char         **n_aliases;
    int            n_addrtype;
    uint32_t       n_net;
};

struct hostent  *gethostbyname(const char *szName);
struct hostent  *gethostbyname2(const char *szName, int nAf);
struct hostent  *gethostbyaddr(const void *pAddr, socklen_t cbLen, int nType);
struct hostent  *gethostent(void);
void             sethostent(int fStayopen);
void             endhostent(void);
int              gethostbyname_r(const char *szName, struct hostent *pResult,
                                 char *szBuf, size_t cb,
                                 struct hostent **ppResult, int *pHErr);
int              gethostbyname2_r(const char *szName, int nAf,
                                  struct hostent *pResult, char *szBuf,
                                  size_t cb, struct hostent **ppResult,
                                  int *pHErr);
int              gethostbyaddr_r(const void *pAddr, socklen_t cbLen, int nType,
                                 struct hostent *pResult, char *szBuf, size_t cb,
                                 struct hostent **ppResult, int *pHErr);
int              gethostent_r(struct hostent *pResult, char *szBuf, size_t cb,
                              struct hostent **ppResult, int *pHErr);
struct servent  *getservbyname(const char *szName, const char *szProto);
struct servent  *getservbyport(int nPort, const char *szProto);
struct servent  *getservent(void);
void             setservent(int fStayopen);
void             endservent(void);
int              getservbyname_r(const char *szName, const char *szProto,
                                 struct servent *pResult, char *szBuf, size_t cb,
                                 struct servent **ppResult);
int              getservbyport_r(int nPort, const char *szProto,
                                 struct servent *pResult, char *szBuf, size_t cb,
                                 struct servent **ppResult);
int              getservent_r(struct servent *pResult, char *szBuf, size_t cb,
                              struct servent **ppResult);
struct protoent *getprotobyname(const char *szName);
struct protoent *getprotobynumber(int nProto);
struct protoent *getprotoent(void);
void             setprotoent(int fStayopen);
void             endprotoent(void);
int              getprotobyname_r(const char *szName, struct protoent *pResult,
                                  char *szBuf, size_t cb,
                                  struct protoent **ppResult);
int              getprotobynumber_r(int nProto, struct protoent *pResult,
                                    char *szBuf, size_t cb,
                                    struct protoent **ppResult);
int              getprotoent_r(struct protoent *pResult, char *szBuf, size_t cb,
                               struct protoent **ppResult);
struct netent   *getnetent(void);
struct netent   *getnetbyname(const char *szName);
struct netent   *getnetbyaddr(uint32_t uNet, int nType);
void             setnetent(int fStayopen);
void             endnetent(void);
int              getnetent_r(struct netent *pResult, char *szBuf, size_t cb,
                             struct netent **ppResult, int *pHErr);
int              getnetbyname_r(const char *szName, struct netent *pResult,
                                char *szBuf, size_t cb, struct netent **ppResult,
                                int *pHErr);
int              getnetbyaddr_r(uint32_t uNet, int nType, struct netent *pResult,
                                char *szBuf, size_t cb, struct netent **ppResult,
                                int *pHErr);
void             herror(const char *szStr);
const char      *hstrerror(int nErr);

#ifdef __cplusplus
}
#endif
