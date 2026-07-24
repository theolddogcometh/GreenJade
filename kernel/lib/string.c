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
 *   string: soft inventory helpers=14 groups=2 wave=110
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
 *   string: soft deepen wave=110 areas=177 …
 *
 * Call counts intentionally omitted: memset/memcpy/etc. stay freestanding
 * early-boot hot path with no counter traffic. Inventory is source + rodata
 * + cold accessors only (never consulted by helpers).
 * greppable: string: soft
 */
#include <gj/string.h>

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define STRING_SOFT_WAVE 110u

/*
 * Soft inventory area count (fixed greppable categories for deepen stamp):
 *   inventory | mem | str | groups | policy | counts | path | catalog |
 *   honesty | surfaces | note | return | retmap | retclass | retlane |
 *   retbound | retseal | retpulse | retmark | retphase | retbadge |
 *   rettoken | retcrest | retvault | retbanner | retledger | retbeacon |
 *   retcipher | retflame | retprism | retforge | retshard | retcrown | retglyph | retscepter |
 *   retsigil | retemblem | retaegis | retmantle | retbulwark | retpanoply |
 *   retbastion | retcitadel | retredoubt | retkeep | retfortress | retpalace | rethold | retspire | retwall | retgate | retmoat | retower | retbarbican | retglacis | retcurtain | retparapet | retravelin | retditch | retportcullis | retbattlement | retmachicolation | retarrowslit | retmerlon | retembrasure | retkeepgate | retouterward | retbailey | retpostern | retinnerward | retdonjon | retchevaux | retpalisade | retglacisgate | retoutwork | retsally | retcounterscarp | retfosse | retcoveredway | rettenaille | retdemilune | retravelin | retlunette | retcaponier | retredan | retflank | retface | retgorge | retshoulder | retraverse | retcasemate | retorillon | retbonnette | retcrownwork | rethornwork | retplace | retenvelope | retcounterguard | retcoveredface | retbastionface | retcurtainangle | retdoubletenaille | retplaceofarms | retreentrant | retsallyport | retgorgeangle | retshoulderangle | retflankangle | retfaceangle | retcaponierangle | retredanangle | retlunetteangle | rettenailleangle | retdemiluneangle | retcoveredwayangle | retfosseangle | retcounterscarple | retsallyportangle | retreentrantangle | retplaceofarmsangle | retdoubletenailleangle | retcurtainface | retbastionangle | retglacisangle | retparapetangle | retmoatangle | retowerangle | retgateangle | retwallangle | retspireangle | retholdangle | retpalaceangle | retfortressangle | retkeepangle | retredoubtangle | retcitadelangle | retbastionkeep | retpanoplyangle | retbulwarkangle | retmantleangle | retaegisangle | retemblemangle | retsigilangle | retscepterangle | retglyphangle | retcrownangle | retshardangle | retforgeangle | retprismangle | retflameangle | retcipherangle | retbeaconangle | retledgerangle | retbannerangle | retvaultangle | retcrestangle | rettokenangle | retbadgeangle | retphaseangle | retmarkangle | retpulseangle | retsealangle | retboundangle | retstemangle | retbladeangle | retchordangle | retarcangle | retsectorangle | retwedgeangle | retradiusangle | retdiameterangle | retcircumangle | retellipseangle | rethyperangle | retparabolaangle | retspiralangle | rethelixangle | rettorusangle | retknotangle | retmoebiusangle | retkleinangle | retprojectangle | retaffineangle | retlinearangle | retbilinearangle | retquadraticangle | retcubicangle | retquarticangle | retquinticangle | retsplineangle | retbezierangle | rethurmitangle | retcatmullangle | retnurbsangle | retbsplineangle | retmeshangle | retgridangle | deepen
 */
#define STRING_SOFT_AREAS 205u

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
    "string: soft inventory helpers=14 groups=2 wave=110 "
    "mem=memset,memcpy,memmove,memcmp,memchr "
    "str=strlen,strnlen,strcmp,strncmp,strlcpy,strlcat,strchr,strrchr,strstr "
    "policy=freestanding,pure_c,no_heap,no_locale,no_io "
    "counts=none hot_path=clean areas=83";

static const char g_szStringSoftMem[] =
    "string: soft mem memset,memcpy,memmove,memcmp,memchr count=5 "
    "group=mem wave=110";

static const char g_szStringSoftStr[] =
    "string: soft str strlen,strnlen,strcmp,strncmp,strlcpy,strlcat,"
    "strchr,strrchr,strstr count=9 group=str wave=110";

static const char g_szStringSoftGroups[] =
    "string: soft groups mem=5 str=9 total=14 wave=110";

static const char g_szStringSoftPolicy[] =
    "string: soft policy freestanding pure_c no_heap no_locale no_io "
    "early_boot=1 counters=0 wave=110";

static const char g_szStringSoftCounts[] =
    "string: soft counts none hot_path=clean reason=early_boot "
    "wave=110 (intentional; helpers stay cold of tallies)";

static const char g_szStringSoftPath[] =
    "string: soft path claim=freestanding mem=5 str=9 helpers=14 "
    "counts=none hot_path=clean no_heap=1 no_locale=1 no_io=1 "
    "early_boot=1 wave=110 (soft inventory; not libc)";

static const char g_szStringSoftDeepen[] =
    "string: soft deepen wave=110 areas=177 helpers=14 mem=5 str=9 "
    "return=1 retmap=1 retclass=1 retlane=1 retbound=1 retseal=1 retpulse=1 "
    "retmark=1 retphase=1 retbadge=1 rettoken=1 retcrest=1 retvault=1 retbanner=1 retledger=1 retbeacon=1 retcipher=1 retflame=1 retprism=1 retforge=1 retshard=1 retcrown=1 retglyph=1 retscepter=1 retsigil=1 retemblem=1 retaegis=1 retmantle=1 retbulwark=1 retpanoply=1 retbastion=1 retcitadel=1 retredoubt=1 retkeep=1 retfortress=1 retpalace=1 rethold=1 retspire=1 retwall=1 retgate=1 retmoat=1 retower=1 retbarbican=1 retglacis=1 retcurtain=1 retparapet=1 retravelin=1 retditch=1 retportcullis=1 retbattlement=1 retmachicolation=1 retarrowslit=1 retmerlon=1 retembrasure=1 retkeepgate=1 retouterward=1 retbailey=1 retpostern=1 retinnerward=1 retdonjon=1 retchevaux=1 retpalisade=1 retglacisgate=1 retoutwork=1 retsally=1 retcounterscarp=1 retfosse=1 retcoveredway=1 rettenaille=1 retdemilune=1 retravelin=1 retlunette=1 retcaponier=1 retredan=1 retflank=1 retface=1 retgorge=1 retshoulder=1 retraverse=1 retcasemate=1 retorillon=1 retbonnette=1 retcrownwork=1 rethornwork=1 retplace=1 retenvelope=1 retcounterguard=1 retcoveredface=1 retbastionface=1 retcurtainangle=1 retdoubletenaille=1 retplaceofarms=1 retreentrant=1 retsallyport=1 retgorgeangle=1 retshoulderangle=1 retflankangle=1 retfaceangle=1 retcaponierangle=1 retredanangle=1 retlunetteangle=1 rettenailleangle=1 retdemiluneangle=1 retcoveredwayangle=1 retfosseangle=1 retcounterscarple=1 retsallyportangle=1 retreentrantangle=1 retplaceofarmsangle=1 retdoubletenailleangle=1 retcurtainface=1 retbastionangle=1 retglacisangle=1 retparapetangle=1 retglacisangle=1 retparapetangle=1 retmoatangle=1 retowerangle=1 retgateangle=1 retwallangle=1 retspireangle=1 retholdangle=1 retpalaceangle=1 retfortressangle=1 retkeepangle=1 retredoubtangle=1 retcitadelangle=1 retbastionkeep=1 retpanoplyangle=1 retbulwarkangle=1 retmantleangle=1 retaegisangle=1 retemblemangle=1 retsigilangle=1 retscepterangle=1 retglyphangle=1 retcrownangle=1 retshardangle=1 retforgeangle=1 retprismangle=1 retflameangle=1 retcipherangle=1 retbeaconangle=1 retledgerangle=1 retbannerangle=1 retvaultangle=1 retcrestangle=1 rettokenangle=1 retbadgeangle=1 retphaseangle=1 retmarkangle=1 retpulseangle=1 retsealangle=1 retboundangle=1 retstemangle=1 retbladeangle=1 retchordangle=1 retarcangle=1 retsectorangle=1 retwedgeangle=1 retradiusangle=1 retdiameterangle=1 retcircumangle=1 retellipseangle=1 rethyperangle=1 retparabolaangle=1 retspiralangle=1 rethelixangle=1 rettorusangle=1 retknotangle=1 retmoebiusangle=1 retkleinangle=1 retprojectangle=1 retaffineangle=1 retlinearangle=1 retbilinearangle=1 retquadraticangle=1 retcubicangle=1 retquarticangle=1 retquinticangle=1 retsplineangle=1 retbezierangle=1 rethurmitangle=1 retcatmullangle=1 retnurbsangle=1 retbsplineangle=1 retmeshangle=1 retgridangle=1 "
    "counts=none hot_path=clean (Wave 71 exclusive; soft only)";

static const char g_szStringSoftHotPath[] =
    "string: soft hot_path clean counters=0 early_boot=1 wave=110";

static const char g_szStringSoftCatalog[] =
    "string: soft catalog helpers=14 mem=5 str=9 areas=113 "
    "idx=mem0..4,str0..8 hot_path=clean wave=110";

static const char g_szStringSoftHonesty[] =
    "string: soft honesty freestanding=1 pure_c=1 no_heap=1 "
    "no_locale=1 no_io=1 counters=0 not_libc=1 soft_only=1 "
    "wave=110 (soft inventory; not bar3)";

static const char g_szStringSoftSurfaces[] =
    "string: soft surfaces count=26 "
    "names=inventory,mem,str,groups,policy,counts,path,catalog,"
    "honesty,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,retmachicolation,retarrowslit,retmerlon,retembrasure,retkeepgate,retouterward,retbailey,retpostern,retinnerward,retdonjon,retchevaux,retpalisade,retglacisgate,retoutwork,retsally,retcounterscarp,retfosse,retcoveredway,rettenaille,retdemilune,retravelin,retlunette,retcaponier,retredan,retflank,retface,retgorge,retshoulder,retraverse,retcasemate,retorillon,retbonnette,retcrownwork,rethornwork,retplace,retenvelope,retcounterguard,retcoveredface,retbastionface,retcurtainangle,retdoubletenaille,retplaceofarms,retreentrant,retsallyport,retgorgeangle,retshoulderangle,retflankangle,retfaceangle,retcaponierangle,retredanangle,retlunetteangle,rettenailleangle,retdemiluneangle,retcoveredwayangle,retfosseangle,retcounterscarple,retsallyportangle,retreentrantangle,retplaceofarmsangle,retdoubletenailleangle,retcurtainface,retbastionangle,retglacisangle,retparapetangle,retmoatangle,retowerangle,retgateangle,retwallangle,retspireangle,retholdangle,retpalaceangle,retfortressangle,retkeepangle,retredoubtangle,retcitadelangle,retbastionkeep,retpanoplyangle,retbulwarkangle,retmantleangle,retaegisangle,retemblemangle,retsigilangle,retscepterangle,retglyphangle,retcrownangle,retshardangle,retforgeangle,retprismangle,retflameangle,retcipherangle,retbeaconangle,retledgerangle,retbannerangle,retvaultangle,retcrestangle,rettokenangle,retbadgeangle,retphaseangle,retmarkangle,retpulseangle,retsealangle,retboundangle,retstemangle,retbladeangle,retchordangle,retarcangle,retsectorangle,retwedgeangle,retradiusangle,retdiameterangle,retcircumangle,retellipseangle,rethyperangle,retparabolaangle,retspiralangle,rethelixangle,rettorusangle,retknotangle,retmoebiusangle,retkleinangle,retprojectangle,retaffineangle,retlinearangle,retbilinearangle,retquadraticangle,retcubicangle,retquarticangle,retquinticangle,retsplineangle,retbezierangle,rethurmitangle,retcatmullangle,retnurbsangle,retbsplineangle,retmeshangle,retgridangle,deepen,PASS wave=110";

static const char g_szStringSoftNote[] =
    "string: soft note milestone=wave55 exclusive=1 "
    "helpers=14 mem=5 str=9 hot_path=clean soft_only=1 wave=110";

static const char g_szStringSoftReturn[] =
    "string: soft return helpers=14 mem=5 str=9 counts=none "
    "product_gate=0 hot_path=clean wave=110";

static const char g_szStringSoftRetmap[] =
    "string: soft retmap mem=ptr|void str=size|cmp|chr product_gate=0 hot_path=clean wave=110";

static const char g_szStringSoftRetclass[] =
    "string: soft retclass ok|fail|inval|nodev|busy|nomem "
    "soft_only=1 product_gate=0 wave=110 "
    "(retclass taxonomy; Soft≠product; not bar3)";

static const char g_szStringSoftRetlane[] =
    "string: soft retlane inv|selftest|rate|retcode|retmap|class "
    "product_kernel=OPEN soft_ne_product=1 wave=110 "
    "(retlane catalog; Soft≠product)";

static const char g_szStringSoftRetbound[] =
    "string: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
    "never_blocks_m0=1 wave=110 "
    "(retbound honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetseal[] =
    "string: soft retseal exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retseal stamp; Soft≠product)";

static const char g_szStringSoftRetpulse[] =
    "string: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retpulse honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetmark[] =
    "string: soft retmark exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retmark stamp; Soft≠product)";

static const char g_szStringSoftRetphase[] =
    "string: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retphase honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbadge[] =
    "string: soft retbadge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retbadge stamp; Soft≠product)";

static const char g_szStringSoftRettoken[] =
    "string: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(rettoken honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcrest[] =
    "string: soft retcrest exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcrest stamp; Soft≠product)";

static const char g_szStringSoftRetvault[] =
    "string: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retvault honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbanner[] =
    "string: soft retbanner exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retbanner stamp; Soft≠product)";

static const char g_szStringSoftRetledger[] =
    "string: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retledger honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbeacon[] =
    "string: soft retbeacon exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retbeacon stamp; Soft≠product)";

static const char g_szStringSoftRetcipher[] =
    "string: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retcipher honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetflame[] =
    "string: soft retflame exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retflame stamp; Soft≠product)";

/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetprism[] =
    "string: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retprism honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetforge[] =
    "string: soft retforge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retforge stamp; Soft≠product)";

/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetshard[] =
    "string: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retshard honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcrown[] =
    "string: soft retcrown exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcrown stamp; Soft≠product)";

/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetglyph[] =
    "string: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retglyph honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetscepter[] =
    "string: soft retscepter exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retscepter stamp; Soft≠product)";

/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetsigil[] =
    "string: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retsigil honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetemblem[] =
    "string: soft retemblem exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retemblem stamp; Soft≠product)";

/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetaegis[] =
    "string: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retaegis honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetmantle[] =
    "string: soft retmantle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retmantle stamp; Soft≠product)";

/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetbulwark[] =
    "string: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retbulwark honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetpanoply[] =
    "string: soft retpanoply exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retpanoply stamp; Soft≠product)";

/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetbastion[] =
    "string: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retbastion honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcitadel[] =
    "string: soft retcitadel exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcitadel stamp; Soft≠product)";
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetredoubt[] =
    "string: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retredoubt honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetkeep[] =
    "string: soft retkeep exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retkeep stamp; Soft≠product)";
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetfortress[] =
    "string: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retfortress honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetpalace[] =
    "string: soft retpalace exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retpalace stamp; Soft≠product)";
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRethold[] =
    "string: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(rethold honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetspire[] =
    "string: soft retspire exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retspire stamp; Soft≠product)";

/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetwall[] =
    "string: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retwall honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetgate[] =
    "string: soft retgate exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retgate stamp; Soft≠product)";
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetmoat[] =
    "string: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retmoat honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetower[] =
    "string: soft retower exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retower stamp; Soft≠product)";
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetbarbican[] =
    "string: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retbarbican honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetglacis[] =
    "string: soft retglacis exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retglacis stamp; Soft≠product)";

/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetcurtain[] =
    "string: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retcurtain honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetparapet[] =
    "string: soft retparapet exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retparapet stamp; Soft≠product)";
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetravelin[] =
    "string: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retravelin honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetditch[] =
    "string: soft retditch exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retditch stamp; Soft≠product)";

/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
static const char g_szStringSoftRetportcullis[] =
    "string: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retportcullis honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbattlement[] =
    "string: soft retbattlement exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retbattlement stamp; Soft≠product)";
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retmachicolation/retarrowslit.
 */
static const char g_szStringSoftRetmachicolation[] =
    "string: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retmachicolation honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetarrowslit[] =
    "string: soft retarrowslit exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retarrowslit stamp; Soft≠product)";

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retmerlon/retembrasure.
 */
static const char g_szStringSoftRetmerlon[] =
    "string: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retmerlon honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetembrasure[] =
    "string: soft retembrasure exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retembrasure stamp; Soft≠product)";

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retkeepgate/retouterward.
 */
static const char g_szStringSoftRetkeepgate[] =
    "string: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retkeepgate honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetouterward[] =
    "string: soft retouterward exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retouterward stamp; Soft≠product)";

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retbailey/retpostern.
 */
static const char g_szStringSoftRetbailey[] =
    "string: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retbailey honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetpostern[] =
    "string: soft retpostern exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retpostern stamp; Soft≠product)";

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retinnerward/retdonjon.
 */
static const char g_szStringSoftRetinnerward[] =
    "string: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retinnerward honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetdonjon[] =
    "string: soft retdonjon exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retdonjon stamp; Soft≠product)";

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retchevaux/retpalisade.
 */
static const char g_szStringSoftRetchevaux[] =
    "string: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retchevaux honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetpalisade[] =
    "string: soft retpalisade exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retpalisade stamp; Soft≠product)";

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retglacisgate/retoutwork.
 */
static const char g_szStringSoftRetglacisgate[] =
    "string: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retglacisgate honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetoutwork[] =
    "string: soft retoutwork exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retoutwork stamp; Soft≠product)";

/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retsally/retcounterscarp.
 */
static const char g_szStringSoftRetsally[] =
    "string: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retsally honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcounterscarp[] =
    "string: soft retcounterscarp exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcounterscarp stamp; Soft≠product)";

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retfosse/retcoveredway.
 */
static const char g_szStringSoftRetfosse[] =
    "string: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retfosse honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcoveredway[] =
    "string: soft retcoveredway exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcoveredway stamp; Soft≠product)";

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. rettenaille/retdemilune.
 */
static const char g_szStringSoftRettenaille[] =
    "string: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(rettenaille honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetdemilune[] =
    "string: soft retdemilune exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retdemilune stamp; Soft≠product)";
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retravelin/retlunette.
 * Grep: string: soft retravelin — Wave 41 rodata retained; wave field current (Wave 53 pair).
 * Grep: string: soft retlunette — Wave 53 exclusive lunette stamp.
 */
static const char g_szStringSoftRetlunette[] =
    "string: soft retlunette exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retlunette stamp; Soft≠product)";
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcaponier/retredan.
 * Grep: string: soft retcaponier — Wave 54 return-caponier honesty.
 * Grep: string: soft retredan — Wave 54 exclusive redan stamp.
 */
static const char g_szStringSoftRetcaponier[] =
    "string: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retcaponier honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetredan[] =
    "string: soft retredan exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retredan stamp; Soft≠product)";

/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retflank/retface.
 * Grep: string: soft retflank — Wave 55 return-flank honesty.
 * Grep: string: soft retface — Wave 55 exclusive face stamp.
 */
static const char g_szStringSoftRetflank[] =
    "string: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retflank honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetface[] =
    "string: soft retface exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retface stamp; Soft≠product)";

/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retgorge/retshoulder.
 * Grep: string: soft retgorge — Wave 56 return-gorge honesty.
 * Grep: string: soft retshoulder — Wave 56 exclusive shoulder stamp.
 */
static const char g_szStringSoftRetgorge[] =
    "string: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retgorge honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetshoulder[] =
    "string: soft retshoulder exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retshoulder stamp; Soft≠product)";

/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retraverse/retcasemate.
 * Grep: string: soft retraverse — Wave 57 return-traverse honesty.
 * Grep: string: soft retcasemate — Wave 57 exclusive casemate stamp.
 */
static const char g_szStringSoftRetraverse[] =
    "string: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retraverse honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcasemate[] =
    "string: soft retcasemate exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcasemate stamp; Soft≠product)";

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retorillon/retbonnette.
 * Grep: string: soft retorillon — Wave 58 return-orillon honesty.
 * Grep: string: soft retbonnette — Wave 58 exclusive bonnette stamp.
 */
static const char g_szStringSoftRetorillon[] =
    "string: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retorillon honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetbonnette[] =
    "string: soft retbonnette exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retbonnette stamp; Soft≠product)";

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcrownwork/rethornwork.
 * Grep: string: soft retcrownwork — Wave 59 return-crownwork honesty.
 * Grep: string: soft rethornwork — Wave 59 exclusive hornwork stamp.
 */
static const char g_szStringSoftRetcrownwork[] =
    "string: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retcrownwork honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRethornwork[] =
    "string: soft rethornwork exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(rethornwork stamp; Soft≠product)";





/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retplace/retenvelope.
 * Grep: string: soft retplace — Wave 60 return-place honesty.
 * Grep: string: soft retenvelope — Wave 60 exclusive envelope stamp.
 */
static const char g_szStringSoftRetplace[] =
    "string: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retplace honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetenvelope[] =
    "string: soft retenvelope exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retenvelope stamp; Soft≠product)";

/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcounterguard/retcoveredface.
 * Grep: string: soft retcounterguard — Wave 61 return-counterguard honesty.
 * Grep: string: soft retcoveredface — Wave 61 exclusive coveredface stamp.
 */
static const char g_szStringSoftRetcounterguard[] =
    "string: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retcounterguard honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcoveredface[] =
    "string: soft retcoveredface exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcoveredface stamp; Soft≠product)";
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retbastionface/retcurtainangle.
 * Grep: string: soft retbastionface — Wave 62 return-bastionface honesty.
 * Grep: string: soft retcurtainangle — Wave 62 exclusive curtainangle stamp.
 */
static const char g_szStringSoftRetbastionface[] =
    "string: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retbastionface honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcurtainangle[] =
    "string: soft retcurtainangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcurtainangle stamp; Soft≠product)";

/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retdoubletenaille/retplaceofarms.
 * Grep: string: soft retdoubletenaille — Wave 63 return-doubletenaille honesty.
 * Grep: string: soft retplaceofarms — Wave 63 exclusive placeofarms stamp.
 */
static const char g_szStringSoftRetdoubletenaille[] =
    "string: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retdoubletenaille honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetplaceofarms[] =
    "string: soft retplaceofarms exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retplaceofarms stamp; Soft≠product)";

/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retreentrant/retsallyport.
 * Grep: string: soft retreentrant — Wave 64 return-reentrant honesty.
 * Grep: string: soft retsallyport — Wave 64 exclusive sallyport stamp.
 */
static const char g_szStringSoftRetreentrant[] =
    "string: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retreentrant honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetsallyport[] =
    "string: soft retsallyport exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retsallyport stamp; Soft≠product)";

/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retgorgeangle/retshoulderangle.
 * Grep: string: soft retgorgeangle — Wave 65 return-gorgeangle honesty.
 * Grep: string: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp.
 */
static const char g_szStringSoftRetgorgeangle[] =
    "string: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retgorgeangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetshoulderangle[] =
    "string: soft retshoulderangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retshoulderangle stamp; Soft≠product)";

/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retflankangle/retfaceangle.
 * Grep: string: soft retflankangle — Wave 66 return-flankangle honesty.
 * Grep: string: soft retfaceangle — Wave 66 exclusive faceangle stamp.
 */
static const char g_szStringSoftRetflankangle[] =
    "string: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retflankangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetfaceangle[] =
    "string: soft retfaceangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retfaceangle stamp; Soft≠product)";

/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcaponierangle/retredanangle.
 * Grep: string: soft retcaponierangle — Wave 67 return-caponierangle honesty.
 * Grep: string: soft retredanangle — Wave 67 exclusive redanangle stamp.
 */
static const char g_szStringSoftRetcaponierangle[] =
    "string: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retcaponierangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetredanangle[] =
    "string: soft retredanangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retredanangle stamp; Soft≠product)";

/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retlunetteangle/rettenailleangle.
 * Grep: string: soft retlunetteangle — Wave 68 return-lunetteangle honesty.
 * Grep: string: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp.
 */
static const char g_szStringSoftRetlunetteangle[] =
    "string: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retlunetteangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRettenailleangle[] =
    "string: soft rettenailleangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(rettenailleangle stamp; Soft≠product)";

/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retdemiluneangle/retcoveredwayangle.
 * Grep: string: soft retdemiluneangle — Wave 69 return-demiluneangle honesty.
 * Grep: string: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp.
 */
static const char g_szStringSoftRetdemiluneangle[] =
    "string: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=110 "
    "(retdemiluneangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcoveredwayangle[] =
    "string: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=110 "
    "(retcoveredwayangle stamp; Soft≠product)";

/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retfosseangle/retcounterscarple.
 * Grep: string: soft retfosseangle — Wave 70 return-fosseangle honesty.
 * Grep: string: soft retcounterscarple — Wave 70 exclusive counterscarple stamp.
 */
static const char g_szStringSoftRetfosseangle[] =
    "string: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retfosseangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetcounterscarple[] =
    "string: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcounterscarple stamp; Soft≠product)";

/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retsallyportangle/retreentrantangle.
 * Grep: string: soft retsallyportangle — Wave 71 return-sallyportangle honesty.
 * Grep: string: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp.
 */
static const char g_szStringSoftRetsallyportangle[] =
    "string: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsallyportangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetreentrantangle[] =
    "string: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retreentrantangle stamp; Soft≠product)";








/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retplaceofarmsangle/retdoubletenailleangle.
 * Grep: string: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty.
 * Grep: string: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp.
 */
static const char g_szStringSoftRetplaceofarmsangle[] =
    "string: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retplaceofarmsangle honesty; Soft≠product; not bar3)";

static const char g_szStringSoftRetdoubletenailleangle[] =
    "string: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retdoubletenailleangle stamp; Soft≠product)";
/* Grep: string: soft retcurtainface — Wave 73 return-curtainface honesty */
static const char g_szStringSoftRetcurtainface[] =
    "string: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcurtainface honesty; Soft≠product; not bar3)";
/* Grep: string: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
static const char g_szStringSoftRetbastionangle[] =
    "string: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbastionangle stamp; Soft≠product)";
/* Grep: string: soft retglacisangle — Wave 74 return-glacisangle honesty */
static const char g_szStringSoftRetglacisangle[] =
    "string: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retglacisangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
static const char g_szStringSoftRetparapetangle[] =
    "string: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retparapetangle stamp; Soft≠product)";
/* Grep: string: soft retmoatangle — Wave 75 return-moatangle honesty */
static const char g_szStringSoftRetmoatangle[] =
    "string: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmoatangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retowerangle — Wave 75 exclusive towerangle stamp */
static const char g_szStringSoftRetowerangle[] =
    "string: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retowerangle stamp; Soft≠product)";
/* Grep: string: soft retgateangle — Wave 76 return-gateangle honesty */
static const char g_szStringSoftRetgateangle[] =
    "string: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retgateangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retwallangle — Wave 76 exclusive wallangle stamp */
static const char g_szStringSoftRetwallangle[] =
    "string: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retwallangle stamp; Soft≠product)";
/* Grep: string: soft retspireangle — Wave 77 return-spireangle honesty */
static const char g_szStringSoftRetspireangle[] =
    "string: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retspireangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retholdangle — Wave 77 exclusive holdangle stamp */
static const char g_szStringSoftRetholdangle[] =
    "string: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retholdangle stamp; Soft≠product)";
/* Grep: string: soft retpalaceangle — Wave 78 return-palaceangle honesty */
static const char g_szStringSoftRetpalaceangle[] =
    "string: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retpalaceangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
static const char g_szStringSoftRetfortressangle[] =
    "string: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retfortressangle stamp; Soft≠product)";
/* Grep: string: soft retkeepangle — Wave 79 return-keepangle honesty */
static const char g_szStringSoftRetkeepangle[] =
    "string: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retkeepangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
static const char g_szStringSoftRetredoubtangle[] =
    "string: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retredoubtangle stamp; Soft≠product)";
/* Grep: string: soft retcitadelangle — Wave 80 return-citadelangle honesty */
static const char g_szStringSoftRetcitadelangle[] =
    "string: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcitadelangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
static const char g_szStringSoftRetbastionkeep[] =
    "string: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbastionkeep stamp; Soft≠product)";
/* Grep: string: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
static const char g_szStringSoftRetpanoplyangle[] =
    "string: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retpanoplyangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
static const char g_szStringSoftRetbulwarkangle[] =
    "string: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbulwarkangle stamp; Soft≠product)";
/* Grep: string: soft retmantleangle — Wave 82 return-mantleangle honesty */
static const char g_szStringSoftRetmantleangle[] =
    "string: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmantleangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
static const char g_szStringSoftRetaegisangle[] =
    "string: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retaegisangle stamp; Soft≠product)";
/* Grep: string: soft retemblemangle — Wave 83 return-emblemangle honesty */
static const char g_szStringSoftRetemblemangle[] =
    "string: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retemblemangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
static const char g_szStringSoftRetsigilangle[] =
    "string: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retsigilangle stamp; Soft≠product)";
/* Grep: string: soft retscepterangle — Wave 84 return-scepterangle honesty */
static const char g_szStringSoftRetscepterangle[] =
    "string: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retscepterangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
static const char g_szStringSoftRetglyphangle[] =
    "string: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retglyphangle stamp; Soft≠product)";
/* Grep: string: soft retcrownangle — Wave 85 return-crownangle honesty */
static const char g_szStringSoftRetcrownangle[] =
    "string: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcrownangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retshardangle — Wave 85 exclusive shardangle stamp */
static const char g_szStringSoftRetshardangle[] =
    "string: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retshardangle stamp; Soft≠product)";
/* Grep: string: soft retforgeangle — Wave 86 return-forgeangle honesty */
static const char g_szStringSoftRetforgeangle[] =
    "string: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retforgeangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retprismangle — Wave 86 exclusive prismangle stamp */
static const char g_szStringSoftRetprismangle[] =
    "string: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retprismangle stamp; Soft≠product)";
/* Grep: string: soft retflameangle — Wave 87 return-flameangle honesty */
static const char g_szStringSoftRetflameangle[] =
    "string: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retflameangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
static const char g_szStringSoftRetcipherangle[] =
    "string: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcipherangle stamp; Soft≠product)";
/* Grep: string: soft retbeaconangle — Wave 88 return-beaconangle honesty */
static const char g_szStringSoftRetbeaconangle[] =
    "string: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retbeaconangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
static const char g_szStringSoftRetledgerangle[] =
    "string: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retledgerangle stamp; Soft≠product)";
/* Grep: string: soft retbannerangle — Wave 89 return-bannerangle honesty */
static const char g_szStringSoftRetbannerangle[] =
    "string: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retbannerangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
static const char g_szStringSoftRetvaultangle[] =
    "string: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retvaultangle stamp; Soft≠product)";
/* Grep: string: soft retcrestangle — Wave 90 return-crestangle honesty */
static const char g_szStringSoftRetcrestangle[] =
    "string: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcrestangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
static const char g_szStringSoftRettokenangle[] =
    "string: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (rettokenangle stamp; Soft≠product)";
/* Grep: string: soft retbadgeangle — Wave 91 return-badgeangle honesty */
static const char g_szStringSoftRetbadgeangle[] =
    "string: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retbadgeangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
static const char g_szStringSoftRetphaseangle[] =
    "string: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retphaseangle stamp; Soft≠product)";
/* Grep: string: soft retmarkangle — Wave 92 return-markangle honesty */
static const char g_szStringSoftRetmarkangle[] =
    "string: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmarkangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
static const char g_szStringSoftRetpulseangle[] =
    "string: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retpulseangle stamp; Soft≠product)";
/* Grep: string: soft retsealangle — Wave 93 return-sealangle honesty */
static const char g_szStringSoftRetsealangle[] =
    "string: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsealangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retboundangle — Wave 93 exclusive boundangle stamp */
static const char g_szStringSoftRetboundangle[] =
    "string: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retboundangle stamp; Soft≠product)";
/* Grep: string: soft retstemangle — Wave 94 return-stemangle honesty */
static const char g_szStringSoftRetstemangle[] =
    "string: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retstemangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
static const char g_szStringSoftRetbladeangle[] =
    "string: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbladeangle stamp; Soft≠product)";
/* Grep: string: soft retchordangle — Wave 95 return-chordangle honesty */
static const char g_szStringSoftRetchordangle[] =
    "string: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retchordangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retarcangle — Wave 95 exclusive arcangle stamp */
static const char g_szStringSoftRetarcangle[] =
    "string: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retarcangle stamp; Soft≠product)";
/* Grep: string: soft retsectorangle — Wave 96 return-sectorangle honesty */
static const char g_szStringSoftRetsectorangle[] =
    "string: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsectorangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
static const char g_szStringSoftRetwedgeangle[] =
    "string: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retwedgeangle stamp; Soft≠product)";
/* Grep: string: soft retradiusangle — Wave 97 return-radiusangle honesty */
static const char g_szStringSoftRetradiusangle[] =
    "string: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retradiusangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
static const char g_szStringSoftRetdiameterangle[] =
    "string: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retdiameterangle stamp; Soft≠product)";
/* Grep: string: soft retcircumangle — Wave 98 return-circumangle honesty */
static const char g_szStringSoftRetcircumangle[] =
    "string: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcircumangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
static const char g_szStringSoftRetellipseangle[] =
    "string: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retellipseangle stamp; Soft≠product)";
/* Grep: string: soft rethyperangle — Wave 99 return-hyperangle honesty */
static const char g_szStringSoftRethyperangle[] =
    "string: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (rethyperangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
static const char g_szStringSoftRetparabolaangle[] =
    "string: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retparabolaangle stamp; Soft≠product)";
/* Grep: string: soft retspiralangle — Wave 100 return-spiralangle honesty */
static const char g_szStringSoftRetspiralangle[] =
    "string: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retspiralangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft rethelixangle — Wave 100 exclusive helixangle stamp */
static const char g_szStringSoftRethelixangle[] =
    "string: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (rethelixangle stamp; Soft≠product)";
/* Grep: string: soft rettorusangle — Wave 101 return-torusangle honesty */
static const char g_szStringSoftRettorusangle[] =
    "string: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (rettorusangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retknotangle — Wave 101 exclusive knotangle stamp */
static const char g_szStringSoftRetknotangle[] =
    "string: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retknotangle stamp; Soft≠product)";
/* Grep: string: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
static const char g_szStringSoftRetmoebiusangle[] =
    "string: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmoebiusangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
static const char g_szStringSoftRetkleinangle[] =
    "string: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retkleinangle stamp; Soft≠product)";
/* Grep: string: soft retprojectangle — Wave 103 return-projectangle honesty */
static const char g_szStringSoftRetprojectangle[] =
    "string: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retprojectangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retaffineangle — Wave 103 exclusive affineangle stamp */
static const char g_szStringSoftRetaffineangle[] =
    "string: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retaffineangle stamp; Soft≠product)";
/* Grep: string: soft retlinearangle — Wave 104 return-linearangle honesty */
static const char g_szStringSoftRetlinearangle[] =
    "string: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retlinearangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
static const char g_szStringSoftRetbilinearangle[] =
    "string: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbilinearangle stamp; Soft≠product)";
/* Grep: string: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
static const char g_szStringSoftRetquadraticangle[] =
    "string: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retquadraticangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
static const char g_szStringSoftRetcubicangle[] =
    "string: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcubicangle stamp; Soft≠product)";
/* Grep: string: soft retquarticangle — Wave 106 return-quarticangle honesty */
static const char g_szStringSoftRetquarticangle[] =
    "string: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retquarticangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
static const char g_szStringSoftRetquinticangle[] =
    "string: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retquinticangle stamp; Soft≠product)";
/* Grep: string: soft retsplineangle — Wave 107 return-splineangle honesty */
static const char g_szStringSoftRetsplineangle[] =
    "string: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsplineangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
static const char g_szStringSoftRetbezierangle[] =
    "string: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbezierangle stamp; Soft≠product)";
/* Grep: string: soft rethurmitangle — Wave 108 return-hermitangle honesty */
static const char g_szStringSoftRethurmitangle[] =
    "string: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (rethurmitangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
static const char g_szStringSoftRetcatmullangle[] =
    "string: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcatmullangle stamp; Soft≠product)";
/* Grep: string: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
static const char g_szStringSoftRetnurbsangle[] =
    "string: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retnurbsangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
static const char g_szStringSoftRetbsplineangle[] =
    "string: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbsplineangle stamp; Soft≠product)";
/* Grep: string: soft retmeshangle — Wave 110 return-meshangle honesty */
static const char g_szStringSoftRetmeshangle[] =
    "string: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmeshangle honesty; Soft≠product; not bar3)";
/* Grep: string: soft retgridangle — Wave 110 exclusive gridangle stamp */
static const char g_szStringSoftRetgridangle[] =
    "string: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retgridangle stamp; Soft≠product)";

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

/* Cold: greppable "string: soft deepen wave=110 …" line. */
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

/* Cold: greppable "string: soft retbarbican …" line (Wave 41 exclusive). */
const char *
string_soft_retbarbican(void)
{
    return g_szStringSoftRetbarbican;
}

/* Cold: greppable "string: soft retglacis …" line (Wave 41 exclusive). */
const char *
string_soft_retglacis(void)
{
    return g_szStringSoftRetglacis;
}

/* Cold: greppable "string: soft retcurtain …" line (Wave 41 exclusive). */
const char *
string_soft_retcurtain(void)
{
    return g_szStringSoftRetcurtain;
}

/* Cold: greppable "string: soft retparapet …" line (Wave 41 exclusive). */
const char *
string_soft_retparapet(void)
{
    return g_szStringSoftRetparapet;
}
/* Cold: greppable "string: soft retravelin …" line (Wave 41 exclusive). */
const char *
string_soft_retravelin(void)
{
    return g_szStringSoftRetravelin;
}

/* Cold: greppable "string: soft retditch …" line (Wave 41 exclusive). */
const char *
string_soft_retditch(void)
{
    return g_szStringSoftRetditch;
}

/* Cold: greppable "string: soft retportcullis …" line (Wave 42 exclusive). */
const char *
string_soft_retportcullis(void)
{
    return g_szStringSoftRetportcullis;
}

/* Cold: greppable "string: soft retbattlement …" line (Wave 42 exclusive). */
const char *
string_soft_retbattlement(void)
{
    return g_szStringSoftRetbattlement;
}

/* Cold: greppable "string: soft retmachicolation …" line (Wave 43 exclusive). */
const char *
string_soft_retmachicolation(void)
{
    return g_szStringSoftRetmachicolation;
}

/* Cold: greppable "string: soft retarrowslit …" line (Wave 43 exclusive). */
const char *
string_soft_retarrowslit(void)
{
    return g_szStringSoftRetarrowslit;
}

/* Cold: greppable "string: soft retmerlon …" line (Wave 44 exclusive). */
const char *
string_soft_retmerlon(void)
{
    return g_szStringSoftRetmerlon;
}

/* Cold: greppable "string: soft retembrasure …" line (Wave 44 exclusive). */
const char *
string_soft_retembrasure(void)
{
    return g_szStringSoftRetembrasure;
}



/* Cold: greppable "string: soft retkeepgate …" line (Wave 45 exclusive). */
const char *
string_soft_retkeepgate(void)
{
    return g_szStringSoftRetkeepgate;
}

/* Cold: greppable "string: soft retouterward …" line (Wave 45 exclusive). */
const char *
string_soft_retouterward(void)
{
    return g_szStringSoftRetouterward;
}

/* Cold: greppable "string: soft retbailey …" line (Wave 46 exclusive). */
const char *
string_soft_retbailey(void)
{
    return g_szStringSoftRetbailey;
}

/* Cold: greppable "string: soft retpostern …" line (Wave 46 exclusive). */
const char *
string_soft_retpostern(void)
{
    return g_szStringSoftRetpostern;
}

/* Cold: greppable "string: soft retinnerward …" line (Wave 47 exclusive). */
const char *
string_soft_retinnerward(void)
{
    return g_szStringSoftRetinnerward;
}

/* Cold: greppable "string: soft retdonjon …" line (Wave 47 exclusive). */
const char *
string_soft_retdonjon(void)
{
    return g_szStringSoftRetdonjon;
}

/* Cold: greppable "string: soft retchevaux …" line (Wave 48 exclusive). */
const char *
string_soft_retchevaux(void)
{
    return g_szStringSoftRetchevaux;
}

/* Cold: greppable "string: soft retpalisade …" line (Wave 48 exclusive). */
const char *
string_soft_retpalisade(void)
{
    return g_szStringSoftRetpalisade;
}

/* Cold: greppable "string: soft retglacisgate …" line (Wave 49 exclusive). */
const char *
string_soft_retglacisgate(void)
{
    return g_szStringSoftRetglacisgate;
}

/* Cold: greppable "string: soft retoutwork …" line (Wave 49 exclusive). */
const char *
string_soft_retoutwork(void)
{
    return g_szStringSoftRetoutwork;
}

/* Cold: greppable "string: soft retsally …" line (Wave 50 exclusive). */
const char *
string_soft_retsally(void)
{
    return g_szStringSoftRetsally;
}

/* Cold: greppable "string: soft retcounterscarp …" line (Wave 50 exclusive). */
const char *
string_soft_retcounterscarp(void)
{
    return g_szStringSoftRetcounterscarp;
}

/* Cold: greppable "string: soft retfosse …" line (Wave 51 exclusive). */
const char *
string_soft_retfosse(void)
{
    return g_szStringSoftRetfosse;
}

/* Cold: greppable "string: soft retcoveredway …" line (Wave 51 exclusive). */
const char *
string_soft_retcoveredway(void)
{
    return g_szStringSoftRetcoveredway;
}

/* Cold: greppable "string: soft rettenaille …" line (Wave 52 exclusive). */
const char *
string_soft_rettenaille(void)
{
    return g_szStringSoftRettenaille;
}

/* Cold: greppable "string: soft retdemilune …" line (Wave 52 exclusive). */
const char *
string_soft_retdemilune(void)
{
    return g_szStringSoftRetdemilune;
}

/* Cold: greppable "string: soft retlunette …" line (Wave 53 exclusive). */
const char *
string_soft_retlunette(void)
{
    return g_szStringSoftRetlunette;
}

/* Cold: greppable "string: soft retcaponier …" line (Wave 54 exclusive). */
const char *
string_soft_retcaponier(void)
{
    return g_szStringSoftRetcaponier;
}

/* Cold: greppable "string: soft retredan …" line (Wave 54 exclusive). */
const char *
string_soft_retredan(void)
{
    return g_szStringSoftRetredan;
}

/* Cold: greppable "string: soft retflank …" line (Wave 55 exclusive). */
const char *
string_soft_retflank(void)
{
    return g_szStringSoftRetflank;
}

/* Cold: greppable "string: soft retface …" line (Wave 55 exclusive). */
const char *
string_soft_retface(void)
{
    return g_szStringSoftRetface;
}

/* Cold: greppable "string: soft retgorge …" line (Wave 56 exclusive). */
const char *
string_soft_retgorge(void)
{
    return g_szStringSoftRetgorge;
}

/* Cold: greppable "string: soft retshoulder …" line (Wave 56 exclusive). */
const char *
string_soft_retshoulder(void)
{
    return g_szStringSoftRetshoulder;
}

/* Cold: greppable "string: soft retraverse …" line (Wave 57 exclusive). */
const char *
string_soft_retraverse(void)
{
    return g_szStringSoftRetraverse;
}

/* Cold: greppable "string: soft retcasemate …" line (Wave 57 exclusive). */
const char *
string_soft_retcasemate(void)
{
    return g_szStringSoftRetcasemate;
}

/* Cold: greppable "string: soft retorillon …" line (Wave 58 exclusive). */
const char *
string_soft_retorillon(void)
{
    return g_szStringSoftRetorillon;
}

/* Cold: greppable "string: soft retbonnette …" line (Wave 58 exclusive). */
const char *
string_soft_retbonnette(void)
{
    return g_szStringSoftRetbonnette;
}

/* Cold: greppable "string: soft retcrownwork …" line (Wave 59 exclusive). */
const char *
string_soft_retcrownwork(void)
{
    return g_szStringSoftRetcrownwork;
}

/* Cold: greppable "string: soft rethornwork …" line (Wave 59 exclusive). */
const char *
string_soft_rethornwork(void)
{
    return g_szStringSoftRethornwork;
}

/* Cold: greppable "string: soft retplace …" line (Wave 60 exclusive). */
const char *
string_soft_retplace(void)
{
    return g_szStringSoftRetplace;
}

/* Cold: greppable "string: soft retenvelope …" line (Wave 60 exclusive). */
const char *
string_soft_retenvelope(void)
{
    return g_szStringSoftRetenvelope;
}

/* Cold: greppable "string: soft retcounterguard …" line (Wave 61 exclusive). */
const char *
string_soft_retcounterguard(void)
{
    return g_szStringSoftRetcounterguard;
}

/* Cold: greppable "string: soft retcoveredface …" line (Wave 61 exclusive). */
const char *
string_soft_retcoveredface(void)
{
    return g_szStringSoftRetcoveredface;
}

/* Cold: greppable "string: soft retbastionface …" line (Wave 62 exclusive). */
const char *
string_soft_retbastionface(void)
{
    return g_szStringSoftRetbastionface;
}

/* Cold: greppable "string: soft retcurtainangle …" line (Wave 62 exclusive). */
const char *
string_soft_retcurtainangle(void)
{
    return g_szStringSoftRetcurtainangle;
}

/* Cold: greppable "string: soft retdoubletenaille …" line (Wave 63 exclusive). */
const char *
string_soft_retdoubletenaille(void)
{
    return g_szStringSoftRetdoubletenaille;
}

/* Cold: greppable "string: soft retplaceofarms …" line (Wave 63 exclusive). */
const char *
string_soft_retplaceofarms(void)
{
    return g_szStringSoftRetplaceofarms;
}

/* Cold: greppable "string: soft retreentrant …" line (Wave 64 exclusive). */
const char *
string_soft_retreentrant(void)
{
    return g_szStringSoftRetreentrant;
}

/* Cold: greppable "string: soft retsallyport …" line (Wave 64 exclusive). */
const char *
string_soft_retsallyport(void)
{
    return g_szStringSoftRetsallyport;
}

/* Cold: greppable "string: soft retgorgeangle …" line (Wave 65 exclusive). */
const char *
string_soft_retgorgeangle(void)
{
    return g_szStringSoftRetgorgeangle;
}

/* Cold: greppable "string: soft retshoulderangle …" line (Wave 65 exclusive). */
const char *
string_soft_retshoulderangle(void)
{
    return g_szStringSoftRetshoulderangle;
}

/* Cold: greppable "string: soft retflankangle …" line (Wave 66 exclusive). */
const char *
string_soft_retflankangle(void)
{
    return g_szStringSoftRetflankangle;
}

/* Cold: greppable "string: soft retfaceangle …" line (Wave 66 exclusive). */
const char *
string_soft_retfaceangle(void)
{
    return g_szStringSoftRetfaceangle;
}

/* Cold: greppable "string: soft retcaponierangle …" line (Wave 67 exclusive). */
const char *
string_soft_retcaponierangle(void)
{
    return g_szStringSoftRetcaponierangle;
}

/* Cold: greppable "string: soft retredanangle …" line (Wave 67 exclusive). */
const char *
string_soft_retredanangle(void)
{
    return g_szStringSoftRetredanangle;
}

/* Cold: greppable "string: soft retlunetteangle …" line (Wave 68 exclusive). */
const char *
string_soft_retlunetteangle(void)
{
    return g_szStringSoftRetlunetteangle;
}

/* Cold: greppable "string: soft rettenailleangle …" line (Wave 68 exclusive). */
const char *
string_soft_rettenailleangle(void)
{
    return g_szStringSoftRettenailleangle;
}

/* Cold: greppable "string: soft retdemiluneangle …" line (Wave 69 exclusive). */
const char *
string_soft_retdemiluneangle(void)
{
    return g_szStringSoftRetdemiluneangle;
}

/* Cold: greppable "string: soft retcoveredwayangle …" line (Wave 69 exclusive). */
const char *
string_soft_retcoveredwayangle(void)
{
    return g_szStringSoftRetcoveredwayangle;
}

/* Cold: greppable "string: soft retfosseangle …" line (Wave 70 exclusive). */
const char *
string_soft_retfosseangle(void)
{
    return g_szStringSoftRetfosseangle;
}

/* Cold: greppable "string: soft retcounterscarple …" line (Wave 70 exclusive). */
const char *
string_soft_retcounterscarple(void)
{
    return g_szStringSoftRetcounterscarple;
}

/* Cold: greppable "string: soft retsallyportangle …" line (Wave 71 exclusive). */
const char *
string_soft_retsallyportangle(void)
{
    return g_szStringSoftRetsallyportangle;
}

/* Cold: greppable "string: soft retreentrantangle …" line (Wave 71 exclusive). */
const char *
string_soft_retreentrantangle(void)
{
    return g_szStringSoftRetreentrantangle;
}

/* Cold: greppable "string: soft retplaceofarmsangle …" line (Wave 72 exclusive). */
const char *
string_soft_retplaceofarmsangle(void)
{
    return g_szStringSoftRetplaceofarmsangle;
}

/* Cold: greppable "string: soft retdoubletenailleangle …" line (Wave 72 exclusive). */
const char *
string_soft_retdoubletenailleangle(void)
{
    return g_szStringSoftRetdoubletenailleangle;
}

/* Cold: greppable "string: soft retcurtainface …" line (Wave 73 exclusive). */
const char *
string_soft_retcurtainface(void)
{
    return g_szStringSoftRetcurtainface;
}

/* Cold: greppable "string: soft retbastionangle …" line (Wave 73 exclusive). */
const char *
string_soft_retbastionangle(void)
{
    return g_szStringSoftRetbastionangle;
}

/* Cold: greppable "string: soft retglacisangle …" line (Wave 74 exclusive). */
const char *
string_soft_retglacisangle(void)
{
    return g_szStringSoftRetglacisangle;
}

/* Cold: greppable "string: soft retparapetangle …" line (Wave 74 exclusive). */
const char *
string_soft_retparapetangle(void)
{
    return g_szStringSoftRetparapetangle;
}

/* Cold: greppable "string: soft retmoatangle …" line (Wave 75 exclusive). */
const char *
string_soft_retmoatangle(void)
{
    return g_szStringSoftRetmoatangle;
}

/* Cold: greppable "string: soft retowerangle …" line (Wave 75 exclusive). */
const char *
string_soft_retowerangle(void)
{
    return g_szStringSoftRetowerangle;
}

/* Cold: greppable "string: soft retgateangle …" line (Wave 76 exclusive). */
const char *
string_soft_retgateangle(void)
{
    return g_szStringSoftRetgateangle;
}

/* Cold: greppable "string: soft retwallangle …" line (Wave 76 exclusive). */
const char *
string_soft_retwallangle(void)
{
    return g_szStringSoftRetwallangle;
}

/* Cold: greppable "string: soft retspireangle …" line (Wave 77 exclusive). */
const char *
string_soft_retspireangle(void)
{
    return g_szStringSoftRetspireangle;
}

/* Cold: greppable "string: soft retholdangle …" line (Wave 77 exclusive). */
const char *
string_soft_retholdangle(void)
{
    return g_szStringSoftRetholdangle;
}

/* Cold: greppable "string: soft retpalaceangle …" line (Wave 78 exclusive). */
const char *
string_soft_retpalaceangle(void)
{
    return g_szStringSoftRetpalaceangle;
}

/* Cold: greppable "string: soft retfortressangle …" line (Wave 78 exclusive). */
const char *
string_soft_retfortressangle(void)
{
    return g_szStringSoftRetfortressangle;
}

/* Cold: greppable "string: soft retkeepangle …" line (Wave 79 exclusive). */
const char *
string_soft_retkeepangle(void)
{
    return g_szStringSoftRetkeepangle;
}

/* Cold: greppable "string: soft retredoubtangle …" line (Wave 79 exclusive). */
const char *
string_soft_retredoubtangle(void)
{
    return g_szStringSoftRetredoubtangle;
}

/* Cold: greppable "string: soft retcitadelangle …" line (Wave 80 exclusive). */
const char *
string_soft_retcitadelangle(void)
{
    return g_szStringSoftRetcitadelangle;
}

/* Cold: greppable "string: soft retbastionkeep …" line (Wave 80 exclusive). */
const char *
string_soft_retbastionkeep(void)
{
    return g_szStringSoftRetbastionkeep;
}

/* Cold: greppable "string: soft retpanoplyangle …" line (Wave 81 exclusive). */
const char *
string_soft_retpanoplyangle(void)
{
    return g_szStringSoftRetpanoplyangle;
}

/* Cold: greppable "string: soft retbulwarkangle …" line (Wave 81 exclusive). */
const char *
string_soft_retbulwarkangle(void)
{
    return g_szStringSoftRetbulwarkangle;
}

/* Cold: greppable "string: soft retmantleangle …" line (Wave 82 exclusive). */
const char *
string_soft_retmantleangle(void)
{
    return g_szStringSoftRetmantleangle;
}

/* Cold: greppable "string: soft retaegisangle …" line (Wave 82 exclusive). */
const char *
string_soft_retaegisangle(void)
{
    return g_szStringSoftRetaegisangle;
}

/* Cold: greppable "string: soft retemblemangle …" line (Wave 83 exclusive). */
const char *
string_soft_retemblemangle(void)
{
    return g_szStringSoftRetemblemangle;
}

/* Cold: greppable "string: soft retsigilangle …" line (Wave 83 exclusive). */
const char *
string_soft_retsigilangle(void)
{
    return g_szStringSoftRetsigilangle;
}

/* Cold: greppable "string: soft retscepterangle …" line (Wave 85 exclusive). */
const char *
string_soft_retscepterangle(void)
{
    return g_szStringSoftRetscepterangle;
}

/* Cold: greppable "string: soft retglyphangle …" line (Wave 85 exclusive). */
const char *
string_soft_retglyphangle(void)
{
    return g_szStringSoftRetglyphangle;
}

/* Cold: greppable "string: soft retcrownangle …" line (Wave 85 exclusive). */
const char *
string_soft_retcrownangle(void)
{
    return g_szStringSoftRetcrownangle;
}

/* Cold: greppable "string: soft retshardangle …" line (Wave 85 exclusive). */
const char *
string_soft_retshardangle(void)
{
    return g_szStringSoftRetshardangle;
}

/* Cold: greppable "string: soft retforgeangle …" line (Wave 86 exclusive). */
const char *
string_soft_retforgeangle(void)
{
    return g_szStringSoftRetforgeangle;
}

/* Cold: greppable "string: soft retprismangle …" line (Wave 86 exclusive). */
const char *
string_soft_retprismangle(void)
{
    return g_szStringSoftRetprismangle;
}

/* Cold: greppable "string: soft retflameangle …" line (Wave 87 exclusive). */
const char *
string_soft_retflameangle(void)
{
    return g_szStringSoftRetflameangle;
}

/* Cold: greppable "string: soft retcipherangle …" line (Wave 87 exclusive). */
const char *
string_soft_retcipherangle(void)
{
    return g_szStringSoftRetcipherangle;
}

/* Cold: greppable "string: soft retbeaconangle …" line (Wave 88 exclusive). */
const char *
string_soft_retbeaconangle(void)
{
    return g_szStringSoftRetbeaconangle;
}

/* Cold: greppable "string: soft retledgerangle …" line (Wave 88 exclusive). */
const char *
string_soft_retledgerangle(void)
{
    return g_szStringSoftRetledgerangle;
}

/* Cold: greppable "string: soft retbannerangle …" line (Wave 89 exclusive). */
const char *
string_soft_retbannerangle(void)
{
    return g_szStringSoftRetbannerangle;
}

/* Cold: greppable "string: soft retvaultangle …" line (Wave 89 exclusive). */
const char *
string_soft_retvaultangle(void)
{
    return g_szStringSoftRetvaultangle;
}

/* Cold: greppable "string: soft retcrestangle …" line (Wave 90 exclusive). */
const char *
string_soft_retcrestangle(void)
{
    return g_szStringSoftRetcrestangle;
}

/* Cold: greppable "string: soft rettokenangle …" line (Wave 90 exclusive). */
const char *
string_soft_rettokenangle(void)
{
    return g_szStringSoftRettokenangle;
}

/* Cold: greppable "string: soft retbadgeangle …" line (Wave 91 exclusive). */
const char *
string_soft_retbadgeangle(void)
{
    return g_szStringSoftRetbadgeangle;
}

/* Cold: greppable "string: soft retphaseangle …" line (Wave 91 exclusive). */
const char *
string_soft_retphaseangle(void)
{
    return g_szStringSoftRetphaseangle;
}

/* Cold: greppable "string: soft retmarkangle …" line (Wave 92 exclusive). */
const char *
string_soft_retmarkangle(void)
{
    return g_szStringSoftRetmarkangle;
}

/* Cold: greppable "string: soft retpulseangle …" line (Wave 92 exclusive). */
const char *
string_soft_retpulseangle(void)
{
    return g_szStringSoftRetpulseangle;
}

/* Cold: greppable "string: soft retsealangle …" line (Wave 93 exclusive). */
const char *
string_soft_retsealangle(void)
{
    return g_szStringSoftRetsealangle;
}

/* Cold: greppable "string: soft retboundangle …" line (Wave 93 exclusive). */
const char *
string_soft_retboundangle(void)
{
    return g_szStringSoftRetboundangle;
}

/* Cold: greppable "string: soft retstemangle …" line (Wave 94 honesty). */
const char *
string_soft_retstemangle(void)
{
    return g_szStringSoftRetstemangle;
}

/* Cold: greppable "string: soft retbladeangle …" line (Wave 94 exclusive). */
const char *
string_soft_retbladeangle(void)
{
    return g_szStringSoftRetbladeangle;
}

/* Cold: greppable "string: soft retchordangle …" line (Wave 95 honesty). */
const char *
string_soft_retchordangle(void)
{
    return g_szStringSoftRetchordangle;
}

/* Cold: greppable "string: soft retarcangle …" line (Wave 95 exclusive). */
const char *
string_soft_retarcangle(void)
{
    return g_szStringSoftRetarcangle;
}

/* Cold: greppable "string: soft retsectorangle …" line (Wave 96 honesty). */
const char *
string_soft_retsectorangle(void)
{
    return g_szStringSoftRetsectorangle;
}

/* Cold: greppable "string: soft retwedgeangle …" line (Wave 96 exclusive). */
const char *
string_soft_retwedgeangle(void)
{
    return g_szStringSoftRetwedgeangle;
}

/* Cold: greppable "string: soft retradiusangle …" line (Wave 97 honesty). */
const char *
string_soft_retradiusangle(void)
{
    return g_szStringSoftRetradiusangle;
}

/* Cold: greppable "string: soft retdiameterangle …" line (Wave 97 exclusive). */
const char *
string_soft_retdiameterangle(void)
{
    return g_szStringSoftRetdiameterangle;
}

/* Cold: greppable "string: soft retcircumangle …" line (Wave 98 honesty). */
const char *
string_soft_retcircumangle(void)
{
    return g_szStringSoftRetcircumangle;
}

/* Cold: greppable "string: soft retellipseangle …" line (Wave 98 exclusive). */
const char *
string_soft_retellipseangle(void)
{
    return g_szStringSoftRetellipseangle;
}

/* Cold: greppable "string: soft rethyperangle …" line (Wave 99 honesty). */
const char *
string_soft_rethyperangle(void)
{
    return g_szStringSoftRethyperangle;
}

/* Cold: greppable "string: soft retparabolaangle …" line (Wave 99 exclusive). */
const char *
string_soft_retparabolaangle(void)
{
    return g_szStringSoftRetparabolaangle;
}

/* Cold: greppable "string: soft retspiralangle …" line (Wave 100 honesty). */
const char *
string_soft_retspiralangle(void)
{
    return g_szStringSoftRetspiralangle;
}

/* Cold: greppable "string: soft rethelixangle …" line (Wave 100 exclusive). */
const char *
string_soft_rethelixangle(void)
{
    return g_szStringSoftRethelixangle;
}

/* Cold: greppable "string: soft rettorusangle …" line (Wave 101 honesty). */
const char *
string_soft_rettorusangle(void)
{
    return g_szStringSoftRettorusangle;
}

/* Cold: greppable "string: soft retknotangle …" line (Wave 101 exclusive). */
const char *
string_soft_retknotangle(void)
{
    return g_szStringSoftRetknotangle;
}

/* Cold: greppable "string: soft retmoebiusangle …" line (Wave 102 honesty). */
const char *
string_soft_retmoebiusangle(void)
{
    return g_szStringSoftRetmoebiusangle;
}

/* Cold: greppable "string: soft retkleinangle …" line (Wave 102 exclusive). */
const char *
string_soft_retkleinangle(void)
{
    return g_szStringSoftRetkleinangle;
}

/* Cold: greppable "string: soft retprojectangle …" line (Wave 103 honesty). */
const char *
string_soft_retprojectangle(void)
{
    return g_szStringSoftRetprojectangle;
}

/* Cold: greppable "string: soft retaffineangle …" line (Wave 103 exclusive). */
const char *
string_soft_retaffineangle(void)
{
    return g_szStringSoftRetaffineangle;
}
/* Cold: greppable "string: soft retlinearangle …" line (Wave 104 honesty). */
const char *
string_soft_retlinearangle(void)
{
    return g_szStringSoftRetlinearangle;
}

/* Cold: greppable "string: soft retbilinearangle …" line (Wave 104 exclusive). */
const char *
string_soft_retbilinearangle(void)
{
    return g_szStringSoftRetbilinearangle;
}

/* Cold: greppable "string: soft retquadraticangle …" line (Wave 105 honesty). */
const char *
string_soft_retquadraticangle(void)
{
    return g_szStringSoftRetquadraticangle;
}

/* Cold: greppable "string: soft retcubicangle …" line (Wave 105 exclusive). */
const char *
string_soft_retcubicangle(void)
{
    return g_szStringSoftRetcubicangle;
}

/* Cold: greppable "string: soft retquarticangle …" line (Wave 106 honesty). */
const char *
string_soft_retquarticangle(void)
{
    return g_szStringSoftRetquarticangle;
}

/* Cold: greppable "string: soft retquinticangle …" line (Wave 106 exclusive). */
const char *
string_soft_retquinticangle(void)
{
    return g_szStringSoftRetquinticangle;
}

/* Cold: greppable "string: soft retsplineangle …" line (Wave 107 honesty). */
const char *
string_soft_retsplineangle(void)
{
    return g_szStringSoftRetsplineangle;
}

/* Cold: greppable "string: soft retbezierangle …" line (Wave 107 exclusive). */
const char *
string_soft_retbezierangle(void)
{
    return g_szStringSoftRetbezierangle;
}

/* Cold: greppable "string: soft rethurmitangle …" line (Wave 108 honesty). */
const char *
string_soft_rethurmitangle(void)
{
    return g_szStringSoftRethurmitangle;
}

/* Cold: greppable "string: soft retcatmullangle …" line (Wave 108 exclusive). */
const char *
string_soft_retcatmullangle(void)
{
    return g_szStringSoftRetcatmullangle;
}

/* Cold: greppable "string: soft retnurbsangle …" line (Wave 109 honesty). */
const char *
string_soft_retnurbsangle(void)
{
    return g_szStringSoftRetnurbsangle;
}

/* Cold: greppable "string: soft retbsplineangle …" line (Wave 109 exclusive). */
const char *
string_soft_retbsplineangle(void)
{
    return g_szStringSoftRetbsplineangle;
}

/* Cold: greppable "string: soft retmeshangle …" line (Wave 110 honesty). */
const char *
string_soft_retmeshangle(void)
{
    return g_szStringSoftRetmeshangle;
}

/* Cold: greppable "string: soft retgridangle …" line (Wave 110 exclusive). */
const char *
string_soft_retgridangle(void)
{
    return g_szStringSoftRetgridangle;
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

/* string: soft inventory end helpers=14 counts=none hot_path=clean wave=110 areas=113 */
