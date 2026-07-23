/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48: freestanding integer-only stubs/helpers for
 * desktop dynlink graphs —
 *   __idna_to_dns_encoding / __idna_from_dns_encoding (ASCII passthrough,
 *     Punycode-free; success for pure-ASCII labels),
 *   __nss_passwd_lookup2 / __nss_group_lookup2 / __nss_hosts_lookup2
 *     (thin wrappers over existing __nss_*_lookup when present, else ENOSYS),
 *   __nscd_get_nl_timestamp (stub: no nscd netlink clock),
 *   __res_ninit / __res_iclose (aliases/wrappers when missing elsewhere),
 *   __libcgj_batch48_marker.
 *
 * Symbols already provided by earlier batches (nm -D verified) are emitted
 * weak so this translation unit stays self-contained without multi-def.
 * Clean-room public ABI; no SSE / no FP.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Existing NSS lookup1 surface (graph_batch26) — may be linked in. */
int __nss_passwd_lookup(void **ppNi, void **ppFct);
int __nss_group_lookup(void **ppNi, void **ppFct);
int __nss_hosts_lookup(void **ppNi, void **ppFct);

/* Existing resolv surface (graph_batch12 / graph_batch15). */
int res_init(void);

const char __libcgj_batch48_marker[] = "libcgj-batch48";

/* ---- helpers ------------------------------------------------------------- */

static int
b48_ascii_label_ok(const char *sz)
{
    size_t i;

    if (sz == NULL) {
        return 0;
    }
    for (i = 0; sz[i] != '\0'; i++) {
        unsigned char c = (unsigned char)sz[i];
        /* DNS labels + dots; reject non-ASCII / control. Punycode not impl. */
        if (c > 0x7eu || c < 0x20u) {
            return 0;
        }
    }
    return 1;
}

static int
b48_idna_ascii_passthrough(const char *szName, char **ppResult)
{
    size_t n;

    if (szName == NULL || ppResult == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (!b48_ascii_label_ok(szName)) {
        /* Non-ASCII would need Punycode; freestanding stub rejects. */
        errno = EILSEQ;
        return -1;
    }
    n = strlen(szName);
    *ppResult = (char *)malloc(n + 1u);
    if (*ppResult == NULL) {
        return -1;
    }
    memcpy(*ppResult, szName, n + 1u);
    return 0;
}

/* ---- IDNA (ASCII passthrough; weak if batch23 already exports) --------- */

__attribute__((weak)) int
__idna_to_dns_encoding(const char *szName, char **ppResult)
{
    return b48_idna_ascii_passthrough(szName, ppResult);
}

__attribute__((weak)) int
__idna_from_dns_encoding(const char *szName, char **ppResult)
{
    /* DNS-encoded ASCII hostnames round-trip as passthrough. */
    return b48_idna_ascii_passthrough(szName, ppResult);
}

/* ---- NSS lookup2 thin wrappers (weak if batch22 already exports) ------- */

static int
b48_nss_lookup2(int (*pLookup1)(void **, void **), void **ppNi, void **ppFct,
                void *pA, void *pB)
{
    (void)pA;
    (void)pB;
    if (pLookup1 != NULL) {
        return pLookup1(ppNi, ppFct);
    }
    if (ppNi != NULL) {
        *ppNi = NULL;
    }
    if (ppFct != NULL) {
        *ppFct = NULL;
    }
    errno = ENOSYS;
    return -1;
}

__attribute__((weak)) int
__nss_passwd_lookup2(void **ppNi, void **ppFct, void *pA, void *pB)
{
    return b48_nss_lookup2(__nss_passwd_lookup, ppNi, ppFct, pA, pB);
}

__attribute__((weak)) int
__nss_group_lookup2(void **ppNi, void **ppFct, void *pA, void *pB)
{
    return b48_nss_lookup2(__nss_group_lookup, ppNi, ppFct, pA, pB);
}

__attribute__((weak)) int
__nss_hosts_lookup2(void **ppNi, void **ppFct, void *pA, void *pB)
{
    return b48_nss_lookup2(__nss_hosts_lookup, ppNi, ppFct, pA, pB);
}

/* ---- nscd netlink timestamp (missing from earlier batches) ------------- */

/*
 * glibc: uint32_t __nscd_get_nl_timestamp(void) — nscd ai-cache validity
 * clock from NETLINK_ROUTE. No nscd / no netlink monitor here: return 0 so
 * callers treat the cache as always-stale / disabled.
 */
uint32_t
__nscd_get_nl_timestamp(void)
{
    return 0u;
}

/* ---- resolv aliases (weak; strong defs live in batch12 / batch22) ------ */

__attribute__((weak)) int
__res_ninit(void *pState)
{
    (void)pState;
    return res_init();
}

__attribute__((weak)) void
__res_iclose(void *pState, int nFree)
{
    (void)pState;
    (void)nFree;
}
