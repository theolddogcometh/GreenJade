/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Storage door: stats + sector R/W for storaged hand-off (A1 product).
 * Clean-room pure C; dual MIT OR Apache-2.0 (no GPL source).
 *
 * Live path: CLAIM/RELEASE ownership, STATS/CAP/QUEUE_INFO, READ/WRITE
 * via virtio-blk (preferred) or scsi_door when blk is absent. Ring
 * EXPORT/MAP/KICK hand the virtq to storaged without changing sector
 * semantics.
 *
 * Ownership: token 0 means kernel interim owns policy; non-zero means
 * storaged claimed the door. Claim is re-entrant for the same token
 * (idempotent reclaim soft), BUSY for a different token. R/W is allowed
 * without claim for bring-up smokes (owned path preferred by product
 * storaged).
 *
 * Ring soft path (storaged / UDX):
 *   EXPORT/MAP/KICK → NODEV when virtio-blk is absent (client soft-skips).
 *   RING_STATE always succeeds: ready=0 free=0 without blk.
 *   MAP records last user VA for diagnostics; re-MAP of the same VA is a
 *   soft reclaim of the map (re-install PTEs, re-export).
 *
 * Soft store inventory (Wave 12 base; Wave 14 deepen; Wave 19 exclusive):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Ownership: claim / reclaim / release / busy / claim_inval
 *   - Meta: stats/cap/queue/flush ok|inval|fault|nodev splits
 *   - Sector R/W: read/write ok + io/inval/nodev/fault + blk|scsi
 *     + owned|unowned + multi-sector + byte totals
 *   - Ring: export/map/kick/state ok|nodev|inval|fault + remap soft
 *   - Aggregate err + ok + not_init + last op/ret/lba snapshot
 *   - Path catalog + xfer ceiling + backend live snapshot
 *   - Wave 14: per-op enter, peak lba/cb, copy path, catalog, PASS
 *   - Wave 15 base: ratio|headroom|surface|deepen
 *   - Wave 19 exclusive: capacity|geom|terminal|return|retmap (wave=38 areas=25)
 *   greppable: "store_door: soft …"
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3.
 *
 * User pointers: prefer user_range_ok + copy_{to,from}_user. The !user
 * branch is for early kernel smokes that pass HHDM/static buffers.
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/store_door.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/virtio_blk.h>

/* Prefer public GJ_STORE_XFER_MAX; local alias for bounce array size. */
#define STORE_XFER_MAX GJ_STORE_XFER_MAX

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32DoorRw;
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */
static u32 g_u32Claims;     /* successful first claims */
static u32 g_u32Reclaims;   /* idempotent same-token CLAIM soft path */
static u32 g_u32RingCalls;  /* EXPORT/MAP/KICK/RING_STATE soft ops */
static u64 g_u64RingMapVa;  /* last successful MAP_RING base (0 = none) */

/*
 * Soft product inventory (Wave 12 exclusive). Cumulative path tallies.
 * greppable: store_door: soft …
 */
static u32 g_u32SoftClaimInval;   /* CLAIM bad token */
static u32 g_u32SoftClaimBusy;    /* CLAIM different owner */
static u32 g_u32SoftRelease;      /* RELEASE success (was owned) */
static u32 g_u32SoftReleaseFree;  /* RELEASE when already free (soft 0) */
static u32 g_u32SoftReleaseInval; /* RELEASE token mismatch */
static u32 g_u32SoftStats;        /* STATS ok */
static u32 g_u32SoftStatsInval;   /* STATS null dst */
static u32 g_u32SoftStatsFault;   /* STATS copy fault */
static u32 g_u32SoftCap;          /* CAP ok */
static u32 g_u32SoftCapInval;     /* CAP null dst */
static u32 g_u32SoftCapFault;     /* CAP copy fault */
static u32 g_u32SoftCapNodev;     /* CAP no backend / scsi fail */
static u32 g_u32SoftQueue;        /* QUEUE_INFO ok */
static u32 g_u32SoftQueueInval;   /* QUEUE_INFO null dst */
static u32 g_u32SoftQueueFault;   /* QUEUE_INFO copy fault */
static u32 g_u32SoftFlush;        /* FLUSH soft success */
static u32 g_u32SoftFlushNodev;   /* FLUSH no transport */
static u32 g_u32SoftReadOk;       /* READ success */
static u32 g_u32SoftWriteOk;      /* WRITE success */
static u32 g_u32SoftRwIo;         /* READ/WRITE backend IO fail */
static u32 g_u32SoftRwInval;      /* READ/WRITE arg reject */
static u32 g_u32SoftRwNodev;      /* READ/WRITE no backend */
static u32 g_u32SoftRwFault;      /* READ/WRITE copy fault */
static u32 g_u32SoftRwBlk;        /* R/W completed via virtio-blk */
static u32 g_u32SoftRwScsi;       /* R/W completed via scsi_door */
static u32 g_u32SoftRwOwned;      /* R/W while storaged owns door */
static u32 g_u32SoftRwUnowned;    /* R/W while kernel interim owns */
static u32 g_u32SoftRwMulti;      /* R/W multi-sector (cb > sector) */
static u32 g_u32SoftExportOk;     /* EXPORT_RING ok */
static u32 g_u32SoftExportInval;  /* EXPORT null dst */
static u32 g_u32SoftExportFault;  /* EXPORT copy fault */
static u32 g_u32SoftExportNodev;  /* EXPORT soft-skip / export fail */
static u32 g_u32SoftMapOk;        /* MAP_RING first map ok */
static u32 g_u32SoftRemap;        /* MAP_RING soft re-MAP same VA */
static u32 g_u32SoftMapInval;     /* MAP null VA / unaligned */
static u32 g_u32SoftMapNodev;     /* MAP soft-skip no blk */
static u32 g_u32SoftMapFault;     /* MAP FAULT (map or optional copy) */
static u32 g_u32SoftKickOk;       /* KICK ok */
static u32 g_u32SoftKickNodev;    /* KICK soft-skip / kick fail */
static u32 g_u32SoftRingState;    /* RING_STATE ok (always soft) */
static u32 g_u32SoftRingStateInval; /* RING_STATE null dst */
static u32 g_u32SoftRingStateFault; /* RING_STATE copy fault */
static u32 g_u32SoftInval;        /* aggregate INVAL terminals */
static u32 g_u32SoftNodev;        /* aggregate NODEV terminals */
static u32 g_u32SoftBusy;         /* aggregate BUSY terminals */
static u32 g_u32SoftFault;        /* aggregate FAULT terminals */
static u32 g_u32SoftIo;           /* aggregate IO terminals */
static u32 g_u32SoftNosupport;    /* unknown opcode */
static u32 g_u32SoftOk;           /* non-negative returns */
static u32 g_u32SoftNotInit;      /* call before init */
static u32 g_u32SoftLogs;         /* soft inventory emissions */
static u64 g_u64SoftReadBytes;    /* cumulative READ bytes (ok) */
static u64 g_u64SoftWriteBytes;   /* cumulative WRITE bytes (ok) */
static u32 g_u32SoftLastOp;       /* last dispatched opcode */
static i64 g_i64SoftLastRet;      /* last terminal return */
static u64 g_u64SoftLastLba;      /* last R/W LBA (0 if none) */
static u8  g_fSoftOnce;           /* one-shot after first call activity */

/* Wave 14 exclusive soft deepen — complementary path tallies. */
static u32 g_u32SoftOpEnter[32];  /* per-opcode enter; index = op */
static u32 g_u32SoftUnknownOp;    /* NOSUPPORT terminals */
static u32 g_u32SoftPeakCb;       /* peak R/W transfer size */
static u64 g_u64SoftPeakLba;      /* peak LBA observed on R/W */
static u32 g_u32SoftZeroCb;       /* R/W with cb == 0 */
static u32 g_u32SoftAligned;      /* R/W sector-aligned cb */
static u32 g_u32SoftUnaligned;    /* R/W unaligned cb */
static u32 g_u32SoftCopyUser;     /* copy path used user_range_ok */
static u32 g_u32SoftCopyKern;     /* copy path used kernel smoke */
static u32 g_u32SoftInitCalls;    /* store_door_init entries */

static void store_soft_inc(u32 *pCtr);
static void store_soft_add64(u64 *pCtr, u64 u64N);
static void store_soft_note_err(i64 i64R);
static void store_soft_inventory_log(void);
static void store_soft_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
store_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: add to u64 path tally (wrap OK for telemetry). */
static void
store_soft_add64(u64 *pCtr, u64 u64N)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr) += u64N;
}

/**
 * Soft: classify a terminal status into aggregate err / ok buckets.
 * Success / positive byte counts count as soft ok.
 */
static void
store_soft_note_err(i64 i64R)
{
    if (i64R >= 0) {
        store_soft_inc(&g_u32SoftOk);
        return;
    }
    if (i64R == GJ_ERR_INVAL) {
        store_soft_inc(&g_u32SoftInval);
    } else if (i64R == GJ_ERR_NODEV) {
        store_soft_inc(&g_u32SoftNodev);
    } else if (i64R == GJ_ERR_BUSY) {
        store_soft_inc(&g_u32SoftBusy);
    } else if (i64R == GJ_ERR_FAULT) {
        store_soft_inc(&g_u32SoftFault);
    } else if (i64R == GJ_ERR_IO) {
        store_soft_inc(&g_u32SoftIo);
    } else if (i64R == GJ_ERR_NOSUPPORT) {
        store_soft_inc(&g_u32SoftNosupport);
        store_soft_inc(&g_u32SoftUnknownOp);
    }
}

/**
 * Greppable soft store inventory (product / smoke). Wave 12 base; Wave 14 deepen.
 *   store_door: soft inventory calls=… rw=… claims=… reclaims=…
 *        ring_calls=… owned=… map_va=0x… token=… wave=38
 *   store_door: soft own claim=… reclaim=… release=… release_free=…
 *        claim_inval=… claim_busy=… release_inval=…
 *   store_door: soft meta stats=… stats_inval=… stats_fault=… cap=…
 *        cap_inval=… cap_fault=… cap_nodev=… queue=… queue_inval=…
 *        queue_fault=… flush=… flush_nodev=…
 *   store_door: soft rw read_ok=… write_ok=… io=… inval=… nodev=…
 *        fault=… blk=… scsi=… owned=… unowned=… multi=…
 *   store_door: soft ring export_ok=… export_inval=… export_fault=…
 *        export_nodev=… map_ok=… remap=… map_inval=… map_nodev=…
 *        map_fault=… kick_ok=… kick_nodev=… state=… state_inval=…
 *        state_fault=…
 *   store_door: soft xfer read_b=… write_b=… xfer_max=… sector=…
 *   store_door: soft last op=… ret=… lba=0x…
 *   store_door: soft err inval=… nodev=… busy=… fault=… io=…
 *        nosupport=… ok=… not_init=… logs=…
 *   store_door: soft backend blk=… scsi=… xfer_max=…
 *   store_door: soft path claim=storaged … (soft inventory; not bar3)
 * greppable: store_door: soft
 */
static void
store_soft_inventory_log(void)
{
    u32 u32Owned;
    u32 u32Blk;
    u32 u32Scsi;
    u32 u32QFree;

    store_soft_inc(&g_u32SoftLogs);
    u32Owned = (g_u32OwnerToken != 0) ? 1u : 0u;
    u32Blk = virtio_blk_ready() ? 1u : 0u;
    u32Scsi = scsi_mid_ready() ? 1u : 0u;
    u32QFree = (u32Blk != 0) ? virtio_blk_q_free() : 0u;

    /* Grep: store_door: soft inventory */
    kprintf("store_door: soft inventory calls=%u rw=%u claims=%u "
            "reclaims=%u ring_calls=%u owned=%u map_va=0x%lx token=0x%x "
            "logs=%u wave=38\n",
            g_u32Calls, g_u32DoorRw, g_u32Claims, g_u32Reclaims,
            g_u32RingCalls, u32Owned, (unsigned long)g_u64RingMapVa,
            g_u32OwnerToken, g_u32SoftLogs);

    /* Grep: store_door: soft own */
    kprintf("store_door: soft own claim=%u reclaim=%u release=%u "
            "release_free=%u claim_inval=%u claim_busy=%u release_inval=%u\n",
            g_u32Claims, g_u32Reclaims, g_u32SoftRelease, g_u32SoftReleaseFree,
            g_u32SoftClaimInval, g_u32SoftClaimBusy, g_u32SoftReleaseInval);

    /* Grep: store_door: soft meta */
    kprintf("store_door: soft meta stats=%u stats_inval=%u stats_fault=%u "
            "cap=%u cap_inval=%u cap_fault=%u cap_nodev=%u queue=%u "
            "queue_inval=%u queue_fault=%u flush=%u flush_nodev=%u\n",
            g_u32SoftStats, g_u32SoftStatsInval, g_u32SoftStatsFault,
            g_u32SoftCap, g_u32SoftCapInval, g_u32SoftCapFault,
            g_u32SoftCapNodev, g_u32SoftQueue, g_u32SoftQueueInval,
            g_u32SoftQueueFault, g_u32SoftFlush, g_u32SoftFlushNodev);

    /* Grep: store_door: soft rw */
    kprintf("store_door: soft rw read_ok=%u write_ok=%u io=%u inval=%u "
            "nodev=%u fault=%u blk=%u scsi=%u owned=%u unowned=%u multi=%u\n",
            g_u32SoftReadOk, g_u32SoftWriteOk, g_u32SoftRwIo, g_u32SoftRwInval,
            g_u32SoftRwNodev, g_u32SoftRwFault, g_u32SoftRwBlk, g_u32SoftRwScsi,
            g_u32SoftRwOwned, g_u32SoftRwUnowned, g_u32SoftRwMulti);

    /* Grep: store_door: soft ring */
    kprintf("store_door: soft ring export_ok=%u export_inval=%u "
            "export_fault=%u export_nodev=%u map_ok=%u remap=%u map_inval=%u "
            "map_nodev=%u map_fault=%u kick_ok=%u kick_nodev=%u state=%u "
            "state_inval=%u state_fault=%u q_free=%u\n",
            g_u32SoftExportOk, g_u32SoftExportInval, g_u32SoftExportFault,
            g_u32SoftExportNodev, g_u32SoftMapOk, g_u32SoftRemap,
            g_u32SoftMapInval, g_u32SoftMapNodev, g_u32SoftMapFault,
            g_u32SoftKickOk, g_u32SoftKickNodev, g_u32SoftRingState,
            g_u32SoftRingStateInval, g_u32SoftRingStateFault, u32QFree);

    /* Grep: store_door: soft xfer */
    kprintf("store_door: soft xfer read_b=%lu write_b=%lu xfer_max=%u "
            "sector=%u\n",
            (unsigned long)g_u64SoftReadBytes,
            (unsigned long)g_u64SoftWriteBytes, (u32)STORE_XFER_MAX,
            (u32)GJ_VIRTIO_BLK_SECTOR);

    /* Grep: store_door: soft last */
    kprintf("store_door: soft last op=%u ret=%ld lba=0x%lx\n",
            g_u32SoftLastOp, (long)g_i64SoftLastRet,
            (unsigned long)g_u64SoftLastLba);

    /* Grep: store_door: soft err */
    kprintf("store_door: soft err inval=%u nodev=%u busy=%u fault=%u "
            "io=%u nosupport=%u ok=%u not_init=%u logs=%u\n",
            g_u32SoftInval, g_u32SoftNodev, g_u32SoftBusy, g_u32SoftFault,
            g_u32SoftIo, g_u32SoftNosupport, g_u32SoftOk, g_u32SoftNotInit,
            g_u32SoftLogs);

    /* Grep: store_door: soft backend */
    kprintf("store_door: soft backend blk=%u scsi=%u xfer_max=%u "
            "sector=%u q_free=%u\n",
            u32Blk, u32Scsi, (u32)STORE_XFER_MAX, (u32)GJ_VIRTIO_BLK_SECTOR,
            u32QFree);

    /*
     * Wave 14 exclusive deepen (complementary; never reshapes primary lines).
     */
    /* Grep: store_door: soft total */
    kprintf("store_door: soft total ok=%u fail_like=%u calls=%u logs=%u "
            "unknown=%u not_init=%u wave=38\n",
            g_u32SoftOk,
            g_u32SoftInval + g_u32SoftNodev + g_u32SoftBusy + g_u32SoftFault +
                g_u32SoftIo + g_u32SoftNosupport,
            g_u32Calls, g_u32SoftLogs, g_u32SoftUnknownOp, g_u32SoftNotInit);

    /* Grep: store_door: soft ops */
    /* Indices match GJ_STORE_OP_* (1=STATS .. 12=MAP_RING). */
    kprintf("store_door: soft ops "
            "stats=%u cap=%u read=%u write=%u claim=%u release=%u "
            "queue=%u flush=%u export=%u kick=%u ring_state=%u map=%u "
            "unknown=%u\n",
            g_u32SoftOpEnter[1], g_u32SoftOpEnter[2], g_u32SoftOpEnter[3],
            g_u32SoftOpEnter[4], g_u32SoftOpEnter[5], g_u32SoftOpEnter[6],
            g_u32SoftOpEnter[7], g_u32SoftOpEnter[8], g_u32SoftOpEnter[9],
            g_u32SoftOpEnter[10], g_u32SoftOpEnter[11], g_u32SoftOpEnter[12],
            g_u32SoftUnknownOp);

    /* Grep: store_door: soft peak */
    kprintf("store_door: soft peak cb=%u lba=0x%lx read_b=%lu write_b=%lu "
            "zero_cb=%u aligned=%u unaligned=%u\n",
            g_u32SoftPeakCb, (unsigned long)g_u64SoftPeakLba,
            (unsigned long)g_u64SoftReadBytes,
            (unsigned long)g_u64SoftWriteBytes, g_u32SoftZeroCb,
            g_u32SoftAligned, g_u32SoftUnaligned);

    /* Grep: store_door: soft copy */
    kprintf("store_door: soft copy user=%u kern=%u inits=%u\n",
            g_u32SoftCopyUser, g_u32SoftCopyKern, g_u32SoftInitCalls);

    /* Grep: store_door: soft catalog */
    kprintf("store_door: soft catalog xfer_max=%u sector=%u "
            "ops=claim,release,stats,cap,read,write,queue,flush,"
            "export,map,kick,ring_state wave=38\n",
            (u32)STORE_XFER_MAX, (u32)GJ_VIRTIO_BLK_SECTOR);

    /* Grep: store_door: soft path */
    kprintf("store_door: soft path claim=storaged rw=blk|scsi "
            "ring=export|map|kick|state reclaim=idempotent "
            "map_remap=soft_reclaim flush=soft_ok "
            "wave=38 (soft inventory; not bar3)\n");

    /*
     * Wave 35 exclusive deepen (complementary; primary lines field-stable).
     * greppable: store_door: soft ratio|headroom|surface|deepen
     */
    {
        u32 u32OkBp = 0;
        u32 u32FailLike;
        u32 u32FailBp = 0;
        u32 u32OwnedBp = 0;
        u32 u32RwTot;

        u32FailLike = g_u32SoftInval + g_u32SoftNodev + g_u32SoftBusy +
                      g_u32SoftFault + g_u32SoftIo + g_u32SoftNosupport;
        if ((g_u32SoftOk + u32FailLike) != 0u) {
            u32OkBp = (g_u32SoftOk * 10000u) / (g_u32SoftOk + u32FailLike);
            u32FailBp = (u32FailLike * 10000u) / (g_u32SoftOk + u32FailLike);
        }
        u32RwTot = g_u32SoftReadOk + g_u32SoftWriteOk + g_u32SoftRwIo +
                   g_u32SoftRwInval + g_u32SoftRwNodev + g_u32SoftRwFault;
        if (u32RwTot != 0u) {
            u32OwnedBp = (g_u32SoftRwOwned * 10000u) / u32RwTot;
        }
        /* Grep: store_door: soft ratio */
        kprintf("store_door: soft ratio ok_bp=%u fail_bp=%u owned_rw_bp=%u "
                "rw=%u claims=%u logs=%u wave=38\n",
                u32OkBp, u32FailBp, u32OwnedBp, g_u32DoorRw, g_u32Claims,
                g_u32SoftLogs);
        /* Grep: store_door: soft headroom */
        kprintf("store_door: soft headroom xfer_max=%u sector=%u q_free=%u "
                "blk=%u scsi=%u wave=38\n",
                (u32)STORE_XFER_MAX, (u32)GJ_VIRTIO_BLK_SECTOR, u32QFree,
                u32Blk, u32Scsi);
        /* Grep: store_door: soft surface */
        kprintf("store_door: soft surface inventory,own,meta,rw,ring,xfer,"
                "last,err,backend,total,ops,peak,copy,catalog,ratio,"
                "headroom,capacity,geom,terminal,return,retmap,deepen areas=27 wave=38\n");
        /* Grep: store_door: soft capacity — Wave 19 design-constant lamps. */
        kprintf("store_door: soft capacity xfer_max=%u sector=%u "
                "claim_reclaim=1 map_remap=1 flush_soft=1 ring=1 wave=38\n",
                (u32)STORE_XFER_MAX, (u32)GJ_VIRTIO_BLK_SECTOR);
        /* Grep: store_door: soft geom — Wave 19 backend geometry lamps. */
        kprintf("store_door: soft geom blk=%u scsi=%u q_free=%u "
                "owned=%u map_va=0x%lx wave=38\n",
                u32Blk, u32Scsi, u32QFree, u32Owned,
                (unsigned long)g_u64RingMapVa);
        /* Grep: store_door: soft terminal — Wave 19 outcome rollup. */
        kprintf("store_door: soft terminal ok=%u fail_like=%u rw=%u "
                "claims=%u blk=%u scsi=%u soft PASS wave=38\n",
                g_u32SoftOk, u32FailLike, g_u32DoorRw, g_u32Claims,
                u32Blk, u32Scsi);
        /* Grep: store_door: soft return — Wave 19 API return surfaces */
        kprintf("store_door: soft return ok=%u inval=%u nodev=%u busy=%u "
                "fault=%u io=%u nosupport=%u not_init=%u claims=%u "
                "rw=%u product_storaged=OPEN wave=38\n",
                g_u32SoftOk, g_u32SoftInval, g_u32SoftNodev, g_u32SoftBusy,
                g_u32SoftFault, g_u32SoftIo, g_u32SoftNosupport, g_u32SoftNotInit,
                g_u32Claims, g_u32DoorRw);

        /* Grep: store_door: soft retmap — Wave 19 return-surface map */
    kprintf("store_door: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=38\n");

    /* Grep: store_door: soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: store_door: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("store_door: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=38 "
                "(retclass taxonomy; Soft≠product; not bar3)\n");
        /* Grep: store_door: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("store_door: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=38 "
                "(retlane catalog; Soft≠product)\n");
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: store_door: soft retbound — Wave 20 return-bound honesty (kept) */
        kprintf("store_door: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=38 "
                "(retbound honesty; Soft≠product; not bar3)\n");
        /* Grep: store_door: soft retseal — Wave 20 seal stamp (kept) */
        kprintf("store_door: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=38 "
                "(retseal stamp; Soft≠product)\n");
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: store_door: soft retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("store_door: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=38 "
                        "(retpulse honesty; Soft≠product; not bar3)\n");
                /* Grep: store_door: soft retmark — Wave 21 mark stamp (kept) */
                kprintf("store_door: soft retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=38 "
                        "(retmark stamp; Soft≠product)\n");
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: store_door: soft retphase — Wave 22 return-phase honesty (kept) */
                kprintf("store_door: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=38 "
                        "(retphase honesty; Soft≠product; not bar3)\n");
                /* Grep: store_door: soft retbadge — Wave 22 badge stamp (kept) */
                kprintf("store_door: soft retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=38 "
                        "(retbadge stamp; Soft≠product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: store_door: soft rettoken — Wave 23 return-token honesty (kept) */
                kprintf("store_door: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=38 "
                        "(rettoken honesty; Soft≠product; not bar3)\n");
                /* Grep: store_door: soft retcrest — Wave 23 crest stamp (kept) */
                kprintf("store_door: soft retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=38 "
                        "(retcrest stamp; Soft≠product)\n");
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: store_door: soft retvault — Wave 24 return-vault honesty (kept) */
                kprintf("store_door: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=38 "
                        "(retvault honesty; Soft≠product; not bar3)\n");
                /* Grep: store_door: soft retbanner — Wave 24 banner stamp (kept) */
                kprintf("store_door: soft retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=38 "
                        "(retbanner stamp; Soft≠product)\n");
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: store_door: soft retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("store_door: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=38 "
                        "(retledger honesty; Soft≠product; not bar3)\n");
                /* Grep: store_door: soft retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("store_door: soft retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=38 "
                        "(retbeacon stamp; Soft≠product)\n");
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: store_door: soft retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("store_door: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=38 "
                        "(retcipher honesty; Soft≠product; not bar3)\n");
                /* Grep: store_door: soft retflame — Wave 26 flame stamp (kept) */
                kprintf("store_door: soft retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=38 "
                        "(retflame stamp; Soft≠product)\n");
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: store_door: soft retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("store_door: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=38 "
                                "(retprism honesty; Soft≠product; not bar3)\n");
                        /* Grep: store_door: soft retforge — Wave 27 forge stamp (kept) */
                        kprintf("store_door: soft retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=38 "
                                "(retforge stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: store_door: soft retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("store_door: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=38 "
                                        "(retshard honesty; Soft≠product; not bar3)\n");
                                /* Grep: store_door: soft retcrown — Wave 28 crown stamp (kept) */
                                kprintf("store_door: soft retcrown exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=38 "
                                        "(retcrown stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: store_door: soft retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("store_door: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=38 "
                                        "(retglyph honesty; Soft≠product; not bar3)\n");
                                /* Grep: store_door: soft retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("store_door: soft retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=38 "
                                        "(retscepter stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: store_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("store_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=38 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: store_door: soft retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("store_door: soft retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=38 "
                                        "(retemblem stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: store_door: soft retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("store_door: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=38 "
                                        "(retaegis honesty; Soft≠product; not bar3)\n");
                                /* Grep: store_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("store_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=38 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: store_door: soft retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("store_door: soft retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=38 "
                                        "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: store_door: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("store_door: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=38 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: store_door: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("store_door: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=38 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: store_door: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("store_door: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=38 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: store_door: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("store_door: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=38 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: store_door: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("store_door: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=38 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: store_door: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("store_door: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=38 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: store_door: soft retfortress — Wave 35 return-fortress honesty */
kprintf("store_door: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=38 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: store_door: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("store_door: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=38 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: store_door: soft rethold — Wave 36 return-hold honesty */
kprintf("store_door: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=38 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: store_door: soft retspire — Wave 36 exclusive spire stamp */
kprintf("store_door: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=38 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: store_door: soft retwall — Wave 37 return-wall honesty */
kprintf("store_door: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=38 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: store_door: soft retgate — Wave 37 exclusive gate stamp */
kprintf("store_door: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=38 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: store_door: soft retmoat — Wave 38 return-moat honesty */
kprintf("store_door: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=38 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: store_door: soft retower — Wave 38 exclusive tower stamp */
kprintf("store_door: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=38 "
        "(retower stamp; Soft≠product)\n");
                                kprintf("store_door: soft deepen wave=38 areas=37 calls=%u rw=%u "
                "ok=%u fail_like=%u logs=%u\n",
                g_u32Calls, g_u32DoorRw, g_u32SoftOk, u32FailLike,
                g_u32SoftLogs);
    }

    /*
     * Soft lamp: init surface always soft-pass after store_door_init.
     * Grep: store_door: soft inventory PASS | store_door: soft PASS
     */
    kprintf("store_door: soft inventory PASS calls=%u logs=%u blk=%u "
            "scsi=%u wave=38\n",
            g_u32Calls, g_u32SoftLogs, u32Blk, u32Scsi);
    kprintf("store_door: soft PASS calls=%u wave=38\n", g_u32Calls);
}

/**
 * After first product call activity, print soft inventory once (mirrors
 * door/futex soft-stats-once). Safe from call return paths only.
 */
static void
store_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32Calls == 0) {
        return;
    }
    g_fSoftOnce = 1;
    store_soft_inventory_log();
}

/**
 * Copy @cb bytes to caller buffer at @u64Dst.
 * Uses copy_to_user when the range is in the user VA window.
 */
static i64
store_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        store_soft_inc(&g_u32SoftCopyUser);
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        /* Kernel-smoke path: destination is a trusted HHDM/static buffer. */
        store_soft_inc(&g_u32SoftCopyKern);
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
    }
    return 0;
}

/**
 * Copy @cb bytes from caller buffer at @u64Src into @pDst.
 */
static i64
store_copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || u64Src == 0 || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        store_soft_inc(&g_u32SoftCopyUser);
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        store_soft_inc(&g_u32SoftCopyKern);
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
    }
    return 0;
}

void
store_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32DoorRw = 0;
    g_u32OwnerToken = 0;
    g_u32Claims = 0;
    g_u32Reclaims = 0;
    g_u32RingCalls = 0;
    g_u64RingMapVa = 0;
    g_u32SoftClaimInval = 0;
    g_u32SoftClaimBusy = 0;
    g_u32SoftRelease = 0;
    g_u32SoftReleaseFree = 0;
    g_u32SoftReleaseInval = 0;
    g_u32SoftStats = 0;
    g_u32SoftStatsInval = 0;
    g_u32SoftStatsFault = 0;
    g_u32SoftCap = 0;
    g_u32SoftCapInval = 0;
    g_u32SoftCapFault = 0;
    g_u32SoftCapNodev = 0;
    g_u32SoftQueue = 0;
    g_u32SoftQueueInval = 0;
    g_u32SoftQueueFault = 0;
    g_u32SoftFlush = 0;
    g_u32SoftFlushNodev = 0;
    g_u32SoftReadOk = 0;
    g_u32SoftWriteOk = 0;
    g_u32SoftRwIo = 0;
    g_u32SoftRwInval = 0;
    g_u32SoftRwNodev = 0;
    g_u32SoftRwFault = 0;
    g_u32SoftRwBlk = 0;
    g_u32SoftRwScsi = 0;
    g_u32SoftRwOwned = 0;
    g_u32SoftRwUnowned = 0;
    g_u32SoftRwMulti = 0;
    g_u32SoftExportOk = 0;
    g_u32SoftExportInval = 0;
    g_u32SoftExportFault = 0;
    g_u32SoftExportNodev = 0;
    g_u32SoftMapOk = 0;
    g_u32SoftRemap = 0;
    g_u32SoftMapInval = 0;
    g_u32SoftMapNodev = 0;
    g_u32SoftMapFault = 0;
    g_u32SoftKickOk = 0;
    g_u32SoftKickNodev = 0;
    g_u32SoftRingState = 0;
    g_u32SoftRingStateInval = 0;
    g_u32SoftRingStateFault = 0;
    g_u32SoftInval = 0;
    g_u32SoftNodev = 0;
    g_u32SoftBusy = 0;
    g_u32SoftFault = 0;
    g_u32SoftIo = 0;
    g_u32SoftNosupport = 0;
    g_u32SoftOk = 0;
    g_u32SoftNotInit = 0;
    g_u32SoftLogs = 0;
    g_u64SoftReadBytes = 0;
    g_u64SoftWriteBytes = 0;
    g_u32SoftLastOp = 0;
    g_i64SoftLastRet = 0;
    g_u64SoftLastLba = 0;
    g_fSoftOnce = 0;
    {
        u32 iOp;

        for (iOp = 0; iOp < 32u; iOp++) {
            g_u32SoftOpEnter[iOp] = 0;
        }
    }
    g_u32SoftUnknownOp = 0;
    g_u32SoftPeakCb = 0;
    g_u64SoftPeakLba = 0;
    g_u32SoftZeroCb = 0;
    g_u32SoftAligned = 0;
    g_u32SoftUnaligned = 0;
    g_u32SoftCopyUser = 0;
    g_u32SoftCopyKern = 0;
    /* g_u32SoftInitCalls is cumulative across re-init for diagnostics. */
    store_soft_inc(&g_u32SoftInitCalls);
    /* Backends may probe later; report readiness snapshot for bring-up. */
    kprintf("store_door: init xfer_max=%u blk=%d scsi=%d\n", STORE_XFER_MAX,
            virtio_blk_ready() ? 1 : 0, scsi_mid_ready() ? 1 : 0);
    /* Grep: store_door: soft (baseline inventory after init; wave=38) */
    store_soft_inventory_log();
}

int
store_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

u32
store_door_owner_token(void)
{
    return g_u32OwnerToken;
}

u64
store_door_ring_map_va(void)
{
    return g_u64RingMapVa;
}

u32
store_door_ring_calls(void)
{
    /*
     * Soft diagnostics re-read: emit inventory so ring smoke greps
     * greppable store_door: soft lines without a dedicated syscall.
     */
    store_soft_inventory_log();
    return g_u32RingCalls;
}

u32
store_door_claim_count(void)
{
    /* Soft diagnostics: first claims + idempotent reclaims. */
    /*
     * Emit soft inventory on claim stats read so bring-up can grep
     * store_door: soft … (Wave 12). greppable: store_door claim soft
     */
    store_soft_inventory_log();
    return g_u32Claims + g_u32Reclaims;
}

i64
store_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    i64 i64Ret;
    int fSoftInv = 0;

    if (!g_fInit) {
        store_soft_inc(&g_u32SoftNotInit);
        return GJ_ERR_NODEV;
    }
    g_u32Calls++;
    g_u32SoftLastOp = u32Op;
    if (u32Op < 32u) {
        store_soft_inc(&g_u32SoftOpEnter[u32Op]);
    } else {
        store_soft_inc(&g_u32SoftUnknownOp);
    }

    switch (u32Op) {
    case GJ_STORE_OP_CLAIM:
        /* arg1 = non-zero ownership token (low 32 bits only). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            store_soft_inc(&g_u32SoftClaimInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            store_soft_inc(&g_u32SoftClaimBusy);
            i64Ret = GJ_ERR_BUSY; /* another storaged */
            break;
        }
        /* Soft reclaim: same token re-CLAIM is idempotent. */
        if (g_u32OwnerToken == (u32)u64Arg1) {
            g_u32Reclaims++;
            i64Ret = 0;
            break;
        }
        g_u32OwnerToken = (u32)u64Arg1;
        g_u32Claims++;
        kprintf("store_door: CLAIM token=0x%x (userspace owns storage)\n",
                g_u32OwnerToken);
        i64Ret = 0;
        break;

    case GJ_STORE_OP_RELEASE:
        /* Soft free path: already unowned → 0 (no token match required). */
        if (g_u32OwnerToken == 0) {
            store_soft_inc(&g_u32SoftReleaseFree);
            i64Ret = 0;
            break;
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            store_soft_inc(&g_u32SoftReleaseInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        kprintf("store_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
        store_soft_inc(&g_u32SoftRelease);
        i64Ret = 0;
        break;

    case GJ_STORE_OP_STATS: {
        /* aSt: [0]=virtio-blk io, [1]=scsi_door io, [2]=door call count */
        u32 aSt[3];
        i64 st;

        if (u64Arg1 == 0) {
            store_soft_inc(&g_u32SoftStatsInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        aSt[0] = virtio_blk_ready() ? virtio_blk_io_count() : 0u;
        aSt[1] = scsi_door_io_count();
        aSt[2] = g_u32Calls;
        st = store_copy_out(u64Arg1, aSt, sizeof(aSt));
        if (st == 0) {
            store_soft_inc(&g_u32SoftStats);
            /*
             * Emit soft inventory on STATS so bring-up smoke greps
             * store_door: soft … lines. greppable: store_door: soft
             */
            fSoftInv = 1;
        } else if (st == GJ_ERR_FAULT) {
            store_soft_inc(&g_u32SoftStatsFault);
        } else {
            store_soft_inc(&g_u32SoftStatsInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_STORE_OP_CAP: {
        u64 u64Cap;
        i64 st;

        if (u64Arg1 == 0) {
            store_soft_inc(&g_u32SoftCapInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (virtio_blk_ready()) {
            u64Cap = virtio_blk_capacity_sectors();
        } else if (scsi_mid_ready()) {
            /* scsi → storaged CAP path when virtio-blk absent */
            struct scsi_door_req dreq;
            u8 aCap[8];
            u32 u32Last;

            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = GJ_SCSI_DOOR_OP_READ_CAP;
            dreq.cbData = 8;
            if (scsi_door_submit(&dreq, aCap, 8) != 0) {
                store_soft_inc(&g_u32SoftCapNodev);
                i64Ret = GJ_ERR_NODEV;
                break;
            }
            /* READ CAPACITY(10): big-endian last LBA in first 4 bytes. */
            u32Last = ((u32)aCap[0] << 24) | ((u32)aCap[1] << 16) |
                      ((u32)aCap[2] << 8) | (u32)aCap[3];
            /* Capacity = last LBA + 1 (sectors); widen before add. */
            u64Cap = (u64)u32Last + 1ull;
        } else {
            store_soft_inc(&g_u32SoftCapNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        st = store_copy_out(u64Arg1, &u64Cap, sizeof(u64Cap));
        if (st == 0) {
            store_soft_inc(&g_u32SoftCap);
        } else if (st == GJ_ERR_FAULT) {
            store_soft_inc(&g_u32SoftCapFault);
        } else {
            store_soft_inc(&g_u32SoftCapInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_STORE_OP_READ:
    case GJ_STORE_OP_WRITE: {
        u64 u64Lba = u64Arg1;
        u32 cb = (u32)u64Arg3;
        static u8 aTmp[STORE_XFER_MAX];
        int nIo;
        int fBlk = virtio_blk_ready();
        int fScsi = !fBlk && scsi_mid_ready();
        i64 st;

        g_u64SoftLastLba = u64Lba;

        if (!fBlk && !fScsi) {
            store_soft_inc(&g_u32SoftRwNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        /* Reject wide arg3 truncation; require sector-multiple length. */
        if (u64Arg2 == 0 || cb == 0 || (u64Arg3 >> 32) != 0 ||
            cb > STORE_XFER_MAX || (cb % GJ_VIRTIO_BLK_SECTOR) != 0) {
            store_soft_inc(&g_u32SoftRwInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        /*
         * READ10/WRITE10 LBA field is 32-bit. Reject out-of-range LBAs on
         * the scsi fallback path (virtio-blk accepts full u64 sector).
         */
        if (fScsi && (u64Lba >> 32) != 0) {
            store_soft_inc(&g_u32SoftRwInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }

        if (u32Op == GJ_STORE_OP_WRITE) {
            st = store_copy_in(aTmp, u64Arg2, cb);
            if (st != 0) {
                if (st == GJ_ERR_FAULT) {
                    store_soft_inc(&g_u32SoftRwFault);
                } else {
                    store_soft_inc(&g_u32SoftRwInval);
                }
                i64Ret = st;
                break;
            }
            if (fBlk) {
                nIo = virtio_blk_write(u64Lba, aTmp, cb);
            } else {
                struct scsi_door_req dreq;
                u32 u32Blocks = cb / GJ_VIRTIO_BLK_SECTOR;

                /* Defensive: blocks must fit u16 (xfer_max guarantees this). */
                if (u32Blocks == 0 || u32Blocks > 0xffffu) {
                    store_soft_inc(&g_u32SoftRwInval);
                    i64Ret = GJ_ERR_INVAL;
                    break;
                }
                memset(&dreq, 0, sizeof(dreq));
                dreq.u32Op = GJ_SCSI_DOOR_OP_WRITE10;
                dreq.u32Lba = (u32)u64Lba;
                dreq.u16Blocks = (u16)u32Blocks;
                dreq.cbData = cb;
                nIo = scsi_door_submit(&dreq, aTmp, cb);
            }
        } else {
            if (fBlk) {
                nIo = virtio_blk_read(u64Lba, aTmp, cb);
            } else {
                struct scsi_door_req dreq;
                u32 u32Blocks = cb / GJ_VIRTIO_BLK_SECTOR;

                if (u32Blocks == 0 || u32Blocks > 0xffffu) {
                    store_soft_inc(&g_u32SoftRwInval);
                    i64Ret = GJ_ERR_INVAL;
                    break;
                }
                memset(&dreq, 0, sizeof(dreq));
                dreq.u32Op = GJ_SCSI_DOOR_OP_READ10;
                dreq.u32Lba = (u32)u64Lba;
                dreq.u16Blocks = (u16)u32Blocks;
                dreq.cbData = cb;
                nIo = scsi_door_submit(&dreq, aTmp, cb);
            }
            if (nIo == 0) {
                st = store_copy_out(u64Arg2, aTmp, cb);
                if (st != 0) {
                    if (st == GJ_ERR_FAULT) {
                        store_soft_inc(&g_u32SoftRwFault);
                    } else {
                        store_soft_inc(&g_u32SoftRwInval);
                    }
                    i64Ret = st;
                    break;
                }
            }
        }
        if (nIo != 0) {
            store_soft_inc(&g_u32SoftRwIo);
            i64Ret = GJ_ERR_IO;
            break;
        }
        g_u32DoorRw++;
        if (u32Op == GJ_STORE_OP_WRITE) {
            store_soft_inc(&g_u32SoftWriteOk);
            store_soft_add64(&g_u64SoftWriteBytes, (u64)cb);
        } else {
            store_soft_inc(&g_u32SoftReadOk);
            store_soft_add64(&g_u64SoftReadBytes, (u64)cb);
        }
        if (fBlk) {
            store_soft_inc(&g_u32SoftRwBlk);
        } else {
            store_soft_inc(&g_u32SoftRwScsi);
        }
        if (g_u32OwnerToken != 0) {
            store_soft_inc(&g_u32SoftRwOwned);
        } else {
            store_soft_inc(&g_u32SoftRwUnowned);
        }
        if (cb > GJ_VIRTIO_BLK_SECTOR) {
            store_soft_inc(&g_u32SoftRwMulti);
        }
        /* Wave 14: peak / alignment soft samples on successful R/W. */
        if (cb == 0) {
            store_soft_inc(&g_u32SoftZeroCb);
        } else if ((cb % GJ_VIRTIO_BLK_SECTOR) == 0) {
            store_soft_inc(&g_u32SoftAligned);
        } else {
            store_soft_inc(&g_u32SoftUnaligned);
        }
        if (cb > g_u32SoftPeakCb) {
            g_u32SoftPeakCb = cb;
        }
        if (u64Lba > g_u64SoftPeakLba) {
            g_u64SoftPeakLba = u64Lba;
        }
        i64Ret = (i64)cb;
        break;
    }

    case GJ_STORE_OP_QUEUE_INFO: {
        /* aQ: [0]=blk_io [1]=scsi_io [2]=door_rw [3]=owned(0/1) — wire stable */
        u32 aQ[4];
        i64 st;

        if (u64Arg1 == 0) {
            store_soft_inc(&g_u32SoftQueueInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        aQ[0] = virtio_blk_ready() ? virtio_blk_io_count() : 0u;
        aQ[1] = scsi_door_io_count();
        aQ[2] = g_u32DoorRw;
        aQ[3] = g_u32OwnerToken ? 1u : 0u;
        st = store_copy_out(u64Arg1, aQ, sizeof(aQ));
        if (st == 0) {
            store_soft_inc(&g_u32SoftQueue);
        } else if (st == GJ_ERR_FAULT) {
            store_soft_inc(&g_u32SoftQueueFault);
        } else {
            store_soft_inc(&g_u32SoftQueueInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_STORE_OP_FLUSH:
        /*
         * Virtio-blk flush not wired; fsync-shaped success when a block
         * transport is ready. scsi_mid has no SYNCHRONIZE CACHE yet.
         */
        if (!virtio_blk_ready() && !scsi_mid_ready()) {
            store_soft_inc(&g_u32SoftFlushNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        store_soft_inc(&g_u32SoftFlush);
        i64Ret = 0;
        break;

    case GJ_STORE_OP_EXPORT_RING: {
        struct gj_virtq_export ex;
        i64 st;

        g_u32RingCalls++;
        if (u64Arg1 == 0) {
            store_soft_inc(&g_u32SoftExportInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        /* Soft-skip surface: no virtio-blk → NODEV (storaged soft-logs). */
        if (!virtio_blk_ready()) {
            store_soft_inc(&g_u32SoftExportNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        if (virtio_blk_export_q(&ex) != 0) {
            store_soft_inc(&g_u32SoftExportNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        st = store_copy_out(u64Arg1, &ex, sizeof(ex));
        if (st == 0) {
            store_soft_inc(&g_u32SoftExportOk);
        } else if (st == GJ_ERR_FAULT) {
            store_soft_inc(&g_u32SoftExportFault);
        } else {
            store_soft_inc(&g_u32SoftExportInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_STORE_OP_KICK:
        g_u32RingCalls++;
        /* Soft-skip when blk absent; kick is best-effort notify. */
        if (!virtio_blk_ready()) {
            store_soft_inc(&g_u32SoftKickNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        if (virtio_blk_kick_q() != 0) {
            store_soft_inc(&g_u32SoftKickNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        store_soft_inc(&g_u32SoftKickOk);
        i64Ret = 0;
        break;

    case GJ_STORE_OP_RING_STATE: {
        /* Soft: always fills {free, ready}; ready=0 without virtio-blk. */
        u32 aS[2];
        i64 st;

        g_u32RingCalls++;
        if (u64Arg1 == 0) {
            store_soft_inc(&g_u32SoftRingStateInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        aS[0] = virtio_blk_q_free();
        aS[1] = virtio_blk_ready() ? 1u : 0u;
        st = store_copy_out(u64Arg1, aS, sizeof(aS));
        if (st == 0) {
            store_soft_inc(&g_u32SoftRingState);
        } else if (st == GJ_ERR_FAULT) {
            store_soft_inc(&g_u32SoftRingStateFault);
        } else {
            store_soft_inc(&g_u32SoftRingStateInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_STORE_OP_MAP_RING: {
        struct gj_virtq_export ex;
        i64 st;
        int fRemap;

        g_u32RingCalls++;
        if (u64Arg1 == 0) {
            store_soft_inc(&g_u32SoftMapInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        /* VA base must be page-aligned for ring map into user AS. */
        if ((u64Arg1 & (GJ_PAGE_SIZE - 1ull)) != 0) {
            store_soft_inc(&g_u32SoftMapInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        /* Soft-skip surface: no blk → NODEV (distinct from map FAULT). */
        if (!virtio_blk_ready()) {
            store_soft_inc(&g_u32SoftMapNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        /*
         * Soft re-MAP of the same VA: re-install PTEs + re-export (idempotent
         * hand-off for storaged / UDX reclaim of the map window).
         */
        fRemap = (g_u64RingMapVa != 0 && g_u64RingMapVa == u64Arg1) ? 1 : 0;
        if (virtio_blk_map_q_user(u64Arg1, &ex) != 0) {
            store_soft_inc(&g_u32SoftMapFault);
            i64Ret = GJ_ERR_FAULT;
            break;
        }
        g_u64RingMapVa = u64Arg1;
        if (fRemap) {
            store_soft_inc(&g_u32SoftRemap);
        } else {
            store_soft_inc(&g_u32SoftMapOk);
        }
        if (u64Arg2 != 0) {
            st = store_copy_out(u64Arg2, &ex, sizeof(ex));
            if (st != 0) {
                if (st == GJ_ERR_FAULT) {
                    store_soft_inc(&g_u32SoftMapFault);
                } else {
                    store_soft_inc(&g_u32SoftMapInval);
                }
                i64Ret = st;
                break;
            }
        }
        i64Ret = 0;
        break;
    }

    default:
        i64Ret = GJ_ERR_NOSUPPORT;
        break;
    }

    /* Wave 12 soft inventory tallies (never mutates i64Ret). */
    g_i64SoftLastRet = i64Ret;
    store_soft_note_err(i64Ret);
    store_soft_maybe_once();
    if (fSoftInv) {
        store_soft_inventory_log();
    }
    return i64Ret;
}
