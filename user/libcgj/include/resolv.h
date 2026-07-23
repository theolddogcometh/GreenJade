/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room resolv stub (no real DNS). Not GNU glibc.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct __res_state {
    int retrans;
    int retry;
    unsigned long options;
    int nscount;
    /* bring-up: rest unused */
    char _pad[256];
};

#define RES_INIT     0x00000001
#define RES_DEBUG    0x00000002
#define RES_USEVC    0x00000008
#define RES_DEFAULT  0x00000000

extern struct __res_state _res;
struct __res_state *__res_state(void);

int  res_init(void);
int  res_query(const char *szDname, int nClass, int nType, unsigned char *pAns,
               int nAnslen);
int  res_search(const char *szDname, int nClass, int nType, unsigned char *pAns,
                int nAnslen);
int  __res_init(void);
int  __res_ninit(void *pState);
int  __res_query(const char *szDname, int nClass, int nType, unsigned char *pAns,
                 int nAnslen);
int  res_ninit(void *pState);
int  res_nclose(void *pState);
int  res_nquery(void *pState, const char *szDname, int nClass, int nType,
                unsigned char *pAns, int nAnslen);
int  res_nsearch(void *pState, const char *szDname, int nClass, int nType,
                 unsigned char *pAns, int nAnslen);
int  __res_nquery(void *pState, const char *szDname, int nClass, int nType,
                  unsigned char *pAns, int nAnslen);
int  __res_nsearch(void *pState, const char *szDname, int nClass, int nType,
                   unsigned char *pAns, int nAnslen);
int  res_mkquery(int nOp, const char *szDname, int nClass, int nType,
                 const unsigned char *pData, int nDatalen,
                 const unsigned char *pNew, unsigned char *pBuf, int nBuflen);
int  res_send(const unsigned char *pMsg, int nMsglen, unsigned char *pAns,
              int nAnslen);

unsigned ns_get16(const unsigned char *p);
unsigned long ns_get32(const unsigned char *p);
void ns_put16(unsigned u, unsigned char *p);
void ns_put32(unsigned long u, unsigned char *p);
int  ns_initparse(const unsigned char *pMsg, int nLen, void *pHandle);
int  ns_parserr(void *pHandle, int nSection, int nRr, void *pRr);
int  ns_name_uncompress(const unsigned char *pMsg, const unsigned char *pEom,
                        const unsigned char *pSrc, char *szDst, size_t cbDst);
int  ns_name_compress(const char *szSrc, unsigned char *pDst, size_t cbDst,
                      const unsigned char **ppDnptrs,
                      const unsigned char **ppLastdnptr);
int  ns_name_ntop(const unsigned char *pSrc, char *szDst, size_t cbDst);
int  ns_name_pton(const char *szSrc, unsigned char *pDst, size_t cbDst);

int  dn_expand(const unsigned char *pMsg, const unsigned char *pEomOrig,
               const unsigned char *pComp, char *szExp, int nExp);
int  dn_comp(const char *szExp, unsigned char *pComp, int nComp,
             unsigned char **ppDnptrs, unsigned char **ppLastdnptr);
int  dn_skipname(const unsigned char *pComp, const unsigned char *pEom);
int  __dn_expand(const unsigned char *pMsg, const unsigned char *pEomOrig,
                 const unsigned char *pComp, char *szExp, int nExp);
int  __dn_comp(const char *szExp, unsigned char *pComp, int nComp,
               unsigned char **ppDnptrs, unsigned char **ppLastdnptr);
int  __dn_skipname(const unsigned char *pComp, const unsigned char *pEom);

#ifdef __cplusplus
}
#endif
