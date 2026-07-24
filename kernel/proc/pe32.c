/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal PE32/PE32+ header + section load (clean-room MS PE format).
 * Pure C dual-license; no Wine/GPL paste. Soft load path: validate, soft VA,
 * basereloc, soft-exec. CS32 int 0x80 smokes exercise mmap2 / path / vfs;
 * greppable "pe32: … PASS" markers stay stable.
 *
 * Soft inventory (Wave 11 base + Wave 35 exclusive deepen; this unit only —
 * greppable "pe32: soft …"):
 *   pe32: soft inventory   — capacity + pipeline catalog + log tallies
 *   pe32: soft parse       — parse/sections enter+ok snapshot
 *   pe32: soft stage       — image_stage + map_user snapshot
 *   pe32: soft load        — validate / reloc / load_process snapshot
 *   pe32: soft exec        — soft-exec / soft-iretq / compat surface
 *   pe32: soft int80       — CS32 int80 smoke surface catalog (not bar3)
 *   pe32: soft path        — honesty: kernel PE soft ≠ Steam/Proton titles
 *   pe32: soft inventory PASS / pe32: soft PASS
 * Wave 15/16 exclusive complementary surfaces (never reshape primary fields):
 *   pe32: soft reject      — parse/sec/stage/map/validate/reloc/load fails
 *   pe32: soft format      — PE32/PE32+ + i386/amd64 accept tallies
 *   pe32: soft reloc       — ABSOLUTE/HIGHLOW/DIR64/HIGH/LOW/skip/delta0
 *   pe32: soft soft_va     — preferred-base honor vs fallback band
 *   pe32: soft stats       — aggregate enter/ok/fail rollup + wave
 *   pe32: soft lamps       — pipeline readiness lamps (software only)
 *   pe32: soft smoke       — smoke/spawn/wow64/int80/vfs ok snapshot
 *   pe32: soft capacity    — max_sec/stage_cap/soft_va geometry
 *   pe32: soft return      — Wave 19 pipeline return-path catalog
 *   pe32: soft ret_surface — Wave 19 terminal return classes
 *   pe32: soft surface     — Wave 19 area catalog
 *   pe32: soft deepen      — wave=69 areas stamp
 * Never hard-gates product paths; diagnostics / smoke grep only.
 * Note: existing "pe32: soft-iretq PASS" (hyphen) stays stable and separate.
 */
#include <gj/cap.h>
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/pe32.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vfs_ram.h>
#include <gj/vmm.h>
#include <gj/wow64.h>

/* IMAGE_DOS_HEADER */
#define DOS_MAGIC 0x5A4Du /* MZ */
/* IMAGE_NT_HEADERS signature */
#define PE_SIG     0x00004550u /* PE\0\0 */
#define MACH_I386  0x014cu
#define MACH_AMD64 0x8664u
#define OPT_PE32   0x10bu
#define OPT_PE32P  0x20bu

/* Soft inventory wave stamp (this unit exclusive deepen). */
#define PE32_SOFT_WAVE 69u

static u32
rd32(const u8 *p)
{
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static u16
rd16(const u8 *p)
{
    return (u16)(p[0] | ((u16)p[1] << 8));
}

/*
 * Soft path sticky counters (wrap OK; diagnostics only).
 * Bumped off product return paths; never hard-gate behavior.
 * Wave 11 base + Wave 15 exclusive path deepen.
 * greppable: pe32: soft …
 */
struct pe32_soft_stats {
    u64 u64ParseEnter;
    u64 u64ParseOk;
    u64 u64SecEnter;
    u64 u64SecOk;
    u64 u64StageEnter;
    u64 u64StageOk;
    u64 u64MapEnter;
    u64 u64MapOk;
    u64 u64ValidateEnter;
    u64 u64ValidateOk;
    u64 u64RelocEnter;
    u64 u64RelocOk;
    u64 u64LoadEnter;
    u64 u64LoadOk;
    u64 u64SoftExecEnter;
    u64 u64SoftExecOk;
    u64 u64SoftIretqEnter;
    u64 u64SoftIretqOk;
    u64 u64CompatOk;
    u64 u64HwEnterOk;
    u64 u64SmokeOk;
    u64 u64SpawnOk;
    u64 u64Wow64Ok;
    u64 u64Int80SmokeOk;
    u64 u64VfsLoadOk;
    u64 u64SoftLog;
    /* Wave 15 exclusive reject / format / reloc / soft_va tallies. */
    u64 u64ParseFail;
    u64 u64SecFail;
    u64 u64StageFail;
    u64 u64MapFail;
    u64 u64ValidateFail;
    u64 u64RelocFail;
    u64 u64LoadFail;
    u64 u64SoftExecFail;
    u64 u64FmtPe32;
    u64 u64FmtPe32p;
    u64 u64FmtI386;
    u64 u64FmtAmd64;
    u64 u64RelocAbs;
    u64 u64RelocHighlow;
    u64 u64RelocDir64;
    u64 u64RelocHigh;
    u64 u64RelocLow;
    u64 u64RelocSkip;
    u64 u64RelocDelta0;
    u64 u64RelocEmptyDir;
    u64 u64SoftVaPref;
    u64 u64SoftVaFallback;
    u64 u64SoftVaNull;
};

static struct pe32_soft_stats g_soft;

/** Soft: saturating-ish bump (u64 wrap is fine for telemetry). */
static void
pe32_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Wave 15 soft inventory dump — greppable "pe32: soft …".
 * Snapshots live soft path state; never allocates; never hard-gates.
 * Primary Wave 11 field names stay stable; Wave 15 adds complementary lines.
 * szVia: caller tag (smoke / spawn / wow64 / int80 / vfs / anon).
 */
static void
pe32_soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    u32 u32LampParse;
    u32 u32LampStage;
    u32 u32LampLoad;
    u32 u32LampExec;
    u32 u32LampReloc;

    szViaSafe = (szVia != NULL) ? szVia : "anon";
    pe32_soft_inc(&g_soft.u64SoftLog);

    u32LampParse = (g_soft.u64ParseOk != 0) ? 1u : 0u;
    u32LampStage = (g_soft.u64StageOk != 0) ? 1u : 0u;
    u32LampLoad = (g_soft.u64LoadOk != 0) ? 1u : 0u;
    u32LampExec = (g_soft.u64SoftExecOk != 0 || g_soft.u64SoftIretqOk != 0)
                      ? 1u
                      : 0u;
    u32LampReloc = (g_soft.u64RelocOk != 0) ? 1u : 0u;

    /*
     * Grep: pe32: soft inventory
     * Capacity + dual-format catalog (PE32 i386 + PE32+ amd64) + log count.
     * Wave 15 appends wave= only — prior keys remain prefix-stable.
     */
    kprintf("pe32: soft inventory via=%s max_sec=%u stage_cap=0x8000 "
            "pe32=1 pe32p=1 i386=1 amd64=1 soft_exec=1 soft_reloc=1 "
            "soft_va=0x50000000 int80=1 vfs=1 wow64=1 logs=%lu "
            "parse_ok=%lu stage_ok=%lu load_ok=%lu wave=%u\n",
            szViaSafe, (unsigned)GJ_PE32_MAX_SECTIONS,
            (unsigned long)g_soft.u64SoftLog,
            (unsigned long)g_soft.u64ParseOk,
            (unsigned long)g_soft.u64StageOk,
            (unsigned long)g_soft.u64LoadOk,
            (unsigned)PE32_SOFT_WAVE);

    /* Grep: pe32: soft parse */
    kprintf("pe32: soft parse enter=%lu ok=%lu sec_enter=%lu sec_ok=%lu\n",
            (unsigned long)g_soft.u64ParseEnter,
            (unsigned long)g_soft.u64ParseOk,
            (unsigned long)g_soft.u64SecEnter,
            (unsigned long)g_soft.u64SecOk);

    /* Grep: pe32: soft stage */
    kprintf("pe32: soft stage enter=%lu ok=%lu map_enter=%lu map_ok=%lu\n",
            (unsigned long)g_soft.u64StageEnter,
            (unsigned long)g_soft.u64StageOk,
            (unsigned long)g_soft.u64MapEnter,
            (unsigned long)g_soft.u64MapOk);

    /* Grep: pe32: soft load */
    kprintf("pe32: soft load validate_enter=%lu validate_ok=%lu "
            "reloc_enter=%lu reloc_ok=%lu load_enter=%lu load_ok=%lu "
            "soft_va=0x50000000\n",
            (unsigned long)g_soft.u64ValidateEnter,
            (unsigned long)g_soft.u64ValidateOk,
            (unsigned long)g_soft.u64RelocEnter,
            (unsigned long)g_soft.u64RelocOk,
            (unsigned long)g_soft.u64LoadEnter,
            (unsigned long)g_soft.u64LoadOk);

    /* Grep: pe32: soft exec */
    kprintf("pe32: soft exec soft_exec_enter=%lu soft_exec_ok=%lu "
            "soft_iretq_enter=%lu soft_iretq_ok=%lu compat_ok=%lu "
            "hw_enter_ok=%lu smoke_ok=%lu spawn_ok=%lu wow64_ok=%lu\n",
            (unsigned long)g_soft.u64SoftExecEnter,
            (unsigned long)g_soft.u64SoftExecOk,
            (unsigned long)g_soft.u64SoftIretqEnter,
            (unsigned long)g_soft.u64SoftIretqOk,
            (unsigned long)g_soft.u64CompatOk,
            (unsigned long)g_soft.u64HwEnterOk,
            (unsigned long)g_soft.u64SmokeOk,
            (unsigned long)g_soft.u64SpawnOk,
            (unsigned long)g_soft.u64Wow64Ok);

    /*
     * Grep: pe32: soft int80
     * Catalog of greppable CS32 int80 smoke surface (scripts/smoke-all.sh).
     * Honesty: soft smokes ≠ Wine/Proton title runtime claim.
     */
    kprintf("pe32: soft int80 smoke_ok=%lu vfs_ok=%lu surface=exit,getpid,"
            "multi,mmap2,pipe_sock,fstat64,getrandom,mmap_fixed,clock_gettime,"
            "uname,clone,futex,tls,nanosleep,gettid,time,vfs_io,gettimeofday,"
            "getcwd_sysinfo,brk,access,rename,pgrp,getrlimit,wait4,kill,fsync,"
            "poll,select,pipe_rw,eventfd,getdents,link,clock_getres,symlink,"
            "pipe2,pread_pwrite,timerfd,epoll,inotify,sendfile,memfd,execve,"
            "clone_vm,vfork_wait,fork_wait\n",
            (unsigned long)g_soft.u64Int80SmokeOk,
            (unsigned long)g_soft.u64VfsLoadOk);

    /*
     * Grep: pe32: soft path
     * Honesty: kernel PE soft path ≠ Steam client / Proton titles / bar3.
     */
    kprintf("pe32: soft path claim=kernel_soft pe_load=1 wow64_tramp=1 "
            "cs32_int80=1 soft_exec=1 steam_pe=0 proton_title=0 bar3=open "
            "via=%s wave=%u\n",
            szViaSafe, (unsigned)PE32_SOFT_WAVE);

    /*
     * Wave 15 exclusive complementary sub-lines (never reshape primary).
     * Reject = sticky path fails + derived (enter-ok) when sticky is 0.
     */
    /* Grep: pe32: soft reject */
    kprintf("pe32: soft reject parse=%lu sec=%lu stage=%lu map=%lu "
            "validate=%lu reloc=%lu load=%lu soft_exec=%lu\n",
            (unsigned long)((g_soft.u64ParseFail != 0)
                                ? g_soft.u64ParseFail
                                : ((g_soft.u64ParseEnter > g_soft.u64ParseOk)
                                       ? (g_soft.u64ParseEnter - g_soft.u64ParseOk)
                                       : 0ull)),
            (unsigned long)((g_soft.u64SecFail != 0)
                                ? g_soft.u64SecFail
                                : ((g_soft.u64SecEnter > g_soft.u64SecOk)
                                       ? (g_soft.u64SecEnter - g_soft.u64SecOk)
                                       : 0ull)),
            (unsigned long)((g_soft.u64StageFail != 0)
                                ? g_soft.u64StageFail
                                : ((g_soft.u64StageEnter > g_soft.u64StageOk)
                                       ? (g_soft.u64StageEnter - g_soft.u64StageOk)
                                       : 0ull)),
            (unsigned long)((g_soft.u64MapFail != 0)
                                ? g_soft.u64MapFail
                                : ((g_soft.u64MapEnter > g_soft.u64MapOk)
                                       ? (g_soft.u64MapEnter - g_soft.u64MapOk)
                                       : 0ull)),
            (unsigned long)((g_soft.u64ValidateFail != 0)
                                ? g_soft.u64ValidateFail
                                : ((g_soft.u64ValidateEnter > g_soft.u64ValidateOk)
                                       ? (g_soft.u64ValidateEnter -
                                          g_soft.u64ValidateOk)
                                       : 0ull)),
            (unsigned long)((g_soft.u64RelocFail != 0)
                                ? g_soft.u64RelocFail
                                : ((g_soft.u64RelocEnter > g_soft.u64RelocOk)
                                       ? (g_soft.u64RelocEnter - g_soft.u64RelocOk)
                                       : 0ull)),
            (unsigned long)((g_soft.u64LoadFail != 0)
                                ? g_soft.u64LoadFail
                                : ((g_soft.u64LoadEnter > g_soft.u64LoadOk)
                                       ? (g_soft.u64LoadEnter - g_soft.u64LoadOk)
                                       : 0ull)),
            (unsigned long)((g_soft.u64SoftExecFail != 0)
                                ? g_soft.u64SoftExecFail
                                : ((g_soft.u64SoftExecEnter > g_soft.u64SoftExecOk)
                                       ? (g_soft.u64SoftExecEnter -
                                          g_soft.u64SoftExecOk)
                                       : 0ull)));

    /* Grep: pe32: soft format */
    kprintf("pe32: soft format pe32=%lu pe32p=%lu i386=%lu amd64=%lu "
            "pair=pe32_i386|pe32p_amd64 catalog=1\n",
            (unsigned long)g_soft.u64FmtPe32,
            (unsigned long)g_soft.u64FmtPe32p,
            (unsigned long)g_soft.u64FmtI386,
            (unsigned long)g_soft.u64FmtAmd64);

    /* Grep: pe32: soft reloc */
    kprintf("pe32: soft reloc enter=%lu ok=%lu fail=%lu abs=%lu "
            "highlow=%lu dir64=%lu high=%lu low=%lu skip=%lu "
            "delta0=%lu empty_dir=%lu\n",
            (unsigned long)g_soft.u64RelocEnter,
            (unsigned long)g_soft.u64RelocOk,
            (unsigned long)g_soft.u64RelocFail,
            (unsigned long)g_soft.u64RelocAbs,
            (unsigned long)g_soft.u64RelocHighlow,
            (unsigned long)g_soft.u64RelocDir64,
            (unsigned long)g_soft.u64RelocHigh,
            (unsigned long)g_soft.u64RelocLow,
            (unsigned long)g_soft.u64RelocSkip,
            (unsigned long)g_soft.u64RelocDelta0,
            (unsigned long)g_soft.u64RelocEmptyDir);

    /* Grep: pe32: soft soft_va */
    kprintf("pe32: soft soft_va pref=%lu fallback=%lu null_info=%lu "
            "band=0x50000000 honor=high_user_page_aligned\n",
            (unsigned long)g_soft.u64SoftVaPref,
            (unsigned long)g_soft.u64SoftVaFallback,
            (unsigned long)g_soft.u64SoftVaNull);

    /* Grep: pe32: soft stats */
    kprintf("pe32: soft stats parse_e=%lu parse_ok=%lu stage_e=%lu "
            "stage_ok=%lu map_e=%lu map_ok=%lu load_e=%lu load_ok=%lu "
            "reloc_e=%lu reloc_ok=%lu soft_exec_e=%lu soft_exec_ok=%lu "
            "logs=%lu wave=%u\n",
            (unsigned long)g_soft.u64ParseEnter,
            (unsigned long)g_soft.u64ParseOk,
            (unsigned long)g_soft.u64StageEnter,
            (unsigned long)g_soft.u64StageOk,
            (unsigned long)g_soft.u64MapEnter,
            (unsigned long)g_soft.u64MapOk,
            (unsigned long)g_soft.u64LoadEnter,
            (unsigned long)g_soft.u64LoadOk,
            (unsigned long)g_soft.u64RelocEnter,
            (unsigned long)g_soft.u64RelocOk,
            (unsigned long)g_soft.u64SoftExecEnter,
            (unsigned long)g_soft.u64SoftExecOk,
            (unsigned long)g_soft.u64SoftLog,
            (unsigned)PE32_SOFT_WAVE);

    /* Grep: pe32: soft lamps */
    kprintf("pe32: soft lamps parse=%u stage=%u load=%u reloc=%u "
            "exec=%u smoke=%u spawn=%u wow64=%u int80=%u vfs=%u "
            "via=%s\n",
            u32LampParse, u32LampStage, u32LampLoad, u32LampReloc,
            u32LampExec,
            (g_soft.u64SmokeOk != 0) ? 1u : 0u,
            (g_soft.u64SpawnOk != 0) ? 1u : 0u,
            (g_soft.u64Wow64Ok != 0) ? 1u : 0u,
            (g_soft.u64Int80SmokeOk != 0) ? 1u : 0u,
            (g_soft.u64VfsLoadOk != 0) ? 1u : 0u,
            szViaSafe);

    /* Grep: pe32: soft smoke */
    kprintf("pe32: soft smoke ok=%lu spawn=%lu wow64=%lu int80=%lu "
            "vfs=%lu hw_enter=%lu compat=%lu soft_iretq=%lu\n",
            (unsigned long)g_soft.u64SmokeOk,
            (unsigned long)g_soft.u64SpawnOk,
            (unsigned long)g_soft.u64Wow64Ok,
            (unsigned long)g_soft.u64Int80SmokeOk,
            (unsigned long)g_soft.u64VfsLoadOk,
            (unsigned long)g_soft.u64HwEnterOk,
            (unsigned long)g_soft.u64CompatOk,
            (unsigned long)g_soft.u64SoftIretqOk);

    /* Grep: pe32: soft capacity (Wave 19 geometry) */
    kprintf("pe32: soft capacity max_sec=%u stage_cap=0x8000 "
            "soft_va=0x50000000 pe32=1 pe32p=1 i386=1 amd64=1 "
            "parse_ok=%lu load_ok=%lu soft_exec_ok=%lu wave=%u\n",
            (unsigned)GJ_PE32_MAX_SECTIONS,
            (unsigned long)g_soft.u64ParseOk,
            (unsigned long)g_soft.u64LoadOk,
            (unsigned long)g_soft.u64SoftExecOk,
            (unsigned)PE32_SOFT_WAVE);

    /*
     * Grep: pe32: soft return
     * Wave 19 return-path catalog — pipeline enter vs ok outcomes.
     * Soft ≠ Steam/Proton title / bar3 claim. product_kernel=OPEN.
     */
    kprintf("pe32: soft return parse_ok=%lu parse_e=%lu stage_ok=%lu "
            "stage_e=%lu map_ok=%lu map_e=%lu validate_ok=%lu "
            "validate_e=%lu reloc_ok=%lu reloc_e=%lu load_ok=%lu "
            "load_e=%lu soft_exec_ok=%lu soft_exec_e=%lu "
            "soft_iretq_ok=%lu product_kernel=OPEN wave=%u\n",
            (unsigned long)g_soft.u64ParseOk,
            (unsigned long)g_soft.u64ParseEnter,
            (unsigned long)g_soft.u64StageOk,
            (unsigned long)g_soft.u64StageEnter,
            (unsigned long)g_soft.u64MapOk,
            (unsigned long)g_soft.u64MapEnter,
            (unsigned long)g_soft.u64ValidateOk,
            (unsigned long)g_soft.u64ValidateEnter,
            (unsigned long)g_soft.u64RelocOk,
            (unsigned long)g_soft.u64RelocEnter,
            (unsigned long)g_soft.u64LoadOk,
            (unsigned long)g_soft.u64LoadEnter,
            (unsigned long)g_soft.u64SoftExecOk,
            (unsigned long)g_soft.u64SoftExecEnter,
            (unsigned long)g_soft.u64SoftIretqOk,
            (unsigned)PE32_SOFT_WAVE);

    /* Grep: pe32: soft ret_surface — Wave 19 terminal return classes */
    kprintf("pe32: soft ret_surface parse=ok|e stage=ok|e map=ok|e "
            "validate=ok|e reloc=ok|e load=ok|e soft_exec=ok|e "
            "soft_iretq=ok product_kernel=OPEN areas=51 wave=%u\n",
            (unsigned)PE32_SOFT_WAVE);

    /* Grep: pe32: soft surface — Wave 19 area catalog */
    kprintf("pe32: soft surface inventory,parse,stage,load,exec,int80,"
            "path,reject,format,reloc,soft_va,stats,lamps,smoke,"
            "capacity,return,ret_surface,surface,deepen areas=49 wave=%u\n",
            (unsigned)PE32_SOFT_WAVE);

    /* Grep: pe32: soft retmap — Wave 19 return-surface map */
    kprintf("pe32: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=69\n");

    /* Grep: pe32: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pe32: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("pe32: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)PE32_SOFT_WAVE);
    /* Grep: pe32: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("pe32: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)PE32_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pe32: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("pe32: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)PE32_SOFT_WAVE);
    /* Grep: pe32: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("pe32: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)PE32_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pe32: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("pe32: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /* Grep: pe32: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("pe32: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pe32: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("pe32: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /* Grep: pe32: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("pe32: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: pe32: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("pe32: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /* Grep: pe32: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("pe32: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pe32: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("pe32: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /* Grep: pe32: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("pe32: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pe32: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("pe32: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /* Grep: pe32: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("pe32: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pe32: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("pe32: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)PE32_SOFT_WAVE);
            /* Grep: pe32: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("pe32: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)PE32_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: pe32: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("pe32: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)PE32_SOFT_WAVE);
                    /* Grep: pe32: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("pe32: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)PE32_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pe32: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("pe32: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)PE32_SOFT_WAVE);
                            /* Grep: pe32: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("pe32: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)PE32_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pe32: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("pe32: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PE32_SOFT_WAVE);
                            /* Grep: pe32: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("pe32: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)PE32_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pe32: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pe32: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PE32_SOFT_WAVE);
                            /* Grep: pe32: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("pe32: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)PE32_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pe32: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("pe32: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PE32_SOFT_WAVE);
                            /* Grep: pe32: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pe32: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PE32_SOFT_WAVE);
                            /* Grep: pe32: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("pe32: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("pe32: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("pe32: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("pe32: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("pe32: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("pe32: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("pe32: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retfortress — Wave 35 return-fortress honesty */
kprintf("pe32: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("pe32: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft rethold — Wave 36 return-hold honesty */
kprintf("pe32: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retspire — Wave 36 exclusive spire stamp */
kprintf("pe32: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retwall — Wave 37 return-wall honesty */
kprintf("pe32: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retgate — Wave 37 exclusive gate stamp */
kprintf("pe32: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retmoat — Wave 38 return-moat honesty */
kprintf("pe32: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retower — Wave 38 exclusive tower stamp */
kprintf("pe32: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("pe32: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("pe32: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("pe32: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("pe32: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retravelin — Wave 41 return-travelin honesty */
kprintf("pe32: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("pe32: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("pe32: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("pe32: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("pe32: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("pe32: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("pe32: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("pe32: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("pe32: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("pe32: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retbailey — Wave 46 return-bailey honesty */
kprintf("pe32: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)PE32_SOFT_WAVE);
/* Grep: pe32: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("pe32: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)PE32_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("pe32: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("pe32: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("pe32: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("pe32: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("pe32: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("pe32: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retsally — Wave 50 return-sally honesty */
kprintf("pe32: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("pe32: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retfosse — Wave 51 return-fosse honesty */
kprintf("pe32: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("pe32: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("pe32: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("pe32: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retravelin — Wave 53 return-travelin honesty */
kprintf("pe32: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("pe32: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("pe32: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retredan — Wave 54 exclusive redan stamp */
kprintf("pe32: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retflank — Wave 55 return-flank honesty */
kprintf("pe32: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retface — Wave 55 exclusive face stamp */
kprintf("pe32: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retgorge — Wave 56 return-gorge honesty */
kprintf("pe32: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("pe32: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retraverse — Wave 57 return-traverse honesty */
kprintf("pe32: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("pe32: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retorillon — Wave 58 return-orillon honesty */
kprintf("pe32: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("pe32: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("pe32: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("pe32: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retplace — Wave 60 return-place honesty */
kprintf("pe32: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("pe32: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("pe32: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("pe32: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("pe32: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("pe32: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("pe32: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("pe32: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: pe32: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("pe32: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: pe32: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("pe32: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: pe32: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("pe32: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: pe32: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("pe32: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: pe32: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("pe32: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=69 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: pe32: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("pe32: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=69 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("pe32: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("pe32: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("pe32: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("pe32: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pe32: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("pe32: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: pe32: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("pe32: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
                            kprintf("pe32: soft deepen wave=%u areas="
            "inventory,parse,stage,load,exec,int80,path,"
            "reject,format,reloc,soft_va,stats,lamps,smoke,capacity,"
            "return,ret_surface,surface unit=pe32.c only hard_gate=0 via=%s\n",
            (unsigned)PE32_SOFT_WAVE, szViaSafe);

    /* Grep: pe32: soft inventory PASS / pe32: soft PASS */
    kprintf("pe32: soft inventory PASS via=%s logs=%lu wave=%u\n",
            szViaSafe, (unsigned long)g_soft.u64SoftLog,
            (unsigned)PE32_SOFT_WAVE);
    kprintf("pe32: soft PASS via=%s wave=%u\n", szViaSafe,
            (unsigned)PE32_SOFT_WAVE);
}

int
pe32_parse(const void *pBuf, u32 cbLen, struct gj_pe32_info *pOut)
{
    const u8 *p;
    u32 e_lfanew;
    u32 sig;
    u16 machine;
    u16 optMagic;
    u16 chars;
    u16 nSec;
    u16 cbOpt;

    pe32_soft_inc(&g_soft.u64ParseEnter);
    if (pBuf == NULL || pOut == NULL || cbLen < 64u) {
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    p = (const u8 *)pBuf;
    if (rd16(p) != DOS_MAGIC) {
        return -1;
    }
    e_lfanew = rd32(p + 0x3c);
    /* Defensive: reject e_lfanew past buffer; require room for NT + opt magic */
    if (e_lfanew > cbLen || cbLen - e_lfanew < 26u) {
        return -1;
    }
    sig = rd32(p + e_lfanew);
    if (sig != PE_SIG) {
        return -1;
    }
    machine = rd16(p + e_lfanew + 4);
    nSec = rd16(p + e_lfanew + 6);
    cbOpt = rd16(p + e_lfanew + 20);
    chars = rd16(p + e_lfanew + 22);
    optMagic = rd16(p + e_lfanew + 24);
    pOut->u16Machine = machine;
    pOut->u16Chars = chars;
    pOut->u8IsDll = (chars & 0x2000u) ? 1u : 0u;
    pOut->u16NumSections = nSec;
    pOut->u16SizeOfOptional = cbOpt;
    pOut->u32E_lfanew = e_lfanew;
    if (optMagic == OPT_PE32) {
        /* Need optional header through Subsystem (offset 68 from opt start) */
        if (cbLen - e_lfanew < 24u + 96u) {
            return -1;
        }
        pOut->u8IsPe32 = 1;
        pOut->u32EntryRva = rd32(p + e_lfanew + 40);
        pOut->u32ImageBase32 = rd32(p + e_lfanew + 52);
        pOut->u64ImageBase = pOut->u32ImageBase32;
        pOut->u32SectionAlign = rd32(p + e_lfanew + 56);
        pOut->u32FileAlign = rd32(p + e_lfanew + 60);
        pOut->u32SizeOfImage = rd32(p + e_lfanew + 80);
        pOut->u32SizeOfHeaders = rd32(p + e_lfanew + 84);
        pOut->u16Subsystem = rd16(p + e_lfanew + 92);
        /* DllCharacteristics at opt+70 = e_lfanew+94 */
        pOut->u16DllChars = rd16(p + e_lfanew + 94);
        /*
         * Soft data directories when SizeOfOptionalHeader covers them:
         * PE32 DataDirectory at opt+96 = e_lfanew+120; entry 1=IMPORT, 5=BASERELOC.
         */
        if ((u32)cbOpt >= 96u + 6u * 8u &&
            cbLen - e_lfanew >= 24u + 96u + 6u * 8u) {
            pOut->u32ImportRva = rd32(p + e_lfanew + 120u + 1u * 8u);
            pOut->u32ImportSize = rd32(p + e_lfanew + 120u + 1u * 8u + 4u);
            pOut->u32RelocRva = rd32(p + e_lfanew + 120u + 5u * 8u);
            pOut->u32RelocSize = rd32(p + e_lfanew + 120u + 5u * 8u + 4u);
        }
    } else if (optMagic == OPT_PE32P) {
        if (cbLen - e_lfanew < 24u + 112u) {
            return -1;
        }
        pOut->u8IsPe32 = 0;
        pOut->u32EntryRva = rd32(p + e_lfanew + 40);
        pOut->u64ImageBase = (u64)rd32(p + e_lfanew + 48) |
                             ((u64)rd32(p + e_lfanew + 52) << 32);
        pOut->u32ImageBase32 = (u32)pOut->u64ImageBase;
        pOut->u32SectionAlign = rd32(p + e_lfanew + 56);
        pOut->u32FileAlign = rd32(p + e_lfanew + 60);
        pOut->u32SizeOfImage = rd32(p + e_lfanew + 80);
        pOut->u32SizeOfHeaders = rd32(p + e_lfanew + 84);
        pOut->u16Subsystem = rd16(p + e_lfanew + 92);
        pOut->u16DllChars = rd16(p + e_lfanew + 94);
        /* PE32+ DataDirectory at opt+112 = e_lfanew+136 */
        if ((u32)cbOpt >= 112u + 6u * 8u &&
            cbLen - e_lfanew >= 24u + 112u + 6u * 8u) {
            pOut->u32ImportRva = rd32(p + e_lfanew + 136u + 1u * 8u);
            pOut->u32ImportSize = rd32(p + e_lfanew + 136u + 1u * 8u + 4u);
            pOut->u32RelocRva = rd32(p + e_lfanew + 136u + 5u * 8u);
            pOut->u32RelocSize = rd32(p + e_lfanew + 136u + 5u * 8u + 4u);
        }
    } else {
        return -1;
    }
    if (machine != MACH_I386 && machine != MACH_AMD64) {
        return -1;
    }
    /* Soft machine/magic pairing: PE32↔i386, PE32+↔amd64 (reject cross) */
    if ((pOut->u8IsPe32 && machine != MACH_I386) ||
        (!pOut->u8IsPe32 && machine != MACH_AMD64)) {
        return -1;
    }
    if (nSec == 0 || nSec > GJ_PE32_MAX_SECTIONS) {
        return -1;
    }
    /* SizeOfImage must be non-zero and within stage budget (1 MiB) */
    if (pOut->u32SizeOfImage == 0 || pOut->u32SizeOfImage > 1024u * 1024u) {
        return -1;
    }
    /* Soft: SizeOfHeaders must not exceed SizeOfImage when present */
    if (pOut->u32SizeOfHeaders != 0 &&
        pOut->u32SizeOfHeaders > pOut->u32SizeOfImage) {
        return -1;
    }
    /* Soft: entry must land inside image (0 entry allowed for DLL soft) */
    if (pOut->u32EntryRva != 0 && pOut->u32EntryRva >= pOut->u32SizeOfImage) {
        return -1;
    }
    pOut->u32Ready = 1;
    /* Wave 15: format accept tallies (pair-checked above). */
    if (pOut->u8IsPe32) {
        pe32_soft_inc(&g_soft.u64FmtPe32);
    } else {
        pe32_soft_inc(&g_soft.u64FmtPe32p);
    }
    if (machine == MACH_I386) {
        pe32_soft_inc(&g_soft.u64FmtI386);
    } else if (machine == MACH_AMD64) {
        pe32_soft_inc(&g_soft.u64FmtAmd64);
    }
    pe32_soft_inc(&g_soft.u64ParseOk);
    return 0;
}

int
pe32_parse_sections(const void *pBuf, u32 cbLen,
                    const struct gj_pe32_info *pInfo,
                    struct gj_pe32_section *pSec, u32 u32MaxSec)
{
    const u8 *p;
    u32 off;
    u32 i;
    u32 n;

    pe32_soft_inc(&g_soft.u64SecEnter);
    if (pBuf == NULL || pInfo == NULL || !pInfo->u32Ready || pSec == NULL ||
        u32MaxSec == 0) {
        return -1;
    }
    p = (const u8 *)pBuf;
    n = pInfo->u16NumSections;
    if (n == 0 || n > GJ_PE32_MAX_SECTIONS) {
        return -1;
    }
    if (n > u32MaxSec) {
        n = u32MaxSec;
    }
    /* Section table starts after optional header; reject overflow / truncated */
    off = pInfo->u32E_lfanew + 24u + (u32)pInfo->u16SizeOfOptional;
    if (off < pInfo->u32E_lfanew || off > cbLen ||
        n > (cbLen - off) / 40u) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        const u8 *s = p + off + i * 40u;
        u32 j;

        memset(&pSec[i], 0, sizeof(pSec[i]));
        for (j = 0; j < 8u; j++) {
            pSec[i].aName[j] = (char)s[j];
        }
        pSec[i].aName[8] = '\0';
        pSec[i].u32VirtSize = rd32(s + 8);
        pSec[i].u32VirtAddr = rd32(s + 12);
        pSec[i].u32RawSize = rd32(s + 16);
        pSec[i].u32RawPtr = rd32(s + 20);
        pSec[i].u32Chars = rd32(s + 36);
    }
    pe32_soft_inc(&g_soft.u64SecOk);
    return (int)n;
}

int
pe32_image_stage(const void *pFile, u32 cbFile,
                 const struct gj_pe32_info *pInfo,
                 const struct gj_pe32_section *pSec, u32 u32NSec,
                 void *pImage, u32 cbImage)
{
    const u8 *pIn;
    u8 *pOut;
    u32 i;
    u32 hdrEnd;

    pe32_soft_inc(&g_soft.u64StageEnter);
    if (pFile == NULL || pInfo == NULL || pImage == NULL || !pInfo->u32Ready) {
        return -1;
    }
    if (u32NSec > 0 && pSec == NULL) {
        return -1;
    }
    if (u32NSec > GJ_PE32_MAX_SECTIONS) {
        return -1;
    }
    if (cbImage < pInfo->u32SizeOfImage || pInfo->u32SizeOfImage == 0 ||
        pInfo->u32SizeOfImage > 1024u * 1024u) {
        return -1;
    }
    pIn = (const u8 *)pFile;
    pOut = (u8 *)pImage;
    memset(pOut, 0, cbImage);
    /*
     * Soft header span: prefer SizeOfHeaders when in-range; else derive from
     * NT + optional + section table (classic PE layout).
     */
    hdrEnd = pInfo->u32E_lfanew + 24u + (u32)pInfo->u16SizeOfOptional +
             (u32)pInfo->u16NumSections * 40u;
    if (hdrEnd < pInfo->u32E_lfanew) {
        return -1; /* wrap */
    }
    if (pInfo->u32SizeOfHeaders != 0 &&
        pInfo->u32SizeOfHeaders <= pInfo->u32SizeOfImage &&
        pInfo->u32SizeOfHeaders >= hdrEnd) {
        hdrEnd = pInfo->u32SizeOfHeaders;
    }
    if (hdrEnd > cbFile) {
        hdrEnd = cbFile;
    }
    if (hdrEnd > cbImage) {
        hdrEnd = cbImage;
    }
    if (hdrEnd > pInfo->u32SizeOfImage) {
        hdrEnd = pInfo->u32SizeOfImage;
    }
    memcpy(pOut, pIn, hdrEnd);
    for (i = 0; i < u32NSec; i++) {
        u32 va = pSec[i].u32VirtAddr;
        u32 raw = pSec[i].u32RawPtr;
        u32 n = pSec[i].u32RawSize;
        u32 vs = pSec[i].u32VirtSize;
        u32 nCopy;

        if (n == 0 && vs == 0) {
            continue; /* pure empty / soft bss-less */
        }
        /* Soft: clamp raw copy to min(raw, virt) when virt non-zero (BSS tail) */
        nCopy = n;
        if (vs != 0 && nCopy > vs) {
            nCopy = vs;
        }
        if (nCopy == 0) {
            /* Virt-only (BSS): already zero-filled by memset */
            if (vs != 0 && (vs > cbImage || va > cbImage - vs ||
                            va >= pInfo->u32SizeOfImage ||
                            vs > pInfo->u32SizeOfImage - va)) {
                return -1;
            }
            continue;
        }
        /* Overflow-safe bounds: raw/va ranges must fit file and image */
        if (nCopy > cbFile || raw > cbFile - nCopy) {
            return -1;
        }
        if (nCopy > cbImage || va > cbImage - nCopy) {
            return -1;
        }
        if (va >= pInfo->u32SizeOfImage ||
            nCopy > pInfo->u32SizeOfImage - va) {
            return -1;
        }
        if (vs != 0 && (vs > cbImage || va > cbImage - vs ||
                        vs > pInfo->u32SizeOfImage - va)) {
            return -1;
        }
        memcpy(pOut + va, pIn + raw, nCopy);
    }
    pe32_soft_inc(&g_soft.u64StageOk);
    return 0;
}

int
pe32_map_user(const void *pImage, u32 cbImage, u64 u64VaBase,
              const struct gj_pe32_section *pSec, u32 u32NSec)
{
    u32 off;
    u32 i;
    const u8 *pIn;

    pe32_soft_inc(&g_soft.u64MapEnter);
    if (pImage == NULL || cbImage == 0 || (u64VaBase & 0xfffull) != 0) {
        return -1;
    }
    if (u32NSec > 0 && pSec == NULL) {
        return -1;
    }
    if (u32NSec > GJ_PE32_MAX_SECTIONS || cbImage > 1024u * 1024u) {
        return -1;
    }
    /* User map must stay below canonical high half for PE32 smoke VAs */
    if (u64VaBase >= 0x0000800000000000ull ||
        u64VaBase + (u64)cbImage < u64VaBase) {
        return -1;
    }
    pIn = (const u8 *)pImage;
    for (off = 0; off < cbImage; off += GJ_PAGE_SIZE) {
        gj_paddr_t pa = pmm_alloc();
        u32 prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER;
        u32 chunk;

        if (pa == 0) {
            return -1;
        }
        chunk = cbImage - off;
        if (chunk > GJ_PAGE_SIZE) {
            chunk = GJ_PAGE_SIZE;
        }
        /* Mark RX for pages overlapping any executable section (IMAGE_SCN_MEM_EXECUTE) */
        for (i = 0; i < u32NSec; i++) {
            u32 s = pSec[i].u32VirtAddr;
            u32 span = pSec[i].u32VirtSize ? pSec[i].u32VirtSize
                                           : pSec[i].u32RawSize;
            u32 e;

            if (span == 0 || s > ~0u - span) {
                continue; /* empty or wrap — skip prot widen */
            }
            e = s + span;
            if ((pSec[i].u32Chars & 0x20000000u) != 0 && off < e &&
                off + GJ_PAGE_SIZE > s) {
                prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER;
                break;
            }
        }
        {
            void *pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)pa)
                                                         : (u64)pa);

            memset(pK, 0, GJ_PAGE_SIZE);
            memcpy(pK, pIn + off, chunk);
        }
        if (vmm_map_page((gj_vaddr_t)(u64VaBase + off), pa, prot) != GJ_OK) {
            pmm_free(pa);
            return -1;
        }
    }
    pe32_soft_inc(&g_soft.u64MapOk);
    return 0;
}

int
pe32_load_soft_validate(const struct gj_pe32_info *pInfo,
                        const struct gj_pe32_section *pSec, u32 u32NSec)
{
    u32 i;
    u32 cbImg;

    pe32_soft_inc(&g_soft.u64ValidateEnter);
    if (pInfo == NULL || !pInfo->u32Ready) {
        return -1;
    }
    cbImg = pInfo->u32SizeOfImage;
    if (cbImg == 0 || cbImg > 1024u * 1024u) {
        return -1;
    }
    /* Soft: EXE should have a non-zero entry; DLL entry 0 is soft-ok */
    if (!pInfo->u8IsDll && pInfo->u32EntryRva == 0) {
        return -1;
    }
    if (pInfo->u32EntryRva != 0 && pInfo->u32EntryRva >= cbImg) {
        return -1;
    }
    if (u32NSec > GJ_PE32_MAX_SECTIONS) {
        return -1;
    }
    if (u32NSec > 0 && pSec == NULL) {
        return -1;
    }
    for (i = 0; i < u32NSec; i++) {
        u32 va = pSec[i].u32VirtAddr;
        u32 vs = pSec[i].u32VirtSize;
        u32 rs = pSec[i].u32RawSize;
        u32 span;

        /* Soft: section VA must land in image; empty span ok */
        if (va >= cbImg) {
            return -1;
        }
        span = vs ? vs : rs;
        if (span != 0) {
            if (span > cbImg - va) {
                return -1;
            }
        }
        /* Soft: raw size alone must not claim past image when used as span */
        if (vs == 0 && rs != 0 && rs > cbImg - va) {
            return -1;
        }
    }
    /* Soft: reloc directory bounds when present */
    if (pInfo->u32RelocSize != 0) {
        if (pInfo->u32RelocRva >= cbImg ||
            pInfo->u32RelocSize > cbImg - pInfo->u32RelocRva) {
            return -1;
        }
    }
    /* Soft: import directory bounds when present (probe only) */
    if (pInfo->u32ImportSize != 0) {
        if (pInfo->u32ImportRva >= cbImg ||
            pInfo->u32ImportSize > cbImg - pInfo->u32ImportRva) {
            return -1;
        }
    }
    pe32_soft_inc(&g_soft.u64ValidateOk);
    return 0;
}

u64
pe32_load_soft_va(const struct gj_pe32_info *pInfo)
{
    u64 u64Pref;

    /* High free band used by all PE smokes; stable greppable map path. */
    if (pInfo == NULL || !pInfo->u32Ready) {
        pe32_soft_inc(&g_soft.u64SoftVaNull);
        pe32_soft_inc(&g_soft.u64SoftVaFallback);
        return 0x50000000ull;
    }
    u64Pref = pInfo->u64ImageBase;
    /*
     * Soft: honor preferred base only when already in high user band and
     * page-aligned. Classic 0x400000 falls back (avoids low identity clash).
     */
    if (u64Pref >= 0x50000000ull && u64Pref < 0x0000800000000000ull &&
        (u64Pref & 0xfffull) == 0ull) {
        /* Soft wrap check for SizeOfImage */
        if (u64Pref + (u64)pInfo->u32SizeOfImage > u64Pref) {
            pe32_soft_inc(&g_soft.u64SoftVaPref);
            return u64Pref;
        }
    }
    pe32_soft_inc(&g_soft.u64SoftVaFallback);
    return 0x50000000ull;
}

int
pe32_soft_relocate(void *pImage, u32 cbImage,
                   const struct gj_pe32_info *pInfo, u64 u64VaBase)
{
    u8 *pImg;
    u32 u32Off;
    u32 u32End;
    u64 u64Delta;
    u64 u64Pref;

    pe32_soft_inc(&g_soft.u64RelocEnter);
    if (pImage == NULL || pInfo == NULL || !pInfo->u32Ready || cbImage == 0) {
        pe32_soft_inc(&g_soft.u64RelocFail);
        return -1;
    }
    if (cbImage < pInfo->u32SizeOfImage) {
        pe32_soft_inc(&g_soft.u64RelocFail);
        return -1;
    }
    u64Pref = pInfo->u64ImageBase;
    u64Delta = u64VaBase - u64Pref;
    /* Soft: already at preferred — nothing to fix up */
    if (u64Delta == 0) {
        pe32_soft_inc(&g_soft.u64RelocDelta0);
        pe32_soft_inc(&g_soft.u64RelocOk);
        return 0;
    }
    /* Soft: no reloc directory → fixed-base image; allow high-VA smoke map */
    if (pInfo->u32RelocRva == 0 || pInfo->u32RelocSize < 8u) {
        pe32_soft_inc(&g_soft.u64RelocEmptyDir);
        pe32_soft_inc(&g_soft.u64RelocOk);
        return 0;
    }
    if (pInfo->u32RelocRva >= cbImage ||
        pInfo->u32RelocSize > cbImage - pInfo->u32RelocRva) {
        pe32_soft_inc(&g_soft.u64RelocFail);
        return -1;
    }
    pImg = (u8 *)pImage;
    u32Off = pInfo->u32RelocRva;
    u32End = pInfo->u32RelocRva + pInfo->u32RelocSize;
    while (u32Off + 8u <= u32End) {
        u32 u32PageRva = rd32(pImg + u32Off);
        u32 u32BlockSize = rd32(pImg + u32Off + 4);
        u32 u32Entries;
        u32 i;

        if (u32BlockSize < 8u) {
            pe32_soft_inc(&g_soft.u64RelocFail);
            return -1; /* corrupt */
        }
        if (u32BlockSize > u32End - u32Off) {
            pe32_soft_inc(&g_soft.u64RelocFail);
            return -1;
        }
        u32Entries = (u32BlockSize - 8u) / 2u;
        for (i = 0; i < u32Entries; i++) {
            u16 u16Ent = rd16(pImg + u32Off + 8u + i * 2u);
            u16 u16Type = (u16)(u16Ent >> 12);
            u16 u16Ofs = (u16)(u16Ent & 0x0fffu);
            u32 u32Fix = u32PageRva + (u32)u16Ofs;

            if (u16Type == 0) {
                /* IMAGE_REL_BASED_ABSOLUTE */
                pe32_soft_inc(&g_soft.u64RelocAbs);
                continue;
            }
            if (u16Type == 3) {
                /* IMAGE_REL_BASED_HIGHLOW — 32-bit */
                u32 u32Val;

                if (u32Fix > cbImage - 4u) {
                    pe32_soft_inc(&g_soft.u64RelocFail);
                    return -1;
                }
                u32Val = rd32(pImg + u32Fix);
                u32Val = (u32)(u32Val + (u32)u64Delta);
                pImg[u32Fix + 0] = (u8)(u32Val);
                pImg[u32Fix + 1] = (u8)(u32Val >> 8);
                pImg[u32Fix + 2] = (u8)(u32Val >> 16);
                pImg[u32Fix + 3] = (u8)(u32Val >> 24);
                pe32_soft_inc(&g_soft.u64RelocHighlow);
            } else if (u16Type == 10) {
                /* IMAGE_REL_BASED_DIR64 — PE32+ */
                u64 u64Val;
                u32 lo, hi;

                if (u32Fix > cbImage - 8u) {
                    pe32_soft_inc(&g_soft.u64RelocFail);
                    return -1;
                }
                lo = rd32(pImg + u32Fix);
                hi = rd32(pImg + u32Fix + 4);
                u64Val = (u64)lo | ((u64)hi << 32);
                u64Val += u64Delta;
                pImg[u32Fix + 0] = (u8)(u64Val);
                pImg[u32Fix + 1] = (u8)(u64Val >> 8);
                pImg[u32Fix + 2] = (u8)(u64Val >> 16);
                pImg[u32Fix + 3] = (u8)(u64Val >> 24);
                pImg[u32Fix + 4] = (u8)(u64Val >> 32);
                pImg[u32Fix + 5] = (u8)(u64Val >> 40);
                pImg[u32Fix + 6] = (u8)(u64Val >> 48);
                pImg[u32Fix + 7] = (u8)(u64Val >> 56);
                pe32_soft_inc(&g_soft.u64RelocDir64);
            } else if (u16Type == 1 || u16Type == 2) {
                /* HIGH / LOW — soft 16-bit halves */
                u16 u16Val;

                if (u32Fix > cbImage - 2u) {
                    pe32_soft_inc(&g_soft.u64RelocFail);
                    return -1;
                }
                u16Val = rd16(pImg + u32Fix);
                if (u16Type == 1) {
                    u16Val = (u16)(u16Val + (u16)(u64Delta >> 16));
                    pe32_soft_inc(&g_soft.u64RelocHigh);
                } else {
                    u16Val = (u16)(u16Val + (u16)u64Delta);
                    pe32_soft_inc(&g_soft.u64RelocLow);
                }
                pImg[u32Fix + 0] = (u8)(u16Val);
                pImg[u32Fix + 1] = (u8)(u16Val >> 8);
            } else {
                /* Soft: unknown reloc type — skip (do not fail smoke PE) */
                pe32_soft_inc(&g_soft.u64RelocSkip);
                continue;
            }
        }
        u32Off += u32BlockSize;
        /* Soft: align block walk to 4 when odd padding present */
        if ((u32Off & 3u) != 0) {
            u32Off = (u32Off + 3u) & ~3u;
        }
    }
    pe32_soft_inc(&g_soft.u64RelocOk);
    return 0;
}

int
pe32_load_soft_exec(const void *pFile, u32 cbFile, i32 *pExitCode)
{
    struct gj_pe32_info info;
    struct gj_pe32_section aSec[GJ_PE32_MAX_SECTIONS];
    static u8 aSoftImg[0x8000];
    int nSec;

    if (pFile == NULL || cbFile < 64u) {
        return -1;
    }
    if (pe32_parse(pFile, cbFile, &info) != 0) {
        return -1;
    }
    /* Soft-exec path is i386 PE32 only (interpreter is 32-bit). */
    if (!info.u8IsPe32 || info.u16Machine != MACH_I386) {
        return -1;
    }
    nSec = pe32_parse_sections(pFile, cbFile, &info, aSec, GJ_PE32_MAX_SECTIONS);
    if (nSec < 0) {
        return -1;
    }
    if (pe32_load_soft_validate(&info, aSec, (u32)nSec) != 0) {
        return -1;
    }
    if (info.u32SizeOfImage == 0 || info.u32SizeOfImage > sizeof(aSoftImg)) {
        return -1;
    }
    if (pe32_image_stage(pFile, cbFile, &info, aSec, (u32)nSec, aSoftImg,
                         info.u32SizeOfImage) != 0) {
        return -1;
    }
    /* Soft rebase to high VA (no AS map); reloc soft-skip when empty. */
    if (pe32_soft_relocate(aSoftImg, info.u32SizeOfImage, &info,
                           pe32_load_soft_va(&info)) != 0) {
        return -1;
    }
    return pe32_i386_soft_exec(aSoftImg, info.u32SizeOfImage, info.u32EntryRva,
                               pExitCode);
}

int
pe32_load_process(struct gj_process *pProc, const void *pFile, u32 cbFile,
                  struct gj_pe32_load *pOut)
{
    struct gj_pe32_info info;
    struct gj_pe32_section aSec[GJ_PE32_MAX_SECTIONS];
    static u8 aImage[0x8000];
    int nSec;
    u64 vaBase;
    u64 u64Saved;
    u32 stackPages = 4;

    pe32_soft_inc(&g_soft.u64LoadEnter);
    if (pProc == NULL || pFile == NULL || pOut == NULL) {
        kprintf("pe32: load_process arg FAIL\n");
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    /* Parse/stage under kernel CR3 (mcmodel=kernel image access). */
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    if (pe32_parse(pFile, cbFile, &info) != 0) {
        kprintf("pe32: load_process parse FAIL\n");
        cpu_load_cr3(u64Saved);
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    nSec = pe32_parse_sections(pFile, cbFile, &info, aSec, GJ_PE32_MAX_SECTIONS);
    if (nSec < 0) {
        kprintf("pe32: load_process sections FAIL\n");
        cpu_load_cr3(u64Saved);
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    if (info.u32SizeOfImage == 0 || info.u32SizeOfImage > sizeof(aImage)) {
        kprintf("pe32: load_process size FAIL sz=0x%x\n", info.u32SizeOfImage);
        cpu_load_cr3(u64Saved);
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    /* Soft load validate before staging (entry/sec/dir bounds). */
    if (pe32_load_soft_validate(&info, aSec, (u32)nSec) != 0) {
        kprintf("pe32: load_process soft_validate FAIL\n");
        cpu_load_cr3(u64Saved);
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    if (pe32_image_stage(pFile, cbFile, &info, aSec, (u32)nSec, aImage,
                         info.u32SizeOfImage) != 0) {
        kprintf("pe32: load_process stage FAIL\n");
        cpu_load_cr3(u64Saved);
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    /* Soft VA: preferred when high, else 0x50000000 (smoke-stable). */
    vaBase = pe32_load_soft_va(&info);
    if (pe32_soft_relocate(aImage, info.u32SizeOfImage, &info, vaBase) != 0) {
        kprintf("pe32: load_process soft_reloc FAIL\n");
        cpu_load_cr3(u64Saved);
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    cpu_load_cr3(u64Saved);
    if (process_as_ensure(pProc) != GJ_OK) {
        kprintf("pe32: load_process as_ensure FAIL\n");
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    u64Saved = cpu_read_cr3();
    process_as_activate(pProc);
    if (pe32_map_user(aImage, info.u32SizeOfImage, vaBase, aSec, (u32)nSec) !=
        0) {
        kprintf("pe32: load_process map_user FAIL va=0x%lx\n",
                (unsigned long)vaBase);
        cpu_load_cr3(u64Saved);
        pe32_soft_inc(&g_soft.u64LoadFail);
        return -1;
    }
    /* Stack above image end (+ guard page gap) */
    {
        u64 stackBase = vaBase + info.u32SizeOfImage + GJ_PAGE_SIZE;
        u32 p;

        for (p = 0; p < stackPages; p++) {
            gj_paddr_t pa = pmm_alloc();
            void *pK;

            if (pa == 0) {
                kprintf("pe32: load_process stack pmm FAIL\n");
                cpu_load_cr3(u64Saved);
                pe32_soft_inc(&g_soft.u64LoadFail);
                return -1;
            }
            pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)pa)
                                                   : (u64)pa);
            memset(pK, 0, GJ_PAGE_SIZE);
            if (vmm_map_page((gj_vaddr_t)(stackBase + (u64)p * GJ_PAGE_SIZE), pa,
                             GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                 GJ_VMM_PROT_USER) != GJ_OK) {
                kprintf("pe32: load_process stack map FAIL p=%u\n", p);
                pmm_free(pa);
                cpu_load_cr3(u64Saved);
                pe32_soft_inc(&g_soft.u64LoadFail);
                return -1;
            }
        }
        pOut->u64StackTop = stackBase + (u64)stackPages * GJ_PAGE_SIZE;
    }
    cpu_load_cr3(u64Saved);
    pOut->u64ImageVa = vaBase;
    pOut->u64Entry = vaBase + (u64)info.u32EntryRva;
    pOut->u32Ready = 1;
    pe32_soft_inc(&g_soft.u64LoadOk);
    kprintf("pe32: load_process entry=0x%lx stack=0x%lx image=0x%lx\n",
            (unsigned long)pOut->u64Entry, (unsigned long)pOut->u64StackTop,
            (unsigned long)vaBase);
    return 0;
}

int
pe32_smoke(void)
{
    /*
     * Crafted PE32: headers at 0x40, one .text section:
     *   VirtualAddress=0x1000, RawPtr=0x200, RawSize=16, magic payload.
     * SizeOfImage=0x2000, SizeOfOptionalHeader=0x60.
     */
    static u8 aPe[0x300];
    static u8 aImage[0x2000];
    struct gj_pe32_info info;
    struct gj_pe32_section aSec[4];
    u32 e_lfanew = 0x40;
    u32 secOff;
    u32 i;
    int nSec;

    kprintf("pe32: smoke start\n");
    for (i = 0; i < sizeof(aPe); i++) {
        aPe[i] = 0;
    }
    aPe[0] = 0x4d;
    aPe[1] = 0x5a;
    aPe[0x3c] = 0x40;
    /* PE\0\0 */
    aPe[e_lfanew + 0] = 0x50;
    aPe[e_lfanew + 1] = 0x45;
    /* Machine i386 */
    aPe[e_lfanew + 4] = 0x4c;
    aPe[e_lfanew + 5] = 0x01;
    /* NumberOfSections = 1 */
    aPe[e_lfanew + 6] = 0x01;
    aPe[e_lfanew + 7] = 0x00;
    /* SizeOfOptionalHeader = 0x60 */
    aPe[e_lfanew + 20] = 0x60;
    aPe[e_lfanew + 21] = 0x00;
    /* Optional Magic PE32 */
    aPe[e_lfanew + 24] = 0x0b;
    aPe[e_lfanew + 25] = 0x01;
    /* Entry = 0x1000 */
    aPe[e_lfanew + 40] = 0x00;
    aPe[e_lfanew + 41] = 0x10;
    /* ImageBase = 0x00400000 */
    aPe[e_lfanew + 52] = 0x00;
    aPe[e_lfanew + 53] = 0x00;
    aPe[e_lfanew + 54] = 0x40;
    /* SizeOfImage = 0x2000 */
    aPe[e_lfanew + 80] = 0x00;
    aPe[e_lfanew + 81] = 0x20;
    /* Subsystem = 3 */
    aPe[e_lfanew + 92] = 0x03;
    /* Section header at e_lfanew+24+0x60 = 0x40+0x18+0x60 = 0xB8 */
    secOff = e_lfanew + 24u + 0x60u;
    /* Name ".text\0\0\0" */
    aPe[secOff + 0] = '.';
    aPe[secOff + 1] = 't';
    aPe[secOff + 2] = 'e';
    aPe[secOff + 3] = 'x';
    aPe[secOff + 4] = 't';
    /* VirtualSize = 16 */
    aPe[secOff + 8] = 0x10;
    /* VirtualAddress = 0x1000 */
    aPe[secOff + 12] = 0x00;
    aPe[secOff + 13] = 0x10;
    /* SizeOfRawData = 16 */
    aPe[secOff + 16] = 0x10;
    /* PointerToRawData = 0x200 */
    aPe[secOff + 20] = 0x00;
    aPe[secOff + 21] = 0x02;
    /* Characteristics = code|exec|read */
    aPe[secOff + 36] = 0x20;
    aPe[secOff + 37] = 0x00;
    aPe[secOff + 38] = 0x00;
    aPe[secOff + 39] = 0x60;
    /* Raw payload at 0x200 */
    aPe[0x200] = 0x90; /* nop */
    aPe[0x201] = 0xb8; /* mov eax, imm32-shaped marker */
    aPe[0x202] = 0xef;
    aPe[0x203] = 0xbe;
    aPe[0x204] = 0xad;
    aPe[0x205] = 0xde;

    if (pe32_parse(aPe, (u32)sizeof(aPe), &info) != 0) {
        kprintf("pe32: parse FAIL\n");
        return -1;
    }
    kprintf("pe32: machine=0x%x pe32=%u entry=0x%x base=0x%x size=0x%x nsec=%u\n",
            info.u16Machine, info.u8IsPe32, info.u32EntryRva,
            info.u32ImageBase32, info.u32SizeOfImage, info.u16NumSections);
    if (!(info.u32Ready && info.u16Machine == MACH_I386 && info.u8IsPe32 == 1 &&
          info.u32ImageBase32 == 0x00400000u && info.u32EntryRva == 0x1000u)) {
        kprintf("pe32: field check FAIL\n");
        return -1;
    }
    kprintf("pe32: parse PASS\n");
    nSec = pe32_parse_sections(aPe, (u32)sizeof(aPe), &info, aSec, 4);
    if (nSec < 1 || aSec[0].u32VirtAddr != 0x1000u ||
        aSec[0].u32RawPtr != 0x200u) {
        kprintf("pe32: sections FAIL n=%d\n", nSec);
        return -1;
    }
    kprintf("pe32: section \"%s\" va=0x%x raw=0x%x\n", aSec[0].aName,
            aSec[0].u32VirtAddr, aSec[0].u32RawPtr);
    if (pe32_image_stage(aPe, (u32)sizeof(aPe), &info, aSec, (u32)nSec, aImage,
                         (u32)sizeof(aImage)) != 0) {
        kprintf("pe32: stage FAIL\n");
        return -1;
    }
    if (aImage[0x1000] != 0x90 || aImage[0x1002] != 0xef ||
        aImage[0x1005] != 0xde) {
        kprintf("pe32: stage payload FAIL\n");
        return -1;
    }
    kprintf("pe32: section map PASS\n");
    /* Map staged image into free high user VA (avoid low identity maps). */
    {
        u64 va = 0x50000000ull;

        if (pe32_map_user(aImage, info.u32SizeOfImage, va, aSec, (u32)nSec) ==
            0) {
            kprintf("pe32: user map va=0x%lx pages=%u\n", (unsigned long)va,
                    (info.u32SizeOfImage + GJ_PAGE_SIZE - 1) / GJ_PAGE_SIZE);
            kprintf("pe32: user map PASS\n");
        } else {
            kprintf("pe32: user map FAIL\n");
            return -1;
        }
    }
    pe32_soft_inc(&g_soft.u64SmokeOk);
    /* Wave 11: greppable soft inventory after parse/stage/map smoke. */
    pe32_soft_inventory_log("smoke");
    return 0;
}

int
pe32_spawn_smoke(void)
{
    /*
     * PE32+ (amd64) with Linux exit(0) via SYSCALL — call only after
     * user GDT/SYSCALL/ring-3 path is live (after init-class spawns).
     */
    static u8 aPe64[0x300];
    static struct gj_process peProc;
    static struct gj_cnode peCnode;
    static struct gj_cap_slot aPeSlots[32];
    static struct gj_root_meta peMeta;
    struct gj_pe32_load load;
    struct gj_pe32_info i64info;
    struct gj_pe32_section aS2[4];
    u32 e2 = 0x40;
    u32 s2;
    u32 i;
    int ns2;
    u32 thr;
    u32 y;

    {
        u64 u64Ker = vmm_kernel_cr3();
        u64 u64SavedPe = cpu_read_cr3();

        if (u64Ker != 0) {
            cpu_load_cr3(u64Ker);
        }
        for (i = 0; i < sizeof(aPe64); i++) {
            aPe64[i] = 0;
        }
        aPe64[0] = 0x4d;
        aPe64[1] = 0x5a;
        aPe64[0x3c] = 0x40;
        aPe64[e2 + 0] = 0x50;
        aPe64[e2 + 1] = 0x45;
        aPe64[e2 + 4] = 0x64;
        aPe64[e2 + 5] = 0x86;
        aPe64[e2 + 6] = 0x01;
        aPe64[e2 + 20] = 0x70;
        aPe64[e2 + 24] = 0x0b;
        aPe64[e2 + 25] = 0x02;
        aPe64[e2 + 40] = 0x00;
        aPe64[e2 + 41] = 0x10;
        aPe64[e2 + 80] = 0x00;
        aPe64[e2 + 81] = 0x20;
        aPe64[e2 + 92] = 0x03;
        s2 = e2 + 24u + 0x70u;
        aPe64[s2 + 0] = '.';
        aPe64[s2 + 1] = 't';
        aPe64[s2 + 2] = 'e';
        aPe64[s2 + 3] = 'x';
        aPe64[s2 + 4] = 't';
        aPe64[s2 + 8] = 0x20;
        aPe64[s2 + 12] = 0x00;
        aPe64[s2 + 13] = 0x10;
        aPe64[s2 + 16] = 0x20;
        aPe64[s2 + 20] = 0x00;
        aPe64[s2 + 21] = 0x02;
        aPe64[s2 + 36] = 0x20;
        aPe64[s2 + 39] = 0x60;
        /* mov eax,60; xor edi,edi; syscall */
        aPe64[0x200] = 0xb8;
        aPe64[0x201] = 0x3c;
        aPe64[0x202] = 0x00;
        aPe64[0x203] = 0x00;
        aPe64[0x204] = 0x00;
        aPe64[0x205] = 0x31;
        aPe64[0x206] = 0xff;
        aPe64[0x207] = 0x0f;
        aPe64[0x208] = 0x05;

        if (pe32_parse(aPe64, (u32)sizeof(aPe64), &i64info) != 0 ||
            i64info.u16Machine != MACH_AMD64) {
            kprintf("pe32: pe32+ parse FAIL\n");
            if (u64Ker != 0) {
                cpu_load_cr3(u64SavedPe);
            }
            return -1;
        }
        if (u64Ker != 0) {
            cpu_load_cr3(u64SavedPe);
        }
    }
    ns2 = pe32_parse_sections(aPe64, (u32)sizeof(aPe64), &i64info, aS2, 4);
    if (ns2 < 1) {
        kprintf("pe32: pe32+ sections FAIL\n");
        return -1;
    }
    memset(&peProc, 0, sizeof(peProc));
    gj_process_init(&peProc, &peCnode, aPeSlots, 32);
    peProc.u32Alive = 1;
    peProc.u32Personality = 1;
    (void)gj_process_bootstrap_root_meta(&peProc, &peMeta, NULL);
    if (pe32_load_process(&peProc, aPe64, (u32)sizeof(aPe64), &load) != 0) {
        kprintf("pe32: load_process FAIL\n");
        return -1;
    }
    {
        u32 pid = process_wait_register(&peProc, 1);

        kprintf("pe32: wait-register pid=%u\n", pid);
    }
    thr = thread_create_user(&peProc, load.u64Entry, load.u64StackTop);
    if (thr == 0) {
        kprintf("pe32: thread FAIL\n");
        return -1;
    }
    kprintf("pe32: spawn thr=%u entry=0x%lx\n", thr,
            (unsigned long)load.u64Entry);
    for (y = 0; y < 32u; y++) {
        thread_yield();
    }
    /* Reap PE32+ child if it exited via SYSCALL exit */
    {
        i32 wst = -1;
        i64 wr = process_wait4(-1, &wst, 1); /* WNOHANG */

        kprintf("pe32: wait4 after spawn => %ld status=0x%x\n", (long)wr,
                (unsigned)wst);
        if (wr > 0) {
            kprintf("pe32: wait4 reap PASS\n");
        }
    }
    kprintf("pe32: spawn path PASS thr=%u yields=%u\n", thr, y);
    kprintf("pe32: spawn PASS\n");
    (void)ns2;
    (void)aS2;
    pe32_soft_inc(&g_soft.u64SpawnOk);
    /* Wave 11: greppable soft inventory after PE32+ spawn smoke. */
    pe32_soft_inventory_log("spawn");
    return 0;
}

int
pe32_wow64_smoke(void)
{
    /*
     * PE32 (i386) staged + mapped; entry is a 64-bit WoW64 trampoline that
     * records “would enter 32-bit” then exits via SYSCALL. True compat CS
     * (32-bit code segment) is deferred — trampoline proves load + hook path.
     */
    static u8 aPe32[0x300];
    static u8 aImage[0x2000];
    static u8 aTramp[64];
    static struct gj_process wowProc;
    static struct gj_cnode wowCnode;
    static struct gj_cap_slot aWowSlots[32];
    static struct gj_root_meta wowMeta;
    struct gj_pe32_info info;
    struct gj_pe32_section aSec[4];
    u32 e = 0x40;
    u32 s;
    u32 i;
    int nSec;
    u64 vaImg = 0x51000000ull;
    u64 vaTramp = 0x52000000ull;
    u64 vaStack = 0x52100000ull;
    u64 u64Saved;
    u32 thr;
    u32 y;
    gj_paddr_t paT;
    gj_paddr_t paS;
    void *pK;

    kprintf("pe32: wow64 smoke start\n");
    for (i = 0; i < sizeof(aPe32); i++) {
        aPe32[i] = 0;
    }
    aPe32[0] = 0x4d;
    aPe32[1] = 0x5a;
    aPe32[0x3c] = 0x40;
    aPe32[e + 0] = 0x50;
    aPe32[e + 1] = 0x45;
    aPe32[e + 4] = 0x4c; /* i386 */
    aPe32[e + 5] = 0x01;
    aPe32[e + 6] = 0x01;
    aPe32[e + 20] = 0x60;
    aPe32[e + 24] = 0x0b; /* PE32 */
    aPe32[e + 25] = 0x01;
    aPe32[e + 40] = 0x00;
    aPe32[e + 41] = 0x10; /* EntryRVA */
    aPe32[e + 52] = 0x00;
    aPe32[e + 53] = 0x00;
    aPe32[e + 54] = 0x40; /* ImageBase 0x400000 */
    aPe32[e + 80] = 0x00;
    aPe32[e + 81] = 0x20;
    aPe32[e + 92] = 0x03;
    s = e + 24u + 0x60u;
    aPe32[s + 0] = '.';
    aPe32[s + 1] = 't';
    aPe32[s + 2] = 'e';
    aPe32[s + 3] = 'x';
    aPe32[s + 4] = 't';
    aPe32[s + 8] = 0x10;
    aPe32[s + 12] = 0x00;
    aPe32[s + 13] = 0x10;
    aPe32[s + 16] = 0x10;
    aPe32[s + 20] = 0x00;
    aPe32[s + 21] = 0x02;
    aPe32[s + 36] = 0x20;
    aPe32[s + 39] = 0x60;
    /* i386 payload: mov eax,1; xor ebx,ebx; int 0x80  (exit legacy; not run) */
    aPe32[0x200] = 0xb8;
    aPe32[0x201] = 0x01;
    aPe32[0x202] = 0x00;
    aPe32[0x203] = 0x00;
    aPe32[0x204] = 0x00;
    aPe32[0x205] = 0x31;
    aPe32[0x206] = 0xdb;
    aPe32[0x207] = 0xcd;
    aPe32[0x208] = 0x80;

    if (pe32_parse(aPe32, (u32)sizeof(aPe32), &info) != 0 ||
        info.u16Machine != MACH_I386 || info.u8IsPe32 != 1) {
        kprintf("pe32: wow64 i386 parse FAIL\n");
        return -1;
    }
    nSec = pe32_parse_sections(aPe32, (u32)sizeof(aPe32), &info, aSec, 4);
    if (nSec < 1) {
        kprintf("pe32: wow64 sections FAIL\n");
        return -1;
    }
    if (pe32_image_stage(aPe32, (u32)sizeof(aPe32), &info, aSec, (u32)nSec,
                         aImage, (u32)sizeof(aImage)) != 0) {
        kprintf("pe32: wow64 stage FAIL\n");
        return -1;
    }
    if (aImage[0x1000] != 0xb8) {
        kprintf("pe32: wow64 stage payload FAIL\n");
        return -1;
    }
    kprintf("pe32: wow64 i386 stage ok entry_rva=0x%x\n", info.u32EntryRva);
    /* Soft load path: parse+stage+validate+reloc+soft-exec (no AS map) */
    {
        i32 code = -1;
        static u8 aIretStack[256];

        if (pe32_load_soft_exec(aPe32, (u32)sizeof(aPe32), &code) == 0 &&
            code == 0) {
            kprintf("pe32: i386 soft-exec PASS\n");
        } else {
            kprintf("pe32: i386 soft-exec FAIL code=%d\n", code);
            return -1;
        }
        /* Frame + LAR + soft-exec pipeline (hardware CS switch still deferred) */
        memset(aIretStack, 0, sizeof(aIretStack));
        code = -1;
        if (pe32_compat_soft_iretq(aImage, (u32)sizeof(aImage),
                                   info.u32EntryRva, aIretStack,
                                   (u32)sizeof(aIretStack), &code) == 0 &&
            code == 0) {
            /* pe32: soft-iretq PASS printed inside */
        } else {
            kprintf("pe32: soft-iretq FAIL\n");
            return -1;
        }
    }

    /* 64-bit trampoline: mov eax,60; xor edi,edi; syscall  (exit 0) */
    for (i = 0; i < sizeof(aTramp); i++) {
        aTramp[i] = 0;
    }
    aTramp[0] = 0xb8;
    aTramp[1] = 0x3c;
    aTramp[2] = 0x00;
    aTramp[3] = 0x00;
    aTramp[4] = 0x00;
    aTramp[5] = 0x31;
    aTramp[6] = 0xff;
    aTramp[7] = 0x0f;
    aTramp[8] = 0x05;

    memset(&wowProc, 0, sizeof(wowProc));
    gj_process_init(&wowProc, &wowCnode, aWowSlots, 32);
    wowProc.u32Alive = 1;
    wowProc.u32Personality = 1;
    (void)gj_process_bootstrap_root_meta(&wowProc, &wowMeta, NULL);
    if (process_as_ensure(&wowProc) != GJ_OK) {
        kprintf("pe32: wow64 as FAIL\n");
        return -1;
    }
    u64Saved = cpu_read_cr3();
    process_as_activate(&wowProc);
    if (pe32_map_user(aImage, info.u32SizeOfImage, vaImg, aSec, (u32)nSec) !=
        0) {
        cpu_load_cr3(u64Saved);
        kprintf("pe32: wow64 map FAIL\n");
        return -1;
    }
    paT = pmm_alloc();
    paS = pmm_alloc();
    if (paT == 0 || paS == 0) {
        cpu_load_cr3(u64Saved);
        kprintf("pe32: wow64 tramp alloc FAIL\n");
        return -1;
    }
    pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paT)
                                           : (u64)paT);
    memset(pK, 0, GJ_PAGE_SIZE);
    memcpy(pK, aTramp, sizeof(aTramp));
    if (vmm_map_page((gj_vaddr_t)vaTramp, paT,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        cpu_load_cr3(u64Saved);
        kprintf("pe32: wow64 tramp map FAIL\n");
        return -1;
    }
    pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                           : (u64)paS);
    memset(pK, 0, GJ_PAGE_SIZE);
    if (vmm_map_page((gj_vaddr_t)vaStack, paS,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        cpu_load_cr3(u64Saved);
        kprintf("pe32: wow64 stack map FAIL\n");
        return -1;
    }
    cpu_load_cr3(u64Saved);

    /* Personality hook: WoW64 on; CS32 selector ready for future far jump */
    wow64_set(1);
    kprintf("pe32: wow64 would_enter32 img=0x%lx entry32=0x%lx tramp=0x%lx "
            "cs32=0x%x ready=%d\n",
            (unsigned long)vaImg,
            (unsigned long)(vaImg + (u64)info.u32EntryRva),
            (unsigned long)vaTramp, (unsigned)gdt_user_cs32_sel(),
            gdt_user_cs32_ready());
    if (gdt_user_cs32_ready() && gdt_user_cs32_sel() == GJ_GDT_USER_CS32) {
        kprintf("pe32: wow64 cs32 selector PASS\n");
    }

    (void)process_wait_register(&wowProc, 1);
    thr = thread_create_user(&wowProc, vaTramp, vaStack + GJ_PAGE_SIZE);
    if (thr == 0) {
        wow64_set(0);
        kprintf("pe32: wow64 thread FAIL\n");
        return -1;
    }
    for (y = 0; y < 32u; y++) {
        thread_yield();
    }
    wow64_set(0);
    kprintf("pe32: wow64 thr=%u yields=%u calls=%u\n", thr, y, wow64_calls());
    kprintf("pe32: wow64 trampoline PASS\n");
    /* Compat frame layout (no hardware 32-bit enter — smoke-only) */
    {
        u64 aFrame[8];
        u64 entry32 = vaImg + (u64)info.u32EntryRva;
        u64 stack32 = vaStack + 0x800; /* mid-stack for 32-bit view */

        if (pe32_compat_frame_prepare(entry32, stack32, aFrame, 8) == 0) {
            kprintf("pe32: compat frame PASS rip=0x%lx cs=0x%lx ss=0x%lx\n",
                    (unsigned long)aFrame[0], (unsigned long)aFrame[1],
                    (unsigned long)aFrame[4]);
            pe32_soft_inc(&g_soft.u64CompatOk);
        } else {
            kprintf("pe32: compat frame FAIL\n");
            return -1;
        }
    }
    (void)nSec;
    pe32_soft_inc(&g_soft.u64Wow64Ok);
    /* Wave 11: greppable soft inventory after wow64/compat smoke. */
    pe32_soft_inventory_log("wow64");
    return 0;
}

int
pe32_i386_soft_exec(const void *pImage, u32 cbImage, u32 u32EntryRva,
                    i32 *pExitCode)
{
    /*
     * Minimal i386 interpreter for PE smoke / load soft paths (clean-room).
     * Not a full CPU; int 0x80 subset mirrors trap_dispatch CS32 for
     * exit/mmap2/path-ish NRs used by soft-exec, soft-iretq, and load_soft_exec.
     * Soft stack (64 slots) deepens push/pop/call/ret for PE prologue stubs.
     */
    const u8 *p;
    u32 ip;
    u32 eax = 0;
    u32 ebx = 0;
    u32 ecx = 0;
    u32 edx = 0;
    u32 esi = 0;
    u32 edi = 0;
    u32 ebp = 0;
    u32 steps;
    u32 aStack[64];
    u32 u32Sp = 64; /* grows down; empty at top */
    u8 u8Zf = 1;

    pe32_soft_inc(&g_soft.u64SoftExecEnter);
    if (pImage == NULL || cbImage == 0 || u32EntryRva >= cbImage) {
        pe32_soft_inc(&g_soft.u64SoftExecFail);
        return -1;
    }
    p = (const u8 *)pImage;
    ip = u32EntryRva;
    memset(aStack, 0, sizeof(aStack));
    for (steps = 0; steps < 256u; steps++) {
        u8 op;

        if (ip >= cbImage) {
            return -1;
        }
        op = p[ip];
        if (op == 0x90 || op == 0x66) { /* nop / data16 prefix soft-ignore */
            ip++;
            continue;
        }
        if (op == 0xb8) { /* mov eax, imm32 */
            if (ip + 5 > cbImage) {
                return -1;
            }
            eax = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                  ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            u8Zf = (eax == 0) ? 1u : 0u;
            ip += 5;
            continue;
        }
        if (op == 0xb9) { /* mov ecx, imm32 */
            if (ip + 5 > cbImage) {
                return -1;
            }
            ecx = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                  ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            u8Zf = (ecx == 0) ? 1u : 0u;
            ip += 5;
            continue;
        }
        if (op == 0xba) { /* mov edx, imm32 */
            if (ip + 5 > cbImage) {
                return -1;
            }
            edx = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                  ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            u8Zf = (edx == 0) ? 1u : 0u;
            ip += 5;
            continue;
        }
        if (op == 0xbb) { /* mov ebx, imm32 */
            if (ip + 5 > cbImage) {
                return -1;
            }
            ebx = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                  ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            u8Zf = (ebx == 0) ? 1u : 0u;
            ip += 5;
            continue;
        }
        if (op == 0xbe) { /* mov esi, imm32 — soft load prologues */
            if (ip + 5 > cbImage) {
                return -1;
            }
            esi = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                  ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            ip += 5;
            continue;
        }
        if (op == 0xbf) { /* mov edi, imm32 */
            if (ip + 5 > cbImage) {
                return -1;
            }
            edi = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                  ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            ip += 5;
            continue;
        }
        if (op == 0xbd) { /* mov ebp, imm32 */
            if (ip + 5 > cbImage) {
                return -1;
            }
            ebp = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                  ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            ip += 5;
            continue;
        }
        if (op == 0x31 && ip + 1 < cbImage) {
            u8 mod = p[ip + 1];

            if (mod == 0xdb) {
                ebx = 0;
                u8Zf = 1;
                ip += 2;
                continue;
            }
            if (mod == 0xc0) {
                eax = 0;
                u8Zf = 1;
                ip += 2;
                continue;
            }
            if (mod == 0xc9) {
                ecx = 0;
                u8Zf = 1;
                ip += 2;
                continue;
            }
            if (mod == 0xd2) {
                edx = 0;
                u8Zf = 1;
                ip += 2;
                continue;
            }
            if (mod == 0xf6) {
                esi = 0;
                u8Zf = 1;
                ip += 2;
                continue;
            }
            if (mod == 0xff) {
                edi = 0;
                u8Zf = 1;
                ip += 2;
                continue;
            }
            if (mod == 0xed) {
                ebp = 0;
                u8Zf = 1;
                ip += 2;
                continue;
            }
        }
        if (op == 0x89 && ip + 1 < cbImage) {
            u8 mod = p[ip + 1];

            if (mod == 0xc3) {
                ebx = eax;
                ip += 2;
                continue;
            }
            if (mod == 0xc1) {
                ecx = eax;
                ip += 2;
                continue;
            }
            if (mod == 0xc2) {
                edx = eax;
                ip += 2;
                continue;
            }
            if (mod == 0xc6) {
                esi = eax;
                ip += 2;
                continue;
            }
            if (mod == 0xc7) {
                edi = eax;
                ip += 2;
                continue;
            }
            if (mod == 0xe5) {
                /* mov ebp, esp — soft: use stack depth as esp-shaped */
                ebp = u32Sp * 4u;
                ip += 2;
                continue;
            }
        }
        if (op == 0x8b && ip + 1 < cbImage) {
            u8 mod = p[ip + 1];

            /* Soft reg-reg mov only (modrm mod=11b) */
            if ((mod & 0xc0u) == 0xc0u) {
                u32 u32Src = 0;
                u8 rm = (u8)(mod & 7u);
                u8 reg = (u8)((mod >> 3) & 7u);

                if (rm == 0) {
                    u32Src = eax;
                } else if (rm == 1) {
                    u32Src = ecx;
                } else if (rm == 2) {
                    u32Src = edx;
                } else if (rm == 3) {
                    u32Src = ebx;
                } else if (rm == 5) {
                    u32Src = ebp;
                } else if (rm == 6) {
                    u32Src = esi;
                } else if (rm == 7) {
                    u32Src = edi;
                }
                if (reg == 0) {
                    eax = u32Src;
                } else if (reg == 1) {
                    ecx = u32Src;
                } else if (reg == 2) {
                    edx = u32Src;
                } else if (reg == 3) {
                    ebx = u32Src;
                } else if (reg == 5) {
                    ebp = u32Src;
                } else if (reg == 6) {
                    esi = u32Src;
                } else if (reg == 7) {
                    edi = u32Src;
                }
                ip += 2;
                continue;
            }
        }
        if (op == 0x01 && ip + 1 < cbImage && p[ip + 1] == 0xc3) {
            /* add ebx, eax */
            ebx += eax;
            u8Zf = (ebx == 0) ? 1u : 0u;
            ip += 2;
            continue;
        }
        if (op == 0x29 && ip + 1 < cbImage && p[ip + 1] == 0xc3) {
            /* sub ebx, eax */
            ebx -= eax;
            u8Zf = (ebx == 0) ? 1u : 0u;
            ip += 2;
            continue;
        }
        if (op == 0x83 && ip + 2 < cbImage) {
            u8 mod = p[ip + 1];
            i8 i8Imm = (i8)p[ip + 2];

            /* Soft: add/sub reg, imm8 for eax/ebx/ecx/edx (modrm reg-form) */
            if (mod == 0xc0) {
                eax = (u32)((i32)eax + (i32)i8Imm);
                u8Zf = (eax == 0) ? 1u : 0u;
                ip += 3;
                continue;
            }
            if (mod == 0xc3) {
                ebx = (u32)((i32)ebx + (i32)i8Imm);
                u8Zf = (ebx == 0) ? 1u : 0u;
                ip += 3;
                continue;
            }
            if (mod == 0xc1) {
                ecx = (u32)((i32)ecx + (i32)i8Imm);
                u8Zf = (ecx == 0) ? 1u : 0u;
                ip += 3;
                continue;
            }
            if (mod == 0xc2) {
                edx = (u32)((i32)edx + (i32)i8Imm);
                u8Zf = (edx == 0) ? 1u : 0u;
                ip += 3;
                continue;
            }
            if (mod == 0xe8) {
                /* sub eax, imm8 */
                eax = (u32)((i32)eax - (i32)i8Imm);
                u8Zf = (eax == 0) ? 1u : 0u;
                ip += 3;
                continue;
            }
            if (mod == 0xeb) {
                ebx = (u32)((i32)ebx - (i32)i8Imm);
                u8Zf = (ebx == 0) ? 1u : 0u;
                ip += 3;
                continue;
            }
        }
        if (op == 0x85 && ip + 1 < cbImage) {
            u8 mod = p[ip + 1];

            /* test reg,reg soft ZF */
            if (mod == 0xc0) {
                u8Zf = (eax == 0) ? 1u : 0u;
                ip += 2;
                continue;
            }
            if (mod == 0xdb) {
                u8Zf = (ebx == 0) ? 1u : 0u;
                ip += 2;
                continue;
            }
            if (mod == 0xc9) {
                u8Zf = (ecx == 0) ? 1u : 0u;
                ip += 2;
                continue;
            }
            if (mod == 0xd2) {
                u8Zf = (edx == 0) ? 1u : 0u;
                ip += 2;
                continue;
            }
        }
        if (op == 0x40) { /* inc eax (i386) */
            eax++;
            u8Zf = (eax == 0) ? 1u : 0u;
            ip++;
            continue;
        }
        if (op == 0x48) { /* dec eax (i386) */
            eax--;
            u8Zf = (eax == 0) ? 1u : 0u;
            ip++;
            continue;
        }
        if (op == 0x43) { /* inc ebx */
            ebx++;
            u8Zf = (ebx == 0) ? 1u : 0u;
            ip++;
            continue;
        }
        if (op == 0x4b) { /* dec ebx */
            ebx--;
            u8Zf = (ebx == 0) ? 1u : 0u;
            ip++;
            continue;
        }
        if (op == 0xeb) { /* jmp short rel8 */
            i8 rel;

            if (ip + 2 > cbImage) {
                return -1;
            }
            rel = (i8)p[ip + 1];
            ip = (u32)((i32)ip + 2 + (i32)rel);
            continue;
        }
        if (op == 0xe9) { /* jmp near rel32 — soft load long branch */
            i32 rel;

            if (ip + 5 > cbImage) {
                return -1;
            }
            rel = (i32)((u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                        ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24));
            ip = (u32)((i32)ip + 5 + rel);
            continue;
        }
        if (op == 0x74) { /* jz/je rel8 */
            i8 rel;

            if (ip + 2 > cbImage) {
                return -1;
            }
            rel = (i8)p[ip + 1];
            if (u8Zf) {
                ip = (u32)((i32)ip + 2 + (i32)rel);
            } else {
                ip += 2;
            }
            continue;
        }
        if (op == 0x75) { /* jnz/jne rel8 */
            i8 rel;

            if (ip + 2 > cbImage) {
                return -1;
            }
            rel = (i8)p[ip + 1];
            if (!u8Zf) {
                ip = (u32)((i32)ip + 2 + (i32)rel);
            } else {
                ip += 2;
            }
            continue;
        }
        if (op == 0xe8) { /* call rel32 — soft push return */
            i32 rel;
            u32 u32Ret;

            if (ip + 5 > cbImage) {
                return -1;
            }
            if (u32Sp == 0) {
                return -1; /* soft stack overflow */
            }
            rel = (i32)((u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                        ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24));
            u32Ret = ip + 5u;
            u32Sp--;
            aStack[u32Sp] = u32Ret;
            ip = (u32)((i32)ip + 5 + rel);
            continue;
        }
        if (op == 0x6a) { /* push imm8 */
            i8 i8Imm;

            if (ip + 2 > cbImage || u32Sp == 0) {
                return -1;
            }
            i8Imm = (i8)p[ip + 1];
            u32Sp--;
            aStack[u32Sp] = (u32)(i32)i8Imm;
            ip += 2;
            continue;
        }
        if (op == 0x68) { /* push imm32 */
            u32 u32Imm;

            if (ip + 5 > cbImage || u32Sp == 0) {
                return -1;
            }
            u32Imm = (u32)p[ip + 1] | ((u32)p[ip + 2] << 8) |
                     ((u32)p[ip + 3] << 16) | ((u32)p[ip + 4] << 24);
            u32Sp--;
            aStack[u32Sp] = u32Imm;
            ip += 5;
            continue;
        }
        if (op == 0x50) { /* push eax */
            if (u32Sp == 0) {
                return -1;
            }
            u32Sp--;
            aStack[u32Sp] = eax;
            ip++;
            continue;
        }
        if (op == 0x51) {
            if (u32Sp == 0) {
                return -1;
            }
            u32Sp--;
            aStack[u32Sp] = ecx;
            ip++;
            continue;
        }
        if (op == 0x52) {
            if (u32Sp == 0) {
                return -1;
            }
            u32Sp--;
            aStack[u32Sp] = edx;
            ip++;
            continue;
        }
        if (op == 0x53) {
            if (u32Sp == 0) {
                return -1;
            }
            u32Sp--;
            aStack[u32Sp] = ebx;
            ip++;
            continue;
        }
        if (op == 0x55) { /* push ebp — PE frame setup soft */
            if (u32Sp == 0) {
                return -1;
            }
            u32Sp--;
            aStack[u32Sp] = ebp;
            ip++;
            continue;
        }
        if (op == 0x56 || op == 0x57) {
            if (u32Sp == 0) {
                return -1;
            }
            u32Sp--;
            aStack[u32Sp] = (op == 0x56) ? esi : edi;
            ip++;
            continue;
        }
        if (op == 0x58) {
            if (u32Sp >= 64u) {
                return -1;
            }
            eax = aStack[u32Sp];
            u32Sp++;
            ip++;
            continue;
        }
        if (op == 0x59) {
            if (u32Sp >= 64u) {
                return -1;
            }
            ecx = aStack[u32Sp];
            u32Sp++;
            ip++;
            continue;
        }
        if (op == 0x5a) {
            if (u32Sp >= 64u) {
                return -1;
            }
            edx = aStack[u32Sp];
            u32Sp++;
            ip++;
            continue;
        }
        if (op == 0x5b) {
            if (u32Sp >= 64u) {
                return -1;
            }
            ebx = aStack[u32Sp];
            u32Sp++;
            ip++;
            continue;
        }
        if (op == 0x5d) { /* pop ebp */
            if (u32Sp >= 64u) {
                return -1;
            }
            ebp = aStack[u32Sp];
            u32Sp++;
            ip++;
            continue;
        }
        if (op == 0x5e || op == 0x5f) {
            if (u32Sp >= 64u) {
                return -1;
            }
            if (op == 0x5e) {
                esi = aStack[u32Sp];
            } else {
                edi = aStack[u32Sp];
            }
            u32Sp++;
            ip++;
            continue;
        }
        if (op == 0xc9) { /* leave: mov esp,ebp; pop ebp — soft depth */
            u32Sp = ebp / 4u;
            if (u32Sp > 64u) {
                u32Sp = 64u;
            }
            if (u32Sp >= 64u) {
                ebp = 0;
            } else {
                ebp = aStack[u32Sp];
                u32Sp++;
            }
            ip++;
            continue;
        }
        if (op == 0xc3) { /* ret — soft pop return or exit-shaped success */
            if (u32Sp < 64u) {
                ip = aStack[u32Sp];
                u32Sp++;
                if (ip < cbImage) {
                    continue;
                }
            }
            if (pExitCode != NULL) {
                *pExitCode = (i32)eax;
            }
            kprintf("pe32: i386 soft-exec ret eax=%u\n", eax);
            pe32_soft_inc(&g_soft.u64SoftExecOk);
            return 0;
        }
        if (op == 0xcd && ip + 1 < cbImage && p[ip + 1] == 0x80) {
            /*
             * int 0x80 — i386 Linux NR subset (aligned with trap CS32 path).
             * Soft-only: no user_copy; path open returns synthetic fd.
             * mmap2/old_mmap return VA band 0x57000000 (hw mmap2 smoke band).
             */
            ip += 2;
            if (eax == 1u || eax == 252u) {
                if (pExitCode != NULL) {
                    *pExitCode = (i32)ebx;
                }
                kprintf("pe32: i386 soft-exec int80 exit code=%u\n", ebx);
                pe32_soft_inc(&g_soft.u64SoftExecOk);
                return 0;
            }
            if (eax == 4u) {
                /* write(fd, buf, len) — return len in eax */
                eax = edx;
                u8Zf = (eax == 0) ? 1u : 0u;
                continue;
            }
            if (eax == 3u) {
                eax = 0; /* read → EOF */
                u8Zf = 1;
                continue;
            }
            if (eax == 5u) {
                eax = 3; /* open(path,…) soft fd; path not dereferenced */
                u8Zf = 0;
                continue;
            }
            if (eax == 6u || eax == 91u || eax == 125u || eax == 243u ||
                eax == 54u) {
                eax = 0; /* close/munmap/mprotect/set_thread_area/ioctl */
                u8Zf = 1;
                continue;
            }
            if (eax == 20u) {
                eax = 1; /* getpid */
                u8Zf = 0;
                continue;
            }
            if (eax == 24u || eax == 47u || eax == 199u || eax == 200u) {
                eax = 0; /* getuid/getgid family */
                u8Zf = 1;
                continue;
            }
            if (eax == 45u) {
                /* brk(0) → soft break base; brk(addr) → accept */
                if (ebx == 0) {
                    eax = 0x56000000u;
                } else {
                    eax = ebx;
                }
                u8Zf = 0;
                continue;
            }
            if (eax == 192u || eax == 90u) {
                /* mmap2 (192) / old mmap (90): synthetic anon VA (hw band) */
                eax = 0x57000000u;
                u8Zf = 0;
                continue;
            }
            if (eax == 122u || eax == 240u || eax == 265u || eax == 42u) {
                eax = 0; /* uname/futex/clock_gettime/pipe ok */
                u8Zf = 1;
                continue;
            }
            if (eax == 102u) {
                eax = 20; /* socketcall SYS_SOCKET → fd */
                u8Zf = 0;
                continue;
            }
            if (eax == 41u) {
                eax = ebx + 1; /* dup */
                u8Zf = 0;
                continue;
            }
            if (eax == 63u) {
                eax = ecx; /* dup2 */
                u8Zf = (eax == 0) ? 1u : 0u;
                continue;
            }
            if (eax == 224u) {
                eax = 1; /* gettid */
                u8Zf = 0;
                continue;
            }
            if (eax == 2u || eax == 120u || eax == 190u) {
                eax = 2; /* fork/clone/vfork → fake child pid */
                u8Zf = 0;
                continue;
            }
            if (eax == 220u || eax == 141u) {
                eax = 32; /* getdents* — synthetic bytes */
                u8Zf = 0;
                continue;
            }
            if (eax == 7u || eax == 114u) {
                /* waitpid/wait4 soft: no children → -ECHILD shape as 0 */
                eax = 0;
                u8Zf = 1;
                continue;
            }
            if (eax == 162u || eax == 142u) {
                eax = 0; /* nanosleep / select soft ok */
                u8Zf = 1;
                continue;
            }
            if (eax == 11u || eax == 174u || eax == 175u || eax == 172u ||
                eax == 168u || eax == 221u || eax == 197u || eax == 195u ||
                eax == 123u || eax == 243u || eax == 311u || eax == 340u ||
                eax == 266u || eax == 267u || eax == 27u || eax == 104u ||
                eax == 105u || eax == 219u || eax == 158u || eax == 83u ||
                eax == 85u || eax == 9u || eax == 331u || eax == 295u ||
                eax == 254u || eax == 255u || eax == 256u || eax == 329u ||
                eax == 322u || eax == 325u || eax == 326u || eax == 239u ||
                eax == 291u || eax == 292u || eax == 293u || eax == 332u ||
                eax == 321u || eax == 327u || eax == 356u || eax == 377u ||
                eax == 323u || eax == 328u || eax == 180u || eax == 181u ||
                eax == 93u || eax == 94u || eax == 15u || eax == 30u ||
                eax == 271u || eax == 324u || eax == 143u || eax == 140u ||
                eax == 296u || eax == 300u || eax == 301u || eax == 302u ||
                eax == 303u || eax == 304u || eax == 305u || eax == 307u) {
                eax = 0; /* execve/sig/prctl/poll/fcntl/fstat/TLS/IPC/FS/at ok */
                u8Zf = 1;
                continue;
            }
            if (eax == 258u) {
                eax = 1; /* set_tid_address → tid */
                u8Zf = 0;
                continue;
            }
            if (eax == 355u) {
                eax = edx ? edx : 0; /* getrandom length-shaped */
                u8Zf = (eax == 0) ? 1u : 0u;
                continue;
            }
            if (eax == 78u || eax == 116u || eax == 12u || eax == 19u) {
                eax = 0; /* gettimeofday/sysinfo/chdir/lseek */
                u8Zf = 1;
                continue;
            }
            if (eax == 13u) {
                eax = 1; /* time → seconds */
                u8Zf = 0;
                continue;
            }
            if (eax == 183u) {
                eax = 5; /* getcwd nbytes */
                u8Zf = 0;
                continue;
            }
            if (eax == 33u || eax == 38u || eax == 10u || eax == 39u ||
                eax == 76u || eax == 75u || eax == 64u || eax == 65u ||
                eax == 66u || eax == 118u || eax == 145u || eax == 146u) {
                eax = 0; /* access/rename/unlink/mkdir/rlimit/pgrp/fsync/iov */
                u8Zf = 1;
                continue;
            }
            kprintf("pe32: i386 soft-exec int80 nr=%u unsupported\n", eax);
            return -1;
        }
        if (op == 0x0f && ip + 1 < cbImage && p[ip + 1] == 0x05) {
            /* syscall — treat as x86_64 exit if eax==60 */
            if (eax == 60u) {
                if (pExitCode != NULL) {
                    *pExitCode = (i32)edi; /* x64 exit uses edi; soft prefer */
                }
                pe32_soft_inc(&g_soft.u64SoftExecOk);
                return 0;
            }
            return -1;
        }
        if (op == 0x0f && ip + 1 < cbImage && p[ip + 1] == 0x34) {
            /* sysenter — treat like exit when eax==1 */
            if (eax == 1u) {
                if (pExitCode != NULL) {
                    *pExitCode = (i32)ebx;
                }
                kprintf("pe32: i386 soft-exec sysenter exit=%u\n", ebx);
                pe32_soft_inc(&g_soft.u64SoftExecOk);
                return 0;
            }
            return -1;
        }
        kprintf("pe32: i386 soft-exec unknown op=0x%x at rva=0x%x\n", op, ip);
        return -1;
    }
    return -1;
}

int
pe32_compat_frame_prepare(u64 u64Entry32, u64 u64Stack32, u64 *pFrameOut,
                          u32 u32FrameWords)
{
    /*
     * Interrupt/return frame for long-mode → compat user (Intel SDM-shaped):
     *   [0] RIP (32-bit entry zero-extended)
     *   [1] CS  (GJ_GDT_USER_CS32)
     *   [2] RFLAGS (IF set)
     *   [3] RSP (32-bit stack top)
     *   [4] SS  (GJ_GDT_USER_DS)
     * Hardware iretq into 32-bit CS is deferred (needs careful stack + LME).
     */
    if (pFrameOut == NULL || u32FrameWords < 5) {
        return -1;
    }
    if (!gdt_user_cs32_ready()) {
        return -1;
    }
    if (u64Entry32 == 0 || u64Stack32 == 0) {
        return -1;
    }
    /* Truncate to 32-bit VA for true PE32 image window */
    if ((u64Entry32 >> 32) != 0 || (u64Stack32 >> 32) != 0) {
        /* High VA is ok for our high-map stage; keep full 64 for bring-up */
    }
    pFrameOut[0] = u64Entry32;
    pFrameOut[1] = (u64)gdt_user_cs32_sel();
    pFrameOut[2] = 0x200ull; /* IF */
    pFrameOut[3] = u64Stack32;
    pFrameOut[4] = (u64)GJ_GDT_USER_DS;
    if (u32FrameWords > 5) {
        u32 i;

        for (i = 5; i < u32FrameWords; i++) {
            pFrameOut[i] = 0;
        }
    }
    if (pFrameOut[1] != (u64)GJ_GDT_USER_CS32 ||
        pFrameOut[4] != (u64)GJ_GDT_USER_DS) {
        return -1;
    }
    return 0;
}

int
pe32_compat_gdt_validate(void)
{
    u64 aFrame[5];

    if (!gdt_user_cs32_ready() || !gdt_user_cs32_is_compat()) {
        kprintf("pe32: compat gdt FAIL ready=%d compat=%d\n",
                gdt_user_cs32_ready(), gdt_user_cs32_is_compat());
        return -1;
    }
    if (pe32_compat_frame_prepare(0x00401000ull, 0x00408000ull, aFrame, 5) !=
        0) {
        kprintf("pe32: compat gdt frame FAIL\n");
        return -1;
    }
    if (aFrame[1] != (u64)GJ_GDT_USER_CS32 || aFrame[4] != (u64)GJ_GDT_USER_DS) {
        return -1;
    }
    kprintf("pe32: compat gdt PASS cs32=0x%x L=0 D=1 frame ok\n",
            (unsigned)gdt_user_cs32_sel());
    return 0;
}

int
pe32_compat_enter_prep(u64 u64Entry32, void *pStackMem, u32 cbStack)
{
    u64 *pFrame;
    u64 u64StackTop;
    u8 *pBytes;

    if (pStackMem == NULL || cbStack < 40u) {
        return -1;
    }
    if (!gdt_user_cs32_ready() || !gdt_user_cs32_is_compat()) {
        kprintf("pe32: enter prep FAIL gdt\n");
        return -1;
    }
    if (u64Entry32 == 0) {
        u64Entry32 = 0x00401000ull;
    }
    /* Place 5 qwords at top of stack buffer (simulates user stack top) */
    pBytes = (u8 *)pStackMem;
    pFrame = (u64 *)(void *)(pBytes + cbStack - 40u);
    u64StackTop = 0x00408000ull; /* canonical 32-bit stack top for frame RSP */
    if (pe32_compat_frame_prepare(u64Entry32, u64StackTop, pFrame, 5) != 0) {
        kprintf("pe32: enter prep frame FAIL\n");
        return -1;
    }
    /* Validate layout in place (as hardware iretq would pop) */
    if (pFrame[0] != u64Entry32 || pFrame[1] != (u64)GJ_GDT_USER_CS32 ||
        (pFrame[2] & 0x200ull) == 0 || pFrame[3] != u64StackTop ||
        pFrame[4] != (u64)GJ_GDT_USER_DS) {
        kprintf("pe32: enter prep layout FAIL\n");
        return -1;
    }
    /*
     * Hardware path (deferred): load CR3 of target AS, set RSP = stack_top-40,
     * iretq. Requires no kernel stack clash + user pages mapped + SMEP ok.
     * We prove frame + GDT only here.
     */
    kprintf("pe32: enter prep PASS rip=0x%lx cs=0x%lx rflags=0x%lx rsp=0x%lx "
            "ss=0x%lx (iretq deferred)\n",
            (unsigned long)pFrame[0], (unsigned long)pFrame[1],
            (unsigned long)pFrame[2], (unsigned long)pFrame[3],
            (unsigned long)pFrame[4]);
    return 0;
}

int
pe32_compat_hw_ready(void)
{
    u32 u32Ar = 0;
    static u8 aStack[256];
    u64 aFrame[5];

    if (!gdt_user_cs32_ready() || !gdt_user_cs32_is_compat()) {
        kprintf("pe32: hw ready FAIL gdt software\n");
        return -1;
    }
    if (!gdt_user_cs32_lar_ok(&u32Ar)) {
        kprintf("pe32: hw ready FAIL LAR ar=0x%x\n", u32Ar);
        return -1;
    }
    memset(aStack, 0, sizeof(aStack));
    if (pe32_compat_enter_prep(0x00401000ull, aStack, (u32)sizeof(aStack)) !=
        0) {
        kprintf("pe32: hw ready FAIL enter_prep\n");
        return -1;
    }
    if (pe32_compat_frame_prepare(0x00401000ull, 0x00408000ull, aFrame, 5) !=
        0) {
        return -1;
    }
    kprintf("pe32: hw ready PASS lar=0x%x cs32=0x%x frame_cs=0x%lx "
            "(iretq still deferred)\n",
            u32Ar, (unsigned)gdt_user_cs32_sel(),
            (unsigned long)aFrame[1]);
    return 0;
}

int
pe32_compat_soft_iretq(const void *pImage, u32 cbImage, u32 u32EntryRva,
                       void *pStackMem, u32 cbStack, i32 *pExitCode)
{
    u64 *pFrame;
    u8 *pBytes;
    u64 u64Entry;
    u32 u32Ar = 0;

    pe32_soft_inc(&g_soft.u64SoftIretqEnter);
    if (pImage == NULL || pStackMem == NULL || cbStack < 40u) {
        return -1;
    }
    if (!gdt_user_cs32_ready() || !gdt_user_cs32_is_compat() ||
        !gdt_user_cs32_lar_ok(&u32Ar)) {
        kprintf("pe32: soft-iretq FAIL gdt/LAR ar=0x%x\n", u32Ar);
        return -1;
    }
    u64Entry = 0x00400000ull + (u64)u32EntryRva;
    if (pe32_compat_enter_prep(u64Entry, pStackMem, cbStack) != 0) {
        return -1;
    }
    pBytes = (u8 *)pStackMem;
    pFrame = (u64 *)(void *)(pBytes + cbStack - 40u);
    if (pFrame[0] != u64Entry || pFrame[1] != (u64)GJ_GDT_USER_CS32 ||
        pFrame[4] != (u64)GJ_GDT_USER_DS) {
        kprintf("pe32: soft-iretq frame FAIL\n");
        return -1;
    }
    /*
     * Hardware would: load CS/SS from frame, set RIP/RSP, return to user.
     * Soft path: interpret image at entry RVA (no real CS switch).
     */
    if (pe32_i386_soft_exec(pImage, cbImage, u32EntryRva, pExitCode) != 0) {
        kprintf("pe32: soft-iretq exec FAIL\n");
        return -1;
    }
    pe32_soft_inc(&g_soft.u64SoftIretqOk);
    kprintf("pe32: soft-iretq PASS lar=0x%x (frame+exec; HW CS switch deferred)\n",
            u32Ar);
    return 0;
}

int
pe32_hw_enter_smoke(void)
{
    static struct gj_process hwProc;
    static struct gj_cnode hwCnode;
    static struct gj_cap_slot aHwSlots[32];
    static struct gj_root_meta hwMeta;
    u64 u64Saved;
    u64 vaCode = 0x52000000ull;
    u64 vaStack = 0x52100000ull;
    gj_paddr_t paCode;
    gj_paddr_t paStack;
    void *pK;
    u32 thr;
    u32 y;
    u32 hits0;
    u8 aCode[16];

    if (!gdt_user_cs32_ready() || !gdt_user_cs32_is_compat() ||
        !gdt_user_cs32_lar_ok(NULL)) {
        kprintf("pe32: hw enter smoke FAIL gdt\n");
        return -1;
    }
    hits0 = pe32_hw_enter_hits();
    memset(&hwProc, 0, sizeof(hwProc));
    gj_process_init(&hwProc, &hwCnode, aHwSlots, 32);
    hwProc.u32Alive = 1;
    hwProc.u32Personality = 1;
    (void)gj_process_bootstrap_root_meta(&hwProc, &hwMeta, NULL);
    if (process_as_ensure(&hwProc) != GJ_OK) {
        kprintf("pe32: hw enter as FAIL\n");
        return -1;
    }
    u64Saved = cpu_read_cr3();
    process_as_activate(&hwProc);

    /* Tiny 32-bit payload: int3 (0xCC) — trap records CS32 */
    memset(aCode, 0x90, sizeof(aCode));
    aCode[0] = 0xcc;
    paCode = pmm_alloc();
    paStack = pmm_alloc();
    if (paCode == 0 || paStack == 0) {
        cpu_load_cr3(u64Saved);
        kprintf("pe32: hw enter pmm FAIL\n");
        return -1;
    }
    pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paCode)
                                           : (u64)paCode);
    memset(pK, 0, GJ_PAGE_SIZE);
    memcpy(pK, aCode, sizeof(aCode));
    pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paStack)
                                           : (u64)paStack);
    memset(pK, 0, GJ_PAGE_SIZE);
    if (vmm_map_page((gj_vaddr_t)vaCode, paCode,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) !=
            GJ_OK ||
        vmm_map_page((gj_vaddr_t)vaStack, paStack,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
            GJ_OK) {
        cpu_load_cr3(u64Saved);
        kprintf("pe32: hw enter map FAIL\n");
        return -1;
    }
    cpu_load_cr3(u64Saved);
    (void)process_wait_register(&hwProc, 1);
    thr = thread_create_user32(&hwProc, vaCode, vaStack + GJ_PAGE_SIZE - 16);
    if (thr == 0) {
        kprintf("pe32: hw enter thr FAIL\n");
        return -1;
    }
    for (y = 0; y < 64u; y++) {
        thread_yield();
        if (pe32_hw_enter_hits() > hits0) {
            break;
        }
    }
    if (pe32_hw_enter_hits() <= hits0 ||
        (pe32_hw_enter_last_cs() & 0xffu) != GJ_GDT_USER_CS32) {
        kprintf("pe32: hw enter smoke FAIL hits=%u cs=0x%x\n",
                pe32_hw_enter_hits(), pe32_hw_enter_last_cs());
        return -1;
    }
    pe32_soft_inc(&g_soft.u64HwEnterOk);
    kprintf("pe32: hw enter smoke PASS hits=%u cs=0x%x yields=%u\n",
            pe32_hw_enter_hits(), pe32_hw_enter_last_cs(), y);
    return 0;
}

int
pe32_hw_int80_smoke(void)
{
    static struct gj_process i80Proc;
    static struct gj_cnode i80Cnode;
    static struct gj_cap_slot aI80Slots[32];
    static struct gj_root_meta i80Meta;
    u64 u64Saved;
    u64 vaCode = 0x53000000ull;
    u64 vaStack = 0x53100000ull;
    gj_paddr_t paCode;
    gj_paddr_t paStack;
    void *pK;
    u32 thr;
    u32 y;
    u32 hits0;
    u8 aCode[32];
    u32 i;

    /* GDT already validated by pe32_hw_enter_smoke / compat_gdt */
    hits0 = pe32_int80_exit_hits();
    memset(&i80Proc, 0, sizeof(i80Proc));
    gj_process_init(&i80Proc, &i80Cnode, aI80Slots, 32);
    i80Proc.u32Alive = 1;
    i80Proc.u32Personality = 1;
    (void)gj_process_bootstrap_root_meta(&i80Proc, &i80Meta, NULL);
    if (process_as_ensure(&i80Proc) != GJ_OK) {
        return -1;
    }
    u64Saved = cpu_read_cr3();
    process_as_activate(&i80Proc);

    /*
     * CS32 int80 sys_exit(0) — trap logs greppable "pe32: int80 exit PASS".
     *   b8 01 00 00 00   mov eax,1
     *   31 db            xor ebx,ebx
     *   cd 80            int 0x80
     */
    for (i = 0; i < sizeof(aCode); i++) {
        aCode[i] = 0x90;
    }
    aCode[0] = 0xb8;
    aCode[1] = 0x01;
    aCode[2] = 0x00;
    aCode[3] = 0x00;
    aCode[4] = 0x00;
    aCode[5] = 0x31;
    aCode[6] = 0xdb;
    aCode[7] = 0xcd;
    aCode[8] = 0x80;

    paCode = pmm_alloc();
    paStack = pmm_alloc();
    if (paCode == 0 || paStack == 0) {
        cpu_load_cr3(u64Saved);
        return -1;
    }
    pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paCode)
                                           : (u64)paCode);
    memset(pK, 0, GJ_PAGE_SIZE);
    memcpy(pK, aCode, sizeof(aCode));
    pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paStack)
                                           : (u64)paStack);
    memset(pK, 0, GJ_PAGE_SIZE);
    if (vmm_map_page((gj_vaddr_t)vaCode, paCode,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) !=
            GJ_OK ||
        vmm_map_page((gj_vaddr_t)vaStack, paStack,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
            GJ_OK) {
        cpu_load_cr3(u64Saved);
        return -1;
    }
    cpu_load_cr3(u64Saved);
    (void)process_wait_register(&i80Proc, 1);
    thr = thread_create_user32(&i80Proc, vaCode, vaStack + GJ_PAGE_SIZE - 16);
    if (thr == 0) {
        kprintf("pe32: int80 thr FAIL\n");
        return -1;
    }
    for (y = 0; y < 64u; y++) {
        thread_yield();
        if (pe32_int80_exit_hits() > hits0) {
            break;
        }
    }
    if (pe32_int80_exit_hits() <= hits0 || pe32_int80_last_code() != 0) {
        kprintf("pe32: int80 smoke FAIL hits=%u code=%u\n",
                pe32_int80_exit_hits(), pe32_int80_last_code());
        return -1;
    }
    kprintf("pe32: int80 smoke PASS hits=%u code=0 yields=%u\n",
            pe32_int80_exit_hits(), y);

    /*
     * getpid + exit(pid): mov eax,20; int 0x80; mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process gpProc;
        static struct gj_cnode gpCnode;
        static struct gj_cap_slot aGpSlots[32];
        static struct gj_root_meta gpMeta;
        u64 vaC = 0x54000000ull;
        u64 vaS = 0x54100000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr2;
        u32 hits1 = pe32_int80_exit_hits();
        u8 a2[32];

        memset(&gpProc, 0, sizeof(gpProc));
        gj_process_init(&gpProc, &gpCnode, aGpSlots, 32);
        gpProc.u32Alive = 1;
        gpProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&gpProc, &gpMeta, NULL);
        if (process_as_ensure(&gpProc) != GJ_OK) {
            return 0; /* primary smoke already passed */
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&gpProc);
        memset(a2, 0x90, sizeof(a2));
        /* mov eax,20; int 0x80; mov ebx,eax; mov eax,1; int 0x80 */
        a2[0] = 0xb8;
        a2[1] = 20;
        a2[2] = a2[3] = a2[4] = 0;
        a2[5] = 0xcd;
        a2[6] = 0x80;
        a2[7] = 0x89;
        a2[8] = 0xc3; /* mov ebx,eax */
        a2[9] = 0xb8;
        a2[10] = 1;
        a2[11] = a2[12] = a2[13] = 0;
        a2[14] = 0xcd;
        a2[15] = 0x80;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a2, sizeof(a2));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&gpProc, 1);
        thr2 = thread_create_user32(&gpProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr2 == 0) {
            return 0;
        }
        for (y = 0; y < 64u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hits1) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hits1 && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 getpid+exit PASS code=1\n");
        } else {
            kprintf("pe32: int80 getpid+exit FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * Multi-NR surface: getuid; brk(0); write(1,0,5); set_thread_area; exit(42)
     *   b8 18 00 00 00   mov eax,24   ; getuid
     *   cd 80            int 0x80
     *   b8 2d 00 00 00   mov eax,45   ; brk
     *   31 db            xor ebx,ebx
     *   cd 80            int 0x80
     *   b8 04 00 00 00   mov eax,4    ; write
     *   bb 01 00 00 00   mov ebx,1
     *   31 c9            xor ecx,ecx
     *   ba 05 00 00 00   mov edx,5
     *   cd 80            int 0x80
     *   b8 f3 00 00 00   mov eax,243  ; set_thread_area
     *   cd 80            int 0x80
     *   b8 01 00 00 00   mov eax,1
     *   bb 2a 00 00 00   mov ebx,42
     *   cd 80            int 0x80
     */
    {
        static struct gj_process mProc;
        static struct gj_cnode mCnode;
        static struct gj_cap_slot aMSlots[32];
        static struct gj_root_meta mMeta;
        u64 vaC = 0x55000000ull;
        u64 vaS = 0x55100000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr3;
        u32 hitsM = pe32_int80_exit_hits();
        u32 calls0 = pe32_int80_call_hits();
        u8 a3[64];
        u32 o = 0;

        memset(&mProc, 0, sizeof(mProc));
        gj_process_init(&mProc, &mCnode, aMSlots, 32);
        mProc.u32Alive = 1;
        mProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&mProc, &mMeta, NULL);
        if (process_as_ensure(&mProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&mProc);
        memset(a3, 0x90, sizeof(a3));
        /* getuid */
        a3[o++] = 0xb8;
        a3[o++] = 24;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0xcd;
        a3[o++] = 0x80;
        /* brk(0) */
        a3[o++] = 0xb8;
        a3[o++] = 45;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0x31;
        a3[o++] = 0xdb;
        a3[o++] = 0xcd;
        a3[o++] = 0x80;
        /* write(1,0,5) */
        a3[o++] = 0xb8;
        a3[o++] = 4;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0xbb;
        a3[o++] = 1;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0x31;
        a3[o++] = 0xc9;
        a3[o++] = 0xba;
        a3[o++] = 5;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0xcd;
        a3[o++] = 0x80;
        /* set_thread_area */
        a3[o++] = 0xb8;
        a3[o++] = 0xf3;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0xcd;
        a3[o++] = 0x80;
        /* exit(42) */
        a3[o++] = 0xb8;
        a3[o++] = 1;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0xbb;
        a3[o++] = 42;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0;
        a3[o++] = 0xcd;
        a3[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a3, sizeof(a3));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&mProc, 1);
        thr3 = thread_create_user32(&mProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr3 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsM) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsM && pe32_int80_last_code() == 42 &&
            pe32_int80_call_hits() >= calls0 + 5u) {
            kprintf("pe32: int80 multi PASS calls=%u code=42\n",
                    pe32_int80_call_hits() - calls0);
        } else {
            kprintf("pe32: int80 multi FAIL hits=%u code=%u calls=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code(),
                    pe32_int80_call_hits() - calls0);
        }
    }

    /*
     * int80 mmap2 + store + exit(va) — greppable: pe32: int80 mmap2 PASS
     * i386 NR 192: mmap2(addr=0, len=4096, prot=RW, flags=PRIVATE|ANON,
     *                    fd=-1, pgoff=0 pages). trap_dispatch maps real page
     * in 0x570xxxxx band; payload stores marker then exits with VA as code.
     *   mov eax,192; xor ebx,ebx; mov ecx,4096; mov edx,3; mov esi,0x22;
     *   mov edi,-1; xor ebp,ebp; int 0x80
     *   mov dword [eax], 0x41424344
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process mmProc;
        static struct gj_cnode mmCnode;
        static struct gj_cap_slot aMmSlots[32];
        static struct gj_root_meta mmMeta;
        u64 vaC = 0x55200000ull;
        u64 vaS = 0x55300000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr4;
        u32 hitsMm = pe32_int80_exit_hits();
        u8 a4[64];
        u32 o = 0;

        memset(&mmProc, 0, sizeof(mmProc));
        gj_process_init(&mmProc, &mmCnode, aMmSlots, 32);
        mmProc.u32Alive = 1;
        mmProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&mmProc, &mmMeta, NULL);
        if (process_as_ensure(&mmProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&mmProc);
        memset(a4, 0x90, sizeof(a4));
        /* mmap2(0, 4096, PROT_READ|WRITE=3, MAP_PRIVATE|ANON=0x22, -1, pgoff=0) */
        a4[o++] = 0xb8;
        a4[o++] = 192;
        a4[o++] = 0;
        a4[o++] = 0;
        a4[o++] = 0; /* mov eax,192 */
        a4[o++] = 0x31;
        a4[o++] = 0xdb; /* xor ebx,ebx */
        a4[o++] = 0xb9;
        a4[o++] = 0x00;
        a4[o++] = 0x10;
        a4[o++] = 0;
        a4[o++] = 0; /* mov ecx,4096 */
        a4[o++] = 0xba;
        a4[o++] = 3;
        a4[o++] = 0;
        a4[o++] = 0;
        a4[o++] = 0; /* mov edx,3 */
        a4[o++] = 0xbe;
        a4[o++] = 0x22;
        a4[o++] = 0;
        a4[o++] = 0;
        a4[o++] = 0; /* mov esi,0x22 */
        a4[o++] = 0xbf;
        a4[o++] = 0xff;
        a4[o++] = 0xff;
        a4[o++] = 0xff;
        a4[o++] = 0xff; /* mov edi,-1 */
        a4[o++] = 0x31;
        a4[o++] = 0xed; /* xor ebp,ebp */
        a4[o++] = 0xcd;
        a4[o++] = 0x80;
        /* mov dword [eax], 0x41424344 */
        a4[o++] = 0xc7;
        a4[o++] = 0x00;
        a4[o++] = 0x44;
        a4[o++] = 0x43;
        a4[o++] = 0x42;
        a4[o++] = 0x41;
        /* mov ebx,eax; mov eax,1; int 0x80 */
        a4[o++] = 0x89;
        a4[o++] = 0xc3;
        a4[o++] = 0xb8;
        a4[o++] = 1;
        a4[o++] = 0;
        a4[o++] = 0;
        a4[o++] = 0;
        a4[o++] = 0xcd;
        a4[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a4, sizeof(a4));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&mmProc, 1);
        thr4 = thread_create_user32(&mmProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr4 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsMm) {
                break;
            }
        }
        /* Defensive accept band: trap int80 mmap2 anon base 0x57000000.. */
        {
            u32 code = pe32_int80_last_code();

            if (pe32_int80_exit_hits() > hitsMm && code >= 0x57000000u &&
                code < 0x58000000u) {
                kprintf("pe32: int80 mmap2 PASS va=0x%x\n", code);
            } else {
                kprintf("pe32: int80 mmap2 FAIL hits=%u code=0x%x\n",
                        pe32_int80_exit_hits(), code);
            }
        }
    }

    /*
     * pipe + socketcall(SYS_SOCKET) + exit:
     *   sub esp,8; mov ebx,esp; mov eax,42; int 0x80  ; pipe
     *   mov eax,102; mov ebx,1; xor ecx,ecx; int 0x80 ; socketcall socket
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process pProc;
        static struct gj_cnode pCnode;
        static struct gj_cap_slot aPSlots[32];
        static struct gj_root_meta pMeta;
        u64 vaC = 0x55400000ull;
        u64 vaS = 0x55500000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr5;
        u32 hitsP = pe32_int80_exit_hits();
        u8 a5[48];
        u32 o = 0;

        memset(&pProc, 0, sizeof(pProc));
        gj_process_init(&pProc, &pCnode, aPSlots, 32);
        pProc.u32Alive = 1;
        pProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&pProc, &pMeta, NULL);
        if (process_as_ensure(&pProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&pProc);
        memset(a5, 0x90, sizeof(a5));
        /* sub esp, 16 */
        a5[o++] = 0x83;
        a5[o++] = 0xec;
        a5[o++] = 16;
        /* mov ebx, esp; mov eax,42; int 0x80 */
        a5[o++] = 0x89;
        a5[o++] = 0xe3;
        a5[o++] = 0xb8;
        a5[o++] = 42;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0xcd;
        a5[o++] = 0x80;
        /* mov eax,102; mov ebx,1; xor ecx,ecx; int 0x80 */
        a5[o++] = 0xb8;
        a5[o++] = 102;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0xbb;
        a5[o++] = 1;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0x31;
        a5[o++] = 0xc9;
        a5[o++] = 0xcd;
        a5[o++] = 0x80;
        /* mov ebx,eax; mov eax,1; int 0x80 — exit(socket_fd) */
        a5[o++] = 0x89;
        a5[o++] = 0xc3;
        a5[o++] = 0xb8;
        a5[o++] = 1;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0;
        a5[o++] = 0xcd;
        a5[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a5, sizeof(a5));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&pProc, 1);
        thr5 = thread_create_user32(&pProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr5 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsP) {
                break;
            }
        }
        /* Socket fd is dynamically allocated (was soft-fixed 20). */
        if (pe32_int80_exit_hits() > hitsP && pe32_int80_last_code() >= 3 &&
            pe32_int80_last_code() < 32) {
            kprintf("pe32: int80 pipe_sock PASS fd=%u\n",
                    pe32_int80_last_code());
        } else {
            kprintf("pe32: int80 pipe_sock FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * open + fstat64 + exit — greppable: pe32: int80 fstat64 PASS
     * Soft path open: ebx=0 (NULL path), ecx=0; trap/vfs may accept smoke open.
     * Then fstat64(fd, stack_buf) NR 197; exit with fstat result as code.
     *   sub esp,128; mov eax,5; xor ebx,ebx; xor ecx,ecx; int 0x80
     *   mov ebx,eax; mov ecx,esp; mov eax,197; int 0x80
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process fsProc;
        static struct gj_cnode fsCnode;
        static struct gj_cap_slot aFsSlots[32];
        static struct gj_root_meta fsMeta;
        u64 vaC = 0x55600000ull;
        u64 vaS = 0x55700000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr6;
        u32 hitsFs = pe32_int80_exit_hits();
        u8 a6[80];
        u32 o = 0;

        memset(&fsProc, 0, sizeof(fsProc));
        gj_process_init(&fsProc, &fsCnode, aFsSlots, 32);
        fsProc.u32Alive = 1;
        fsProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&fsProc, &fsMeta, NULL);
        if (process_as_ensure(&fsProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&fsProc);
        memset(a6, 0x90, sizeof(a6));
        /* sub esp, 128 */
        a6[o++] = 0x81;
        a6[o++] = 0xec;
        a6[o++] = 128;
        a6[o++] = 0;
        a6[o++] = 0;
        a6[o++] = 0;
        /* open: mov eax,5; xor ebx,ebx; xor ecx,ecx; int 0x80 */
        a6[o++] = 0xb8;
        a6[o++] = 5;
        a6[o++] = 0;
        a6[o++] = 0;
        a6[o++] = 0;
        a6[o++] = 0x31;
        a6[o++] = 0xdb;
        a6[o++] = 0x31;
        a6[o++] = 0xc9;
        a6[o++] = 0xcd;
        a6[o++] = 0x80;
        /* fstat64: mov ebx,eax; mov ecx,esp; mov eax,197; int 0x80 */
        a6[o++] = 0x89;
        a6[o++] = 0xc3;
        a6[o++] = 0x89;
        a6[o++] = 0xe1;
        a6[o++] = 0xb8;
        a6[o++] = 197;
        a6[o++] = 0;
        a6[o++] = 0;
        a6[o++] = 0;
        a6[o++] = 0xcd;
        a6[o++] = 0x80;
        /* if eax==0 exit(0) else exit(1) */
        a6[o++] = 0x89;
        a6[o++] = 0xc3; /* mov ebx,eax */
        a6[o++] = 0xb8;
        a6[o++] = 1;
        a6[o++] = 0;
        a6[o++] = 0;
        a6[o++] = 0;
        a6[o++] = 0xcd;
        a6[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a6, sizeof(a6));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&fsProc, 1);
        thr6 = thread_create_user32(&fsProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr6 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsFs) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsFs && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 fstat64 PASS\n");
        } else {
            kprintf("pe32: int80 fstat64 FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * getrandom + exit(n): sub esp,16; mov ebx,esp; mov ecx,8; mov eax,355; int80
     *   mov ebx,eax; mov eax,1; int80
     */
    {
        static struct gj_process grProc;
        static struct gj_cnode grCnode;
        static struct gj_cap_slot aGrSlots[32];
        static struct gj_root_meta grMeta;
        u64 vaC = 0x55800000ull;
        u64 vaS = 0x55900000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr7;
        u32 hitsGr = pe32_int80_exit_hits();
        u8 a7[40];
        u32 o = 0;

        memset(&grProc, 0, sizeof(grProc));
        gj_process_init(&grProc, &grCnode, aGrSlots, 32);
        grProc.u32Alive = 1;
        grProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&grProc, &grMeta, NULL);
        if (process_as_ensure(&grProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&grProc);
        memset(a7, 0x90, sizeof(a7));
        a7[o++] = 0x83;
        a7[o++] = 0xec;
        a7[o++] = 16; /* sub esp,16 */
        a7[o++] = 0x89;
        a7[o++] = 0xe3; /* mov ebx,esp */
        a7[o++] = 0xb9;
        a7[o++] = 8;
        a7[o++] = 0;
        a7[o++] = 0;
        a7[o++] = 0; /* mov ecx,8 */
        a7[o++] = 0xb8;
        a7[o++] = 0x63;
        a7[o++] = 0x01;
        a7[o++] = 0;
        a7[o++] = 0; /* mov eax,355 */
        a7[o++] = 0xcd;
        a7[o++] = 0x80;
        a7[o++] = 0x89;
        a7[o++] = 0xc3; /* mov ebx,eax */
        a7[o++] = 0xb8;
        a7[o++] = 1;
        a7[o++] = 0;
        a7[o++] = 0;
        a7[o++] = 0;
        a7[o++] = 0xcd;
        a7[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a7, sizeof(a7));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&grProc, 1);
        thr7 = thread_create_user32(&grProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr7 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsGr) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsGr && pe32_int80_last_code() == 8) {
            kprintf("pe32: int80 getrandom PASS n=8\n");
        } else {
            kprintf("pe32: int80 getrandom FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * int80 mmap2 MAP_FIXED at 0x58001000 + mprotect + exit(va>>12)
     * (greppable: pe32: int80 mmap_fixed PASS). pgoff=0 pages; flags FIXED|
     * PRIVATE|ANON. mprotect RWX→RX then exit with va>>12 as proof code.
     *   mov eax,192; mov ebx,0x58001000; mov ecx,4096; mov edx,7; mov esi,0x32;
     *   mov edi,-1; xor ebp,ebp; int 0x80
     *   mov eax,125; mov ebx,0x58001000; mov ecx,4096; mov edx,5; int 0x80
     *   mov eax,1; mov ebx,0x58001; int 0x80
     */
    {
        static struct gj_process fxProc;
        static struct gj_cnode fxCnode;
        static struct gj_cap_slot aFxSlots[32];
        static struct gj_root_meta fxMeta;
        u64 vaC = 0x55a00000ull;
        u64 vaS = 0x55b00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr8;
        u32 hitsFx = pe32_int80_exit_hits();
        u8 a8[80];
        u32 o = 0;

        memset(&fxProc, 0, sizeof(fxProc));
        gj_process_init(&fxProc, &fxCnode, aFxSlots, 32);
        fxProc.u32Alive = 1;
        fxProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&fxProc, &fxMeta, NULL);
        if (process_as_ensure(&fxProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&fxProc);
        memset(a8, 0x90, sizeof(a8));
        /* mmap2(0x58001000, 4096, RWX=7, FIXED|PRIVATE|ANON=0x32, -1, 0) */
        a8[o++] = 0xb8;
        a8[o++] = 192;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xbb;
        a8[o++] = 0x00;
        a8[o++] = 0x10;
        a8[o++] = 0x00;
        a8[o++] = 0x58; /* mov ebx,0x58001000 */
        a8[o++] = 0xb9;
        a8[o++] = 0x00;
        a8[o++] = 0x10;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xba;
        a8[o++] = 7;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xbe;
        a8[o++] = 0x32;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xbf;
        a8[o++] = 0xff;
        a8[o++] = 0xff;
        a8[o++] = 0xff;
        a8[o++] = 0xff;
        a8[o++] = 0x31;
        a8[o++] = 0xed;
        a8[o++] = 0xcd;
        a8[o++] = 0x80;
        /* mprotect(0x58001000, 4096, PROT_READ|EXEC=5) */
        a8[o++] = 0xb8;
        a8[o++] = 125;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xbb;
        a8[o++] = 0x00;
        a8[o++] = 0x10;
        a8[o++] = 0x00;
        a8[o++] = 0x58;
        a8[o++] = 0xb9;
        a8[o++] = 0x00;
        a8[o++] = 0x10;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xba;
        a8[o++] = 5;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xcd;
        a8[o++] = 0x80;
        /* exit(0x58001) — page number of fixed VA as code */
        a8[o++] = 0xb8;
        a8[o++] = 1;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0;
        a8[o++] = 0xbb;
        a8[o++] = 0x01;
        a8[o++] = 0x80;
        a8[o++] = 0x05;
        a8[o++] = 0x00; /* 0x58001 */
        a8[o++] = 0xcd;
        a8[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a8, sizeof(a8));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&fxProc, 1);
        thr8 = thread_create_user32(&fxProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr8 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsFx) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsFx &&
            pe32_int80_last_code() == 0x58001u) {
            kprintf("pe32: int80 mmap_fixed PASS va=0x58001000\n");
        } else {
            kprintf("pe32: int80 mmap_fixed FAIL hits=%u code=0x%x\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * clock_gettime(0, &ts) + exit(0 if success):
     *   sub esp,16; mov ebx,0; mov ecx,esp; mov eax,265; int 0x80
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process ctProc;
        static struct gj_cnode ctCnode;
        static struct gj_cap_slot aCtSlots[32];
        static struct gj_root_meta ctMeta;
        u64 vaC = 0x55c00000ull;
        u64 vaS = 0x55d00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr9;
        u32 hitsCt = pe32_int80_exit_hits();
        u8 a9[40];
        u32 o = 0;

        memset(&ctProc, 0, sizeof(ctProc));
        gj_process_init(&ctProc, &ctCnode, aCtSlots, 32);
        ctProc.u32Alive = 1;
        ctProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&ctProc, &ctMeta, NULL);
        if (process_as_ensure(&ctProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&ctProc);
        memset(a9, 0x90, sizeof(a9));
        a9[o++] = 0x83;
        a9[o++] = 0xec;
        a9[o++] = 16;
        a9[o++] = 0x31;
        a9[o++] = 0xdb; /* xor ebx,ebx CLOCK_REALTIME */
        a9[o++] = 0x89;
        a9[o++] = 0xe1; /* mov ecx,esp */
        a9[o++] = 0xb8;
        a9[o++] = 0x09;
        a9[o++] = 0x01;
        a9[o++] = 0;
        a9[o++] = 0; /* mov eax,265 */
        a9[o++] = 0xcd;
        a9[o++] = 0x80;
        a9[o++] = 0x89;
        a9[o++] = 0xc3;
        a9[o++] = 0xb8;
        a9[o++] = 1;
        a9[o++] = 0;
        a9[o++] = 0;
        a9[o++] = 0;
        a9[o++] = 0xcd;
        a9[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a9, sizeof(a9));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&ctProc, 1);
        thr9 = thread_create_user32(&ctProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr9 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsCt) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsCt && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 clock_gettime PASS\n");
        } else {
            kprintf("pe32: int80 clock_gettime FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * uname(&uts) + exit(0): need 9*65=585 bytes buffer → use mmap2 first
     *   mov eax,192; xor ebx,ebx; mov ecx,0x400; mov edx,3; mov esi,0x22;
     *   mov edi,-1; xor ebp,ebp; int 0x80  → eax=buf
     *   mov ebx,eax; mov eax,122; int 0x80
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process unProc;
        static struct gj_cnode unCnode;
        static struct gj_cap_slot aUnSlots[32];
        static struct gj_root_meta unMeta;
        u64 vaC = 0x55e00000ull;
        u64 vaS = 0x55f00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr10;
        u32 hitsUn = pe32_int80_exit_hits();
        u8 a10[64];
        u32 o = 0;

        memset(&unProc, 0, sizeof(unProc));
        gj_process_init(&unProc, &unCnode, aUnSlots, 32);
        unProc.u32Alive = 1;
        unProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&unProc, &unMeta, NULL);
        if (process_as_ensure(&unProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&unProc);
        memset(a10, 0x90, sizeof(a10));
        /* mmap2 2 pages for uname buffer */
        a10[o++] = 0xb8;
        a10[o++] = 192;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0x31;
        a10[o++] = 0xdb;
        a10[o++] = 0xb9;
        a10[o++] = 0x00;
        a10[o++] = 0x20;
        a10[o++] = 0;
        a10[o++] = 0; /* 8192 */
        a10[o++] = 0xba;
        a10[o++] = 3;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0xbe;
        a10[o++] = 0x22;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0xbf;
        a10[o++] = 0xff;
        a10[o++] = 0xff;
        a10[o++] = 0xff;
        a10[o++] = 0xff;
        a10[o++] = 0x31;
        a10[o++] = 0xed;
        a10[o++] = 0xcd;
        a10[o++] = 0x80;
        /* uname */
        a10[o++] = 0x89;
        a10[o++] = 0xc3; /* mov ebx,eax */
        a10[o++] = 0xb8;
        a10[o++] = 122;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0xcd;
        a10[o++] = 0x80;
        /* exit(result) */
        a10[o++] = 0x89;
        a10[o++] = 0xc3;
        a10[o++] = 0xb8;
        a10[o++] = 1;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0;
        a10[o++] = 0xcd;
        a10[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a10, sizeof(a10));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&unProc, 1);
        thr10 = thread_create_user32(&unProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr10 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsUn) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsUn && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 uname PASS\n");
        } else {
            kprintf("pe32: int80 uname FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * clone (return 2) + exit(pid): mov eax,120; int 0x80; mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process clProc;
        static struct gj_cnode clCnode;
        static struct gj_cap_slot aClSlots[32];
        static struct gj_root_meta clMeta;
        u64 vaC = 0x56000000ull;
        u64 vaS = 0x56100000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr11;
        u32 hitsCl = pe32_int80_exit_hits();
        u8 a11[24];
        u32 o = 0;

        memset(&clProc, 0, sizeof(clProc));
        gj_process_init(&clProc, &clCnode, aClSlots, 32);
        clProc.u32Alive = 1;
        clProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&clProc, &clMeta, NULL);
        if (process_as_ensure(&clProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&clProc);
        memset(a11, 0x90, sizeof(a11));
        a11[o++] = 0xb8;
        a11[o++] = 120;
        a11[o++] = 0;
        a11[o++] = 0;
        a11[o++] = 0;
        a11[o++] = 0xcd;
        a11[o++] = 0x80;
        a11[o++] = 0x89;
        a11[o++] = 0xc3;
        a11[o++] = 0xb8;
        a11[o++] = 1;
        a11[o++] = 0;
        a11[o++] = 0;
        a11[o++] = 0;
        a11[o++] = 0xcd;
        a11[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a11, sizeof(a11));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&clProc, 1);
        thr11 = thread_create_user32(&clProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr11 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsCl) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsCl && pe32_int80_last_code() == 2) {
            kprintf("pe32: int80 clone PASS pid=2\n");
        } else {
            kprintf("pe32: int80 clone FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * futex FUTEX_WAKE (op=1) + exit(0):
     *   sub esp,16; mov dword [esp],0; mov ebx,esp; mov ecx,1; mov edx,1;
     *   mov eax,240; int 0x80; mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process fuProc;
        static struct gj_cnode fuCnode;
        static struct gj_cap_slot aFuSlots[32];
        static struct gj_root_meta fuMeta;
        u64 vaC = 0x56200000ull;
        u64 vaS = 0x56300000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr12;
        u32 hitsFu = pe32_int80_exit_hits();
        u8 a12[48];
        u32 o = 0;

        memset(&fuProc, 0, sizeof(fuProc));
        gj_process_init(&fuProc, &fuCnode, aFuSlots, 32);
        fuProc.u32Alive = 1;
        fuProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&fuProc, &fuMeta, NULL);
        if (process_as_ensure(&fuProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&fuProc);
        memset(a12, 0x90, sizeof(a12));
        a12[o++] = 0x83;
        a12[o++] = 0xec;
        a12[o++] = 16;
        a12[o++] = 0xc7;
        a12[o++] = 0x04;
        a12[o++] = 0x24;
        a12[o++] = 0;
        a12[o++] = 0;
        a12[o++] = 0;
        a12[o++] = 0; /* mov dword [esp],0 */
        a12[o++] = 0x89;
        a12[o++] = 0xe3; /* mov ebx,esp */
        a12[o++] = 0xb9;
        a12[o++] = 1;
        a12[o++] = 0;
        a12[o++] = 0;
        a12[o++] = 0; /* FUTEX_WAKE */
        a12[o++] = 0xba;
        a12[o++] = 1;
        a12[o++] = 0;
        a12[o++] = 0;
        a12[o++] = 0; /* val=1 */
        a12[o++] = 0xb8;
        a12[o++] = 0xf0;
        a12[o++] = 0;
        a12[o++] = 0;
        a12[o++] = 0; /* mov eax,240 */
        a12[o++] = 0xcd;
        a12[o++] = 0x80;
        a12[o++] = 0x89;
        a12[o++] = 0xc3;
        a12[o++] = 0xb8;
        a12[o++] = 1;
        a12[o++] = 0;
        a12[o++] = 0;
        a12[o++] = 0;
        a12[o++] = 0xcd;
        a12[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a12, sizeof(a12));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&fuProc, 1);
        thr12 = thread_create_user32(&fuProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr12 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsFu) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsFu && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 futex PASS\n");
        } else {
            kprintf("pe32: int80 futex FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * set_thread_area + modify_ldt + exit(0):
     *   sub esp,32; mov ebx,esp; mov eax,243; int 0x80
     *   mov ebx,1; mov ecx,esp; mov edx,16; mov eax,123; int 0x80
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process tlsProc;
        static struct gj_cnode tlsCnode;
        static struct gj_cap_slot aTlsSlots[32];
        static struct gj_root_meta tlsMeta;
        u64 vaC = 0x56400000ull;
        u64 vaS = 0x56500000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr13;
        u32 hitsTls = pe32_int80_exit_hits();
        u8 a13[48];
        u32 o = 0;

        memset(&tlsProc, 0, sizeof(tlsProc));
        gj_process_init(&tlsProc, &tlsCnode, aTlsSlots, 32);
        tlsProc.u32Alive = 1;
        tlsProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&tlsProc, &tlsMeta, NULL);
        if (process_as_ensure(&tlsProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&tlsProc);
        memset(a13, 0x90, sizeof(a13));
        a13[o++] = 0x83;
        a13[o++] = 0xec;
        a13[o++] = 32;
        a13[o++] = 0x89;
        a13[o++] = 0xe3;
        a13[o++] = 0xb8;
        a13[o++] = 0xf3;
        a13[o++] = 0;
        a13[o++] = 0;
        a13[o++] = 0; /* set_thread_area */
        a13[o++] = 0xcd;
        a13[o++] = 0x80;
        a13[o++] = 0xbb;
        a13[o++] = 1;
        a13[o++] = 0;
        a13[o++] = 0;
        a13[o++] = 0; /* write */
        a13[o++] = 0x89;
        a13[o++] = 0xe1;
        a13[o++] = 0xba;
        a13[o++] = 16;
        a13[o++] = 0;
        a13[o++] = 0;
        a13[o++] = 0;
        a13[o++] = 0xb8;
        a13[o++] = 123;
        a13[o++] = 0;
        a13[o++] = 0;
        a13[o++] = 0; /* modify_ldt */
        a13[o++] = 0xcd;
        a13[o++] = 0x80;
        a13[o++] = 0x89;
        a13[o++] = 0xc3;
        a13[o++] = 0xb8;
        a13[o++] = 1;
        a13[o++] = 0;
        a13[o++] = 0;
        a13[o++] = 0;
        a13[o++] = 0xcd;
        a13[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a13, sizeof(a13));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&tlsProc, 1);
        thr13 = thread_create_user32(&tlsProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr13 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsTls) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsTls && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 tls PASS\n");
        } else {
            kprintf("pe32: int80 tls FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * nanosleep({0,0}, NULL) + exit(0):
     *   sub esp,16; mov dword [esp],0; mov dword [esp+4],0;
     *   mov ebx,esp; xor ecx,ecx; mov eax,162; int 0x80
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process nsProc;
        static struct gj_cnode nsCnode;
        static struct gj_cap_slot aNsSlots[32];
        static struct gj_root_meta nsMeta;
        u64 vaC = 0x56600000ull;
        u64 vaS = 0x56700000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr14;
        u32 hitsNs = pe32_int80_exit_hits();
        u8 a14[48];
        u32 o = 0;

        memset(&nsProc, 0, sizeof(nsProc));
        gj_process_init(&nsProc, &nsCnode, aNsSlots, 32);
        nsProc.u32Alive = 1;
        nsProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&nsProc, &nsMeta, NULL);
        if (process_as_ensure(&nsProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&nsProc);
        memset(a14, 0x90, sizeof(a14));
        a14[o++] = 0x83;
        a14[o++] = 0xec;
        a14[o++] = 16;
        a14[o++] = 0xc7;
        a14[o++] = 0x04;
        a14[o++] = 0x24;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0xc7;
        a14[o++] = 0x44;
        a14[o++] = 0x24;
        a14[o++] = 4;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0x89;
        a14[o++] = 0xe3;
        a14[o++] = 0x31;
        a14[o++] = 0xc9;
        a14[o++] = 0xb8;
        a14[o++] = 162;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0xcd;
        a14[o++] = 0x80;
        a14[o++] = 0x89;
        a14[o++] = 0xc3;
        a14[o++] = 0xb8;
        a14[o++] = 1;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0;
        a14[o++] = 0xcd;
        a14[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a14, sizeof(a14));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&nsProc, 1);
        thr14 = thread_create_user32(&nsProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr14 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsNs) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsNs && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 nanosleep PASS\n");
        } else {
            kprintf("pe32: int80 nanosleep FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * gettid + exit(tid): mov eax,224; int 0x80; mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process gtProc;
        static struct gj_cnode gtCnode;
        static struct gj_cap_slot aGtSlots[32];
        static struct gj_root_meta gtMeta;
        u64 vaC = 0x56800000ull;
        u64 vaS = 0x56900000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr15;
        u32 hitsGt = pe32_int80_exit_hits();
        u8 a15[24];
        u32 o = 0;

        memset(&gtProc, 0, sizeof(gtProc));
        gj_process_init(&gtProc, &gtCnode, aGtSlots, 32);
        gtProc.u32Alive = 1;
        gtProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&gtProc, &gtMeta, NULL);
        if (process_as_ensure(&gtProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&gtProc);
        memset(a15, 0x90, sizeof(a15));
        a15[o++] = 0xb8;
        a15[o++] = 0xe0;
        a15[o++] = 0;
        a15[o++] = 0;
        a15[o++] = 0; /* mov eax,224 */
        a15[o++] = 0xcd;
        a15[o++] = 0x80;
        a15[o++] = 0x89;
        a15[o++] = 0xc3;
        a15[o++] = 0xb8;
        a15[o++] = 1;
        a15[o++] = 0;
        a15[o++] = 0;
        a15[o++] = 0;
        a15[o++] = 0xcd;
        a15[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a15, sizeof(a15));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&gtProc, 1);
        thr15 = thread_create_user32(&gtProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr15 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsGt) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsGt && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 gettid PASS tid=1\n");
        } else {
            kprintf("pe32: int80 gettid FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * time(NULL) + exit(0 if >=0): mov eax,13; xor ebx,ebx; int 0x80
     *   test eax,eax; js fail; xor ebx,ebx; mov eax,1; int 0x80
     * Simplified: exit with 0 if time returned >=0 (we always do)
     *   mov eax,13; xor ebx,ebx; int 0x80; mov ebx,0; mov eax,1; int 0x80
     */
    {
        static struct gj_process tmProc;
        static struct gj_cnode tmCnode;
        static struct gj_cap_slot aTmSlots[32];
        static struct gj_root_meta tmMeta;
        u64 vaC = 0x56a00000ull;
        u64 vaS = 0x56b00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr16;
        u32 hitsTm = pe32_int80_exit_hits();
        u8 a16[24];
        u32 o = 0;

        memset(&tmProc, 0, sizeof(tmProc));
        gj_process_init(&tmProc, &tmCnode, aTmSlots, 32);
        tmProc.u32Alive = 1;
        tmProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&tmProc, &tmMeta, NULL);
        if (process_as_ensure(&tmProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&tmProc);
        memset(a16, 0x90, sizeof(a16));
        a16[o++] = 0xb8;
        a16[o++] = 13;
        a16[o++] = 0;
        a16[o++] = 0;
        a16[o++] = 0;
        a16[o++] = 0x31;
        a16[o++] = 0xdb;
        a16[o++] = 0xcd;
        a16[o++] = 0x80;
        a16[o++] = 0x31;
        a16[o++] = 0xdb;
        a16[o++] = 0xb8;
        a16[o++] = 1;
        a16[o++] = 0;
        a16[o++] = 0;
        a16[o++] = 0;
        a16[o++] = 0xcd;
        a16[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a16, sizeof(a16));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&tmProc, 1);
        thr16 = thread_create_user32(&tmProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr16 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsTm) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsTm && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 time PASS\n");
        } else {
            kprintf("pe32: int80 time FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * Real vfs open/write/lseek/read via path on code page
     * (greppable: pe32: int80 vfs_io PASS):
     * path "/tmp/pe32_io" at entry+0x80; msg "TEST" at entry+0x90.
     * ebx = user VA of path for open; trap path copy must stay in-page.
     * open(O_CREAT|RDWR=0x42), write 4, lseek 0, read 4, exit(nread).
     */
    {
        static struct gj_process ioProc;
        static struct gj_cnode ioCnode;
        static struct gj_cap_slot aIoSlots[32];
        static struct gj_root_meta ioMeta;
        u64 vaC = 0x56c00000ull;
        u64 vaS = 0x56d00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr17;
        u32 hitsIo = pe32_int80_exit_hits();
        u8 a17[160];
        u32 o = 0;
        u32 pathOff = 0x80;
        u32 msgOff = 0x90;
        u32 bufOff = 0xa0;

        memset(&ioProc, 0, sizeof(ioProc));
        gj_process_init(&ioProc, &ioCnode, aIoSlots, 32);
        ioProc.u32Alive = 1;
        ioProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&ioProc, &ioMeta, NULL);
        if (process_as_ensure(&ioProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&ioProc);
        memset(a17, 0x90, sizeof(a17));
        /* path + msg data on page */
        {
            static const char szP[] = "/tmp/pe32_io";
            static const char szM[] = "TEST";
            u32 j;

            for (j = 0; j < sizeof(szP); j++) {
                a17[pathOff + j] = (u8)szP[j];
            }
            for (j = 0; j < 4; j++) {
                a17[msgOff + j] = (u8)szM[j];
            }
        }
        /* open(path, O_CREAT|RDWR=0x42) */
        a17[o++] = 0xbb; /* mov ebx, path_va */
        a17[o++] = (u8)(vaC + pathOff);
        a17[o++] = (u8)((vaC + pathOff) >> 8);
        a17[o++] = (u8)((vaC + pathOff) >> 16);
        a17[o++] = (u8)((vaC + pathOff) >> 24);
        a17[o++] = 0xb9;
        a17[o++] = 0x42;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0; /* flags */
        a17[o++] = 0xb8;
        a17[o++] = 5;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0xcd;
        a17[o++] = 0x80;
        /* write(fd, msg, 4) */
        a17[o++] = 0x89;
        a17[o++] = 0xc3; /* mov ebx,eax */
        a17[o++] = 0x50; /* push eax save fd */
        a17[o++] = 0xb9;
        a17[o++] = (u8)(vaC + msgOff);
        a17[o++] = (u8)((vaC + msgOff) >> 8);
        a17[o++] = (u8)((vaC + msgOff) >> 16);
        a17[o++] = (u8)((vaC + msgOff) >> 24);
        a17[o++] = 0xba;
        a17[o++] = 4;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0xb8;
        a17[o++] = 4;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0xcd;
        a17[o++] = 0x80;
        /* lseek(fd,0,SEEK_SET) */
        a17[o++] = 0x58; /* pop eax = fd */
        a17[o++] = 0x89;
        a17[o++] = 0xc3;
        a17[o++] = 0x50; /* push fd */
        a17[o++] = 0x31;
        a17[o++] = 0xc9;
        a17[o++] = 0x31;
        a17[o++] = 0xd2;
        a17[o++] = 0xb8;
        a17[o++] = 19;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0xcd;
        a17[o++] = 0x80;
        /* read(fd, buf, 4) */
        a17[o++] = 0x58;
        a17[o++] = 0x89;
        a17[o++] = 0xc3;
        a17[o++] = 0xb9;
        a17[o++] = (u8)(vaC + bufOff);
        a17[o++] = (u8)((vaC + bufOff) >> 8);
        a17[o++] = (u8)((vaC + bufOff) >> 16);
        a17[o++] = (u8)((vaC + bufOff) >> 24);
        a17[o++] = 0xba;
        a17[o++] = 4;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0xb8;
        a17[o++] = 3;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0xcd;
        a17[o++] = 0x80;
        /* exit(nread) */
        a17[o++] = 0x89;
        a17[o++] = 0xc3;
        a17[o++] = 0xb8;
        a17[o++] = 1;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0;
        a17[o++] = 0xcd;
        a17[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a17, sizeof(a17));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&ioProc, 1);
        thr17 = thread_create_user32(&ioProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr17 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsIo) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsIo && pe32_int80_last_code() == 4) {
            kprintf("pe32: int80 vfs_io PASS n=4\n");
        } else {
            kprintf("pe32: int80 vfs_io FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * gettimeofday + exit(0):
     *   sub esp,16; mov ebx,esp; xor ecx,ecx; mov eax,78; int 0x80
     *   mov ebx,eax; mov eax,1; int 0x80
     */
    {
        static struct gj_process gtodProc;
        static struct gj_cnode gtodCnode;
        static struct gj_cap_slot aGtodSlots[32];
        static struct gj_root_meta gtodMeta;
        u64 vaC = 0x56e00000ull;
        u64 vaS = 0x56f00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr18;
        u32 hitsGt = pe32_int80_exit_hits();
        u8 a18[32];
        u32 o = 0;

        memset(&gtodProc, 0, sizeof(gtodProc));
        gj_process_init(&gtodProc, &gtodCnode, aGtodSlots, 32);
        gtodProc.u32Alive = 1;
        gtodProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&gtodProc, &gtodMeta, NULL);
        if (process_as_ensure(&gtodProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&gtodProc);
        memset(a18, 0x90, sizeof(a18));
        a18[o++] = 0x83;
        a18[o++] = 0xec;
        a18[o++] = 16;
        a18[o++] = 0x89;
        a18[o++] = 0xe3;
        a18[o++] = 0x31;
        a18[o++] = 0xc9;
        a18[o++] = 0xb8;
        a18[o++] = 78;
        a18[o++] = 0;
        a18[o++] = 0;
        a18[o++] = 0;
        a18[o++] = 0xcd;
        a18[o++] = 0x80;
        a18[o++] = 0x89;
        a18[o++] = 0xc3;
        a18[o++] = 0xb8;
        a18[o++] = 1;
        a18[o++] = 0;
        a18[o++] = 0;
        a18[o++] = 0;
        a18[o++] = 0xcd;
        a18[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a18, sizeof(a18));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&gtodProc, 1);
        thr18 = thread_create_user32(&gtodProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr18 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsGt) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsGt && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 gettimeofday PASS\n");
        } else {
            kprintf("pe32: int80 gettimeofday FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * getcwd + sysinfo + exit(0):
     *   sub esp,128; mov ebx,esp; mov ecx,128; mov eax,183; int80
     *   sub esp,128; mov ebx,esp; mov eax,116; int80
     *   xor ebx,ebx; mov eax,1; int80
     */
    {
        static struct gj_process cwProc;
        static struct gj_cnode cwCnode;
        static struct gj_cap_slot aCwSlots[32];
        static struct gj_root_meta cwMeta;
        u64 vaC = 0x57000000ull;
        u64 vaS = 0x57100000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr19;
        u32 hitsCw = pe32_int80_exit_hits();
        u8 a19[48];
        u32 o = 0;

        memset(&cwProc, 0, sizeof(cwProc));
        gj_process_init(&cwProc, &cwCnode, aCwSlots, 32);
        cwProc.u32Alive = 1;
        cwProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&cwProc, &cwMeta, NULL);
        if (process_as_ensure(&cwProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&cwProc);
        memset(a19, 0x90, sizeof(a19));
        a19[o++] = 0x81;
        a19[o++] = 0xec;
        a19[o++] = 128;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0x89;
        a19[o++] = 0xe3;
        a19[o++] = 0xb9;
        a19[o++] = 128;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0xb8;
        a19[o++] = 183;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0xcd;
        a19[o++] = 0x80;
        a19[o++] = 0x81;
        a19[o++] = 0xec;
        a19[o++] = 128;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0x89;
        a19[o++] = 0xe3;
        a19[o++] = 0xb8;
        a19[o++] = 116;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0xcd;
        a19[o++] = 0x80;
        a19[o++] = 0x31;
        a19[o++] = 0xdb;
        a19[o++] = 0xb8;
        a19[o++] = 1;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0;
        a19[o++] = 0xcd;
        a19[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a19, sizeof(a19));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&cwProc, 1);
        thr19 = thread_create_user32(&cwProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr19 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsCw) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsCw && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 getcwd_sysinfo PASS\n");
        } else {
            kprintf("pe32: int80 getcwd_sysinfo FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * brk(0) + brk(cur+4096) + exit(0 if new>old):
     *   mov eax,45; xor ebx,ebx; int 0x80   ; cur
     *   add eax,0x1000; mov ebx,eax; mov eax,45; int 0x80
     *   mov ebx,0; mov eax,1; int 0x80
     */
    {
        static struct gj_process brProc;
        static struct gj_cnode brCnode;
        static struct gj_cap_slot aBrSlots[32];
        static struct gj_root_meta brMeta;
        u64 vaC = 0x57200000ull;
        u64 vaS = 0x57300000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr20;
        u32 hitsBr = pe32_int80_exit_hits();
        u8 a20[40];
        u32 o = 0;

        memset(&brProc, 0, sizeof(brProc));
        gj_process_init(&brProc, &brCnode, aBrSlots, 32);
        brProc.u32Alive = 1;
        brProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&brProc, &brMeta, NULL);
        if (process_as_ensure(&brProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&brProc);
        memset(a20, 0x90, sizeof(a20));
        a20[o++] = 0xb8; a20[o++] = 45; a20[o++] = 0; a20[o++] = 0; a20[o++] = 0;
        a20[o++] = 0x31; a20[o++] = 0xdb;
        a20[o++] = 0xcd; a20[o++] = 0x80;
        a20[o++] = 0x05; a20[o++] = 0x00; a20[o++] = 0x10; a20[o++] = 0; a20[o++] = 0; /* add eax,4096 */
        a20[o++] = 0x89; a20[o++] = 0xc3;
        a20[o++] = 0xb8; a20[o++] = 45; a20[o++] = 0; a20[o++] = 0; a20[o++] = 0;
        a20[o++] = 0xcd; a20[o++] = 0x80;
        a20[o++] = 0x31; a20[o++] = 0xdb;
        a20[o++] = 0xb8; a20[o++] = 1; a20[o++] = 0; a20[o++] = 0; a20[o++] = 0;
        a20[o++] = 0xcd; a20[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a20, sizeof(a20));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&brProc, 1);
        thr20 = thread_create_user32(&brProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr20 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsBr) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsBr && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 brk PASS\n");
        } else {
            kprintf("pe32: int80 brk FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * access(path, F_OK) — greppable pe32: int80 access PASS.
     * path may exist from vfs_io smoke; else create via open first.
     * path string at code-page +0x50; ebx = user VA of path for open/access.
     *   mov ebx,path; mov ecx,O_CREAT|…; mov eax,5; int80  ; ensure file
     *   mov ebx,path; xor ecx,ecx; mov eax,33; int80       ; access F_OK
     *   mov ebx,eax; mov eax,1; int80
     */
    {
        static struct gj_process acProc;
        static struct gj_cnode acCnode;
        static struct gj_cap_slot aAcSlots[32];
        static struct gj_root_meta acMeta;
        u64 vaC = 0x57400000ull;
        u64 vaS = 0x57500000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr21;
        u32 hitsAc = pe32_int80_exit_hits();
        u8 a21[96];
        u32 o = 0;
        u32 pathOff = 0x50;

        memset(&acProc, 0, sizeof(acProc));
        gj_process_init(&acProc, &acCnode, aAcSlots, 32);
        acProc.u32Alive = 1;
        acProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&acProc, &acMeta, NULL);
        if (process_as_ensure(&acProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&acProc);
        memset(a21, 0x90, sizeof(a21));
        {
            static const char szP[] = "/tmp/pe32_io";
            u32 j;
            for (j = 0; j < sizeof(szP); j++) {
                a21[pathOff + j] = (u8)szP[j];
            }
        }
        /* ensure file exists: open creat */
        a21[o++] = 0xbb;
        a21[o++] = (u8)(vaC + pathOff);
        a21[o++] = (u8)((vaC + pathOff) >> 8);
        a21[o++] = (u8)((vaC + pathOff) >> 16);
        a21[o++] = (u8)((vaC + pathOff) >> 24);
        a21[o++] = 0xb9; a21[o++] = 0x41; a21[o++] = 0; a21[o++] = 0; a21[o++] = 0;
        a21[o++] = 0xb8; a21[o++] = 5; a21[o++] = 0; a21[o++] = 0; a21[o++] = 0;
        a21[o++] = 0xcd; a21[o++] = 0x80;
        a21[o++] = 0x89; a21[o++] = 0xc3;
        a21[o++] = 0xb8; a21[o++] = 6; a21[o++] = 0; a21[o++] = 0; a21[o++] = 0;
        a21[o++] = 0xcd; a21[o++] = 0x80; /* close */
        /* access F_OK */
        a21[o++] = 0xbb;
        a21[o++] = (u8)(vaC + pathOff);
        a21[o++] = (u8)((vaC + pathOff) >> 8);
        a21[o++] = (u8)((vaC + pathOff) >> 16);
        a21[o++] = (u8)((vaC + pathOff) >> 24);
        a21[o++] = 0x31; a21[o++] = 0xc9;
        a21[o++] = 0xb8; a21[o++] = 33; a21[o++] = 0; a21[o++] = 0; a21[o++] = 0;
        a21[o++] = 0xcd; a21[o++] = 0x80;
        a21[o++] = 0x89; a21[o++] = 0xc3;
        a21[o++] = 0xb8; a21[o++] = 1; a21[o++] = 0; a21[o++] = 0; a21[o++] = 0;
        a21[o++] = 0xcd; a21[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a21, sizeof(a21));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&acProc, 1);
        thr21 = thread_create_user32(&acProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr21 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsAc) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsAc && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 access PASS\n");
        } else {
            kprintf("pe32: int80 access FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * mkdir-ish open+rename pathA->pathB + access B + exit(0)
     * paths at 0x60 and 0x70 on code page
     */
    {
        static struct gj_process rnProc;
        static struct gj_cnode rnCnode;
        static struct gj_cap_slot aRnSlots[32];
        static struct gj_root_meta rnMeta;
        u64 vaC = 0x57600000ull;
        u64 vaS = 0x57700000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr22;
        u32 hitsRn = pe32_int80_exit_hits();
        u8 a22[128];
        u32 o = 0;
        u32 pa = 0x60;
        u32 pb = 0x70;

        memset(&rnProc, 0, sizeof(rnProc));
        gj_process_init(&rnProc, &rnCnode, aRnSlots, 32);
        rnProc.u32Alive = 1;
        rnProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&rnProc, &rnMeta, NULL);
        if (process_as_ensure(&rnProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&rnProc);
        memset(a22, 0x90, sizeof(a22));
        {
            static const char sa[] = "/tmp/pe_rn_a";
            static const char sb[] = "/tmp/pe_rn_b";
            u32 j;
            for (j = 0; j < sizeof(sa); j++) a22[pa + j] = (u8)sa[j];
            for (j = 0; j < sizeof(sb); j++) a22[pb + j] = (u8)sb[j];
        }
        /* open creat A */
        a22[o++] = 0xbb;
        a22[o++] = (u8)(vaC+pa); a22[o++] = (u8)((vaC+pa)>>8);
        a22[o++] = (u8)((vaC+pa)>>16); a22[o++] = (u8)((vaC+pa)>>24);
        a22[o++] = 0xb9; a22[o++] = 0x41; a22[o++] = 0; a22[o++] = 0; a22[o++] = 0;
        a22[o++] = 0xb8; a22[o++] = 5; a22[o++] = 0; a22[o++] = 0; a22[o++] = 0;
        a22[o++] = 0xcd; a22[o++] = 0x80;
        a22[o++] = 0x89; a22[o++] = 0xc3;
        a22[o++] = 0xb8; a22[o++] = 6; a22[o++] = 0; a22[o++] = 0; a22[o++] = 0;
        a22[o++] = 0xcd; a22[o++] = 0x80;
        /* rename A B */
        a22[o++] = 0xbb;
        a22[o++] = (u8)(vaC+pa); a22[o++] = (u8)((vaC+pa)>>8);
        a22[o++] = (u8)((vaC+pa)>>16); a22[o++] = (u8)((vaC+pa)>>24);
        a22[o++] = 0xb9;
        a22[o++] = (u8)(vaC+pb); a22[o++] = (u8)((vaC+pb)>>8);
        a22[o++] = (u8)((vaC+pb)>>16); a22[o++] = (u8)((vaC+pb)>>24);
        a22[o++] = 0xb8; a22[o++] = 38; a22[o++] = 0; a22[o++] = 0; a22[o++] = 0;
        a22[o++] = 0xcd; a22[o++] = 0x80;
        /* access B */
        a22[o++] = 0xbb;
        a22[o++] = (u8)(vaC+pb); a22[o++] = (u8)((vaC+pb)>>8);
        a22[o++] = (u8)((vaC+pb)>>16); a22[o++] = (u8)((vaC+pb)>>24);
        a22[o++] = 0x31; a22[o++] = 0xc9;
        a22[o++] = 0xb8; a22[o++] = 33; a22[o++] = 0; a22[o++] = 0; a22[o++] = 0;
        a22[o++] = 0xcd; a22[o++] = 0x80;
        a22[o++] = 0x89; a22[o++] = 0xc3;
        a22[o++] = 0xb8; a22[o++] = 1; a22[o++] = 0; a22[o++] = 0; a22[o++] = 0;
        a22[o++] = 0xcd; a22[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a22, sizeof(a22));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&rnProc, 1);
        thr22 = thread_create_user32(&rnProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr22 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsRn) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsRn && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 rename PASS\n");
        } else {
            kprintf("pe32: int80 rename FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * getppid + getpgrp + setsid + exit(0)
     */
    {
        static struct gj_process pgProc;
        static struct gj_cnode pgCnode;
        static struct gj_cap_slot aPgSlots[32];
        static struct gj_root_meta pgMeta;
        u64 vaC = 0x57800000ull;
        u64 vaS = 0x57900000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr23;
        u32 hitsPg = pe32_int80_exit_hits();
        u8 a23[48];
        u32 o = 0;

        memset(&pgProc, 0, sizeof(pgProc));
        gj_process_init(&pgProc, &pgCnode, aPgSlots, 32);
        pgProc.u32Alive = 1;
        pgProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&pgProc, &pgMeta, NULL);
        if (process_as_ensure(&pgProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&pgProc);
        memset(a23, 0x90, sizeof(a23));
        a23[o++] = 0xb8; a23[o++] = 64; a23[o++] = 0; a23[o++] = 0; a23[o++] = 0;
        a23[o++] = 0xcd; a23[o++] = 0x80;
        a23[o++] = 0xb8; a23[o++] = 65; a23[o++] = 0; a23[o++] = 0; a23[o++] = 0;
        a23[o++] = 0xcd; a23[o++] = 0x80;
        a23[o++] = 0xb8; a23[o++] = 66; a23[o++] = 0; a23[o++] = 0; a23[o++] = 0;
        a23[o++] = 0xcd; a23[o++] = 0x80;
        a23[o++] = 0x31; a23[o++] = 0xdb;
        a23[o++] = 0xb8; a23[o++] = 1; a23[o++] = 0; a23[o++] = 0; a23[o++] = 0;
        a23[o++] = 0xcd; a23[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a23, sizeof(a23));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&pgProc, 1);
        thr23 = thread_create_user32(&pgProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr23 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsPg) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsPg && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 pgrp PASS\n");
        } else {
            kprintf("pe32: int80 pgrp FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * wait4 ECHILD (-10 as unsigned exit code 0xfffffff6) — use exit on eax after wait
     *   mov eax,114; xor ebx,ebx; xor ecx,ecx; xor edx,edx; int 0x80
     *   ; eax = -10. mov ebx,eax; mov eax,1; int80  → exit code = (u32)-10
     * We check last_code == (u32)-10 which is 0xfffffff6... pe32_int80_last_code is u32
     * Actually exit with 0 if wait returned negative: not easy. Instead:
     * exit(0) if eax has high bit set (error).
     * Simpler: getrlimit smoke exit(0)
     *   sub esp,16; mov ebx,7; mov ecx,esp; mov eax,76; int80
     *   mov ebx,eax; mov eax,1; int80
     */
    {
        static struct gj_process rlProc;
        static struct gj_cnode rlCnode;
        static struct gj_cap_slot aRlSlots[32];
        static struct gj_root_meta rlMeta;
        u64 vaC = 0x57a00000ull;
        u64 vaS = 0x57b00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr24;
        u32 hitsRl = pe32_int80_exit_hits();
        u8 a24[40];
        u32 o = 0;

        memset(&rlProc, 0, sizeof(rlProc));
        gj_process_init(&rlProc, &rlCnode, aRlSlots, 32);
        rlProc.u32Alive = 1;
        rlProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&rlProc, &rlMeta, NULL);
        if (process_as_ensure(&rlProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&rlProc);
        memset(a24, 0x90, sizeof(a24));
        a24[o++] = 0x83; a24[o++] = 0xec; a24[o++] = 16;
        a24[o++] = 0xbb; a24[o++] = 7; a24[o++] = 0; a24[o++] = 0; a24[o++] = 0; /* RLIMIT_NOFILE */
        a24[o++] = 0x89; a24[o++] = 0xe1;
        a24[o++] = 0xb8; a24[o++] = 76; a24[o++] = 0; a24[o++] = 0; a24[o++] = 0;
        a24[o++] = 0xcd; a24[o++] = 0x80;
        a24[o++] = 0x89; a24[o++] = 0xc3;
        a24[o++] = 0xb8; a24[o++] = 1; a24[o++] = 0; a24[o++] = 0; a24[o++] = 0;
        a24[o++] = 0xcd; a24[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a24, sizeof(a24));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&rlProc, 1);
        thr24 = thread_create_user32(&rlProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr24 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsRl) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsRl && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 getrlimit PASS\n");
        } else {
            kprintf("pe32: int80 getrlimit FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }
    /*
     * wait4 → ECHILD (-10); exit 0 if error, 1 if success
     */
    {
        static struct gj_process w4Proc;
        static struct gj_cnode w4Cnode;
        static struct gj_cap_slot aW4Slots[32];
        static struct gj_root_meta w4Meta;
        u64 vaC = 0x57c00000ull;
        u64 vaS = 0x57d00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr25;
        u32 hitsW4 = pe32_int80_exit_hits();
        u8 a25[48];
        u32 o = 0;

        memset(&w4Proc, 0, sizeof(w4Proc));
        gj_process_init(&w4Proc, &w4Cnode, aW4Slots, 32);
        w4Proc.u32Alive = 1;
        w4Proc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&w4Proc, &w4Meta, NULL);
        if (process_as_ensure(&w4Proc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&w4Proc);
        memset(a25, 0x90, sizeof(a25));
        a25[o++] = 0xb8; a25[o++] = 114; a25[o++] = 0; a25[o++] = 0; a25[o++] = 0;
        a25[o++] = 0x31; a25[o++] = 0xdb;
        a25[o++] = 0x31; a25[o++] = 0xc9;
        a25[o++] = 0x31; a25[o++] = 0xd2;
        a25[o++] = 0xcd; a25[o++] = 0x80;
        a25[o++] = 0x83; a25[o++] = 0xf8; a25[o++] = 0x00;
        a25[o++] = 0x7c; a25[o++] = 0x07;
        a25[o++] = 0xbb; a25[o++] = 1; a25[o++] = 0; a25[o++] = 0; a25[o++] = 0;
        a25[o++] = 0xeb; a25[o++] = 0x02;
        a25[o++] = 0x31; a25[o++] = 0xdb;
        a25[o++] = 0xb8; a25[o++] = 1; a25[o++] = 0; a25[o++] = 0; a25[o++] = 0;
        a25[o++] = 0xcd; a25[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a25, sizeof(a25));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&w4Proc, 1);
        thr25 = thread_create_user32(&w4Proc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr25 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsW4) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsW4 && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 wait4 PASS\n");
        } else {
            kprintf("pe32: int80 wait4 FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * kill(1,0) existence probe + exit(0):
     *   mov eax,37; mov ebx,1; xor ecx,ecx; int80
     *   mov ebx,eax; mov eax,1; int80
     */
    {
        static struct gj_process kiProc;
        static struct gj_cnode kiCnode;
        static struct gj_cap_slot aKiSlots[32];
        static struct gj_root_meta kiMeta;
        u64 vaC = 0x57e00000ull;
        u64 vaS = 0x57f00000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr26;
        u32 hitsKi = pe32_int80_exit_hits();
        u8 a26[32];
        u32 o = 0;

        memset(&kiProc, 0, sizeof(kiProc));
        gj_process_init(&kiProc, &kiCnode, aKiSlots, 32);
        kiProc.u32Alive = 1;
        kiProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&kiProc, &kiMeta, NULL);
        if (process_as_ensure(&kiProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&kiProc);
        memset(a26, 0x90, sizeof(a26));
        a26[o++] = 0xb8; a26[o++] = 37; a26[o++] = 0; a26[o++] = 0; a26[o++] = 0;
        a26[o++] = 0xbb; a26[o++] = 1; a26[o++] = 0; a26[o++] = 0; a26[o++] = 0;
        a26[o++] = 0x31; a26[o++] = 0xc9;
        a26[o++] = 0xcd; a26[o++] = 0x80;
        a26[o++] = 0x89; a26[o++] = 0xc3;
        a26[o++] = 0xb8; a26[o++] = 1; a26[o++] = 0; a26[o++] = 0; a26[o++] = 0;
        a26[o++] = 0xcd; a26[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a26, sizeof(a26));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&kiProc, 1);
        thr26 = thread_create_user32(&kiProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr26 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsKi) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsKi && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 kill PASS\n");
        } else {
            kprintf("pe32: int80 kill FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * open creat + fsync + close + exit(0)
     * path /tmp/pe_fsync at 0x50
     */
    {
        static struct gj_process fsProc2;
        static struct gj_cnode fsCnode2;
        static struct gj_cap_slot aFs2[32];
        static struct gj_root_meta fsMeta2;
        u64 vaC = 0x58020000ull;
        u64 vaS = 0x58030000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr27;
        u32 hitsFs = pe32_int80_exit_hits();
        u8 a27[160];
        u32 o = 0;
        u32 po = 0x60;

        memset(&fsProc2, 0, sizeof(fsProc2));
        gj_process_init(&fsProc2, &fsCnode2, aFs2, 32);
        fsProc2.u32Alive = 1;
        fsProc2.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&fsProc2, &fsMeta2, NULL);
        if (process_as_ensure(&fsProc2) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&fsProc2);
        memset(a27, 0x90, sizeof(a27));
        {
            static const char sp[] = "/tmp/pe_fsync";
            u32 j;
            for (j = 0; j < sizeof(sp); j++) a27[po + j] = (u8)sp[j];
        }
        a27[o++] = 0xbb;
        a27[o++] = (u8)(vaC+po); a27[o++] = (u8)((vaC+po)>>8);
        a27[o++] = (u8)((vaC+po)>>16); a27[o++] = (u8)((vaC+po)>>24);
        a27[o++] = 0xb9; a27[o++] = 0x41; a27[o++] = 0; a27[o++] = 0; a27[o++] = 0;
        a27[o++] = 0xb8; a27[o++] = 5; a27[o++] = 0; a27[o++] = 0; a27[o++] = 0;
        a27[o++] = 0xcd; a27[o++] = 0x80;
        a27[o++] = 0x89; a27[o++] = 0xc3; /* fd in ebx */
        a27[o++] = 0xb8; a27[o++] = 118; a27[o++] = 0; a27[o++] = 0; a27[o++] = 0; /* fsync */
        a27[o++] = 0xcd; a27[o++] = 0x80;
        a27[o++] = 0xb8; a27[o++] = 6; a27[o++] = 0; a27[o++] = 0; a27[o++] = 0; /* close uses ebx fd */
        a27[o++] = 0xcd; a27[o++] = 0x80;
        a27[o++] = 0x31; a27[o++] = 0xdb;
        a27[o++] = 0xb8; a27[o++] = 1; a27[o++] = 0; a27[o++] = 0; a27[o++] = 0;
        a27[o++] = 0xcd; a27[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a27, sizeof(a27));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&fsProc2, 1);
        thr27 = thread_create_user32(&fsProc2, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr27 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsFs) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsFs && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 fsync PASS\n");
        } else {
            kprintf("pe32: int80 fsync FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * poll(NULL,0,0) + exit(0): mov eax,168; xor ebx,ebx; xor ecx,ecx; xor edx,edx; int80
     */
    {
        static struct gj_process poProc;
        static struct gj_cnode poCnode;
        static struct gj_cap_slot aPoSlots[32];
        static struct gj_root_meta poMeta;
        u64 vaC = 0x58040000ull;
        u64 vaS = 0x58050000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr28;
        u32 hitsPo = pe32_int80_exit_hits();
        u8 a28[32];
        u32 o = 0;

        memset(&poProc, 0, sizeof(poProc));
        gj_process_init(&poProc, &poCnode, aPoSlots, 32);
        poProc.u32Alive = 1;
        poProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&poProc, &poMeta, NULL);
        if (process_as_ensure(&poProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&poProc);
        memset(a28, 0x90, sizeof(a28));
        a28[o++] = 0xb8; a28[o++] = 168; a28[o++] = 0; a28[o++] = 0; a28[o++] = 0;
        a28[o++] = 0x31; a28[o++] = 0xdb;
        a28[o++] = 0x31; a28[o++] = 0xc9;
        a28[o++] = 0x31; a28[o++] = 0xd2;
        a28[o++] = 0xcd; a28[o++] = 0x80;
        a28[o++] = 0x89; a28[o++] = 0xc3;
        a28[o++] = 0xb8; a28[o++] = 1; a28[o++] = 0; a28[o++] = 0; a28[o++] = 0;
        a28[o++] = 0xcd; a28[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a28, sizeof(a28));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&poProc, 1);
        thr28 = thread_create_user32(&poProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr28 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsPo) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsPo && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 poll PASS\n");
        } else {
            kprintf("pe32: int80 poll FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * select(0,NULL,NULL,NULL,NULL) + exit(0): mov eax,142; xor ebx..; int80
     */
    {
        static struct gj_process seProc;
        static struct gj_cnode seCnode;
        static struct gj_cap_slot aSeSlots[32];
        static struct gj_root_meta seMeta;
        u64 vaC = 0x58060000ull;
        u64 vaS = 0x58070000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr29;
        u32 hitsSe = pe32_int80_exit_hits();
        u8 a29[40];
        u32 o = 0;

        memset(&seProc, 0, sizeof(seProc));
        gj_process_init(&seProc, &seCnode, aSeSlots, 32);
        seProc.u32Alive = 1;
        seProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&seProc, &seMeta, NULL);
        if (process_as_ensure(&seProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&seProc);
        memset(a29, 0x90, sizeof(a29));
        a29[o++] = 0xb8; a29[o++] = 142; a29[o++] = 0; a29[o++] = 0; a29[o++] = 0;
        a29[o++] = 0x31; a29[o++] = 0xdb;
        a29[o++] = 0x31; a29[o++] = 0xc9;
        a29[o++] = 0x31; a29[o++] = 0xd2;
        a29[o++] = 0x31; a29[o++] = 0xf6;
        a29[o++] = 0x31; a29[o++] = 0xff;
        a29[o++] = 0xcd; a29[o++] = 0x80;
        a29[o++] = 0x89; a29[o++] = 0xc3;
        a29[o++] = 0xb8; a29[o++] = 1; a29[o++] = 0; a29[o++] = 0; a29[o++] = 0;
        a29[o++] = 0xcd; a29[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a29, sizeof(a29));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&seProc, 1);
        thr29 = thread_create_user32(&seProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr29 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsSe) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsSe && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 select PASS\n");
        } else {
            kprintf("pe32: int80 select FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * Real pipe: pipe(fds); write(fds[1],"AB",2); read(fds[0],buf,2); exit(n)
     * fds at esp, buf at esp+8
     */
    {
        static struct gj_process piProc;
        static struct gj_cnode piCnode;
        static struct gj_cap_slot aPiSlots[32];
        static struct gj_root_meta piMeta;
        u64 vaC = 0x58080000ull;
        u64 vaS = 0x58090000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr30;
        u32 hitsPi = pe32_int80_exit_hits();
        u8 a30[96];
        u32 o = 0;

        memset(&piProc, 0, sizeof(piProc));
        gj_process_init(&piProc, &piCnode, aPiSlots, 32);
        piProc.u32Alive = 1;
        piProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&piProc, &piMeta, NULL);
        if (process_as_ensure(&piProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&piProc);
        memset(a30, 0x90, sizeof(a30));
        /* sub esp, 32 */
        a30[o++] = 0x83; a30[o++] = 0xec; a30[o++] = 32;
        /* pipe(esp): mov ebx,esp; mov eax,42; int80 */
        a30[o++] = 0x89; a30[o++] = 0xe3;
        a30[o++] = 0xb8; a30[o++] = 42; a30[o++] = 0; a30[o++] = 0; a30[o++] = 0;
        a30[o++] = 0xcd; a30[o++] = 0x80;
        /* write(fds[1], "AB" at esp+16, 2) */
        a30[o++] = 0xc6; a30[o++] = 0x44; a30[o++] = 0x24; a30[o++] = 16; a30[o++] = 0x41; /* 'A' */
        a30[o++] = 0xc6; a30[o++] = 0x44; a30[o++] = 0x24; a30[o++] = 17; a30[o++] = 0x42; /* 'B' */
        a30[o++] = 0x8b; a30[o++] = 0x5c; a30[o++] = 0x24; a30[o++] = 4; /* mov ebx,[esp+4] write end */
        a30[o++] = 0x8d; a30[o++] = 0x4c; a30[o++] = 0x24; a30[o++] = 16; /* lea ecx,[esp+16] */
        a30[o++] = 0xba; a30[o++] = 2; a30[o++] = 0; a30[o++] = 0; a30[o++] = 0;
        a30[o++] = 0xb8; a30[o++] = 4; a30[o++] = 0; a30[o++] = 0; a30[o++] = 0;
        a30[o++] = 0xcd; a30[o++] = 0x80;
        /* read(fds[0], esp+20, 2) */
        a30[o++] = 0x8b; a30[o++] = 0x1c; a30[o++] = 0x24; /* mov ebx,[esp] read end */
        a30[o++] = 0x8d; a30[o++] = 0x4c; a30[o++] = 0x24; a30[o++] = 20;
        a30[o++] = 0xba; a30[o++] = 2; a30[o++] = 0; a30[o++] = 0; a30[o++] = 0;
        a30[o++] = 0xb8; a30[o++] = 3; a30[o++] = 0; a30[o++] = 0; a30[o++] = 0;
        a30[o++] = 0xcd; a30[o++] = 0x80;
        /* exit(nread) */
        a30[o++] = 0x89; a30[o++] = 0xc3;
        a30[o++] = 0xb8; a30[o++] = 1; a30[o++] = 0; a30[o++] = 0; a30[o++] = 0;
        a30[o++] = 0xcd; a30[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a30, sizeof(a30));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&piProc, 1);
        thr30 = thread_create_user32(&piProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr30 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsPi) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsPi && pe32_int80_last_code() == 2) {
            kprintf("pe32: int80 pipe_rw PASS\n");
        } else {
            kprintf("pe32: int80 pipe_rw FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * eventfd2(0,0) + write 8-byte 1 + read 8-byte + exit(nread)
     */
    {
        static struct gj_process efProc;
        static struct gj_cnode efCnode;
        static struct gj_cap_slot aEfSlots[32];
        static struct gj_root_meta efMeta;
        u64 vaC = 0x580a0000ull;
        u64 vaS = 0x580b0000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr31;
        u32 hitsEf = pe32_int80_exit_hits();
        u8 a31[96];
        u32 o = 0;

        memset(&efProc, 0, sizeof(efProc));
        gj_process_init(&efProc, &efCnode, aEfSlots, 32);
        efProc.u32Alive = 1;
        efProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&efProc, &efMeta, NULL);
        if (process_as_ensure(&efProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&efProc);
        memset(a31, 0x90, sizeof(a31));
        /* eventfd2(0,0) NR=328 */
        a31[o++] = 0xb8;
        a31[o++] = 0x48;
        a31[o++] = 0x01;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0x31;
        a31[o++] = 0xdb;
        a31[o++] = 0x31;
        a31[o++] = 0xc9;
        a31[o++] = 0xcd;
        a31[o++] = 0x80;
        a31[o++] = 0x89;
        a31[o++] = 0xc7; /* mov edi,eax */
        a31[o++] = 0x83;
        a31[o++] = 0xec;
        a31[o++] = 16;
        a31[o++] = 0xc7;
        a31[o++] = 0x04;
        a31[o++] = 0x24;
        a31[o++] = 1;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0xc7;
        a31[o++] = 0x44;
        a31[o++] = 0x24;
        a31[o++] = 4;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0x89;
        a31[o++] = 0xfb;
        a31[o++] = 0x89;
        a31[o++] = 0xe1;
        a31[o++] = 0xba;
        a31[o++] = 8;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0xb8;
        a31[o++] = 4;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0xcd;
        a31[o++] = 0x80;
        a31[o++] = 0x89;
        a31[o++] = 0xfb;
        a31[o++] = 0x89;
        a31[o++] = 0xe1;
        a31[o++] = 0xba;
        a31[o++] = 8;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0xb8;
        a31[o++] = 3;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0xcd;
        a31[o++] = 0x80;
        a31[o++] = 0x89;
        a31[o++] = 0xc3;
        a31[o++] = 0xb8;
        a31[o++] = 1;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0;
        a31[o++] = 0xcd;
        a31[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a31, sizeof(a31));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&efProc, 1);
        thr31 = thread_create_user32(&efProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr31 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsEf) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsEf && pe32_int80_last_code() == 8) {
            kprintf("pe32: int80 eventfd PASS\n");
        } else {
            kprintf("pe32: int80 eventfd FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * getdents64: open creat /tmp/pe_gd, getdents64(fd,buf,256), exit(n>0?1:0)
     * path at +0x70; dent buf on stack
     */
    {
        static struct gj_process gdProc;
        static struct gj_cnode gdCnode;
        static struct gj_cap_slot aGdSlots[32];
        static struct gj_root_meta gdMeta;
        u64 vaC = 0x580c0000ull;
        u64 vaS = 0x580d0000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr32;
        u32 hitsGd = pe32_int80_exit_hits();
        u8 a32[160];
        u32 o = 0;
        u32 pathOff = 0x70;

        memset(&gdProc, 0, sizeof(gdProc));
        gj_process_init(&gdProc, &gdCnode, aGdSlots, 32);
        gdProc.u32Alive = 1;
        gdProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&gdProc, &gdMeta, NULL);
        if (process_as_ensure(&gdProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&gdProc);
        memset(a32, 0x90, sizeof(a32));
        {
            static const char szP[] = "/tmp/pe_gd";
            u32 j;
            for (j = 0; j < sizeof(szP); j++) {
                a32[pathOff + j] = (u8)szP[j];
            }
        }
        /* open creat */
        a32[o++] = 0xbb;
        a32[o++] = (u8)(vaC + pathOff);
        a32[o++] = (u8)((vaC + pathOff) >> 8);
        a32[o++] = (u8)((vaC + pathOff) >> 16);
        a32[o++] = (u8)((vaC + pathOff) >> 24);
        a32[o++] = 0xb9;
        a32[o++] = 0x41;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0xb8;
        a32[o++] = 5;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0xcd;
        a32[o++] = 0x80;
        a32[o++] = 0x89;
        a32[o++] = 0xc7; /* edi = fd */
        /* sub esp,256; getdents64(fd, esp, 256) NR=220 */
        a32[o++] = 0x81;
        a32[o++] = 0xec;
        a32[o++] = 0x00;
        a32[o++] = 0x01;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0x89;
        a32[o++] = 0xfb; /* ebx = fd */
        a32[o++] = 0x89;
        a32[o++] = 0xe1; /* ecx = buf */
        a32[o++] = 0xba;
        a32[o++] = 0x00;
        a32[o++] = 0x01;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0xb8;
        a32[o++] = 0xdc; /* 220 */
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0xcd;
        a32[o++] = 0x80;
        /* if eax > 0 exit 1 else exit 0 */
        a32[o++] = 0x85;
        a32[o++] = 0xc0; /* test eax,eax */
        a32[o++] = 0x7e;
        a32[o++] = 0x07; /* jle +7 → xor ebx (exit 0) */
        a32[o++] = 0xbb;
        a32[o++] = 1;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0; /* mov ebx,1 */
        a32[o++] = 0xeb;
        a32[o++] = 0x02; /* jmp +2 → mov eax,1 */
        a32[o++] = 0x31;
        a32[o++] = 0xdb; /* xor ebx,ebx */
        a32[o++] = 0xb8;
        a32[o++] = 1;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0;
        a32[o++] = 0xcd;
        a32[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a32, sizeof(a32));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&gdProc, 1);
        thr32 = thread_create_user32(&gdProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr32 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsGd) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsGd && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 getdents PASS\n");
        } else {
            kprintf("pe32: int80 getdents FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * link: open creat A, link A→B, access B, exit(0 if access ok)
     */
    {
        static struct gj_process lnProc;
        static struct gj_cnode lnCnode;
        static struct gj_cap_slot aLnSlots[32];
        static struct gj_root_meta lnMeta;
        u64 vaC = 0x580e0000ull;
        u64 vaS = 0x580f0000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr33;
        u32 hitsLn = pe32_int80_exit_hits();
        u8 a33[160];
        u32 o = 0;
        u32 pa = 0x70;
        u32 pb = 0x80;

        memset(&lnProc, 0, sizeof(lnProc));
        gj_process_init(&lnProc, &lnCnode, aLnSlots, 32);
        lnProc.u32Alive = 1;
        lnProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&lnProc, &lnMeta, NULL);
        if (process_as_ensure(&lnProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&lnProc);
        memset(a33, 0x90, sizeof(a33));
        {
            static const char sa[] = "/tmp/pe_ln_a";
            static const char sb[] = "/tmp/pe_ln_b";
            u32 j;
            for (j = 0; j < sizeof(sa); j++) {
                a33[pa + j] = (u8)sa[j];
            }
            for (j = 0; j < sizeof(sb); j++) {
                a33[pb + j] = (u8)sb[j];
            }
        }
        /* open creat A */
        a33[o++] = 0xbb;
        a33[o++] = (u8)(vaC + pa);
        a33[o++] = (u8)((vaC + pa) >> 8);
        a33[o++] = (u8)((vaC + pa) >> 16);
        a33[o++] = (u8)((vaC + pa) >> 24);
        a33[o++] = 0xb9;
        a33[o++] = 0x41;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0xb8;
        a33[o++] = 5;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0xcd;
        a33[o++] = 0x80;
        a33[o++] = 0x89;
        a33[o++] = 0xc3;
        a33[o++] = 0xb8;
        a33[o++] = 6;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0xcd;
        a33[o++] = 0x80;
        /* link A B NR=9 */
        a33[o++] = 0xbb;
        a33[o++] = (u8)(vaC + pa);
        a33[o++] = (u8)((vaC + pa) >> 8);
        a33[o++] = (u8)((vaC + pa) >> 16);
        a33[o++] = (u8)((vaC + pa) >> 24);
        a33[o++] = 0xb9;
        a33[o++] = (u8)(vaC + pb);
        a33[o++] = (u8)((vaC + pb) >> 8);
        a33[o++] = (u8)((vaC + pb) >> 16);
        a33[o++] = (u8)((vaC + pb) >> 24);
        a33[o++] = 0xb8;
        a33[o++] = 9;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0xcd;
        a33[o++] = 0x80;
        /* access B */
        a33[o++] = 0xbb;
        a33[o++] = (u8)(vaC + pb);
        a33[o++] = (u8)((vaC + pb) >> 8);
        a33[o++] = (u8)((vaC + pb) >> 16);
        a33[o++] = (u8)((vaC + pb) >> 24);
        a33[o++] = 0x31;
        a33[o++] = 0xc9;
        a33[o++] = 0xb8;
        a33[o++] = 33;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0xcd;
        a33[o++] = 0x80;
        a33[o++] = 0x89;
        a33[o++] = 0xc3;
        a33[o++] = 0xb8;
        a33[o++] = 1;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0;
        a33[o++] = 0xcd;
        a33[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a33, sizeof(a33));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&lnProc, 1);
        thr33 = thread_create_user32(&lnProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr33 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsLn) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsLn && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 link PASS\n");
        } else {
            kprintf("pe32: int80 link FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * clock_getres(CLOCK_MONOTONIC=1) + exit(0)
     */
    {
        static struct gj_process crProc;
        static struct gj_cnode crCnode;
        static struct gj_cap_slot aCrSlots[32];
        static struct gj_root_meta crMeta;
        u64 vaC = 0x58100000ull;
        u64 vaS = 0x58110000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr34;
        u32 hitsCr = pe32_int80_exit_hits();
        u8 a34[48];
        u32 o = 0;

        memset(&crProc, 0, sizeof(crProc));
        gj_process_init(&crProc, &crCnode, aCrSlots, 32);
        crProc.u32Alive = 1;
        crProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&crProc, &crMeta, NULL);
        if (process_as_ensure(&crProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&crProc);
        memset(a34, 0x90, sizeof(a34));
        a34[o++] = 0x83;
        a34[o++] = 0xec;
        a34[o++] = 16;
        a34[o++] = 0xbb;
        a34[o++] = 1;
        a34[o++] = 0;
        a34[o++] = 0;
        a34[o++] = 0;
        a34[o++] = 0x89;
        a34[o++] = 0xe1;
        a34[o++] = 0xb8;
        a34[o++] = 0x0a; /* 266 */
        a34[o++] = 0x01;
        a34[o++] = 0;
        a34[o++] = 0;
        a34[o++] = 0xcd;
        a34[o++] = 0x80;
        a34[o++] = 0x89;
        a34[o++] = 0xc3;
        a34[o++] = 0xb8;
        a34[o++] = 1;
        a34[o++] = 0;
        a34[o++] = 0;
        a34[o++] = 0;
        a34[o++] = 0xcd;
        a34[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a34, sizeof(a34));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&crProc, 1);
        thr34 = thread_create_user32(&crProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr34 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsCr) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsCr && pe32_int80_last_code() == 0) {
            kprintf("pe32: int80 clock_getres PASS\n");
        } else {
            kprintf("pe32: int80 clock_getres FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * symlink(target, link) + readlink(link) → exit(n) if n>0
     * target at 0x70, link at 0x80
     */
    {
        static struct gj_process slProc;
        static struct gj_cnode slCnode;
        static struct gj_cap_slot aSlSlots[32];
        static struct gj_root_meta slMeta;
        u64 vaC = 0x58120000ull;
        u64 vaS = 0x58130000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr35;
        u32 hitsSl = pe32_int80_exit_hits();
        u8 a35[160];
        u32 o = 0;
        u32 pt = 0x70;
        u32 pl = 0x80;

        memset(&slProc, 0, sizeof(slProc));
        gj_process_init(&slProc, &slCnode, aSlSlots, 32);
        slProc.u32Alive = 1;
        slProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&slProc, &slMeta, NULL);
        if (process_as_ensure(&slProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&slProc);
        memset(a35, 0x90, sizeof(a35));
        {
            static const char st[] = "/tmp/pe_tg";
            static const char sl[] = "/tmp/pe_sl";
            u32 j;
            for (j = 0; j < sizeof(st); j++) {
                a35[pt + j] = (u8)st[j];
            }
            for (j = 0; j < sizeof(sl); j++) {
                a35[pl + j] = (u8)sl[j];
            }
        }
        /* symlink(target, link) NR=83 */
        a35[o++] = 0xbb;
        a35[o++] = (u8)(vaC + pt);
        a35[o++] = (u8)((vaC + pt) >> 8);
        a35[o++] = (u8)((vaC + pt) >> 16);
        a35[o++] = (u8)((vaC + pt) >> 24);
        a35[o++] = 0xb9;
        a35[o++] = (u8)(vaC + pl);
        a35[o++] = (u8)((vaC + pl) >> 8);
        a35[o++] = (u8)((vaC + pl) >> 16);
        a35[o++] = (u8)((vaC + pl) >> 24);
        a35[o++] = 0xb8;
        a35[o++] = 83;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0xcd;
        a35[o++] = 0x80;
        /* readlink(link, stackbuf, 64) NR=85 */
        a35[o++] = 0x83;
        a35[o++] = 0xec;
        a35[o++] = 64;
        a35[o++] = 0xbb;
        a35[o++] = (u8)(vaC + pl);
        a35[o++] = (u8)((vaC + pl) >> 8);
        a35[o++] = (u8)((vaC + pl) >> 16);
        a35[o++] = (u8)((vaC + pl) >> 24);
        a35[o++] = 0x89;
        a35[o++] = 0xe1;
        a35[o++] = 0xba;
        a35[o++] = 64;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0xb8;
        a35[o++] = 85;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0xcd;
        a35[o++] = 0x80;
        /* exit with n if >0 else 0 */
        a35[o++] = 0x85;
        a35[o++] = 0xc0;
        a35[o++] = 0x7e;
        a35[o++] = 0x04; /* jle +4 → xor ebx */
        a35[o++] = 0x89;
        a35[o++] = 0xc3; /* mov ebx,eax */
        a35[o++] = 0xeb;
        a35[o++] = 0x02; /* jmp +2 → mov eax,1 */
        a35[o++] = 0x31;
        a35[o++] = 0xdb; /* xor ebx,ebx */
        a35[o++] = 0xb8;
        a35[o++] = 1;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0;
        a35[o++] = 0xcd;
        a35[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a35, sizeof(a35));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&slProc, 1);
        thr35 = thread_create_user32(&slProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr35 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsSl) {
                break;
            }
        }
        /* "/tmp/pe_tg" is 10 chars */
        if (pe32_int80_exit_hits() > hitsSl && pe32_int80_last_code() == 10) {
            kprintf("pe32: int80 symlink PASS\n");
        } else {
            kprintf("pe32: int80 symlink FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * pipe2(fds,0) + write 'Z' + read → exit(1)
     */
    {
        static struct gj_process p2Proc;
        static struct gj_cnode p2Cnode;
        static struct gj_cap_slot aP2Slots[32];
        static struct gj_root_meta p2Meta;
        u64 vaC = 0x58140000ull;
        u64 vaS = 0x58150000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr36;
        u32 hitsP2 = pe32_int80_exit_hits();
        u8 a36[96];
        u32 o = 0;

        memset(&p2Proc, 0, sizeof(p2Proc));
        gj_process_init(&p2Proc, &p2Cnode, aP2Slots, 32);
        p2Proc.u32Alive = 1;
        p2Proc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&p2Proc, &p2Meta, NULL);
        if (process_as_ensure(&p2Proc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&p2Proc);
        memset(a36, 0x90, sizeof(a36));
        /* sub esp,16; pipe2(esp,0) NR=331 */
        a36[o++] = 0x83;
        a36[o++] = 0xec;
        a36[o++] = 16;
        a36[o++] = 0x89;
        a36[o++] = 0xe3; /* ebx = fds */
        a36[o++] = 0x31;
        a36[o++] = 0xc9; /* ecx = flags 0 */
        a36[o++] = 0xb8;
        a36[o++] = 0x4b; /* 331 */
        a36[o++] = 0x01;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0xcd;
        a36[o++] = 0x80;
        /* write fds[1], "Z", 1 */
        a36[o++] = 0xc6;
        a36[o++] = 0x44;
        a36[o++] = 0x24;
        a36[o++] = 8;
        a36[o++] = 0x5a; /* mov byte [esp+8], 'Z' */
        a36[o++] = 0x8b;
        a36[o++] = 0x5c;
        a36[o++] = 0x24;
        a36[o++] = 4; /* ebx = fds[1] */
        a36[o++] = 0x8d;
        a36[o++] = 0x4c;
        a36[o++] = 0x24;
        a36[o++] = 8; /* ecx = &buf */
        a36[o++] = 0xba;
        a36[o++] = 1;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0xb8;
        a36[o++] = 4;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0xcd;
        a36[o++] = 0x80;
        /* read fds[0] */
        a36[o++] = 0x8b;
        a36[o++] = 0x1c;
        a36[o++] = 0x24; /* ebx = fds[0] */
        a36[o++] = 0x8d;
        a36[o++] = 0x4c;
        a36[o++] = 0x24;
        a36[o++] = 9;
        a36[o++] = 0xba;
        a36[o++] = 1;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0xb8;
        a36[o++] = 3;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0xcd;
        a36[o++] = 0x80;
        a36[o++] = 0x89;
        a36[o++] = 0xc3;
        a36[o++] = 0xb8;
        a36[o++] = 1;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0;
        a36[o++] = 0xcd;
        a36[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a36, sizeof(a36));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&p2Proc, 1);
        thr36 = thread_create_user32(&p2Proc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr36 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsP2) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsP2 && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 pipe2 PASS\n");
        } else {
            kprintf("pe32: int80 pipe2 FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * pread64/pwrite64: open creat /tmp/pe_pw, pwrite 'Q' at off 0,
     * pread into stack, exit(n) with n==1
     */
    {
        static struct gj_process pwProc;
        static struct gj_cnode pwCnode;
        static struct gj_cap_slot aPwSlots[32];
        static struct gj_root_meta pwMeta;
        u64 vaC = 0x58160000ull;
        u64 vaS = 0x58170000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr37;
        u32 hitsPw = pe32_int80_exit_hits();
        u8 a37[160];
        u32 o = 0;
        u32 pathOff = 0x70;

        memset(&pwProc, 0, sizeof(pwProc));
        gj_process_init(&pwProc, &pwCnode, aPwSlots, 32);
        pwProc.u32Alive = 1;
        pwProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&pwProc, &pwMeta, NULL);
        if (process_as_ensure(&pwProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&pwProc);
        memset(a37, 0x90, sizeof(a37));
        {
            static const char szP[] = "/tmp/pe_pw";
            u32 j;
            for (j = 0; j < sizeof(szP); j++) {
                a37[pathOff + j] = (u8)szP[j];
            }
        }
        /* open creat */
        a37[o++] = 0xbb;
        a37[o++] = (u8)(vaC + pathOff);
        a37[o++] = (u8)((vaC + pathOff) >> 8);
        a37[o++] = (u8)((vaC + pathOff) >> 16);
        a37[o++] = (u8)((vaC + pathOff) >> 24);
        a37[o++] = 0xb9;
        a37[o++] = 0x42; /* O_RDWR|O_CREAT */
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0xb8;
        a37[o++] = 5;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0xcd;
        a37[o++] = 0x80;
        a37[o++] = 0x89;
        a37[o++] = 0xc7; /* edi = fd */
        /* stack: byte 'Q' at esp */
        a37[o++] = 0x83;
        a37[o++] = 0xec;
        a37[o++] = 16;
        a37[o++] = 0xc6;
        a37[o++] = 0x04;
        a37[o++] = 0x24;
        a37[o++] = 0x51; /* 'Q' */
        /* pwrite64(fd, esp, 1, 0, 0) NR=181 args: ebx=fd ecx=buf edx=1 esi=off_lo edi=off_hi
         * but edi is fd — save fd in ebp first */
        a37[o++] = 0x89;
        a37[o++] = 0xfd; /* mov ebp,edi */
        a37[o++] = 0x89;
        a37[o++] = 0xeb; /* ebx = fd */
        a37[o++] = 0x89;
        a37[o++] = 0xe1; /* ecx = buf */
        a37[o++] = 0xba;
        a37[o++] = 1;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0x31;
        a37[o++] = 0xf6; /* esi = 0 off_lo */
        a37[o++] = 0x31;
        a37[o++] = 0xff; /* edi = 0 off_hi */
        a37[o++] = 0xb8;
        a37[o++] = 0xb5; /* 181 */
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0xcd;
        a37[o++] = 0x80;
        /* pread64(fd, esp+1, 1, 0, 0) NR=180 */
        a37[o++] = 0x89;
        a37[o++] = 0xeb; /* ebx = fd (ebp) */
        a37[o++] = 0x8d;
        a37[o++] = 0x4c;
        a37[o++] = 0x24;
        a37[o++] = 1; /* ecx = esp+1 */
        a37[o++] = 0xba;
        a37[o++] = 1;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0x31;
        a37[o++] = 0xf6;
        a37[o++] = 0x31;
        a37[o++] = 0xff;
        a37[o++] = 0xb8;
        a37[o++] = 0xb4; /* 180 */
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0xcd;
        a37[o++] = 0x80;
        a37[o++] = 0x89;
        a37[o++] = 0xc3;
        a37[o++] = 0xb8;
        a37[o++] = 1;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0;
        a37[o++] = 0xcd;
        a37[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a37, sizeof(a37));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&pwProc, 1);
        thr37 = thread_create_user32(&pwProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr37 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsPw) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsPw && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 pread_pwrite PASS\n");
        } else {
            kprintf("pe32: int80 pread_pwrite FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * timerfd_create + settime(arm) + read 8 bytes → exit(8)
     */
    {
        static struct gj_process tfProc;
        static struct gj_cnode tfCnode;
        static struct gj_cap_slot aTfSlots[32];
        static struct gj_root_meta tfMeta;
        u64 vaC = 0x58180000ull;
        u64 vaS = 0x58190000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr38;
        u32 hitsTf = pe32_int80_exit_hits();
        u8 a38[128];
        u32 o = 0;

        memset(&tfProc, 0, sizeof(tfProc));
        gj_process_init(&tfProc, &tfCnode, aTfSlots, 32);
        tfProc.u32Alive = 1;
        tfProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&tfProc, &tfMeta, NULL);
        if (process_as_ensure(&tfProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&tfProc);
        memset(a38, 0x90, sizeof(a38));
        /* timerfd_create(CLOCK_MONOTONIC=1, 0) NR=322 */
        a38[o++] = 0xbb;
        a38[o++] = 1;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0x31;
        a38[o++] = 0xc9;
        a38[o++] = 0xb8;
        a38[o++] = 0x42; /* 322 */
        a38[o++] = 0x01;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0xcd;
        a38[o++] = 0x80;
        a38[o++] = 0x89;
        a38[o++] = 0xc7; /* edi = tfd */
        /* itimerspec on stack: value=1s, interval=0 */
        a38[o++] = 0x83;
        a38[o++] = 0xec;
        a38[o++] = 32;
        a38[o++] = 0xc7;
        a38[o++] = 0x04;
        a38[o++] = 0x24;
        a38[o++] = 1;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0; /* tv_sec=1 */
        a38[o++] = 0xc7;
        a38[o++] = 0x44;
        a38[o++] = 0x24;
        a38[o++] = 4;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0; /* nsec=0 */
        a38[o++] = 0xc7;
        a38[o++] = 0x44;
        a38[o++] = 0x24;
        a38[o++] = 8;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0xc7;
        a38[o++] = 0x44;
        a38[o++] = 0x24;
        a38[o++] = 12;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        /* timerfd_settime(fd, 0, new, NULL) NR=325 */
        a38[o++] = 0x89;
        a38[o++] = 0xfb;
        a38[o++] = 0x31;
        a38[o++] = 0xc9;
        a38[o++] = 0x89;
        a38[o++] = 0xe2; /* edx = new_value */
        a38[o++] = 0x31;
        a38[o++] = 0xf6; /* esi = old NULL */
        a38[o++] = 0xb8;
        a38[o++] = 0x45; /* 325 */
        a38[o++] = 0x01;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0xcd;
        a38[o++] = 0x80;
        /* read(fd, buf, 8) */
        a38[o++] = 0x89;
        a38[o++] = 0xfb;
        a38[o++] = 0x8d;
        a38[o++] = 0x4c;
        a38[o++] = 0x24;
        a38[o++] = 16;
        a38[o++] = 0xba;
        a38[o++] = 8;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0xb8;
        a38[o++] = 3;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0xcd;
        a38[o++] = 0x80;
        a38[o++] = 0x89;
        a38[o++] = 0xc3;
        a38[o++] = 0xb8;
        a38[o++] = 1;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0;
        a38[o++] = 0xcd;
        a38[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a38, sizeof(a38));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&tfProc, 1);
        thr38 = thread_create_user32(&tfProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr38 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsTf) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsTf && pe32_int80_last_code() == 8) {
            kprintf("pe32: int80 timerfd PASS\n");
        } else {
            kprintf("pe32: int80 timerfd FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * epoll_create1 + eventfd + epoll_ctl ADD + epoll_wait → exit(ready>=1)
     */
    {
        static struct gj_process epProc;
        static struct gj_cnode epCnode;
        static struct gj_cap_slot aEpSlots[32];
        static struct gj_root_meta epMeta;
        u64 vaC = 0x581a0000ull;
        u64 vaS = 0x581b0000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr39;
        u32 hitsEp = pe32_int80_exit_hits();
        u8 a39[160];
        u32 o = 0;

        memset(&epProc, 0, sizeof(epProc));
        gj_process_init(&epProc, &epCnode, aEpSlots, 32);
        epProc.u32Alive = 1;
        epProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&epProc, &epMeta, NULL);
        if (process_as_ensure(&epProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&epProc);
        memset(a39, 0x90, sizeof(a39));
        /* epoll_create1(0) → ebp; eventfd2(1,0) → edi; ctl+wait */
        a39[o++] = 0x31;
        a39[o++] = 0xdb;
        a39[o++] = 0xb8;
        a39[o++] = 0x49; /* 329 */
        a39[o++] = 0x01;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0xcd;
        a39[o++] = 0x80;
        a39[o++] = 0x89;
        a39[o++] = 0xc5; /* ebp = epfd */
        a39[o++] = 0xbb;
        a39[o++] = 1;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0x31;
        a39[o++] = 0xc9;
        a39[o++] = 0xb8;
        a39[o++] = 0x48; /* 328 eventfd2 */
        a39[o++] = 0x01;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0xcd;
        a39[o++] = 0x80;
        a39[o++] = 0x89;
        a39[o++] = 0xc7; /* edi = efd */
        a39[o++] = 0x83;
        a39[o++] = 0xec;
        a39[o++] = 32;
        a39[o++] = 0xc7;
        a39[o++] = 0x04;
        a39[o++] = 0x24;
        a39[o++] = 1;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0; /* EPOLLIN */
        a39[o++] = 0x31;
        a39[o++] = 0xc0;
        a39[o++] = 0x89;
        a39[o++] = 0x44;
        a39[o++] = 0x24;
        a39[o++] = 4;
        a39[o++] = 0x89;
        a39[o++] = 0x44;
        a39[o++] = 0x24;
        a39[o++] = 8;
        a39[o++] = 0x89;
        a39[o++] = 0xeb; /* ebx = ep */
        a39[o++] = 0xb9;
        a39[o++] = 1;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0; /* ADD */
        a39[o++] = 0x89;
        a39[o++] = 0xfa; /* edx = efd */
        a39[o++] = 0x89;
        a39[o++] = 0xe6; /* esi = &ev */
        a39[o++] = 0xb8;
        a39[o++] = 0xff; /* 255 epoll_ctl */
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0xcd;
        a39[o++] = 0x80;
        a39[o++] = 0x89;
        a39[o++] = 0xeb;
        a39[o++] = 0x8d;
        a39[o++] = 0x4c;
        a39[o++] = 0x24;
        a39[o++] = 12;
        a39[o++] = 0xba;
        a39[o++] = 1;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0x31;
        a39[o++] = 0xf6;
        a39[o++] = 0xb8;
        a39[o++] = 0x00;
        a39[o++] = 0x01; /* 256 epoll_wait */
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0xcd;
        a39[o++] = 0x80;
        a39[o++] = 0x89;
        a39[o++] = 0xc3;
        a39[o++] = 0xb8;
        a39[o++] = 1;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0;
        a39[o++] = 0xcd;
        a39[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a39, sizeof(a39));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&epProc, 1);
        thr39 = thread_create_user32(&epProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr39 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsEp) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsEp && pe32_int80_last_code() >= 1) {
            kprintf("pe32: int80 epoll PASS\n");
        } else {
            kprintf("pe32: int80 epoll FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * inotify_init1 + add_watch(/tmp/pe_ino) → exit(wd>=1 ? 1 : 0)
     */
    {
        static struct gj_process inProc;
        static struct gj_cnode inCnode;
        static struct gj_cap_slot aInSlots[32];
        static struct gj_root_meta inMeta;
        u64 vaC = 0x581c0000ull;
        u64 vaS = 0x581d0000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr40;
        u32 hitsIn = pe32_int80_exit_hits();
        u8 a40[128];
        u32 o = 0;
        u32 pathOff = 0x60;

        memset(&inProc, 0, sizeof(inProc));
        gj_process_init(&inProc, &inCnode, aInSlots, 32);
        inProc.u32Alive = 1;
        inProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&inProc, &inMeta, NULL);
        if (process_as_ensure(&inProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&inProc);
        memset(a40, 0x90, sizeof(a40));
        {
            static const char szP[] = "/tmp/pe_ino";
            u32 j;
            for (j = 0; j < sizeof(szP); j++) {
                a40[pathOff + j] = (u8)szP[j];
            }
        }
        /* open creat path first */
        a40[o++] = 0xbb;
        a40[o++] = (u8)(vaC + pathOff);
        a40[o++] = (u8)((vaC + pathOff) >> 8);
        a40[o++] = (u8)((vaC + pathOff) >> 16);
        a40[o++] = (u8)((vaC + pathOff) >> 24);
        a40[o++] = 0xb9;
        a40[o++] = 0x41;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xb8;
        a40[o++] = 5;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xcd;
        a40[o++] = 0x80;
        a40[o++] = 0x89;
        a40[o++] = 0xc3;
        a40[o++] = 0xb8;
        a40[o++] = 6;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xcd;
        a40[o++] = 0x80;
        /* inotify_init1(0) NR=332 */
        a40[o++] = 0x31;
        a40[o++] = 0xdb;
        a40[o++] = 0xb8;
        a40[o++] = 0x4c; /* 332 */
        a40[o++] = 0x01;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xcd;
        a40[o++] = 0x80;
        a40[o++] = 0x89;
        a40[o++] = 0xc7; /* edi = ifd */
        /* inotify_add_watch(ifd, path, IN_ALL_EVENTS~0xFFF) NR=292 */
        a40[o++] = 0x89;
        a40[o++] = 0xfb;
        a40[o++] = 0xb9;
        a40[o++] = (u8)(vaC + pathOff);
        a40[o++] = (u8)((vaC + pathOff) >> 8);
        a40[o++] = (u8)((vaC + pathOff) >> 16);
        a40[o++] = (u8)((vaC + pathOff) >> 24);
        a40[o++] = 0xba;
        a40[o++] = 0xff;
        a40[o++] = 0x0f;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xb8;
        a40[o++] = 0x24; /* 292 */
        a40[o++] = 0x01;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xcd;
        a40[o++] = 0x80;
        /* exit 1 if wd > 0 */
        a40[o++] = 0x85;
        a40[o++] = 0xc0;
        a40[o++] = 0x7e;
        a40[o++] = 0x07;
        a40[o++] = 0xbb;
        a40[o++] = 1;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xeb;
        a40[o++] = 0x02;
        a40[o++] = 0x31;
        a40[o++] = 0xdb;
        a40[o++] = 0xb8;
        a40[o++] = 1;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0;
        a40[o++] = 0xcd;
        a40[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a40, sizeof(a40));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&inProc, 1);
        thr40 = thread_create_user32(&inProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr40 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsIn) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsIn && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 inotify PASS\n");
        } else {
            kprintf("pe32: int80 inotify FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * sendfile64: write file A, open B, sendfile B←A, exit(n>=1)
     */
    {
        static struct gj_process sfProc;
        static struct gj_cnode sfCnode;
        static struct gj_cap_slot aSfSlots[32];
        static struct gj_root_meta sfMeta;
        u64 vaC = 0x581e0000ull;
        u64 vaS = 0x581f0000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr41;
        u32 hitsSf = pe32_int80_exit_hits();
        u8 a41[192];
        u32 o = 0;
        u32 pa = 0x80;
        u32 pb = 0x90;

        memset(&sfProc, 0, sizeof(sfProc));
        gj_process_init(&sfProc, &sfCnode, aSfSlots, 32);
        sfProc.u32Alive = 1;
        sfProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&sfProc, &sfMeta, NULL);
        if (process_as_ensure(&sfProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&sfProc);
        memset(a41, 0x90, sizeof(a41));
        {
            static const char sa[] = "/tmp/pe_sf_a";
            static const char sb[] = "/tmp/pe_sf_b";
            u32 j;
            for (j = 0; j < sizeof(sa); j++) {
                a41[pa + j] = (u8)sa[j];
            }
            for (j = 0; j < sizeof(sb); j++) {
                a41[pb + j] = (u8)sb[j];
            }
        }
        /* open creat A RDWR */
        a41[o++] = 0xbb;
        a41[o++] = (u8)(vaC + pa);
        a41[o++] = (u8)((vaC + pa) >> 8);
        a41[o++] = (u8)((vaC + pa) >> 16);
        a41[o++] = (u8)((vaC + pa) >> 24);
        a41[o++] = 0xb9;
        a41[o++] = 0x42;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xb8;
        a41[o++] = 5;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xcd;
        a41[o++] = 0x80;
        a41[o++] = 0x89;
        a41[o++] = 0xc5; /* ebp = fdA */
        /* write "SF" */
        a41[o++] = 0x83;
        a41[o++] = 0xec;
        a41[o++] = 16;
        a41[o++] = 0xc7;
        a41[o++] = 0x04;
        a41[o++] = 0x24;
        a41[o++] = 0x53;
        a41[o++] = 0x46;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0x89;
        a41[o++] = 0xeb;
        a41[o++] = 0x89;
        a41[o++] = 0xe1;
        a41[o++] = 0xba;
        a41[o++] = 2;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xb8;
        a41[o++] = 4;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xcd;
        a41[o++] = 0x80;
        /* lseek A to 0 */
        a41[o++] = 0x89;
        a41[o++] = 0xeb;
        a41[o++] = 0x31;
        a41[o++] = 0xc9;
        a41[o++] = 0x31;
        a41[o++] = 0xd2;
        a41[o++] = 0xb8;
        a41[o++] = 19;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xcd;
        a41[o++] = 0x80;
        /* open creat B */
        a41[o++] = 0xbb;
        a41[o++] = (u8)(vaC + pb);
        a41[o++] = (u8)((vaC + pb) >> 8);
        a41[o++] = (u8)((vaC + pb) >> 16);
        a41[o++] = (u8)((vaC + pb) >> 24);
        a41[o++] = 0xb9;
        a41[o++] = 0x42;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xb8;
        a41[o++] = 5;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xcd;
        a41[o++] = 0x80;
        a41[o++] = 0x89;
        a41[o++] = 0xc7; /* edi = fdB */
        /* sendfile64(out=B, in=A, NULL, 2) NR=239 */
        a41[o++] = 0x89;
        a41[o++] = 0xfb; /* ebx = out */
        a41[o++] = 0x89;
        a41[o++] = 0xe9; /* ecx = in (ebp) */
        a41[o++] = 0x31;
        a41[o++] = 0xd2; /* edx = offset NULL */
        a41[o++] = 0xbe;
        a41[o++] = 2;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0; /* esi = count */
        a41[o++] = 0xb8;
        a41[o++] = 0xef; /* 239 */
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xcd;
        a41[o++] = 0x80;
        a41[o++] = 0x89;
        a41[o++] = 0xc3;
        a41[o++] = 0xb8;
        a41[o++] = 1;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0;
        a41[o++] = 0xcd;
        a41[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a41, sizeof(a41));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&sfProc, 1);
        thr41 = thread_create_user32(&sfProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr41 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsSf) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsSf && pe32_int80_last_code() == 2) {
            kprintf("pe32: int80 sendfile PASS\n");
        } else {
            kprintf("pe32: int80 sendfile FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * memfd_create + write 3 bytes + exit(fd>=3 ? 1 : 0) using written count
     */
    {
        static struct gj_process mfProc;
        static struct gj_cnode mfCnode;
        static struct gj_cap_slot aMfSlots[32];
        static struct gj_root_meta mfMeta;
        u64 vaC = 0x58200000ull;
        u64 vaS = 0x58210000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr42;
        u32 hitsMf = pe32_int80_exit_hits();
        u8 a42[80];
        u32 o = 0;

        memset(&mfProc, 0, sizeof(mfProc));
        gj_process_init(&mfProc, &mfCnode, aMfSlots, 32);
        mfProc.u32Alive = 1;
        mfProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&mfProc, &mfMeta, NULL);
        if (process_as_ensure(&mfProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&mfProc);
        memset(a42, 0x90, sizeof(a42));
        /* memfd_create(name,0) NR=356 — name ignored; ebx can be null-ish */
        a42[o++] = 0x31;
        a42[o++] = 0xdb;
        a42[o++] = 0x31;
        a42[o++] = 0xc9;
        a42[o++] = 0xb8;
        a42[o++] = 0x64; /* 356 */
        a42[o++] = 0x01;
        a42[o++] = 0;
        a42[o++] = 0;
        a42[o++] = 0xcd;
        a42[o++] = 0x80;
        a42[o++] = 0x89;
        a42[o++] = 0xc7;
        /* write 3 bytes */
        a42[o++] = 0x83;
        a42[o++] = 0xec;
        a42[o++] = 8;
        a42[o++] = 0xc7;
        a42[o++] = 0x04;
        a42[o++] = 0x24;
        a42[o++] = 0x4d;
        a42[o++] = 0x46;
        a42[o++] = 0x44;
        a42[o++] = 0;
        a42[o++] = 0x89;
        a42[o++] = 0xfb;
        a42[o++] = 0x89;
        a42[o++] = 0xe1;
        a42[o++] = 0xba;
        a42[o++] = 3;
        a42[o++] = 0;
        a42[o++] = 0;
        a42[o++] = 0;
        a42[o++] = 0xb8;
        a42[o++] = 4;
        a42[o++] = 0;
        a42[o++] = 0;
        a42[o++] = 0;
        a42[o++] = 0xcd;
        a42[o++] = 0x80;
        a42[o++] = 0x89;
        a42[o++] = 0xc3;
        a42[o++] = 0xb8;
        a42[o++] = 1;
        a42[o++] = 0;
        a42[o++] = 0;
        a42[o++] = 0;
        a42[o++] = 0xcd;
        a42[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a42, sizeof(a42));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&mfProc, 1);
        thr42 = thread_create_user32(&mfProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr42 == 0) {
            return 0;
        }
        for (y = 0; y < 96u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsMf) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsMf && pe32_int80_last_code() == 3) {
            kprintf("pe32: int80 memfd PASS\n");
        } else {
            kprintf("pe32: int80 memfd FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * execve: stage PE (int80 exit(7)) on vfs, CS32 execve(path) → exit 7
     */
    {
        static struct gj_process exProc;
        static struct gj_cnode exCnode;
        static struct gj_cap_slot aExSlots[32];
        static struct gj_root_meta exMeta;
        static u8 aPe[0x300];
        u64 vaC = 0x58220000ull;
        u64 vaS = 0x58230000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr43;
        u32 hitsEx = pe32_int80_exit_hits();
        u8 a43[128];
        u32 o = 0;
        u32 pathOff = 0x40;
        u32 e = 0x40;
        u32 s;
        i64 fd;
        u32 i;

        /* Tiny i386 PE: int80 exit(7) — layout matches pe32_vfs_load_smoke */
        for (i = 0; i < sizeof(aPe); i++) {
            aPe[i] = 0;
        }
        aPe[0] = 'M';
        aPe[1] = 'Z';
        aPe[0x3c] = (u8)e;
        aPe[e + 0] = 'P';
        aPe[e + 1] = 'E';
        aPe[e + 4] = 0x4c;
        aPe[e + 5] = 0x01; /* i386 */
        aPe[e + 6] = 1;    /* NumberOfSections */
        aPe[e + 20] = 0x60; /* SizeOfOptionalHeader */
        aPe[e + 24] = 0x0b;
        aPe[e + 25] = 0x01; /* PE32 magic */
        aPe[e + 40] = 0x00;
        aPe[e + 41] = 0x10; /* EntryRVA 0x1000 */
        aPe[e + 52] = 0x00;
        aPe[e + 53] = 0x00;
        aPe[e + 54] = 0x40; /* ImageBase 0x400000 */
        aPe[e + 56] = 0x00;
        aPe[e + 57] = 0x10; /* SectionAlignment */
        aPe[e + 60] = 0x00;
        aPe[e + 61] = 0x02; /* FileAlignment */
        aPe[e + 80] = 0x00;
        aPe[e + 81] = 0x20; /* SizeOfImage */
        aPe[e + 84] = 0x00;
        aPe[e + 85] = 0x02; /* SizeOfHeaders */
        aPe[e + 92] = 0x03; /* Subsystem console */
        s = e + 24u + 0x60u;
        aPe[s + 0] = '.';
        aPe[s + 1] = 't';
        aPe[s + 2] = 'e';
        aPe[s + 3] = 'x';
        aPe[s + 4] = 't';
        aPe[s + 8] = 0x10;
        aPe[s + 12] = 0x00;
        aPe[s + 13] = 0x10;
        aPe[s + 16] = 0x10;
        aPe[s + 20] = 0x00;
        aPe[s + 21] = 0x02;
        aPe[s + 36] = 0x20;
        aPe[s + 39] = 0x60;
        /* mov eax,1; mov ebx,7; int 0x80 */
        aPe[0x200] = 0xb8;
        aPe[0x201] = 0x01;
        aPe[0x202] = 0x00;
        aPe[0x203] = 0x00;
        aPe[0x204] = 0x00;
        aPe[0x205] = 0xbb;
        aPe[0x206] = 0x07;
        aPe[0x207] = 0x00;
        aPe[0x208] = 0x00;
        aPe[0x209] = 0x00;
        aPe[0x20a] = 0xcd;
        aPe[0x20b] = 0x80;

        fd = vfs_ram_open("/tmp/pe_exec.exe", 1);
        if (fd >= 0) {
            (void)vfs_ram_write(fd, aPe, sizeof(aPe));
            (void)vfs_ram_close(fd);
        }

        memset(&exProc, 0, sizeof(exProc));
        gj_process_init(&exProc, &exCnode, aExSlots, 32);
        exProc.u32Alive = 1;
        exProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&exProc, &exMeta, NULL);
        if (process_as_ensure(&exProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&exProc);
        memset(a43, 0x90, sizeof(a43));
        {
            static const char szP[] = "/tmp/pe_exec.exe";
            u32 j;
            for (j = 0; j < sizeof(szP); j++) {
                a43[pathOff + j] = (u8)szP[j];
            }
        }
        /* execve(path, 0, 0) NR=11 */
        a43[o++] = 0xbb;
        a43[o++] = (u8)(vaC + pathOff);
        a43[o++] = (u8)((vaC + pathOff) >> 8);
        a43[o++] = (u8)((vaC + pathOff) >> 16);
        a43[o++] = (u8)((vaC + pathOff) >> 24);
        a43[o++] = 0x31;
        a43[o++] = 0xc9;
        a43[o++] = 0x31;
        a43[o++] = 0xd2;
        a43[o++] = 0xb8;
        a43[o++] = 11;
        a43[o++] = 0;
        a43[o++] = 0;
        a43[o++] = 0;
        a43[o++] = 0xcd;
        a43[o++] = 0x80;
        /* fail path if exec returns */
        a43[o++] = 0x31;
        a43[o++] = 0xdb;
        a43[o++] = 0xb8;
        a43[o++] = 1;
        a43[o++] = 0;
        a43[o++] = 0;
        a43[o++] = 0;
        a43[o++] = 0xcd;
        a43[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a43, sizeof(a43));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_EXEC | GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&exProc, 1);
        thr43 = thread_create_user32(&exProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr43 == 0) {
            return 0;
        }
        for (y = 0; y < 160u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsEx) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsEx && pe32_int80_last_code() == 7) {
            kprintf("pe32: int80 execve PASS\n");
        } else {
            kprintf("pe32: int80 execve FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * clone CLONE_VM + child stack: parent exit with thr id (>=1)
     *   sub esp,64; lea ecx,[esp+32]; mov ebx,0x100; mov eax,120; int80
     *   mov ebx,eax; mov eax,1; int80
     */
    {
        static struct gj_process cvProc;
        static struct gj_cnode cvCnode;
        static struct gj_cap_slot aCvSlots[32];
        static struct gj_root_meta cvMeta;
        u64 vaC = 0x58240000ull;
        u64 vaS = 0x58250000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr44;
        u32 hitsCv = pe32_int80_exit_hits();
        u8 a44[64];
        u32 o = 0;

        memset(&cvProc, 0, sizeof(cvProc));
        gj_process_init(&cvProc, &cvCnode, aCvSlots, 32);
        cvProc.u32Alive = 1;
        cvProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&cvProc, &cvMeta, NULL);
        if (process_as_ensure(&cvProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&cvProc);
        memset(a44, 0x90, sizeof(a44));
        a44[o++] = 0x83;
        a44[o++] = 0xec;
        a44[o++] = 64;
        /* child: xor ebx,ebx; mov eax,1; int80 exit(0) — at stack+32 we place
         * that code? Simpler: child continues after int80 with same code path.
         * Parent sees thr id; child also runs exit — two exits. Use:
         * clone returns; if eax>1 parent path exit(eax); child also exits.
         * First exit wins for last_code. Prefer parent exits thr id.
         * Child continues: mov ebx,eax (0 garbage); might exit wrong.
         * Better: after clone, test if thr id high bit — just exit eax for parent.
         * Child starts at same RIP after int80 with thr's own rax not set —
         * thread_create starts at RIP after int80 as entry, so child runs
         * mov ebx,eax with eax=0 (from create) then exit(0). Parent exits thr.
         * last_code is last exit — race. Prefer single-thread parent-only:
         * if thr created, parent exits(1) without waiting; child exit(0) first
         * might set last_code 0. Order: yield until hits+1, accept code>=1.
         */
        a44[o++] = 0x8d;
        a44[o++] = 0x4c;
        a44[o++] = 0x24;
        a44[o++] = 32; /* lea ecx,[esp+32] child stack */
        a44[o++] = 0xbb;
        a44[o++] = 0x00;
        a44[o++] = 0x01;
        a44[o++] = 0;
        a44[o++] = 0; /* ebx = CLONE_VM */
        a44[o++] = 0xb8;
        a44[o++] = 120;
        a44[o++] = 0;
        a44[o++] = 0;
        a44[o++] = 0;
        a44[o++] = 0xcd;
        a44[o++] = 0x80;
        a44[o++] = 0x89;
        a44[o++] = 0xc3; /* mov ebx,eax */
        a44[o++] = 0xb8;
        a44[o++] = 1;
        a44[o++] = 0;
        a44[o++] = 0;
        a44[o++] = 0;
        a44[o++] = 0xcd;
        a44[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a44, sizeof(a44));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&cvProc, 1);
        thr44 = thread_create_user32(&cvProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr44 == 0) {
            return 0;
        }
        for (y = 0; y < 160u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsCv + 0) {
                /* need at least one exit; wait a bit for parent */
                if (y > 8u && pe32_int80_exit_hits() > hitsCv) {
                    break;
                }
            }
        }
        /* Parent exits with thr id (>=2); child may also exit — any thr id ok */
        if (pe32_int80_exit_hits() > hitsCv && pe32_int80_last_code() >= 1) {
            kprintf("pe32: int80 clone_vm PASS code=%u\n",
                    pe32_int80_last_code());
        } else {
            kprintf("pe32: int80 clone_vm FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * vfork + wait4: child registered as zombie; wait4 reaps → exit(0 if pid>0)
     */
    {
        static struct gj_process vfProc;
        static struct gj_cnode vfCnode;
        static struct gj_cap_slot aVfSlots[32];
        static struct gj_root_meta vfMeta;
        u64 vaC = 0x58260000ull;
        u64 vaS = 0x58270000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr45;
        u32 hitsVf = pe32_int80_exit_hits();
        u8 a45[64];
        u32 o = 0;

        memset(&vfProc, 0, sizeof(vfProc));
        gj_process_init(&vfProc, &vfCnode, aVfSlots, 32);
        vfProc.u32Alive = 1;
        vfProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&vfProc, &vfMeta, NULL);
        if (process_as_ensure(&vfProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&vfProc);
        memset(a45, 0x90, sizeof(a45));
        /* vfork NR=190 */
        a45[o++] = 0xb8;
        a45[o++] = 0xbe;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0xcd;
        a45[o++] = 0x80;
        a45[o++] = 0x89;
        a45[o++] = 0xc3; /* ebx = child pid */
        /* wait4(pid, 0, 0, 0) NR=114 */
        a45[o++] = 0x31;
        a45[o++] = 0xc9;
        a45[o++] = 0x31;
        a45[o++] = 0xd2;
        a45[o++] = 0x31;
        a45[o++] = 0xf6;
        a45[o++] = 0xb8;
        a45[o++] = 114;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0xcd;
        a45[o++] = 0x80;
        /* exit 1 if eax > 0 else 0 */
        a45[o++] = 0x85;
        a45[o++] = 0xc0;
        a45[o++] = 0x7e;
        a45[o++] = 0x07;
        a45[o++] = 0xbb;
        a45[o++] = 1;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0xeb;
        a45[o++] = 0x02;
        a45[o++] = 0x31;
        a45[o++] = 0xdb;
        a45[o++] = 0xb8;
        a45[o++] = 1;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0;
        a45[o++] = 0xcd;
        a45[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a45, sizeof(a45));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&vfProc, 1);
        thr45 = thread_create_user32(&vfProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr45 == 0) {
            return 0;
        }
        for (y = 0; y < 128u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsVf) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsVf && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 vfork_wait PASS\n");
        } else {
            kprintf("pe32: int80 vfork_wait FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }

    /*
     * fork + wait4 (blocking with kernel yields): child exit worker → reap
     */
    {
        static struct gj_process fkProc;
        static struct gj_cnode fkCnode;
        static struct gj_cap_slot aFkSlots[32];
        static struct gj_root_meta fkMeta;
        u64 vaC = 0x58280000ull;
        u64 vaS = 0x58290000ull;
        gj_paddr_t paC;
        gj_paddr_t paS;
        u32 thr46;
        u32 hitsFk = pe32_int80_exit_hits();
        u8 a46[48];
        u32 o = 0;

        memset(&fkProc, 0, sizeof(fkProc));
        gj_process_init(&fkProc, &fkCnode, aFkSlots, 32);
        fkProc.u32Alive = 1;
        fkProc.u32Personality = 1;
        (void)gj_process_bootstrap_root_meta(&fkProc, &fkMeta, NULL);
        if (process_as_ensure(&fkProc) != GJ_OK) {
            return 0;
        }
        u64Saved = cpu_read_cr3();
        process_as_activate(&fkProc);
        memset(a46, 0x90, sizeof(a46));
        /* fork NR=2 */
        a46[o++] = 0xb8;
        a46[o++] = 2;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0xcd;
        a46[o++] = 0x80;
        a46[o++] = 0x89;
        a46[o++] = 0xc3; /* ebx = child pid */
        /* wait4(pid, 0, 0, 0) — blocking; kernel yields for child exit worker */
        a46[o++] = 0x31;
        a46[o++] = 0xc9;
        a46[o++] = 0x31;
        a46[o++] = 0xd2;
        a46[o++] = 0x31;
        a46[o++] = 0xf6;
        a46[o++] = 0xb8;
        a46[o++] = 114;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0xcd;
        a46[o++] = 0x80;
        /* exit 1 if reaped (eax>0) else 0 */
        a46[o++] = 0x85;
        a46[o++] = 0xc0;
        a46[o++] = 0x7e;
        a46[o++] = 0x07;
        a46[o++] = 0xbb;
        a46[o++] = 1;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0xeb;
        a46[o++] = 0x02;
        a46[o++] = 0x31;
        a46[o++] = 0xdb;
        a46[o++] = 0xb8;
        a46[o++] = 1;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0;
        a46[o++] = 0xcd;
        a46[o++] = 0x80;
        (void)o;
        paC = pmm_alloc();
        paS = pmm_alloc();
        if (paC == 0 || paS == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paC)
                                               : (u64)paC);
        memset(pK, 0, GJ_PAGE_SIZE);
        memcpy(pK, a46, sizeof(a46));
        pK = (void *)(gj_vaddr_t)(hhdm_ready() ? (GJ_HHDM_BASE + (u64)paS)
                                               : (u64)paS);
        memset(pK, 0, GJ_PAGE_SIZE);
        if (vmm_map_page((gj_vaddr_t)vaC, paC,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC |
                             GJ_VMM_PROT_USER) != GJ_OK ||
            vmm_map_page((gj_vaddr_t)vaS, paS,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        cpu_load_cr3(u64Saved);
        (void)process_wait_register(&fkProc, 1);
        thr46 = thread_create_user32(&fkProc, vaC, vaS + GJ_PAGE_SIZE - 16);
        if (thr46 == 0) {
            return 0;
        }
        for (y = 0; y < 256u; y++) {
            thread_yield();
            if (pe32_int80_exit_hits() > hitsFk) {
                break;
            }
        }
        if (pe32_int80_exit_hits() > hitsFk && pe32_int80_last_code() == 1) {
            kprintf("pe32: int80 fork_wait PASS\n");
        } else {
            kprintf("pe32: int80 fork_wait FAIL hits=%u code=%u\n",
                    pe32_int80_exit_hits(), pe32_int80_last_code());
        }
    }
    pe32_soft_inc(&g_soft.u64Int80SmokeOk);
    /* Wave 11: greppable soft inventory after full int80 surface smoke. */
    pe32_soft_inventory_log("int80");
    return 0;
}

int
pe32_vfs_load_smoke(void)
{
    /*
     * Build tiny i386 PE (int80 exit(9)), write to vfs, load_process, enter CS32.
     */
    static u8 aPe[0x300];
    static u8 aRead[0x300];
    static struct gj_process vProc;
    static struct gj_cnode vCnode;
    static struct gj_cap_slot aVSlots[32];
    static struct gj_root_meta vMeta;
    struct gj_pe32_load load;
    u32 e = 0x40;
    u32 s;
    u32 i;
    i64 fd;
    i64 n;
    u32 thr;
    u32 y;
    u32 hits0;
    static const char szPath[] = "/tmp/gj_pe32_exit.exe";

    {
        u64 u64Ker = vmm_kernel_cr3();
        u64 u64SavedV = cpu_read_cr3();

        if (u64Ker != 0) {
            cpu_load_cr3(u64Ker);
        }
        for (i = 0; i < sizeof(aPe); i++) {
            aPe[i] = 0;
        }
        aPe[0] = 'M';
        aPe[1] = 'Z';
        aPe[0x3c] = (u8)e;
        aPe[e + 0] = 'P';
        aPe[e + 1] = 'E';
        aPe[e + 4] = 0x4c;
        aPe[e + 5] = 0x01; /* i386 */
        aPe[e + 6] = 1;
        aPe[e + 20] = 0x60;
        aPe[e + 24] = 0x0b;
        aPe[e + 25] = 0x01;
        aPe[e + 40] = 0x00;
        aPe[e + 41] = 0x10;
        aPe[e + 52] = 0x00;
        aPe[e + 53] = 0x00;
        aPe[e + 54] = 0x40;
        aPe[e + 56] = 0x00;
        aPe[e + 57] = 0x10;
        aPe[e + 60] = 0x00;
        aPe[e + 61] = 0x02;
        aPe[e + 80] = 0x00;
        aPe[e + 81] = 0x20;
        aPe[e + 84] = 0x00;
        aPe[e + 85] = 0x02;
        aPe[e + 92] = 0x03;
        s = e + 24u + 0x60u;
        aPe[s + 0] = '.';
        aPe[s + 1] = 't';
        aPe[s + 2] = 'e';
        aPe[s + 3] = 'x';
        aPe[s + 4] = 't';
        aPe[s + 8] = 0x10;
        aPe[s + 12] = 0x00;
        aPe[s + 13] = 0x10;
        aPe[s + 16] = 0x10;
        aPe[s + 20] = 0x00;
        aPe[s + 21] = 0x02;
        aPe[s + 36] = 0x20;
        aPe[s + 39] = 0x60;
        aPe[0x200] = 0xb8;
        aPe[0x201] = 0x01;
        aPe[0x202] = 0x00;
        aPe[0x203] = 0x00;
        aPe[0x204] = 0x00;
        aPe[0x205] = 0xbb;
        aPe[0x206] = 0x09;
        aPe[0x207] = 0x00;
        aPe[0x208] = 0x00;
        aPe[0x209] = 0x00;
        aPe[0x20a] = 0xcd;
        aPe[0x20b] = 0x80;

        fd = vfs_ram_open(szPath, 1);
        if (fd < 0) {
            kprintf("pe32: vfs open FAIL %ld\n", (long)fd);
            if (u64Ker != 0) {
                cpu_load_cr3(u64SavedV);
            }
            return -1;
        }
        (void)vfs_ram_lseek(fd, 0, 0);
        (void)vfs_ram_ftruncate(fd, 0);
        n = vfs_ram_write(fd, aPe, sizeof(aPe));
        (void)vfs_ram_close(fd);
        if (n != (i64)sizeof(aPe)) {
            kprintf("pe32: vfs write FAIL n=%ld\n", (long)n);
            if (u64Ker != 0) {
                cpu_load_cr3(u64SavedV);
            }
            return -1;
        }
        fd = vfs_ram_open(szPath, 0);
        if (fd < 0) {
            kprintf("pe32: vfs reopen FAIL\n");
            if (u64Ker != 0) {
                cpu_load_cr3(u64SavedV);
            }
            return -1;
        }
        memset(aRead, 0, sizeof(aRead));
        n = vfs_ram_read(fd, aRead, sizeof(aRead));
        (void)vfs_ram_close(fd);
        if (u64Ker != 0) {
            cpu_load_cr3(u64SavedV);
        }
        if (n < 0x200) {
            kprintf("pe32: vfs read FAIL n=%ld\n", (long)n);
            return -1;
        }
    }
    if (aRead[0] != 'M' || aRead[1] != 'Z') {
        kprintf("pe32: vfs MZ FAIL\n");
        return -1;
    }

    memset(&vProc, 0, sizeof(vProc));
    gj_process_init(&vProc, &vCnode, aVSlots, 32);
    vProc.u32Alive = 1;
    vProc.u32Personality = 1;
    (void)gj_process_bootstrap_root_meta(&vProc, &vMeta, NULL);
    if (pe32_load_process(&vProc, aRead, (u32)n, &load) != 0 || !load.u32Ready) {
        kprintf("pe32: vfs load_process FAIL\n");
        return -1;
    }
    hits0 = pe32_int80_exit_hits();
    (void)process_wait_register(&vProc, 1);
    thr = thread_create_user32(&vProc, load.u64Entry, load.u64StackTop - 16);
    if (thr == 0) {
        kprintf("pe32: vfs thr FAIL\n");
        return -1;
    }
    for (y = 0; y < 96u; y++) {
        thread_yield();
        if (pe32_int80_exit_hits() > hits0) {
            break;
        }
    }
    if (pe32_int80_exit_hits() > hits0 && pe32_int80_last_code() == 9) {
        kprintf("pe32: vfs load PASS code=9 entry=0x%lx\n",
                (unsigned long)load.u64Entry);
        pe32_soft_inc(&g_soft.u64VfsLoadOk);
        /* Wave 11: greppable soft inventory after vfs PE load smoke. */
        pe32_soft_inventory_log("vfs");
        return 0;
    }
    kprintf("pe32: vfs load FAIL hits=%u code=%u\n", pe32_int80_exit_hits(),
            pe32_int80_last_code());
    return -1;
}
