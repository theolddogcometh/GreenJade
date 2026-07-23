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
    /* Skip empty segments (,,) soft deepen. */
    while (*pOpt == ',') {
        pOpt++;
    }
    if (*pOpt == '\0') {
        *ppOptionp = pOpt;
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
    if (nKey == 0) {
        /* "=value" with empty key: unknown */
        if (ppValuep != NULL && *ppValuep == NULL) {
            *ppValuep = pOpt;
        }
        return -1;
    }
    for (i = 0; ppTokens[i] != NULL; i++) {
        if (ppTokens[i][0] == '\0') {
            continue;
        }
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
    gid_t aExtra[16];
    int nExtra = 0;
    int i;

    (void)szUser; /* bring-up: no /etc/group; soft primary + getgroups */
    if (pNgids == NULL) {
        errno = EINVAL;
        return -1;
    }
    /* Soft deepen: include supplementary groups from the kernel when present. */
    nExtra = getgroups((int)(sizeof(aExtra) / sizeof(aExtra[0])), aExtra);
    if (nExtra < 0) {
        nExtra = 0;
    }
    nNeed = 1;
    for (i = 0; i < nExtra; i++) {
        if (aExtra[i] != gid) {
            nNeed++;
        }
    }
    if (*pNgids < nNeed) {
        *pNgids = nNeed;
        return -1;
    }
    if (pGroups != NULL) {
        int nOut = 0;

        pGroups[nOut++] = gid;
        for (i = 0; i < nExtra && nOut < nNeed; i++) {
            int j;
            int fDup = 0;

            if (aExtra[i] == gid) {
                continue;
            }
            for (j = 0; j < nOut; j++) {
                if (pGroups[j] == aExtra[i]) {
                    fDup = 1;
                    break;
                }
            }
            if (!fDup) {
                pGroups[nOut++] = aExtra[i];
            }
        }
        *pNgids = nOut;
        return nOut;
    }
    *pNgids = nNeed;
    return nNeed;
}
