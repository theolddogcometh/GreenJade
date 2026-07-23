/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * resolv stubs — apps link; name resolution via getaddrinfo numeric only.
 */
#include <errno.h>
#include <resolv.h>
#include <string.h>

struct __res_state _res;

int
res_init(void)
{
    memset(&_res, 0, sizeof(_res));
    _res.options = RES_INIT;
    _res.retrans = 5;
    _res.retry = 2;
    _res.nscount = 0;
    return 0;
}

int
res_query(const char *szDname, int nClass, int nType, unsigned char *pAns,
          int nAnslen)
{
    (void)szDname;
    (void)nClass;
    (void)nType;
    (void)pAns;
    (void)nAnslen;
    errno = ENOSYS;
    return -1;
}

int
res_search(const char *szDname, int nClass, int nType, unsigned char *pAns,
           int nAnslen)
{
    return res_query(szDname, nClass, nType, pAns, nAnslen);
}
