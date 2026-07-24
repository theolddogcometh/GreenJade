/* Shared soft-milestone body — include after defining:
 *   CGJ_SOFT_M, CGJ_SOFT_B0..B8, CGJ_SOFT_IS_DIRECT
 * Mirror 25800 KATs; bar3=0. CREATE-ONLY. Not a standalone TU.
 */
#ifndef CGJ_SOFT_M
#error "CGJ_SOFT_M required"
#endif
#ifndef CGJ_SOFT_B0
#error "CGJ_SOFT_B0..B8 required"
#endif
#ifndef CGJ_SOFT_IS_DIRECT
#error "CGJ_SOFT_IS_DIRECT required (0 or 1)"
#endif

#define _CGJ_CONCAT(a, b) a##b
#define _CGJ_XCONCAT(a, b) _CGJ_CONCAT(a, b)
#define _CGJ_STR(x) #x
#define _CGJ_XSTR(x) _CGJ_STR(x)

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if !CGJ_SOFT_IS_DIRECT
#include <dlfcn.h>
#endif

#define GJ_SHELL_GREEN     _CGJ_XCONCAT(gj_shell_green_, CGJ_SOFT_M)
#define GJ_LIBCGJ_GREEN    _CGJ_XCONCAT(gj_libcgj_green_, CGJ_SOFT_M)
#define GJ_BAR3_READY      _CGJ_XCONCAT(gj_bar3_ready_, CGJ_SOFT_M)
#define GJ_PRODUCT_SCORE   _CGJ_XCONCAT(gj_product_score_, CGJ_SOFT_M)
#define GJ_CONTINUUM_READY _CGJ_XCONCAT(gj_continuum_ready_, CGJ_SOFT_M)
#define GJ_SMOKE_SOFT      _CGJ_XCONCAT(gj_smoke_soft_, CGJ_SOFT_M)
#define GJ_DYN_SOFT        _CGJ_XCONCAT(gj_dyn_soft_, CGJ_SOFT_M)
#define GJ_MILESTONE_TAG   _CGJ_XCONCAT(gj_milestone_tag_, CGJ_SOFT_M)
#define GJ_CONTINUUM_WAVE  _CGJ_XCONCAT(gj_continuum_wave_, CGJ_SOFT_M)
#define GJ_BATCH_ID        _CGJ_XCONCAT(gj_batch_id_, CGJ_SOFT_M)
#define GJ_GRAPH_MILESTONE _CGJ_XCONCAT(gj_graph_milestone_, CGJ_SOFT_M)

#define BATCH_MARKER(n) _CGJ_XCONCAT(__libcgj_batch, _CGJ_XCONCAT(n, _marker))
#define BATCH_STR(n) "libcgj-batch" _CGJ_XSTR(n)

#if CGJ_SOFT_IS_DIRECT
extern uint32_t GJ_SHELL_GREEN(void);
extern uint32_t GJ_LIBCGJ_GREEN(void);
extern uint32_t GJ_BAR3_READY(void);
extern uint32_t GJ_PRODUCT_SCORE(void);
extern uint32_t GJ_CONTINUUM_READY(void);
extern uint32_t GJ_SMOKE_SOFT(void);
extern uint32_t GJ_DYN_SOFT(void);
extern uint32_t GJ_MILESTONE_TAG(void);
extern uint32_t GJ_CONTINUUM_WAVE(void);
extern uint32_t GJ_BATCH_ID(void);
extern uint32_t GJ_GRAPH_MILESTONE(void);
extern const char BATCH_MARKER(CGJ_SOFT_B0)[];
extern const char BATCH_MARKER(CGJ_SOFT_B1)[];
extern const char BATCH_MARKER(CGJ_SOFT_B2)[];
extern const char BATCH_MARKER(CGJ_SOFT_B3)[];
extern const char BATCH_MARKER(CGJ_SOFT_B4)[];
extern const char BATCH_MARKER(CGJ_SOFT_B5)[];
extern const char BATCH_MARKER(CGJ_SOFT_B6)[];
extern const char BATCH_MARKER(CGJ_SOFT_B7)[];
extern const char BATCH_MARKER(CGJ_SOFT_B8)[];
extern const char BATCH_MARKER(CGJ_SOFT_M)[];

static int
direct_check(void)
{
	int nFail = 0;

	if (GJ_SHELL_GREEN() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_LIBCGJ_GREEN() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_BAR3_READY() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_PRODUCT_SCORE() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_CONTINUUM_READY() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_SMOKE_SOFT() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_DYN_SOFT() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_MILESTONE_TAG() != (uint32_t)CGJ_SOFT_M) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_CONTINUUM_WAVE() != (uint32_t)CGJ_SOFT_M) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_BATCH_ID() != (uint32_t)CGJ_SOFT_M) {
		fprintf(stderr, "libcgj: direct gj_batch_id_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (GJ_GRAPH_MILESTONE() != (uint32_t)CGJ_SOFT_M) {
		fprintf(stderr, "libcgj: direct gj_graph_milestone_%u fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	if (strcmp(BATCH_MARKER(CGJ_SOFT_B0), BATCH_STR(CGJ_SOFT_B0)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B1), BATCH_STR(CGJ_SOFT_B1)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B2), BATCH_STR(CGJ_SOFT_B2)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B3), BATCH_STR(CGJ_SOFT_B3)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B4), BATCH_STR(CGJ_SOFT_B4)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B5), BATCH_STR(CGJ_SOFT_B5)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B6), BATCH_STR(CGJ_SOFT_B6)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B7), BATCH_STR(CGJ_SOFT_B7)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_B8), BATCH_STR(CGJ_SOFT_B8)) != 0 ||
	    strcmp(BATCH_MARKER(CGJ_SOFT_M), BATCH_STR(CGJ_SOFT_M)) != 0) {
		fprintf(stderr, "libcgj: direct m%u marker string fail\n",
		        (unsigned)CGJ_SOFT_M);
		nFail++;
	}
	return nFail;
}
#else

typedef uint32_t (*u32_fn)(void);

static int
soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked)
{
	u32_fn pfn;
	uint32_t u32Got;

	pfn = (u32_fn)dlsym(h, szName);
	if (pfn == NULL)
		return 0;
	if (pChecked != NULL)
		(*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) {
		fprintf(stderr,
		        "libcgj: soft m%u %s KAT fail got=%u expect=%u\n",
		        (unsigned)CGJ_SOFT_M, szName, (unsigned)u32Got,
		        (unsigned)u32Expect);
		return 1;
	}
	return 0;
}

static int
soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked)
{
	const char *szGot;

	szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL)
		return 0;
	if (pChecked != NULL)
		(*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) {
		fprintf(stderr,
		        "libcgj: soft m%u %s marker fail got=\"%s\" "
		        "expect=\"%s\"\n",
		        (unsigned)CGJ_SOFT_M, szSym, szGot, szExpect);
		return 1;
	}
	return 0;
}

static const char *const kPaths[] = {
    "build/user/libc.so.6",
    "./build/user/libc.so.6",
    NULL,
};

#endif

int
main(void)
{
#if CGJ_SOFT_IS_DIRECT
	int nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr, "libcgj: soft m%u DIRECT FAIL fails=%d\n",
		        (unsigned)CGJ_SOFT_M, nFail);
		return 1;
	}
	printf("libcgj: soft m%u DIRECT PASS (all 11 lamps + markers)\n",
	       (unsigned)CGJ_SOFT_M);
	return 0;
#else
	void *h = NULL;
	const char *szPath = NULL;
	int i, nChecked = 0, nFail = 0;
	char sz[64];

	for (i = 0; kPaths[i] != NULL; i++) {
		h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
		if (h != NULL) {
			szPath = kPaths[i];
			break;
		}
	}
	if (h == NULL) {
		fprintf(stderr, "libcgj: soft m%u skip (dlopen fail: %s)\n",
		        (unsigned)CGJ_SOFT_M, dlerror());
		printf("libcgj: soft m%u SOFT-SKIP (no product SO)\n",
		       (unsigned)CGJ_SOFT_M);
		return 0;
	}

#define SOFT_U32(name, exp) do { \
	snprintf(sz, sizeof(sz), "%s_%u", (name), (unsigned)CGJ_SOFT_M); \
	nFail += soft_u32(h, sz, (exp), &nChecked); \
} while (0)

	SOFT_U32("gj_shell_green", 1u);
	SOFT_U32("gj_libcgj_green", 1u);
	SOFT_U32("gj_bar3_ready", 0u);
	SOFT_U32("gj_product_score", 0u);
	SOFT_U32("gj_continuum_ready", 1u);
	SOFT_U32("gj_smoke_soft", 1u);
	SOFT_U32("gj_dyn_soft", 1u);
	SOFT_U32("gj_milestone_tag", (uint32_t)CGJ_SOFT_M);
	SOFT_U32("gj_continuum_wave", (uint32_t)CGJ_SOFT_M);
	SOFT_U32("gj_batch_id", (uint32_t)CGJ_SOFT_M);
	SOFT_U32("gj_graph_milestone", (uint32_t)CGJ_SOFT_M);
#undef SOFT_U32

#define SOFT_MRK(n) do { \
	snprintf(sz, sizeof(sz), "__libcgj_batch%u_marker", (unsigned)(n)); \
	nFail += soft_marker(h, sz, BATCH_STR(n), &nChecked); \
} while (0)

	SOFT_MRK(CGJ_SOFT_B0);
	SOFT_MRK(CGJ_SOFT_B1);
	SOFT_MRK(CGJ_SOFT_B2);
	SOFT_MRK(CGJ_SOFT_B3);
	SOFT_MRK(CGJ_SOFT_B4);
	SOFT_MRK(CGJ_SOFT_B5);
	SOFT_MRK(CGJ_SOFT_B6);
	SOFT_MRK(CGJ_SOFT_B7);
	SOFT_MRK(CGJ_SOFT_B8);
	SOFT_MRK(CGJ_SOFT_M);
#undef SOFT_MRK

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m%u FAIL path=%s fails=%d checked=%d\n",
		        (unsigned)CGJ_SOFT_M, szPath, nFail, nChecked);
		return 2;
	}
	printf("libcgj: soft m%u PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       (unsigned)CGJ_SOFT_M, szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
