/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared freestanding order-0 freelist (GJ_ARCH_* product scaffold).
 * Identity-mapped PA pool only - no HHDM, no hierarchical orders.
 *
 * Exclusive soft residual (this unit only - greppable "pmm_core: soft ..."):
 *   pmm_core: soft honesty       - order-0 only; Soft!=product hierarchical pmm
 *   pmm_core: soft exclusive     - multi_server=0 confine=0 product_kernel=OPEN
 *   pmm_core: soft open          - open-lamp rollup (soft only)
 *   pmm_core: soft inventory     - free/total/base/limit/page_size snapshot
 *   pmm_core: soft pool          - span, head, free_le_total, empty
 *   pmm_core: soft chain         - freelist walk length vs free_count
 *   pmm_core: soft reject        - OOB/unaligned free paths (count stable)
 *   pmm_core: soft align         - alloc PA page-aligned + in [base,limit)
 *   pmm_core: soft counts        - free-count steps across alloc/free
 *   pmm_core: soft api           - free_count/total_count API match
 *   pmm_core: soft scrub         - alloc scrubs node header to 0
 *   pmm_core: soft head          - free pushes LIFO head
 *   pmm_core: soft lifo          - free pushes front; next alloc reuses PA
 *   pmm_core: soft double        - two-free LIFO order
 *   pmm_core: soft payload       - payload past node header survives free+alloc
 *   pmm_core: soft empty         - empty freelist alloc returns 0 (1-page pool)
 *   pmm_core: soft total         - total immutable across soft exercise
 *   pmm_core: soft restore       - free/total restored after selftest
 *   pmm_core: soft path          - surface catalog + product non-claims
 *   pmm_core: soft geom          - page/span/node geometry
 *   pmm_core: soft return        - API return surfaces + product_kernel=OPEN
 *   pmm_core: soft retclass      - soft return-class taxonomy (product_gate=0)
 *   pmm_core: soft law            - Soft!=product G-AC-1 DualDoD=OPEN rollup
 *   pmm_core: soft deepen        - area count rollup (no stamp storm)
 *   pmm_core: soft residual lean - Soft!=product dual license; stamp_storm=0
 *   pmm_core: soft residual lean PASS - lean residual self-check PASS lamp
 *   pmm_core: soft PASS|FAIL / pmm_core: soft inventory PASS|FAIL
 *
 * Honesty: soft inventory only - not hierarchical pmm / not 1 TiB product.
 * Dual license: MIT OR Apache-2.0 (Soft!=product dual license; no GPL).
 * G-AC-1: soft residual != product AC. Soft!=product.
 * Dual DoD A/B remain OPEN (this residual does not close product DoD).
 * No version stamp; no stamp storms; stamp-free residual (BAR v2026.08.04.75).
 * Never bumps GJ_IMAGE_VERSION. C0 eng residual only.
 */
#include <gj/klog.h>
#include <gj/pmm_core.h>
#include <gj/string.h>

struct pmm_core_node {
	struct pmm_core_node *pNext;
};

static struct pmm_core_node *g_pFree;
static u64 g_u64Base;
static u64 g_u64Limit;
static unsigned g_cFree;
static unsigned g_cTotal;
/* Soft inventory emission counter (observability only). */
static u32 g_cSoftInvLogs;

void
gj_pmm_core_init(u64 u64Base, u64 u64Limit)
{
	u64 pa;

	g_pFree = 0;
	g_cFree = 0;
	g_cTotal = 0;
	g_u64Base = u64Base;
	g_u64Limit = u64Limit;

	if (u64Limit <= u64Base) {
		return;
	}
	for (pa = u64Base; pa + GJ_PMM_CORE_PAGE_SIZE <= u64Limit;
	     pa += GJ_PMM_CORE_PAGE_SIZE) {
		struct pmm_core_node *n =
		    (struct pmm_core_node *)(void *)(gj_vaddr_t)pa;

		n->pNext = g_pFree;
		g_pFree = n;
		g_cFree++;
		g_cTotal++;
	}
}

u64
gj_pmm_core_alloc(void)
{
	struct pmm_core_node *n = g_pFree;

	if (n == 0) {
		return 0;
	}
	g_pFree = n->pNext;
	if (g_cFree > 0u) {
		g_cFree--;
	}
	memset(n, 0, sizeof(*n));
	return (u64)(gj_vaddr_t)(void *)n;
}

void
gj_pmm_core_free(u64 u64Pa)
{
	struct pmm_core_node *n;

	if (u64Pa < g_u64Base || u64Pa >= g_u64Limit) {
		return;
	}
	if ((u64Pa & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
		return;
	}
	n = (struct pmm_core_node *)(void *)(gj_vaddr_t)u64Pa;
	n->pNext = g_pFree;
	g_pFree = n;
	g_cFree++;
}

unsigned
gj_pmm_core_free_count(void)
{
	return g_cFree;
}

unsigned
gj_pmm_core_total_count(void)
{
	return g_cTotal;
}

/*
 * Soft freelist chain walk (bounded + node range/align).
 * Returns node count, or 0xffffffffu if walk exceeded total+1 (cycle) or a
 * node PA is OOB / unaligned (corruption soft signal). Soft!=product.
 */
static unsigned
soft_walk_free(void)
{
	unsigned c = 0;
	struct pmm_core_node *p = g_pFree;
	unsigned cBound = g_cTotal + 1u;

	while (p != 0) {
		u64 pa;

		if (c >= cBound) {
			return 0xffffffffu;
		}
		pa = (u64)(gj_vaddr_t)(void *)p;
		if (pa < g_u64Base || pa >= g_u64Limit) {
			return 0xffffffffu;
		}
		if ((pa & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
			return 0xffffffffu;
		}
		c++;
		p = p->pNext;
	}
	return c;
}

/*
 * Lean soft inventory (never hard-gates boot). Safe after init; no alloc.
 * Prefix-stable: "pmm_core: soft ...". Stamp-free residual (no wave/version).
 * Soft!=product; dual MIT OR Apache-2.0; G-AC-1 soft residual != product AC.
 */
static void
pmm_core_soft_inventory(int fPass, unsigned cAreas, unsigned cChain,
                        unsigned cFreeSnap, unsigned cTotalSnap, u32 u32Surf)
{
	u64 u64Span;
	u32 u32Head;
	u32 u32FreeLeTotal;
	u32 u32PageShift;
	const char *szVerdict;

	if (g_cSoftInvLogs < 0xffffffffu) {
		g_cSoftInvLogs++;
	}

	if (g_u64Limit > g_u64Base) {
		u64Span = g_u64Limit - g_u64Base;
	} else {
		u64Span = 0;
	}
	u32Head = (g_pFree != 0) ? 1u : 0u;
	u32FreeLeTotal = (cFreeSnap <= cTotalSnap) ? 1u : 0u;
	u32PageShift = GJ_PMM_CORE_PAGE_SHIFT;
	szVerdict = (fPass != 0) ? "PASS" : "FAIL";

	/*
	 * Soft!=product: order-0 freelist only - not hierarchical / HHDM / 1 TiB.
	 * greppable: pmm_core: soft honesty
	 */
	kprintf("pmm_core: soft honesty order0=1 hierarchical=0 hhdm=0 "
	        "zones=0 product_tib=0 pmem3=OPEN DualDoD=OPEN "
	        "(soft inventory only; Soft!=product; G-AC-1)\n");

	/* Grep: pmm_core: soft exclusive */
	kprintf("pmm_core: soft exclusive multi_server=0 confine=0 "
	        "product_kernel=OPEN soft_only=1 order0=1 DualDoD=OPEN "
	        "stamp_storm=0\n");

	/* Grep: pmm_core: soft open */
	kprintf("pmm_core: soft open multi_server=0 confine=0 "
	        "product_kernel=OPEN soft_only=1 DualDoD=OPEN\n");

	/* Grep: pmm_core: soft inventory */
	kprintf("pmm_core: soft inventory free=%u total=%u base=0x%lx "
	        "limit=0x%lx page_size=%lu page_shift=%u head=%u "
	        "span=0x%lx chain=%u areas=%u logs=%u surf=0x%x\n",
	        cFreeSnap, cTotalSnap,
	        (unsigned long)g_u64Base, (unsigned long)g_u64Limit,
	        (unsigned long)GJ_PMM_CORE_PAGE_SIZE, u32PageShift,
	        u32Head, (unsigned long)u64Span, cChain, cAreas,
	        g_cSoftInvLogs, u32Surf);

	/* Grep: pmm_core: soft pool */
	kprintf("pmm_core: soft pool free=%u total=%u free_le_total=%u "
	        "head=%u span_pages=%u empty=%u\n",
	        cFreeSnap, cTotalSnap, u32FreeLeTotal, u32Head,
	        (cTotalSnap > 0u) ? cTotalSnap : 0u,
	        (cFreeSnap == 0u) ? 1u : 0u);

	/* Grep: pmm_core: soft chain */
	kprintf("pmm_core: soft chain walk=%u free=%u match=%u bound=%u\n",
	        cChain, cFreeSnap,
	        (cChain == cFreeSnap) ? 1u : 0u,
	        cTotalSnap + 1u);

	/* Grep: pmm_core: soft path - surface catalog + non-claims */
	kprintf("pmm_core: soft path init=1 alloc=1 free=1 free_count=1 "
	        "total_count=1 selftest=1 hierarchical=0 hhdm=0 "
	        "order_max=0 product_tib=0\n");

	/* Grep: pmm_core: soft geom */
	kprintf("pmm_core: soft geom page_size=%lu page_shift=%u "
	        "span=0x%lx span_pages=%u base=0x%lx limit=0x%lx "
	        "node_bytes=%u\n",
	        (unsigned long)GJ_PMM_CORE_PAGE_SIZE, u32PageShift,
	        (unsigned long)u64Span,
	        (cTotalSnap > 0u) ? cTotalSnap : 0u,
	        (unsigned long)g_u64Base, (unsigned long)g_u64Limit,
	        (unsigned)sizeof(struct pmm_core_node));

	/* Grep: pmm_core: soft return */
	kprintf("pmm_core: soft return alloc_pa=1 free_void=1 free_count=1 "
	        "total_count=1 selftest_bool=1 inv_ret=%u "
	        "product_kernel=OPEN DualDoD=OPEN\n",
	        (fPass != 0) ? 1u : 0u);

	/*
	 * Grep: pmm_core: soft retclass
	 * Soft return-class taxonomy only - never product gate / never stamp storm.
	 */
	kprintf("pmm_core: soft retclass ok|fail|inval|nomem soft_only=1 "
	        "product_gate=0 hard_gate=0 stamp_storm=0 "
	        "(retclass taxonomy; Soft!=product)\n");

	/*
	 * Grep: pmm_core: soft law
	 * Law rollup: Soft!=product · G-AC-1 · Dual DoD OPEN · no GPL · stamp-free.
	 */
	kprintf("pmm_core: soft law Soft!=product G-AC-1=1 DualDoD=OPEN "
	        "no_gpl=1 dual=MIT_OR_Apache-2.0 stamp_storm=0 "
	        "version_stamp=0 product_kernel=OPEN order0=1 "
	        "hierarchical=0 hhdm=0 zones=0 product_tib=0 "
	        "(C0 residual only; not Dual DoD close; not hierarchical pmm)\n");

	/* Grep: pmm_core: soft deepen - lean rollup only (no stamp storm). */
	kprintf("pmm_core: soft deepen areas=%u free=%u total=%u "
	        "logs=%u surf=0x%x stamp_storm=0\n",
	        cAreas, cFreeSnap, cTotalSnap, g_cSoftInvLogs, u32Surf);

	/*
	 * Grep: pmm_core: soft residual lean
	 * Lean residual honesty - Soft!=product dual license; no version stamp.
	 * Not hierarchical product pmm; order-0 soft only. G-AC-1.
	 * Dual DoD A/B remain OPEN. stamp_storm=0. No GPL.
	 */
	kprintf("pmm_core: soft residual lean %s areas=%u surf=0x%x "
	        "free=%u total=%u order0=1 hierarchical=0 hhdm=0 "
	        "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
	        "product_kernel=OPEN G-AC-1=1 DualDoD=OPEN "
	        "stamp_storm=0 no_gpl=1 "
	        "(Soft!=product; dual MIT OR Apache-2.0; "
	        "no version stamp; no stamp storms; "
	        "not hierarchical product pmm; Dual DoD OPEN)\n",
	        szVerdict, cAreas, u32Surf, cFreeSnap, cTotalSnap);

	/*
	 * Grep: pmm_core: soft residual lean PASS
	 * Once-style PASS lamp when inventory verdict is PASS (soft only).
	 */
	if (fPass != 0) {
		kprintf("pmm_core: soft residual lean PASS areas=%u surf=0x%x "
		        "free=%u total=%u order0=1 soft_ne_product=1 "
		        "dual=MIT_OR_Apache-2.0 G-AC-1=1 DualDoD=OPEN "
		        "stamp_storm=0 no_gpl=1 "
		        "(Soft!=product; dual MIT OR Apache-2.0; "
		        "no version stamp; C0 residual only)\n",
		        cAreas, u32Surf, cFreeSnap, cTotalSnap);
	}

	/* Grep: pmm_core: soft inventory PASS|FAIL / pmm_core: soft PASS|FAIL */
	kprintf("pmm_core: soft inventory %s free=%u total=%u\n",
	        szVerdict, cFreeSnap, cTotalSnap);
	kprintf("pmm_core: soft %s free=%u total=%u areas=%u surf=0x%x\n",
	        szVerdict, cFreeSnap, cTotalSnap, cAreas, u32Surf);
}

/*
 * Soft residual selftest deepen (stamp-free): reject OOB/unaligned free,
 * chain walk+node range, alloc align+range, free-count steps, API counts,
 * node-header scrub, LIFO head, LIFO reuse + double LIFO, payload survive,
 * empty freelist alloc=0 (1- or 2-page pools only), freelist chain post,
 * total immutable, restore pool counts.
 * Returns 1 on PASS, 0 on soft FAIL. Does not drain large pools.
 * Soft!=product; G-AC-1; Dual DoD OPEN; dual MIT OR Apache-2.0;
 * no version stamp; stamp_storm=0; no GPL.
 */
int
gj_pmm_core_selftest(void)
{
	u64 paA;
	u64 paB;
	u64 paC;
	u64 paD;
	u64 paE;
	unsigned cFree0;
	unsigned cTotal0;
	unsigned cChain;
	unsigned cAreas;
	u32 u32Surf;
	volatile u32 *pMark;
	volatile u32 *pMark2;
	int fOk;

	/* Surface bits for greppable per-area lamps (stamp-free). */
	enum {
		SURF_REJECT  = 1u << 0,
		SURF_CHAIN   = 1u << 1,
		SURF_ALIGN   = 1u << 2,
		SURF_COUNTS  = 1u << 3,
		SURF_LIFO    = 1u << 4,
		SURF_PAYLOAD = 1u << 5,
		SURF_DOUBLE  = 1u << 6,
		SURF_RESTORE = 1u << 7,
		SURF_TOTAL   = 1u << 8,
		SURF_API     = 1u << 9,
		SURF_SCRUB   = 1u << 10,
		SURF_HEAD    = 1u << 11,
		SURF_EMPTY   = 1u << 12
	};

	cFree0 = g_cFree;
	cTotal0 = g_cTotal;
	cAreas = 0;
	u32Surf = 0;
	fOk = 1;
	paA = 0;
	paB = 0;
	paC = 0;
	paD = 0;
	paE = 0;

	if (cTotal0 == 0u || cFree0 == 0u) {
		pmm_core_soft_inventory(0, 0, soft_walk_free(), cFree0, cTotal0,
		                        0);
		return 0;
	}
	if (cFree0 > cTotal0) {
		pmm_core_soft_inventory(0, 0, soft_walk_free(), cFree0, cTotal0,
		                        0);
		return 0;
	}

	/* --- API free_count/total_count match globals --- */
	if (gj_pmm_core_free_count() != cFree0 ||
	    gj_pmm_core_total_count() != cTotal0) {
		fOk = 0;
		goto soft_out;
	}
	u32Surf |= SURF_API;
	cAreas++; /* api */

	/* --- reject paths must not touch free/total --- */
	gj_pmm_core_free(0);
	gj_pmm_core_free(1ul); /* unaligned */
	gj_pmm_core_free(g_u64Limit); /* OOB high (limit exclusive) */
	if (g_u64Base >= GJ_PMM_CORE_PAGE_SIZE) {
		gj_pmm_core_free(g_u64Base - GJ_PMM_CORE_PAGE_SIZE);
	}
	if (g_u64Limit > g_u64Base + 1ul) {
		gj_pmm_core_free(g_u64Base + 1ul);
		gj_pmm_core_free(g_u64Base + (GJ_PMM_CORE_PAGE_SIZE / 2ul));
	}
	if (g_u64Limit > 0ul) {
		gj_pmm_core_free(g_u64Limit - 1ul);
	}
	if (g_cFree != cFree0 || g_cTotal != cTotal0) {
		fOk = 0;
		goto soft_out;
	}
	if (gj_pmm_core_free_count() != cFree0 ||
	    gj_pmm_core_total_count() != cTotal0) {
		fOk = 0;
		goto soft_out;
	}
	u32Surf |= SURF_REJECT;
	cAreas++; /* reject */

	/* --- freelist chain matches free_count (pre; range/align walk) --- */
	cChain = soft_walk_free();
	if (cChain != cFree0) {
		fOk = 0;
		goto soft_out;
	}
	u32Surf |= SURF_CHAIN;
	cAreas++; /* chain_pre */

	/* --- alloc A: align, range, count step, node scrub --- */
	paA = gj_pmm_core_alloc();
	if (paA == 0) {
		fOk = 0;
		goto soft_out;
	}
	if ((paA & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
		gj_pmm_core_free(paA);
		paA = 0;
		fOk = 0;
		goto soft_out;
	}
	if (paA < g_u64Base || paA >= g_u64Limit) {
		gj_pmm_core_free(paA);
		paA = 0;
		fOk = 0;
		goto soft_out;
	}
	if (g_cFree != cFree0 - 1u || g_cTotal != cTotal0) {
		gj_pmm_core_free(paA);
		paA = 0;
		fOk = 0;
		goto soft_out;
	}
	/* Node header scrubbed on alloc (sizeof(node) only). */
	if (*(volatile u64 *)(void *)(gj_vaddr_t)paA != 0ull) {
		gj_pmm_core_free(paA);
		paA = 0;
		fOk = 0;
		goto soft_out;
	}
	u32Surf |= SURF_ALIGN;
	u32Surf |= SURF_COUNTS;
	u32Surf |= SURF_SCRUB;
	cAreas++; /* align */
	cAreas++; /* counts_step_a */
	cAreas++; /* scrub */

	paB = gj_pmm_core_alloc();
	if (paB == 0) {
		/*
		 * Empty freelist after single alloc (cFree0 was 1): alloc must
		 * return 0 and free count stay 0. Soft!=product empty arm.
		 * Grep: pmm_core: soft empty
		 */
		if (g_cFree != 0u || g_pFree != 0 ||
		    gj_pmm_core_free_count() != 0u) {
			gj_pmm_core_free(paA);
			paA = 0;
			fOk = 0;
			goto soft_out;
		}
		u32Surf |= SURF_EMPTY;
		cAreas++; /* empty */
		/* Single-page pool: free A and accept restore. */
		gj_pmm_core_free(paA);
		/* free must push LIFO head */
		if (g_pFree != (struct pmm_core_node *)(void *)(gj_vaddr_t)paA) {
			paA = 0;
			fOk = 0;
			goto soft_out;
		}
		u32Surf |= SURF_HEAD;
		cAreas++; /* head */
		paA = 0;
		if (g_cFree != cFree0 || g_cTotal != cTotal0) {
			fOk = 0;
		} else {
			u32Surf |= SURF_RESTORE;
			u32Surf |= SURF_TOTAL;
			cAreas++; /* restore_single */
			cAreas++; /* total_single */
		}
		goto soft_out;
	}
	if (paA == paB) {
		gj_pmm_core_free(paA);
		paA = 0;
		fOk = 0;
		goto soft_out;
	}
	if ((paB & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul ||
	    paB < g_u64Base || paB >= g_u64Limit) {
		gj_pmm_core_free(paA);
		gj_pmm_core_free(paB);
		paA = 0;
		paB = 0;
		fOk = 0;
		goto soft_out;
	}
	if (g_cFree != cFree0 - 2u || g_cTotal != cTotal0) {
		gj_pmm_core_free(paA);
		gj_pmm_core_free(paB);
		paA = 0;
		paB = 0;
		fOk = 0;
		goto soft_out;
	}
	if (gj_pmm_core_free_count() != cFree0 - 2u ||
	    gj_pmm_core_total_count() != cTotal0) {
		gj_pmm_core_free(paA);
		gj_pmm_core_free(paB);
		paA = 0;
		paB = 0;
		fOk = 0;
		goto soft_out;
	}
	cAreas++; /* counts_step_b */

	/*
	 * Empty freelist when pool had exactly 2 free: third alloc returns 0.
	 * Bounded - never drains large pools. Soft!=product empty arm.
	 * Grep: pmm_core: soft empty
	 */
	if (g_cFree == 0u) {
		u64 paZ = gj_pmm_core_alloc();

		if (paZ != 0) {
			gj_pmm_core_free(paA);
			gj_pmm_core_free(paB);
			gj_pmm_core_free(paZ);
			paA = 0;
			paB = 0;
			fOk = 0;
			goto soft_out;
		}
		if (g_cFree != 0u || g_pFree != 0 ||
		    gj_pmm_core_free_count() != 0u) {
			gj_pmm_core_free(paA);
			gj_pmm_core_free(paB);
			paA = 0;
			paB = 0;
			fOk = 0;
			goto soft_out;
		}
		u32Surf |= SURF_EMPTY;
		cAreas++; /* empty */
	}

	/*
	 * LIFO head: free pushes front. Next alloc must return paB.
	 * Payload past node header survives free+alloc (only sizeof(node) scrubbed).
	 */
	{
		u64 u64WasB = paB;

		pMark = (volatile u32 *)(void *)((gj_vaddr_t)paB + 64ul);
		*pMark = 0xC0FFEEU;
		gj_pmm_core_free(paB);
		if (g_pFree !=
		    (struct pmm_core_node *)(void *)(gj_vaddr_t)u64WasB) {
			paB = 0;
			gj_pmm_core_free(paA);
			paA = 0;
			fOk = 0;
			goto soft_out;
		}
		u32Surf |= SURF_HEAD;
		cAreas++; /* head */
		paB = 0;
		if (g_cFree != cFree0 - 1u || g_cTotal != cTotal0) {
			gj_pmm_core_free(paA);
			paA = 0;
			fOk = 0;
			goto soft_out;
		}
		paC = gj_pmm_core_alloc();
		if (paC != u64WasB) {
			if (paC != 0) {
				gj_pmm_core_free(paC);
				paC = 0;
			}
			gj_pmm_core_free(paA);
			paA = 0;
			fOk = 0;
			goto soft_out;
		}
		/* Scrub again after re-alloc of same page. */
		if (*(volatile u64 *)(void *)(gj_vaddr_t)paC != 0ull) {
			gj_pmm_core_free(paA);
			gj_pmm_core_free(paC);
			paA = 0;
			paC = 0;
			fOk = 0;
			goto soft_out;
		}
		if (*pMark != 0xC0FFEEU) {
			gj_pmm_core_free(paA);
			gj_pmm_core_free(paC);
			paA = 0;
			paC = 0;
			fOk = 0;
			goto soft_out;
		}
	}
	u32Surf |= SURF_LIFO;
	u32Surf |= SURF_PAYLOAD;
	cAreas++; /* lifo */
	cAreas++; /* payload */

	/* Double LIFO: free C then A; next two allocs = A then former-B. */
	pMark2 = (volatile u32 *)(void *)((gj_vaddr_t)paA + 128ul);
	*pMark2 = 0xA11CEU;
	{
		u64 u64WasB = paC;
		u64 u64WasA = paA;

		gj_pmm_core_free(paC);
		paC = 0;
		gj_pmm_core_free(paA);
		if (g_pFree !=
		    (struct pmm_core_node *)(void *)(gj_vaddr_t)u64WasA) {
			paA = 0;
			fOk = 0;
			goto soft_out;
		}
		paA = 0;
		if (g_cFree != cFree0 || g_cTotal != cTotal0) {
			fOk = 0;
			goto soft_out;
		}
		paD = gj_pmm_core_alloc();
		paE = gj_pmm_core_alloc();
		if (paD != u64WasA || paE != u64WasB) {
			if (paD != 0) {
				gj_pmm_core_free(paD);
				paD = 0;
			}
			if (paE != 0) {
				gj_pmm_core_free(paE);
				paE = 0;
			}
			fOk = 0;
			goto soft_out;
		}
		if (*pMark2 != 0xA11CEU || *pMark != 0xC0FFEEU) {
			gj_pmm_core_free(paD);
			gj_pmm_core_free(paE);
			paD = 0;
			paE = 0;
			fOk = 0;
			goto soft_out;
		}
		if (g_cFree != cFree0 - 2u || g_cTotal != cTotal0) {
			gj_pmm_core_free(paD);
			gj_pmm_core_free(paE);
			paD = 0;
			paE = 0;
			fOk = 0;
			goto soft_out;
		}
	}
	u32Surf |= SURF_DOUBLE;
	cAreas++; /* double_lifo */

	/* Restore both pages. */
	gj_pmm_core_free(paD);
	gj_pmm_core_free(paE);
	paD = 0;
	paE = 0;
	if (g_cFree != cFree0 || g_cTotal != cTotal0) {
		fOk = 0;
		goto soft_out;
	}
	if (gj_pmm_core_free_count() != cFree0 ||
	    gj_pmm_core_total_count() != cTotal0) {
		fOk = 0;
		goto soft_out;
	}
	u32Surf |= SURF_RESTORE;
	cAreas++; /* restore */

	/* --- freelist chain matches free_count (post) --- */
	cChain = soft_walk_free();
	if (cChain != cFree0) {
		fOk = 0;
		goto soft_out;
	}
	u32Surf |= SURF_CHAIN;
	cAreas++; /* chain_post */

	/* total immutable soft lamp */
	if (g_cTotal != cTotal0) {
		fOk = 0;
		goto soft_out;
	}
	u32Surf |= SURF_TOTAL;
	cAreas++; /* total */

soft_out:
	cChain = soft_walk_free();
	/* Best-effort restore if a path left pages outstanding. */
	if (paA != 0) {
		gj_pmm_core_free(paA);
	}
	if (paB != 0) {
		gj_pmm_core_free(paB);
	}
	if (paC != 0) {
		gj_pmm_core_free(paC);
	}
	if (paD != 0) {
		gj_pmm_core_free(paD);
	}
	if (paE != 0) {
		gj_pmm_core_free(paE);
	}
	pmm_core_soft_inventory(fOk, cAreas, cChain, g_cFree, g_cTotal,
	                        u32Surf);

	/* Grep: per-surface soft PASS lamps (only surfaces that actually ran). */
	if ((u32Surf & SURF_API) != 0u) {
		kprintf("pmm_core: soft api soft PASS free=%u total=%u\n",
		        g_cFree, g_cTotal);
	}
	if ((u32Surf & SURF_REJECT) != 0u) {
		kprintf("pmm_core: soft reject soft PASS\n");
	}
	if ((u32Surf & SURF_ALIGN) != 0u) {
		kprintf("pmm_core: soft align soft PASS\n");
	}
	if ((u32Surf & SURF_COUNTS) != 0u) {
		kprintf("pmm_core: soft counts soft PASS\n");
	}
	if ((u32Surf & SURF_SCRUB) != 0u) {
		kprintf("pmm_core: soft scrub soft PASS\n");
	}
	if ((u32Surf & SURF_HEAD) != 0u) {
		kprintf("pmm_core: soft head soft PASS\n");
	}
	if ((u32Surf & SURF_LIFO) != 0u) {
		kprintf("pmm_core: soft lifo soft PASS\n");
	}
	if ((u32Surf & SURF_DOUBLE) != 0u) {
		kprintf("pmm_core: soft double soft PASS\n");
	}
	if ((u32Surf & SURF_PAYLOAD) != 0u) {
		kprintf("pmm_core: soft payload soft PASS\n");
	}
	if ((u32Surf & SURF_EMPTY) != 0u) {
		kprintf("pmm_core: soft empty soft PASS\n");
	}
	if ((u32Surf & SURF_CHAIN) != 0u) {
		kprintf("pmm_core: soft chain soft PASS\n");
	}
	if ((u32Surf & SURF_TOTAL) != 0u) {
		kprintf("pmm_core: soft total soft PASS\n");
	}
	if ((u32Surf & SURF_RESTORE) != 0u) {
		kprintf("pmm_core: soft restore soft PASS free=%u total=%u\n",
		        g_cFree, g_cTotal);
	}

	/* Grep: pmm_core: soft return selftest — terminal return surface */
	kprintf("pmm_core: soft return selftest_ret=%d surf=0x%x areas=%u "
	        "product_kernel=OPEN DualDoD=OPEN G-AC-1=1\n",
	        fOk, u32Surf, cAreas);

	return fOk;
}
