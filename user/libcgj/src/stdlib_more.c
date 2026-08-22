/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * getsubopt + getgrouplist — desktop link surface.
 */
#include <errno.h>
#include <grp.h>
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
    gid_t aNeed[64];
    int nNeed = 0;
    int i;
    struct group *pGr;

    if (pNgids == NULL) {
        errno = EINVAL;
        return -1;
    }
    aNeed[nNeed++] = gid;
    if (szUser != NULL && szUser[0] != '\0') {
        setgrent();
        while ((pGr = getgrent()) != NULL && nNeed < 64) {
            int fMem = 0;
            char **pp;

            if (pGr->gr_gid == gid) {
                fMem = 1;
            }
            for (pp = pGr->gr_mem; !fMem && pp != NULL && *pp != NULL; pp++) {
                if (strcmp(*pp, szUser) == 0) {
                    fMem = 1;
                }
            }
            if (fMem) {
                int fDup = 0;

                for (i = 0; i < nNeed; i++) {
                    if (aNeed[i] == pGr->gr_gid) {
                        fDup = 1;
                        break;
                    }
                }
                if (!fDup) {
                    aNeed[nNeed++] = pGr->gr_gid;
                }
            }
        }
        endgrent();
    }
    if (*pNgids < nNeed) {
        *pNgids = nNeed;
        return -1;
    }
    if (pGroups != NULL) {
        for (i = 0; i < nNeed; i++) {
            pGroups[i] = aNeed[i];
        }
    }
    *pNgids = nNeed;
    return nNeed;
}
