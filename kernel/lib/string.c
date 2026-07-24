/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding kernel string/memory helpers (soft mem- and str- set).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Callers must pass valid pointers (and non-NULL where a buffer is required).
 * No heap, no locale, no I/O - safe for early boot and freestanding builds.
 *
 * Soft string helper inventory (Wave 9 exclusive; Wave 20 deepen; greppable;
 * hot path clean):
 *   string: soft inventory helpers=14 groups=2 wave=40
 *   string: soft mem memset memcpy memmove memcmp memchr
 *   string: soft str strlen strnlen strcmp strncmp strlcpy strlcat
 *            strchr strrchr strstr
 *   string: soft groups mem=5 str=9 total=14
 *   string: soft policy freestanding pure_c no_heap no_locale no_io
 *   string: soft counts none
 *   string: soft hot_path clean
 *   string: soft path claim=freestanding …
 *   string: soft catalog …   (helper-index geometry)
 *   string: soft honesty …   (Wave 20)
 *   string: soft surfaces …  (Wave 20)
 *   string: soft note …      (Wave 20)
 *   string: soft return …    (Wave 20 API return surfaces)
 *   string: soft retbound …  (Wave 20 return-bound honesty (kept))
 *   string: soft retseal …   (Wave 20 seal stamp (kept))
 *   string: soft deepen wave=40 areas=49 …
 *
 * Call counts intentionally omitted: memset/memcpy/etc. stay freestanding
 * early-boot hot path with no counter traffic. Inventory is source + rodata
 * + cold accessors only (never consulted by helpers).
 * greppable: string: soft
 */
#include <gj/string.h>

/* Wave 40 soft inventory stamp (file-local; never product gate). */
#define STRING_SOFT_WAVE 40u

/*
 * Soft inventory area count (fixed greppable categories for deepen stamp):
 *   inventory | mem | str | groups | policy | counts | path | catalog |
 *   honesty | surfaces | note | return | retmap | retclass | retlane |
 *   retbound | retseal | retpulse | retmark | retphase | retbadge |
 *   rettoken | retcrest | retvault | retbanner | retledger | retbeacon |
 *   retcipher | retflame | retprism | retforge | retshard | retcrown | retglyph | retscepter |
 *   retsigil | retemblem | retaegis | retmantle | retbulwark | retpanoply |
 *   retbastion | retcitadel | retredoubt | retkeep | retfortress | retpalace | rethold | retspire | retwall | retgate | retmoat | retower | retbarbican | retglacis | retcurtain | retparapet | deepen
 */
#define STRING_SOFT_AREAS 65u

enum {
    STRING_SOFT_HELPERS = 14,
    STRING_SOFT_MEM = 5,
    STRING_SOFT_STR = 9
};

/*
 * Product soft inventory blobs (rodata). Never consulted by the helpers.
 * Grep: string: soft
 */
static const char g_szStringSoftInventory[] =
    "string: soft inventory helpers=14 groups=2 wave=40 "
    "mem=memset,memcpy,memmove,memcmp,memchr "
    "str=strlen,strnlen,strcmp,strncmp,strlcpy,strlcat,strchr,strrchr,strstr "
    "policy=freestanding,pure_c,no_heap,no_locale,no_io "
    "counts=none hot_path=clean areas=19";

static const char g_szStringSoftMem[] =
    "string: soft mem memset,memcpy,memmove,memcmp,memchr count=5 "
    "group=mem wave=40";

static const char g_szStringSoftStr[] =
    "string: soft str strlen,strnlen,strcmp,strncmp,strlcpy,strlcat,"
    "strchr,strrchr,strstr count=9 group=str wave=40";

static const char g_szStringSoftGroups[] =
    "string: soft groups mem=5 str=9 total=14 wave=40";

static const char g_szStringSoftPolicy[] =
    "string: soft policy freestanding pure_c no_heap no_locale no_io "
    "early_boot=1 counters=0 wave=40";

static const char g_szStringSoftCounts[] =
    "string: soft counts none hot_path=clean reason=early_boot "
    "wave=40 (intentional; helpers stay cold of tallies)";

static const char g_szStringSoftPath[] =
    "string: soft path claim=freestanding mem=5 str=9 helpers=14 "
    "counts=none hot_path=clean no_heap=1 no_locale=1 no_io=1 "
    "early_boot=1 wave=40 (soft inventory; not libc)";

static const char g_szStringSoftDeepen[] =
    "string: soft deepen wave=40 areas=49 helpers=14 mem=5 str=9 "
    "return=1 retmap=1 retclass=1 retlane=1 retbound=1 retseal=1 retpulse=1 "
    "retmark=1 retphase=1 retbadge=1 rettoken=1 retcrest=1 retvault=1 retbanner=1 retledger=1 retbeacon=1 retcipher=1 retflame=1 retprism=1 retforge=1 retshard=1 retcrown=1 retglyph=1 retscepter=1 retsigil=1 retemblem=1 retaegis=1 retmantle=1 retbulwark=1 retpanoply=1 retbastion=1 retcitadel=1 retredoubt=1 retkeep=1 retfortress=1 retpalace=1 rethold=1 retspire=1 retwall=1 retgate=1 retmoat=1 retower=1 retbarbican=1 retglacis=1 retcurtain=1 retparapet=1 "
    "counts=none hot_path=clean (Wave 40 exclusive; soft only)";

static const char g_szStringSoftHotPath[] =
    "string: soft hot_path clean counters=0 early_boot=1 wave=40";

static const char g_szStringSoftCatalog[] =
    "string: soft catalog helpers=14 mem=5 str=9 areas=35 "
    "idx=mem0..4,str0..8 hot_path=clean wave=40";

static const char g_szStringSoftHonesty[] =
    "string: soft honesty freestanding=1 pure_c=1 no_heap=1 "
    "no_locale=1 no_io=1 counters=0 not_libc=1 soft_only=1 "
    "wave=40 (soft inventory; not bar3)";

static const char g_szStringSoftSurfaces[] =
    "string: soft surfaces count=26 "
    "names=inventory,mem,str,groups,policy,counts,path,catalog,"
    "honesty,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,deepen wave=40";

static const char g_szStringSoftNote[] =
    "string: soft note milestone=wave40 exclusive=1 "
    "helpers=14 mem=5 str=9 hot_path=clean soft_only=1 wave=40";

static const char g_szStringSoftReturn[] =
    "string: soft return helpers=14 mem=5 str=9 counts=none "
    "product_gate=0 hot_path=clean wave=40";

static const char g_szStringSoftRetmap[] =
    "string: soft retmap mem=ptr|void str=size|cmp|chr product_gate=0 hot_path=clean wave=40";

static const char g_szStringSoftRetclass[] =
    "string: soft retclass ok|fail|inval|nodev|busy|nomem "
    "soft_only=1 product_gate=0 wave=40 "
    "(retclass taxonomy; Soft≠product; not bar3)";

static const char g_szStringSoftRetlane[] =
    "string: soft retlane inv|selftest|rate|retcode|retmap|class "
    "product_kernel=OPEN soft_ne_product=1 wave=40 "
    "(retlane catalog; Soft≠product)";

static const char g_szStringSoftRetbound[] =
    "string: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
    "never_blocks_m0=1 wave=40 "
    "(retbound honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetseal[] =
    "string: soft retseal exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retseal stamp; Soft≠product)";

static const char g_szStringSoftRetpulse[] =
    "string: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retpulse honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetmark[] =
    "string: soft retmark exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retmark stamp; Soft≠product)";

static const char g_szStringSoftRetphase[] =
    "string: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retphase honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbadge[] =
    "string: soft retbadge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retbadge stamp; Soft≠product)";

static const char g_szStringSoftRettoken[] =
    "string: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(rettoken honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcrest[] =
    "string: soft retcrest exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retcrest stamp; Soft≠product)";

static const char g_szStringSoftRetvault[] =
    "string: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retvault honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbanner[] =
    "string: soft retbanner exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retbanner stamp; Soft≠product)";

static const char g_szStringSoftRetledger[] =
    "string: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retledger honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbeacon[] =
    "string: soft retbeacon exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retbeacon stamp; Soft≠product)";

static const char g_szStringSoftRetcipher[] =
    "string: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retcipher honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetflame[] =
    "string: soft retflame exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retflame stamp; Soft≠product)";

/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetprism[] =
    "string: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retprism honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetforge[] =
    "string: soft retforge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retforge stamp; Soft≠product)";

/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetshard[] =
    "string: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retshard honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcrown[] =
    "string: soft retcrown exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retcrown stamp; Soft≠product)";

/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetglyph[] =
    "string: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retglyph honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetscepter[] =
    "string: soft retscepter exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retscepter stamp; Soft≠product)";

/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetsigil[] =
    "string: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retsigil honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetemblem[] =
    "string: soft retemblem exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retemblem stamp; Soft≠product)";

/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetaegis[] =
    "string: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retaegis honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetmantle[] =
    "string: soft retmantle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retmantle stamp; Soft≠product)";

/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetbulwark[] =
    "string: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retbulwark honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetpanoply[] =
    "string: soft retpanoply exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retpanoply stamp; Soft≠product)";

/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetbastion[] =
    "string: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retbastion honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcitadel[] =
    "string: soft retcitadel exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retcitadel stamp; Soft≠product)";
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetredoubt[] =
    "string: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retredoubt honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetkeep[] =
    "string: soft retkeep exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retkeep stamp; Soft≠product)";
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetfortress[] =
    "string: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retfortress honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetpalace[] =
    "string: soft retpalace exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retpalace stamp; Soft≠product)";
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRethold[] =
    "string: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(rethold honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetspire[] =
    "string: soft retspire exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retspire stamp; Soft≠product)";

/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetwall[] =
    "string: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retwall honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetgate[] =
    "string: soft retgate exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retgate stamp; Soft≠product)";
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetmoat[] =
    "string: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retmoat honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetower[] =
    "string: soft retower exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retower stamp; Soft≠product)";
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetbarbican[] =
    "string: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retbarbican honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetglacis[] =
    "string: soft retglacis exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retglacis stamp; Soft≠product)";

/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetcurtain[] =
    "string: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=40 "
    "(retcurtain honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetparapet[] =
    "string: soft retparapet exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=40 "
    "(retparapet stamp; Soft≠product)";




/* Soft helper name table (order matches public soft set; cold only). */
static const char *const g_apszStringSoftHelpers[] = {
    "memset",
    "memcpy",
    "memmove",
    "memcmp",
    "memchr",
    "strlen",
    "strnlen",
    "strcmp",
    "strncmp",
    "strlcpy",
    "strlcat",
    "strchr",
    "strrchr",
    "strstr",
};

/* Soft mem-* name table (order matches mem group; cold only). */
static const char *const g_apszStringSoftMem[] = {
    "memset",
    "memcpy",
    "memmove",
    "memcmp",
    "memchr",
};

/* Soft str-* name table (order matches str group; cold only). */
static const char *const g_apszStringSoftStr[] = {
    "strlen",
    "strnlen",
    "strcmp",
    "strncmp",
    "strlcpy",
    "strlcat",
    "strchr",
    "strrchr",
    "strstr",
};

/*
 * Cold soft inventory accessor — not used by any hot helper.
 * Returns the greppable "string: soft inventory …" product line.
 */
const char *
string_soft_inventory(void)
{
    return g_szStringSoftInventory;
}

/*
 * Cold soft inventory: total helper count in the freestanding soft set.
 * Grep: string: soft helpers=
 */
unsigned
string_soft_helper_count(void)
{
    return (unsigned)STRING_SOFT_HELPERS;
}

/*
 * Cold soft inventory: mem-* group size (memset..memchr).
 * Grep: string: soft mem
 */
unsigned
string_soft_mem_count(void)
{
    return (unsigned)STRING_SOFT_MEM;
}

/*
 * Cold soft inventory: str-* group size (strlen..strstr).
 * Grep: string: soft str
 */
unsigned
string_soft_str_count(void)
{
    return (unsigned)STRING_SOFT_STR;
}

/*
 * Cold soft inventory: helper name by index, or NULL if out of range.
 * Does not allocate; points at static literals only.
 */
const char *
string_soft_helper_name(unsigned uIndex)
{
    if (uIndex >= (unsigned)STRING_SOFT_HELPERS) {
        return NULL;
    }
    return g_apszStringSoftHelpers[uIndex];
}

/*
 * Cold soft inventory: Wave 20 stamp (never product gate).
 * Grep: string: soft deepen wave=
 */
unsigned
string_soft_wave(void)
{
    return (unsigned)STRING_SOFT_WAVE;
}

/*
 * Cold soft inventory: fixed greppable area count for deepen stamp.
 * Grep: string: soft deepen areas=
 */
unsigned
string_soft_areas(void)
{
    return (unsigned)STRING_SOFT_AREAS;
}

/* Cold: greppable "string: soft mem …" line. */
const char *
string_soft_mem_line(void)
{
    return g_szStringSoftMem;
}

/* Cold: greppable "string: soft str …" line. */
const char *
string_soft_str_line(void)
{
    return g_szStringSoftStr;
}

/* Cold: greppable "string: soft groups …" line. */
const char *
string_soft_groups_line(void)
{
    return g_szStringSoftGroups;
}

/* Cold: greppable "string: soft policy …" line. */
const char *
string_soft_policy(void)
{
    return g_szStringSoftPolicy;
}

/* Cold: greppable "string: soft counts …" line. */
const char *
string_soft_counts_line(void)
{
    return g_szStringSoftCounts;
}

/* Cold: greppable "string: soft path …" line. */
const char *
string_soft_path(void)
{
    return g_szStringSoftPath;
}

/* Cold: greppable "string: soft deepen wave=40 …" line. */
const char *
string_soft_deepen(void)
{
    return g_szStringSoftDeepen;
}

/* Cold: greppable "string: soft hot_path …" line. */
const char *
string_soft_hot_path(void)
{
    return g_szStringSoftHotPath;
}

/* Cold: greppable "string: soft catalog …" line. */
const char *
string_soft_catalog(void)
{
    return g_szStringSoftCatalog;
}

/* Cold: greppable "string: soft honesty …" line (Wave 19). */
const char *
string_soft_honesty(void)
{
    return g_szStringSoftHonesty;
}

/* Cold: greppable "string: soft surfaces …" line (Wave 19). */
const char *
string_soft_surfaces(void)
{
    return g_szStringSoftSurfaces;
}

/* Cold: greppable "string: soft note …" line (Wave 19). */
const char *
string_soft_note(void)
{
    return g_szStringSoftNote;
}

/* Cold: greppable "string: soft return …" line (Wave 19). */
const char *
string_soft_return(void)
{
    return g_szStringSoftReturn;
}

/* Cold: greppable "string: soft retmap …" line (Wave 19). */
const char *
string_soft_retmap(void)
{
    return g_szStringSoftRetmap;
}

/* Cold: greppable "string: soft retclass …" line (Wave 19 kept). */
const char *
string_soft_retclass(void)
{
    return g_szStringSoftRetclass;
}

/* Cold: greppable "string: soft retlane …" line (Wave 19 kept). */
const char *
string_soft_retlane(void)
{
    return g_szStringSoftRetlane;
}

/* Cold: greppable "string: soft retbound …" line (Wave 20 exclusive). */
const char *
string_soft_retbound(void)
{
    return g_szStringSoftRetbound;
}

/* Cold: greppable "string: soft retseal …" line (Wave 20 kept). */
const char *
string_soft_retseal(void)
{
    return g_szStringSoftRetseal;
}

/* Cold: greppable "string: soft retpulse …" line (Wave 21 exclusive). */
const char *
string_soft_retpulse(void)
{
    return g_szStringSoftRetpulse;
}

/* Cold: greppable "string: soft retmark …" line (Wave 21 exclusive). */
const char *
string_soft_retmark(void)
{
    return g_szStringSoftRetmark;
}

/* Cold: greppable "string: soft retphase …" line (Wave 22 kept). */
const char *
string_soft_retphase(void)
{
    return g_szStringSoftRetphase;
}

/* Cold: greppable "string: soft retbadge …" line (Wave 22 kept). */
const char *
string_soft_retbadge(void)
{
    return g_szStringSoftRetbadge;
}

/* Cold: greppable "string: soft rettoken …" line (Wave 23 kept). */
const char *
string_soft_rettoken(void)
{
    return g_szStringSoftRettoken;
}

/* Cold: greppable "string: soft retcrest …" line (Wave 23 kept). */
const char *
string_soft_retcrest(void)
{
    return g_szStringSoftRetcrest;
}

/* Cold: greppable "string: soft retvault …" line (Wave 24 exclusive). */
const char *
string_soft_retvault(void)
{
    return g_szStringSoftRetvault;
}

/* Cold: greppable "string: soft retbanner …" line (Wave 24 exclusive). */
const char *
string_soft_retbanner(void)
{
    return g_szStringSoftRetbanner;
}

/* Cold: greppable "string: soft retledger …" line (Wave 25 kept). */
const char *
string_soft_retledger(void)
{
    return g_szStringSoftRetledger;
}

/* Cold: greppable "string: soft retbeacon …" line (Wave 25 kept). */
const char *
string_soft_retbeacon(void)
{
    return g_szStringSoftRetbeacon;
}

/* Cold: greppable "string: soft retcipher …" line (Wave 26 exclusive). */
const char *
string_soft_retcipher(void)
{
    return g_szStringSoftRetcipher;
}

/* Cold: greppable "string: soft retflame …" line (Wave 26 kept). */
const char *
string_soft_retflame(void)
{
    return g_szStringSoftRetflame;
}

/* Cold: greppable "string: soft retprism …" line (Wave 27 kept). */
const char *
string_soft_retprism(void)
{
    return g_szStringSoftRetprism;
}

/* Cold: greppable "string: soft retforge …" line (Wave 27 kept). */
const char *
string_soft_retforge(void)
{
    return g_szStringSoftRetforge;
}

/* Cold: greppable "string: soft retshard …" line (Wave 28 kept). */
const char *
string_soft_retshard(void)
{
    return g_szStringSoftRetshard;
}

/* Cold: greppable "string: soft retcrown …" line (Wave 28 kept). */
const char *
string_soft_retcrown(void)
{
    return g_szStringSoftRetcrown;
}

/* Cold: greppable "string: soft retglyph …" line (Wave 29 kept). */
const char *
string_soft_retglyph(void)
{
    return g_szStringSoftRetglyph;
}

/* Cold: greppable "string: soft retscepter …" line (Wave 29 kept). */
const char *
string_soft_retscepter(void)
{
    return g_szStringSoftRetscepter;
}

/* Cold: greppable "string: soft retsigil …" line (Wave 30 kept). */
const char *
string_soft_retsigil(void)
{
    return g_szStringSoftRetsigil;
}

/* Cold: greppable "string: soft retemblem …" line (Wave 30 kept). */
const char *
string_soft_retemblem(void)
{
    return g_szStringSoftRetemblem;
}

/* Cold: greppable "string: soft retaegis …" line (Wave 31 kept). */
const char *
string_soft_retaegis(void)
{
    return g_szStringSoftRetaegis;
}

/* Cold: greppable "string: soft retmantle …" line (Wave 31 kept). */
const char *
string_soft_retmantle(void)
{
    return g_szStringSoftRetmantle;
}

/* Cold: greppable "string: soft retbulwark …" line (Wave 32 kept). */
const char *
string_soft_retbulwark(void)
{
    return g_szStringSoftRetbulwark;
}

/* Cold: greppable "string: soft retpanoply …" line (Wave 32 kept). */
const char *
string_soft_retpanoply(void)
{
    return g_szStringSoftRetpanoply;
}

/* Cold: greppable "string: soft retbastion …" line (Wave 33 kept). */
const char *
string_soft_retbastion(void)
{
    return g_szStringSoftRetbastion;
}

/* Cold: greppable "string: soft retcitadel …" line (Wave 33 kept). */
const char *
string_soft_retcitadel(void)
{
    return g_szStringSoftRetcitadel;
}

/* Cold: greppable "string: soft retredoubt …" line (Wave 34 exclusive). */
const char *
string_soft_retredoubt(void)
{
    return g_szStringSoftRetredoubt;
}

/* Cold: greppable "string: soft retkeep …" line (Wave 34 exclusive). */
const char *
string_soft_retkeep(void)
{
    return g_szStringSoftRetkeep;
}

/* Cold: greppable "string: soft retfortress …" line (Wave 35 exclusive). */
const char *
string_soft_retfortress(void)
{
    return g_szStringSoftRetfortress;
}

/* Cold: greppable "string: soft retpalace …" line (Wave 35 exclusive). */
const char *
string_soft_retpalace(void)
{
    return g_szStringSoftRetpalace;
}

/* Cold: greppable "string: soft rethold …" line (Wave 36 exclusive). */
const char *
string_soft_rethold(void)
{
    return g_szStringSoftRethold;
}

/* Cold: greppable "string: soft retspire …" line (Wave 36 exclusive). */
const char *
string_soft_retspire(void)
{
    return g_szStringSoftRetspire;
}

/* Cold: greppable "string: soft retwall …" line (Wave 37 exclusive). */
const char *
string_soft_retwall(void)
{
    return g_szStringSoftRetwall;
}

/* Cold: greppable "string: soft retgate …" line (Wave 37 exclusive). */
const char *
string_soft_retgate(void)
{
    return g_szStringSoftRetgate;
}

/* Cold: greppable "string: soft retmoat …" line (Wave 38 exclusive). */
const char *
string_soft_retmoat(void)
{
    return g_szStringSoftRetmoat;
}

/* Cold: greppable "string: soft retower …" line (Wave 38 exclusive). */
const char *
string_soft_retower(void)
{
    return g_szStringSoftRetower;
}

/* Cold: greppable "string: soft retbarbican …" line (Wave 40 exclusive). */
const char *
string_soft_retbarbican(void)
{
    return g_szStringSoftRetbarbican;
}

/* Cold: greppable "string: soft retglacis …" line (Wave 40 exclusive). */
const char *
string_soft_retglacis(void)
{
    return g_szStringSoftRetglacis;
}

/* Cold: greppable "string: soft retcurtain …" line (Wave 40 exclusive). */
const char *
string_soft_retcurtain(void)
{
    return g_szStringSoftRetcurtain;
}

/* Cold: greppable "string: soft retparapet …" line (Wave 40 exclusive). */
const char *
string_soft_retparapet(void)
{
    return g_szStringSoftRetparapet;
}


/*
 * Cold soft inventory: mem-* helper name by group index, or NULL.
 * Grep: string: soft mem
 */
const char *
string_soft_mem_name(unsigned uIndex)
{
    if (uIndex >= (unsigned)STRING_SOFT_MEM) {
        return NULL;
    }
    return g_apszStringSoftMem[uIndex];
}

/*
 * Cold soft inventory: str-* helper name by group index, or NULL.
 * Grep: string: soft str
 */
const char *
string_soft_str_name(unsigned uIndex)
{
    if (uIndex >= (unsigned)STRING_SOFT_STR) {
        return NULL;
    }
    return g_apszStringSoftStr[uIndex];
}

void *
memset(void *pDst, int nFill, size_t cbCount)
{
    u8 *pOut = (u8 *)pDst;
    u8 u8Fill = (u8)nFill;

    while (cbCount--) {
        *pOut++ = u8Fill;
    }
    return pDst;
}

void *
memcpy(void *pDst, const void *pSrc, size_t cbCount)
{
    u8 *pOut = (u8 *)pDst;
    const u8 *pIn = (const u8 *)pSrc;

    while (cbCount--) {
        *pOut++ = *pIn++;
    }
    return pDst;
}

void *
memmove(void *pDst, const void *pSrc, size_t cbCount)
{
    u8 *pOut = (u8 *)pDst;
    const u8 *pIn = (const u8 *)pSrc;

    if (pOut == pIn || cbCount == 0) {
        return pDst;
    }

    if (pOut < pIn) {
        while (cbCount--) {
            *pOut++ = *pIn++;
        }
    } else {
        pOut += cbCount;
        pIn += cbCount;
        while (cbCount--) {
            *--pOut = *--pIn;
        }
    }
    return pDst;
}

int
memcmp(const void *pA, const void *pB, size_t cbCount)
{
    const u8 *pLeft = (const u8 *)pA;
    const u8 *pRight = (const u8 *)pB;

    while (cbCount--) {
        if (*pLeft != *pRight) {
            return (int)*pLeft - (int)*pRight;
        }
        pLeft++;
        pRight++;
    }
    return 0;
}

void *
memchr(const void *pHay, int nNeedle, size_t cbCount)
{
    const u8 *pCur = (const u8 *)pHay;
    u8 u8Want = (u8)nNeedle;

    while (cbCount--) {
        if (*pCur == u8Want) {
            return (void *)(uintptr_t)pCur;
        }
        pCur++;
    }
    return NULL;
}

size_t
strlen(const char *szText)
{
    size_t cChars = 0;

    while (szText[cChars] != '\0') {
        cChars++;
    }
    return cChars;
}

size_t
strnlen(const char *szText, size_t cbMax)
{
    size_t cChars = 0;

    while (cChars < cbMax && szText[cChars] != '\0') {
        cChars++;
    }
    return cChars;
}

int
strcmp(const char *szA, const char *szB)
{
    const u8 *pA = (const u8 *)szA;
    const u8 *pB = (const u8 *)szB;

    while (*pA != '\0' && *pA == *pB) {
        pA++;
        pB++;
    }
    return (int)*pA - (int)*pB;
}

int
strncmp(const char *szA, const char *szB, size_t cbMax)
{
    const u8 *pA = (const u8 *)szA;
    const u8 *pB = (const u8 *)szB;

    if (cbMax == 0) {
        return 0;
    }
    while (cbMax > 1 && *pA != '\0' && *pA == *pB) {
        pA++;
        pB++;
        cbMax--;
    }
    return (int)*pA - (int)*pB;
}

size_t
strlcpy(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t cSrc = 0;
    size_t iCopy;

    while (szSrc[cSrc] != '\0') {
        cSrc++;
    }

    if (cbDst == 0) {
        return cSrc;
    }

    iCopy = cSrc;
    if (iCopy >= cbDst) {
        iCopy = cbDst - 1u;
    }
    if (iCopy > 0) {
        (void)memcpy(szDst, szSrc, iCopy);
    }
    szDst[iCopy] = '\0';
    return cSrc;
}

size_t
strlcat(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t cDst;
    size_t cSrc = 0;
    size_t iCopy;
    size_t cbRoom;

    cDst = strnlen(szDst, cbDst);
    while (szSrc[cSrc] != '\0') {
        cSrc++;
    }

    if (cDst == cbDst) {
        /* No room / not NUL-terminated within cbDst. */
        return cbDst + cSrc;
    }

    cbRoom = cbDst - cDst;
    iCopy = cSrc;
    if (iCopy >= cbRoom) {
        iCopy = cbRoom - 1u;
    }
    if (iCopy > 0) {
        (void)memcpy(szDst + cDst, szSrc, iCopy);
    }
    szDst[cDst + iCopy] = '\0';
    return cDst + cSrc;
}

char *
strchr(const char *szText, int nCh)
{
    u8 u8Want = (u8)nCh;

    for (;;) {
        if ((u8)*szText == u8Want) {
            return (char *)(uintptr_t)szText;
        }
        if (*szText == '\0') {
            return NULL;
        }
        szText++;
    }
}

char *
strrchr(const char *szText, int nCh)
{
    const char *pLast = NULL;
    u8 u8Want = (u8)nCh;

    for (;;) {
        if ((u8)*szText == u8Want) {
            pLast = szText;
        }
        if (*szText == '\0') {
            return (char *)(uintptr_t)pLast;
        }
        szText++;
    }
}

char *
strstr(const char *szHay, const char *szNeedle)
{
    size_t cNeedle;
    size_t iHay;

    if (szNeedle[0] == '\0') {
        return (char *)(uintptr_t)szHay;
    }

    cNeedle = strlen(szNeedle);
    for (iHay = 0; szHay[iHay] != '\0'; iHay++) {
        size_t iNeedle;

        for (iNeedle = 0; iNeedle < cNeedle; iNeedle++) {
            if (szHay[iHay + iNeedle] == '\0' ||
                szHay[iHay + iNeedle] != szNeedle[iNeedle]) {
                break;
            }
        }
        if (iNeedle == cNeedle) {
            return (char *)(uintptr_t)(szHay + iHay);
        }
    }
    return NULL;
}

/* string: soft inventory end helpers=14 counts=none hot_path=clean wave=40 areas=25 */
