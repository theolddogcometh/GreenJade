/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * getsubopt + getgrouplist — desktop link surface.
 */
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int
getsubopt(char **ppOptionp, char *const *ppTokens, char **ppValuep)
{
    char *pOpt;
    char *pEq;
    char *pComma;
    size_t nKey;
    int i;

    if (ppOptionp == NULL || *ppOptionp == NULL || ppTokens == NULL) {
        return -1;
    }
    if (ppValuep != NULL) {
        *ppValuep = NULL;
    }
    pOpt = *ppOptionp;
    if (*pOpt == '\0') {
        return -1;
    }
    pComma = strchr(pOpt, ',');
    if (pComma != NULL) {
        *pComma = '\0';
        *ppOptionp = pComma + 1;
    } else {
        *ppOptionp = pOpt + strlen(pOpt);
    }
    pEq = strchr(pOpt, '=');
    if (pEq != NULL) {
        *pEq = '\0';
        if (ppValuep != NULL) {
            *ppValuep = pEq + 1;
        }
    }
    nKey = strlen(pOpt);
    for (i = 0; ppTokens[i] != NULL; i++) {
        if (strncmp(pOpt, ppTokens[i], nKey) == 0 &&
            ppTokens[i][nKey] == '\0') {
            return i;
        }
    }
    /* Unknown token: leave *ppOptionp already advanced; return -1 */
    if (ppValuep != NULL) {
        *ppValuep = pOpt;
    }
    return -1;
}

int
getgrouplist(const char *szUser, gid_t gid, gid_t *pGroups, int *pNgids)
{
    int nNeed = 1;

    (void)szUser; /* bring-up: no /etc/group */
    if (pNgids == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (*pNgids < nNeed) {
        *pNgids = nNeed;
        return -1;
    }
    if (pGroups != NULL) {
        pGroups[0] = gid;
    }
    *pNgids = nNeed;
    return nNeed;
}
