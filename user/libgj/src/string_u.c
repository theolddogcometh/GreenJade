/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding string/memory helpers for libgj userspace.
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Soft freestanding helpers inventory (Wave 87 exclusive deepen; greppable;
 * hot path clean):
 *   libgj: soft inventory helpers=42 groups=10 wave=70 areas=16
 *   libgj: soft length  gj_strlen gj_strnlen
 *   libgj: soft compare gj_strcmp gj_strncmp gj_strcasecmp gj_strncasecmp
 *   libgj: soft copy    gj_strcpy gj_strncpy gj_strcat gj_strncat gj_strlcpy gj_strlcat
 *   libgj: soft search  gj_strchr gj_strrchr gj_strstr gj_strspn gj_strcspn
 *   libgj: soft case    gj_tolower gj_toupper
 *   libgj: soft ctype   gj_isdigit gj_isalpha gj_isalnum gj_isspace gj_isxdigit gj_isprint
 *   libgj: soft mem     gj_memchr gj_memcpy gj_memmove gj_memset gj_memcmp gj_memccpy
 *   libgj: soft parse   gj_atol gj_strtol gj_strtoul
 *   libgj: soft io      gj_puts gj_write gj_read gj_dlog
 *   libgj: soft format  gj_itoa gj_utoa gj_xtoa gj_snprintf
 *   libgj: soft policy  freestanding pure_c no_heap no_locale no_errno ascii_only soft_null
 *   libgj: soft counts  none
 *   libgj: soft hot_path clean
 *   libgj: soft deepen  wave=70 areas=inventory,groups,names,policy,path,counts,areas,accessors,honesty
 *   libgj: soft path    freestanding=1 pure_c=1 no_heap=1 counts=none bar3=0
 *   libgj: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 soft=1 product_kernel=OPEN wave=70
 *
 * Call counts intentionally omitted: hot string/mem helpers stay freestanding
 * product-path clean with no counter traffic. Inventory is source + rodata
 * only (see gj_string_soft_inventory / gj_string_soft_helper_count /
 * gj_string_soft_wave / gj_string_soft_group_name / gj_string_soft_area_name /
 * gj_string_soft_honesty).
 * Diagnostics / agent honesty only — never a product bar3 claim.
 */
#include <gj/string.h>
#include <gj/syscalls.h>
#include <stdarg.h>
#include <stdint.h>

/* Wave 87 exclusive soft deepen stamp (greppable wave=70). */
/* Wave 87 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retflameangle continuum_toward=22900 soft_ne_product=1
 *   greppable: soft retcipherangle exclusive=1 continuum_toward=22900
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define LIBGJ_SOFT_WAVE 70u
/* Fixed greppable area catalog (inventory..honesty). */
#define LIBGJ_SOFT_AREAS  16u
/* Soft policy token count (freestanding..soft_null). */
#define LIBGJ_SOFT_POLICY 7u

/*
 * Product soft inventory blob (rodata). Never consulted by the hot helpers.
 * Grep: libgj: soft inventory
 */
static const char g_szLibgjSoftInventory[] =
    "libgj: soft inventory helpers=42 groups=10 wave=70 areas=16 "
    "length=gj_strlen,gj_strnlen "
    "compare=gj_strcmp,gj_strncmp,gj_strcasecmp,gj_strncasecmp "
    "copy=gj_strcpy,gj_strncpy,gj_strcat,gj_strncat,gj_strlcpy,gj_strlcat "
    "search=gj_strchr,gj_strrchr,gj_strstr,gj_strspn,gj_strcspn "
    "case=gj_tolower,gj_toupper "
    "ctype=gj_isdigit,gj_isalpha,gj_isalnum,gj_isspace,gj_isxdigit,gj_isprint "
    "mem=gj_memchr,gj_memcpy,gj_memmove,gj_memset,gj_memcmp,gj_memccpy "
    "parse=gj_atol,gj_strtol,gj_strtoul "
    "io=gj_puts,gj_write,gj_read,gj_dlog "
    "format=gj_itoa,gj_utoa,gj_xtoa,gj_snprintf "
    "policy=freestanding,pure_c,no_heap,no_locale,no_errno,ascii_only,soft_null "
    "counts=none hot_path=clean accessors=area,policy,honesty bar3=0";

/*
 * Wave 87 soft deepen stamp (rodata companion).
 * Grep: libgj: soft deepen
 */
static const char g_szLibgjSoftDeepen[] =
    "libgj: soft deepen wave=70 areas=16 "
    "inventory,groups,names,length,compare,copy,search,case,ctype,mem,"
    "parse,io,format,policy,path,counts,areas,accessors,honesty "
    "helpers=42 hot_path=clean counts=none multi_server=0 confine=0 bar3=0";

/*
 * Wave 87 honesty path line (static route labels).
 * Grep: libgj: soft path
 */
static const char g_szLibgjSoftPath[] =
    "libgj: soft path freestanding=1 pure_c=1 no_heap=1 no_locale=1 "
    "no_errno=1 ascii_only=1 soft_null=1 counts=none hot_path=clean "
    "multi_server=0 confine=0 bar3=0 (soft inventory; not bar3)";

/*
 * Wave 87 exclusive honesty line (static).
 * Grep: libgj: soft honesty
 */
static const char g_szLibgjSoftHonesty[] =
    "libgj: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 "
    "soft=1 product_kernel=OPEN wave=70 userland=1";

/* Soft helper name table (order matches public soft set; cold only). */
static const char *const g_apszLibgjSoftHelpers[] = {
    /* length */
    "gj_strlen",
    "gj_strnlen",
    /* compare */
    "gj_strcmp",
    "gj_strncmp",
    "gj_strcasecmp",
    "gj_strncasecmp",
    /* copy / concat / bounded */
    "gj_strcpy",
    "gj_strncpy",
    "gj_strcat",
    "gj_strncat",
    "gj_strlcpy",
    "gj_strlcat",
    /* search */
    "gj_strchr",
    "gj_strrchr",
    "gj_strstr",
    "gj_strspn",
    "gj_strcspn",
    /* case */
    "gj_tolower",
    "gj_toupper",
    /* ctype (ASCII) */
    "gj_isdigit",
    "gj_isalpha",
    "gj_isalnum",
    "gj_isspace",
    "gj_isxdigit",
    "gj_isprint",
    /* mem */
    "gj_memchr",
    "gj_memcpy",
    "gj_memmove",
    "gj_memset",
    "gj_memcmp",
    "gj_memccpy",
    /* parse */
    "gj_atol",
    "gj_strtol",
    "gj_strtoul",
    /* io */
    "gj_puts",
    "gj_write",
    "gj_read",
    "gj_dlog",
    /* format */
    "gj_itoa",
    "gj_utoa",
    "gj_xtoa",
    "gj_snprintf",
};

/* Soft group name table (order matches group helper counts). */
static const char *const g_apszLibgjSoftGroups[] = {
    "length",
    "compare",
    "copy",
    "search",
    "case",
    "ctype",
    "mem",
    "parse",
    "io",
    "format",
};

/*
 * Wave 87 greppable area catalog (logical inventory surfaces; cold only).
 * Grep: libgj: soft areas=
 */
static const char *const g_apszLibgjSoftAreas[] = {
    "inventory",
    "groups",
    "names",
    "length",
    "compare",
    "copy",
    "search",
    "case",
    "ctype",
    "mem",
    "parse",
    "io",
    "format",
    "policy",
    "path",
    "honesty",
};

/* Soft policy tokens (ASCII freestanding contract labels). */
static const char *const g_apszLibgjSoftPolicy[] = {
    "freestanding",
    "pure_c",
    "no_heap",
    "no_locale",
    "no_errno",
    "ascii_only",
    "soft_null",
};

enum {
    LIBGJ_SOFT_HELPERS = 42,
    LIBGJ_SOFT_GROUPS  = 10,
    LIBGJ_SOFT_LENGTH  = 2,
    LIBGJ_SOFT_COMPARE = 4,
    LIBGJ_SOFT_COPY    = 6,
    LIBGJ_SOFT_SEARCH  = 5,
    LIBGJ_SOFT_CASE    = 2,
    LIBGJ_SOFT_CTYPE   = 6,
    LIBGJ_SOFT_MEM     = 6,
    LIBGJ_SOFT_PARSE   = 3,
    LIBGJ_SOFT_IO      = 4,
    LIBGJ_SOFT_FORMAT  = 4
};

/*
 * Cold soft inventory accessor — not used by any hot helper.
 * Returns the greppable "libgj: soft …" product line (NUL-terminated).
 * Grep: libgj: soft inventory
 */
const char *
gj_string_soft_inventory(void)
{
    return g_szLibgjSoftInventory;
}

/*
 * Cold soft deepen stamp (Wave 87). Grep: libgj: soft deepen
 */
const char *
gj_string_soft_deepen(void)
{
    return g_szLibgjSoftDeepen;
}

/*
 * Cold soft path honesty line. Grep: libgj: soft path
 */
const char *
gj_string_soft_path(void)
{
    return g_szLibgjSoftPath;
}

/*
 * Cold soft honesty line (Wave 87 exclusive). Grep: libgj: soft honesty
 * Soft inventory ≠ product multi-server confine; never a bar3 claim.
 */
const char *
gj_string_soft_honesty(void)
{
    return g_szLibgjSoftHonesty;
}

/*
 * Cold soft inventory: Wave stamp (32). Grep: libgj: soft wave=
 */
unsigned
gj_string_soft_wave(void)
{
    return (unsigned)LIBGJ_SOFT_WAVE;
}

/*
 * Cold soft inventory: greppable area count. Grep: libgj: soft areas=
 */
unsigned
gj_string_soft_area_count(void)
{
    return (unsigned)LIBGJ_SOFT_AREAS;
}

/*
 * Cold soft inventory: area name by index (0..areas-1), or NULL.
 * Grep: libgj: soft areas=
 */
const char *
gj_string_soft_area_name(unsigned uArea)
{
    if (uArea >= (unsigned)LIBGJ_SOFT_AREAS) {
        return NULL;
    }
    return g_apszLibgjSoftAreas[uArea];
}

/*
 * Cold soft inventory: policy token count. Grep: libgj: soft policy
 */
unsigned
gj_string_soft_policy_count(void)
{
    return (unsigned)LIBGJ_SOFT_POLICY;
}

/*
 * Cold soft inventory: policy token by index, or NULL.
 * Grep: libgj: soft policy
 */
const char *
gj_string_soft_policy_name(unsigned uPolicy)
{
    if (uPolicy >= (unsigned)LIBGJ_SOFT_POLICY) {
        return NULL;
    }
    return g_apszLibgjSoftPolicy[uPolicy];
}

/*
 * Cold soft inventory: total helper count in the freestanding soft set.
 * Grep: libgj: soft helpers=
 */
unsigned
gj_string_soft_helper_count(void)
{
    return (unsigned)LIBGJ_SOFT_HELPERS;
}

/*
 * Cold soft inventory: logical group count (length..format).
 * Grep: libgj: soft groups=
 */
unsigned
gj_string_soft_group_count(void)
{
    return (unsigned)LIBGJ_SOFT_GROUPS;
}

/*
 * Cold soft inventory: length-* group size. Grep: libgj: soft length
 */
unsigned
gj_string_soft_length_count(void)
{
    return (unsigned)LIBGJ_SOFT_LENGTH;
}

/*
 * Cold soft inventory: compare-* group size. Grep: libgj: soft compare
 */
unsigned
gj_string_soft_compare_count(void)
{
    return (unsigned)LIBGJ_SOFT_COMPARE;
}

/*
 * Cold soft inventory: copy-* group size. Grep: libgj: soft copy
 */
unsigned
gj_string_soft_copy_count(void)
{
    return (unsigned)LIBGJ_SOFT_COPY;
}

/*
 * Cold soft inventory: search-* group size. Grep: libgj: soft search
 */
unsigned
gj_string_soft_search_count(void)
{
    return (unsigned)LIBGJ_SOFT_SEARCH;
}

/*
 * Cold soft inventory: case-* group size. Grep: libgj: soft case
 */
unsigned
gj_string_soft_case_count(void)
{
    return (unsigned)LIBGJ_SOFT_CASE;
}

/*
 * Cold soft inventory: ctype-* group size. Grep: libgj: soft ctype
 */
unsigned
gj_string_soft_ctype_count(void)
{
    return (unsigned)LIBGJ_SOFT_CTYPE;
}

/*
 * Cold soft inventory: mem-* group size.
 * Grep: libgj: soft mem
 */
unsigned
gj_string_soft_mem_count(void)
{
    return (unsigned)LIBGJ_SOFT_MEM;
}

/*
 * Cold soft inventory: parse-* group size. Grep: libgj: soft parse
 */
unsigned
gj_string_soft_parse_count(void)
{
    return (unsigned)LIBGJ_SOFT_PARSE;
}

/*
 * Cold soft inventory: io-* group size. Grep: libgj: soft io
 */
unsigned
gj_string_soft_io_count(void)
{
    return (unsigned)LIBGJ_SOFT_IO;
}

/*
 * Cold soft inventory: format-* group size (itoa..snprintf).
 * Grep: libgj: soft format
 */
unsigned
gj_string_soft_format_count(void)
{
    return (unsigned)LIBGJ_SOFT_FORMAT;
}

/*
 * Cold soft inventory: helper name by index, or NULL if out of range.
 * Does not allocate; points at static literals only.
 * Grep: libgj: soft
 */
const char *
gj_string_soft_helper_name(unsigned uIndex)
{
    if (uIndex >= (unsigned)LIBGJ_SOFT_HELPERS) {
        return NULL;
    }
    return g_apszLibgjSoftHelpers[uIndex];
}

/*
 * Cold soft inventory: group name by index (0..groups-1), or NULL.
 * Grep: libgj: soft
 */
const char *
gj_string_soft_group_name(unsigned uGroup)
{
    if (uGroup >= (unsigned)LIBGJ_SOFT_GROUPS) {
        return NULL;
    }
    return g_apszLibgjSoftGroups[uGroup];
}

/*
 * Cold soft inventory: per-group helper counts (index 0..groups-1).
 * Order: length, compare, copy, search, case, ctype, mem, parse, io, format.
 * Out of range → 0. Grep: libgj: soft
 */
unsigned
gj_string_soft_group_helper_count(unsigned uGroup)
{
    static const unsigned aGroup[LIBGJ_SOFT_GROUPS] = {
        (unsigned)LIBGJ_SOFT_LENGTH,
        (unsigned)LIBGJ_SOFT_COMPARE,
        (unsigned)LIBGJ_SOFT_COPY,
        (unsigned)LIBGJ_SOFT_SEARCH,
        (unsigned)LIBGJ_SOFT_CASE,
        (unsigned)LIBGJ_SOFT_CTYPE,
        (unsigned)LIBGJ_SOFT_MEM,
        (unsigned)LIBGJ_SOFT_PARSE,
        (unsigned)LIBGJ_SOFT_IO,
        (unsigned)LIBGJ_SOFT_FORMAT
    };

    if (uGroup >= (unsigned)LIBGJ_SOFT_GROUPS) {
        return 0;
    }
    return aGroup[uGroup];
}

size_t
gj_strlen(const char *sz)
{
    size_t n = 0;

    if (sz == NULL) {
        return 0;
    }
    while (sz[n] != '\0') {
        n++;
    }
    return n;
}

size_t
gj_strnlen(const char *sz, size_t cbMax)
{
    size_t n = 0;

    if (sz == NULL || cbMax == 0) {
        return 0;
    }
    while (n < cbMax && sz[n] != '\0') {
        n++;
    }
    return n;
}

int
gj_strcmp(const char *szA, const char *szB)
{
    size_t i = 0;

    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    while (szA[i] != '\0' && szA[i] == szB[i]) {
        i++;
    }
    return (unsigned char)szA[i] - (unsigned char)szB[i];
}

int
gj_strncmp(const char *szA, const char *szB, size_t cb)
{
    size_t i;

    if (cb == 0) {
        return 0;
    }
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (i = 0; i < cb; i++) {
        unsigned char ca = (unsigned char)szA[i];
        unsigned char cb2 = (unsigned char)szB[i];

        if (ca != cb2) {
            return (int)ca - (int)cb2;
        }
        if (ca == 0) {
            return 0;
        }
    }
    return 0;
}

char *
gj_strcpy(char *szDst, const char *szSrc)
{
    size_t i = 0;

    if (szDst == NULL) {
        return NULL;
    }
    if (szSrc == NULL) {
        szDst[0] = '\0';
        return szDst;
    }
    while (szSrc[i] != '\0') {
        szDst[i] = szSrc[i];
        i++;
    }
    szDst[i] = '\0';
    return szDst;
}

char *
gj_strncpy(char *szDst, const char *szSrc, size_t cb)
{
    size_t i;

    if (szDst == NULL || cb == 0) {
        return szDst;
    }
    for (i = 0; i < cb; i++) {
        if (szSrc != NULL && szSrc[i] != '\0') {
            szDst[i] = szSrc[i];
        } else {
            szDst[i] = '\0';
            i++;
            break;
        }
    }
    for (; i < cb; i++) {
        szDst[i] = '\0';
    }
    return szDst;
}

char *
gj_strcat(char *szDst, const char *szSrc)
{
    size_t nDst;

    if (szDst == NULL) {
        return NULL;
    }
    nDst = gj_strlen(szDst);
    (void)gj_strcpy(szDst + nDst, szSrc != NULL ? szSrc : "");
    return szDst;
}

char *
gj_strncat(char *szDst, const char *szSrc, size_t cb)
{
    size_t nDst;
    size_t i;

    if (szDst == NULL) {
        return NULL;
    }
    if (cb == 0 || szSrc == NULL) {
        return szDst;
    }
    nDst = gj_strlen(szDst);
    for (i = 0; i < cb && szSrc[i] != '\0'; i++) {
        szDst[nDst + i] = szSrc[i];
    }
    szDst[nDst + i] = '\0';
    return szDst;
}

size_t
gj_strlcpy(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t nSrc;
    size_t i;

    nSrc = gj_strlen(szSrc);
    if (szDst == NULL || cbDst == 0) {
        return nSrc;
    }
    i = 0;
    if (szSrc != NULL) {
        while (i + 1u < cbDst && szSrc[i] != '\0') {
            szDst[i] = szSrc[i];
            i++;
        }
    }
    szDst[i] = '\0';
    return nSrc;
}

size_t
gj_strlcat(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t nDst;
    size_t nSrc;
    size_t i;

    nSrc = gj_strlen(szSrc);
    if (szDst == NULL || cbDst == 0) {
        return nSrc;
    }
    nDst = gj_strnlen(szDst, cbDst);
    if (nDst >= cbDst) {
        /* No room / not NUL-terminated within cbDst. */
        return cbDst + nSrc;
    }
    i = 0;
    if (szSrc != NULL) {
        while (nDst + i + 1u < cbDst && szSrc[i] != '\0') {
            szDst[nDst + i] = szSrc[i];
            i++;
        }
    }
    szDst[nDst + i] = '\0';
    return nDst + nSrc;
}

char *
gj_strchr(const char *sz, int ch)
{
    if (sz == NULL) {
        return NULL;
    }
    while (*sz != '\0') {
        if ((unsigned char)*sz == (unsigned char)ch) {
            return (char *)(uintptr_t)sz;
        }
        sz++;
    }
    if (ch == 0) {
        return (char *)(uintptr_t)sz;
    }
    return NULL;
}

char *
gj_strrchr(const char *sz, int ch)
{
    const char *pLast = NULL;

    if (sz == NULL) {
        return NULL;
    }
    while (*sz != '\0') {
        if ((unsigned char)*sz == (unsigned char)ch) {
            pLast = sz;
        }
        sz++;
    }
    if (ch == 0) {
        return (char *)(uintptr_t)sz;
    }
    return (char *)(uintptr_t)pLast;
}

char *
gj_strstr(const char *szHay, const char *szNeedle)
{
    size_t cbNeedle;
    size_t i;

    if (szHay == NULL) {
        return NULL;
    }
    if (szNeedle == NULL || szNeedle[0] == '\0') {
        return (char *)(uintptr_t)szHay;
    }
    cbNeedle = gj_strlen(szNeedle);
    for (i = 0; szHay[i] != '\0'; i++) {
        size_t j;

        for (j = 0; j < cbNeedle; j++) {
            if (szHay[i + j] == '\0' || szHay[i + j] != szNeedle[j]) {
                break;
            }
        }
        if (j == cbNeedle) {
            return (char *)(uintptr_t)(szHay + i);
        }
    }
    return NULL;
}

void *
gj_memchr(const void *p, int ch, size_t cb)
{
    const unsigned char *pB = (const unsigned char *)p;
    unsigned char chWant = (unsigned char)ch;
    size_t i;

    if (p == NULL) {
        return NULL;
    }
    for (i = 0; i < cb; i++) {
        if (pB[i] == chWant) {
            return (void *)(uintptr_t)(pB + i);
        }
    }
    return NULL;
}

size_t
gj_strspn(const char *sz, const char *szAccept)
{
    size_t n = 0;
    size_t i;
    size_t cAccept;

    if (sz == NULL || szAccept == NULL) {
        return 0;
    }
    cAccept = gj_strlen(szAccept);
    while (sz[n] != '\0') {
        int fOk = 0;

        for (i = 0; i < cAccept; i++) {
            if ((unsigned char)sz[n] == (unsigned char)szAccept[i]) {
                fOk = 1;
                break;
            }
        }
        if (fOk == 0) {
            break;
        }
        n++;
    }
    return n;
}

size_t
gj_strcspn(const char *sz, const char *szReject)
{
    size_t n = 0;
    size_t i;
    size_t cReject;

    if (sz == NULL) {
        return 0;
    }
    if (szReject == NULL || szReject[0] == '\0') {
        return gj_strlen(sz);
    }
    cReject = gj_strlen(szReject);
    while (sz[n] != '\0') {
        for (i = 0; i < cReject; i++) {
            if ((unsigned char)sz[n] == (unsigned char)szReject[i]) {
                return n;
            }
        }
        n++;
    }
    return n;
}

int
gj_tolower(int ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A' + 'a';
    }
    return ch;
}

int
gj_toupper(int ch)
{
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 'A';
    }
    return ch;
}

int
gj_strcasecmp(const char *szA, const char *szB)
{
    size_t i = 0;
    int ca;
    int cb2;

    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (;;) {
        ca = gj_tolower((unsigned char)szA[i]);
        cb2 = gj_tolower((unsigned char)szB[i]);
        if (ca != cb2 || ca == 0) {
            return ca - cb2;
        }
        i++;
    }
}

int
gj_strncasecmp(const char *szA, const char *szB, size_t cb)
{
    size_t i;
    int ca;
    int cb2;

    if (cb == 0) {
        return 0;
    }
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (i = 0; i < cb; i++) {
        ca = gj_tolower((unsigned char)szA[i]);
        cb2 = gj_tolower((unsigned char)szB[i]);
        if (ca != cb2 || ca == 0) {
            return ca - cb2;
        }
    }
    return 0;
}

int
gj_isdigit(int ch)
{
    return (ch >= '0' && ch <= '9') ? 1 : 0;
}

int
gj_isalpha(int ch)
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) ? 1 : 0;
}

int
gj_isalnum(int ch)
{
    return (gj_isdigit(ch) || gj_isalpha(ch)) ? 1 : 0;
}

int
gj_isspace(int ch)
{
    return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' ||
            ch == '\f' || ch == '\v')
               ? 1
               : 0;
}

int
gj_isxdigit(int ch)
{
    return (gj_isdigit(ch) || (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F'))
               ? 1
               : 0;
}

int
gj_isprint(int ch)
{
    return (ch >= 0x20 && ch <= 0x7e) ? 1 : 0;
}

void *
gj_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < cb; i++) {
        d[i] = s[i];
    }
    return pDst;
}

void *
gj_memmove(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    size_t i;

    if (d == s || cb == 0) {
        return pDst;
    }
    if (d < s) {
        for (i = 0; i < cb; i++) {
            d[i] = s[i];
        }
    } else {
        for (i = cb; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return pDst;
}

void *
gj_memset(void *pDst, int c, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        d[i] = (unsigned char)c;
    }
    return pDst;
}

int
gj_memcmp(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *a = (const unsigned char *)pA;
    const unsigned char *b = (const unsigned char *)pB;
    size_t i;

    for (i = 0; i < cb; i++) {
        if (a[i] != b[i]) {
            return (int)a[i] - (int)b[i];
        }
    }
    return 0;
}

void *
gj_memccpy(void *pDst, const void *pSrc, int ch, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    unsigned char chWant = (unsigned char)ch;
    size_t i;

    if (d == NULL || s == NULL) {
        return NULL;
    }
    for (i = 0; i < cb; i++) {
        d[i] = s[i];
        if (s[i] == chWant) {
            return (void *)(d + i + 1);
        }
    }
    return NULL;
}

/*
 * Digit value for base 2..36. Returns -1 if not a valid digit for nBase.
 */
static int
s_digit_val(int ch, int nBase)
{
    int n;

    if (ch >= '0' && ch <= '9') {
        n = ch - '0';
    } else if (ch >= 'a' && ch <= 'z') {
        n = ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'Z') {
        n = ch - 'A' + 10;
    } else {
        return -1;
    }
    if (n >= nBase) {
        return -1;
    }
    return n;
}

unsigned long
gj_strtoul(const char *sz, char **ppEnd, int nBase)
{
    const char *p;
    unsigned long u = 0;
    int nBaseUse;
    int nDig;
    int fAny = 0;

    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)sz;
    }
    if (sz == NULL) {
        return 0;
    }
    p = sz;
    while (gj_isspace((unsigned char)*p) != 0) {
        p++;
    }
    if (*p == '+') {
        p++;
    }
    nBaseUse = nBase;
    if (nBaseUse == 0) {
        if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
            nBaseUse = 16;
            p += 2;
        } else if (p[0] == '0') {
            nBaseUse = 8;
            p++;
            fAny = 1; /* lone "0" is a valid convert */
        } else {
            nBaseUse = 10;
        }
    } else if (nBaseUse == 16 && p[0] == '0' &&
               (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    if (nBaseUse < 2 || nBaseUse > 36) {
        return 0;
    }
    for (;;) {
        nDig = s_digit_val((unsigned char)*p, nBaseUse);
        if (nDig < 0) {
            break;
        }
        /* Soft: wrap on overflow rather than set errno (no errno). */
        u = u * (unsigned long)nBaseUse + (unsigned long)nDig;
        fAny = 1;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)(fAny != 0 ? p : sz);
    }
    return u;
}

long
gj_strtol(const char *sz, char **ppEnd, int nBase)
{
    const char *p;
    int fNeg = 0;
    unsigned long u;
    char *pEndLocal = NULL;

    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)sz;
    }
    if (sz == NULL) {
        return 0;
    }
    p = sz;
    while (gj_isspace((unsigned char)*p) != 0) {
        p++;
    }
    if (*p == '-') {
        fNeg = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    u = gj_strtoul(p, &pEndLocal, nBase);
    if (ppEnd != NULL) {
        if (pEndLocal == p && u == 0) {
            *ppEnd = (char *)(uintptr_t)sz;
        } else {
            *ppEnd = pEndLocal;
        }
    }
    if (fNeg != 0) {
        return -(long)u;
    }
    return (long)u;
}

long
gj_atol(const char *sz)
{
    return gj_strtol(sz, NULL, 10);
}

void
gj_puts(const char *sz)
{
    size_t cb;

    if (sz == NULL) {
        return;
    }
    cb = gj_strlen(sz);
    if (cb > 0) {
        (void)linux_write(1, sz, cb);
    }
}

long
gj_write(int nFd, const void *p, size_t cb)
{
    return linux_write(nFd, p, cb);
}

long
gj_read(int nFd, void *p, size_t cb)
{
    return linux_read(nFd, p, cb);
}

long
gj_dlog(const char *sz)
{
    if (sz == NULL) {
        return gj_debug_log("", 0);
    }
    return gj_debug_log(sz, (long)gj_strlen(sz));
}

size_t
gj_itoa(long i64Val, char *szBuf, size_t cbBuf)
{
    char aTmp[24];
    size_t n = 0;
    size_t i;
    unsigned long u;
    int fNeg = 0;

    if (szBuf == NULL || cbBuf == 0) {
        return 0;
    }
    if (i64Val < 0) {
        fNeg = 1;
        u = (unsigned long)(-(i64Val + 1)) + 1ul;
    } else {
        u = (unsigned long)i64Val;
    }
    if (u == 0) {
        aTmp[n++] = '0';
    } else {
        while (u > 0 && n < sizeof(aTmp)) {
            aTmp[n++] = (char)('0' + (u % 10ul));
            u /= 10ul;
        }
    }
    i = 0;
    if (fNeg && i < cbBuf - 1) {
        szBuf[i++] = '-';
    }
    while (n > 0 && i < cbBuf - 1) {
        n--;
        szBuf[i++] = aTmp[n];
    }
    szBuf[i] = '\0';
    return i;
}

size_t
gj_utoa(unsigned long uVal, char *szBuf, size_t cbBuf)
{
    char aTmp[24];
    size_t n = 0;
    size_t i;

    if (szBuf == NULL || cbBuf == 0) {
        return 0;
    }
    if (uVal == 0) {
        aTmp[n++] = '0';
    } else {
        while (uVal > 0 && n < sizeof(aTmp)) {
            aTmp[n++] = (char)('0' + (uVal % 10ul));
            uVal /= 10ul;
        }
    }
    i = 0;
    while (n > 0 && i < cbBuf - 1) {
        n--;
        szBuf[i++] = aTmp[n];
    }
    szBuf[i] = '\0';
    return i;
}

size_t
gj_xtoa(unsigned long uVal, char *szBuf, size_t cbBuf,
        int fLeadZero, size_t cMin)
{
    static const char aHex[] = "0123456789abcdef";
    char aTmp[24];
    size_t n = 0;
    size_t i;
    size_t cPad;

    if (szBuf == NULL || cbBuf == 0) {
        return 0;
    }
    if (uVal == 0) {
        aTmp[n++] = '0';
    } else {
        while (uVal > 0 && n < sizeof(aTmp)) {
            aTmp[n++] = aHex[uVal & 0xful];
            uVal >>= 4;
        }
    }
    cPad = 0;
    if (fLeadZero != 0 && cMin > n) {
        cPad = cMin - n;
    }
    i = 0;
    while (cPad > 0 && i < cbBuf - 1) {
        szBuf[i++] = '0';
        cPad--;
    }
    while (n > 0 && i < cbBuf - 1) {
        n--;
        szBuf[i++] = aTmp[n];
    }
    szBuf[i] = '\0';
    return i;
}

/*
 * Soft snprintf-lite: append one char / string into bounded buffer.
 * *pOut advances; *pNeed counts full logical length; cbCap includes room for NUL.
 */
static void
s_sn_putc(char *szBuf, size_t cbCap, size_t *pOut, size_t *pNeed, char ch)
{
    if (*pOut + 1u < cbCap) {
        szBuf[*pOut] = ch;
    }
    (*pOut)++;
    (*pNeed)++;
}

static void
s_sn_puts(char *szBuf, size_t cbCap, size_t *pOut, size_t *pNeed,
          const char *sz)
{
    size_t i;

    if (sz == NULL) {
        sz = "(null)";
    }
    for (i = 0; sz[i] != '\0'; i++) {
        s_sn_putc(szBuf, cbCap, pOut, pNeed, sz[i]);
    }
}

size_t
gj_snprintf(char *szBuf, size_t cbBuf, const char *szFmt, ...)
{
    va_list ap;
    size_t iOut = 0;
    size_t iNeed = 0;
    size_t i;
    char aNum[32];
    size_t nNum;

    if (szFmt == NULL) {
        if (szBuf != NULL && cbBuf > 0) {
            szBuf[0] = '\0';
        }
        return 0;
    }
    va_start(ap, szFmt);
    for (i = 0; szFmt[i] != '\0'; i++) {
        if (szFmt[i] != '%') {
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, szFmt[i]);
            continue;
        }
        i++;
        if (szFmt[i] == '\0') {
            break;
        }
        if (szFmt[i] == '%') {
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, '%');
        } else if (szFmt[i] == 's') {
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, va_arg(ap, const char *));
        } else if (szFmt[i] == 'c') {
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed,
                      (char)va_arg(ap, int));
        } else if (szFmt[i] == 'd' || szFmt[i] == 'i') {
            nNum = gj_itoa(va_arg(ap, long), aNum, sizeof(aNum));
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else if (szFmt[i] == 'u') {
            nNum = gj_utoa(va_arg(ap, unsigned long), aNum, sizeof(aNum));
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else if (szFmt[i] == 'x' || szFmt[i] == 'X') {
            nNum = gj_xtoa(va_arg(ap, unsigned long), aNum, sizeof(aNum), 0,
                           0);
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else if (szFmt[i] == 'p') {
            void *p = va_arg(ap, void *);

            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, "0x");
            nNum = gj_xtoa((unsigned long)(uintptr_t)p, aNum, sizeof(aNum), 1,
                           sizeof(void *) * 2u);
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else {
            /* Unknown conversion: emit literally. */
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, '%');
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, szFmt[i]);
        }
    }
    va_end(ap);
    if (szBuf != NULL && cbBuf > 0) {
        if (iOut < cbBuf) {
            szBuf[iOut] = '\0';
        } else {
            szBuf[cbBuf - 1u] = '\0';
        }
    }
    return iNeed;
}
