# Option B — hostish `netdev_ops` / `.ko` `ndo_open` call safety

| Field | Value |
|-------|--------|
| **Status** | **Abandoned leftover** (not linked). Option B **FORBIDDEN** as product. Dual DoD B hop is `rtl8168_udx` until interactive SSH login. |
| **Gate (handoff)** | `GJ_SOFT_R8169_MMIO_HANDOFF` — **default 0** |
| **Gate (Option B)** | `GJ_SOFT_R8169_KO_NDO_OPEN` — **default 0** (requires handoff gate **1**) |
| **Companion** | [R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md) phase 3 · [PCI_DEV_SOFT_LAYOUT.md](PCI_DEV_SOFT_LAYOUT.md) Strategy A · [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) D7 |
| **Law** | Dual **MIT OR Apache-2.0**; Soft ≠ product; **G-AC-1**; **no GPL source** in tree |
| **Honesty** | Soft-loaded host `.ko` is lab engineering only — **not** bar3 / product AC |

**Do not enable Option B as tree default.** Soft ≠ product.

---

## 1. What Option B would do

After freestanding MMIO quiesce + hostish sole-owner + phase-3 soft-open path:

| Step | Option A (default when handoff gate 1) | Option B (extra gate) |
|------|----------------------------------------|------------------------|
| `g_fMmioHandoff` | **1** after sole-owner | same |
| Soft carrier / queue | `linux_netdev_soft_open_primary()` | same |
| `.ko` `netdev_ops->ndo_open` | **never** | would call if layout proven |
| Wire TX/RX ownership | still phase **4** OPEN | still phase **4** OPEN |

Today with Option B gate on, code only **logs DEFER** — it does **not** invoke `.ko` `ndo_open`. Grep: `soft mmio handoff phase3 Option B`.

Default soft open path already refuses foreign ops:

```text
linux_netdev_soft: soft open skip .ko ops (MMIO freestanding owns) …
```

---

## 2. Soft `struct net_device` vs host layout (critical)

Soft netdev is an **incomplete clean-room shape** (front fields + pad to **2048** B). It is **not** host ABI.

### Soft layout (knano — `linux_netdev_soft.h`)

| Field | Soft offset | Notes |
|-------|-------------|--------|
| `name[16]` | **0x00** | Soft name |
| `ml_priv` | **0x10** | Soft |
| **`netdev_ops`** | **0x18 (24)** | Soft reader / soft open |
| `flags` / `mtu` / `dev_addr` | 0x20 / 0x24 / 0x28 | Soft |
| Soft bookkeeping | ~0x48… | Soft-only |
| Front pad to | **0x800 (2048)** | |
| Soft priv base | **+2048** | `abPriv[]` after front |

### Host oracle (RHEL **5.14.0-687** x86_64 — measured 2026-08-04)

Same class of measurement as `linux_pci_hostish_off.h` (throwaway OOT module + `offsetof`; **numbers only**, no GPL source committed).

| Field / fact | Host offset / size |
|--------------|--------------------|
| **`netdev_ops`** | **0x08** |
| `mtu` | 0x38 |
| `flags` | 0xb0 |
| `name` | 0x120 |
| `dev_addr` | 0x3e0 |
| `ml_priv` | 0x550 |
| embedded `struct device dev` | 0x578 |
| **`sizeof(struct net_device)`** | **0xaf0 (2800)** |
| `sizeof(struct net_device_ops)` | 0x3c0 (960) |
| `netdev_ops.ndo_init` | +0x00 |
| `netdev_ops.ndo_uninit` | +0x08 |
| **`netdev_ops.ndo_open`** | **+0x10** |
| `netdev_ops.ndo_stop` | +0x18 |
| `netdev_ops.ndo_start_xmit` | +0x20 |

Host field order is **cacheline-reorganized** (TX hotpath early). Soft order is **not** that layout.

### Soft ≠ host — consequence for REAL probe

REAL `r8169` probe (`ST=0`, `NETDEV SOFT 1`) allocates via soft `alloc_etherdev_mqs` / `devm_alloc_etherdev_mqs` (soft **2048**-byte front + priv). The `.ko` then **stores** fields at **host** offsets into that blob:

| Host write | Lands in soft object at… | Soft reader sees… |
|------------|--------------------------|-------------------|
| `dev->netdev_ops = …` @ **+0x8** | middle of soft **`name[8..15]`** | Soft `netdev_ops` @ **+0x18** is a **different** word |
| mid/late fields up to **0xaf0** | soft front + first **~752** B of **`abPriv`** | Soft bookkeeping / priv corruption risk |
| `netdev_priv(dev)` shape | host expects priv after **aligned 0xaf0** | Soft priv base is **+0x800** — **mismatch** |

So:

1. Soft `pNd->netdev_ops` is **not** reliably the pointer r8169 wrote.
2. Soft may report ops NULL / garbage / non-`.ko` while hostish slot **+0x8** holds the real ops pointer (or vice versa).
3. Calling “open” via soft `soft_netdev_ops` layout (`LNDS` magic + `ndo_open` at soft ops **+0x8**) is **not** host `net_device_ops` (`ndo_open` at **+0x10**).

**Conclusion:** soft layout ≠ host. A naïve Option B that does `pNd->netdev_ops->ndo_open(pNd)` on the soft struct is **unsafe** (wrong ops pointer, wrong ops table layout, wrong `net_device` this-call field offsets inside open).

---

## 3. Hostish `net_device` — Strategy A dual blob (landed)

**Yes — required before any real `.ko` `ndo_open`.** Dual object is **implemented** (fill + getters); **Option B call remains off**.

| Piece | Hostish pattern (proven for PCI) | Netdev analogue (this wave) |
|-------|----------------------------------|-----------------------------|
| Problem | Soft `pci_dev` ≠ offsets ≠ host inlined stores | Soft `net_device` ≠ host |
| Solution | Strategy A: **0xb40** blob + `linux_pci_hostish_off.h` | Hostish **net_device** blob **0xc00 ≥ 0xaf0** + `linux_netdev_hostish_off.h` |
| Fill | BAR/IRQ/vendor from inventory | **memcpy** soft primary slab → hostish **0xaf0** after `register_netdev` (captures `.ko` host-offset stores into soft VA) + optional soft name/mtu/MAC overlay |
| Gate | REAL probe only when hostish filled | Option B **call** only when hostish netdev proven + both gates |

### Landed sizing / APIs (mirror PCI hostish)

| Item | Value |
|------|--------|
| Offset header | [`abandoned/kernel/include/gj/linux_netdev_hostish_off.h`](../abandoned/kernel/include/gj/linux_netdev_hostish_off.h) — kver **5.14.0-687.15.1.el9_8.x86_64** (historical) |
| Blob size | **`LINUX_NETDEV_HOSTISH_BLOB_BYTES = 0xc00`** ≥ **`SIZE_NET_DEVICE = 0xaf0`** |
| Pool | **1** slot (static); dual object — soft inventory stays on soft struct |
| Fill entry | `linux_netdev_soft_hostish_fill()` after soft `register_netdev` PASS (primary) and from ops diagnostic refresh |
| Ops recovery | `linux_netdev_soft_hostish_ops(p)` → `*(void **)(p + 0x08)` (soft VA **or** hostish blob) |
| `ndo_open` recover | `linux_netdev_soft_hostish_ndo_open(p)` → `*(void **)(ops + 0x10)` — **stored**, **never called** |
| Getters | `linux_netdev_soft_hostish_primary()` · `linux_netdev_soft_hostish_filled()` |
| Grep | `linux_netdev_soft: soft hostish net_device fill PASS ops=… ndo_open=… in_r8169=…` |

### Soft priv vs host priv (documented conflict)

| Base | Offset |
|------|--------|
| Soft priv | soft front **+0x800** (`abPriv[]`) |
| Host `netdev_priv` | after aligned **0xaf0** |

Soft slab is large enough that host mid-fields through **0xaf0** land in soft front + first **~752** B of `abPriv` (`.ko` host writes during REAL probe). Dual hostish blob copies those bytes; soft bookkeeping fields remain soft-owned. **Option B must not assume soft priv == host priv.**

### Soft VA as hostish-shaped read source

During REAL probe, r8169 receives the soft-allocated `net_device` pointer and writes **host** offsets into that buffer. Therefore:

1. Soft primary VA is a valid **read** source for host `netdev_ops` @ **+0x8**.
2. Dual hostish blob **memcpy**s `0xaf0` from that slab so Option B can later pass a dedicated blob (layout-stable fill target) without depending on soft field order.
3. Soft `netdev_ops` @ **+0x18** remains the soft reader path (L2 / open_primary).

Until diagnostics show hostish `netdev_ops` / `ndo_open` ∈ r8169 load image **and** freestanding is quiesced, **keep Option B DEFER only** (no call).

---

## 4. Prerequisites (hard gates)

Before any future Option B **call** (not today’s DEFER log):

| # | Prerequisite | Evidence / API |
|---|--------------|----------------|
| P1 | Handoff gate **1** | `-DGJ_SOFT_R8169_MMIO_HANDOFF=1` (never ship default 1) |
| P2 | Freestanding prepare done | `rtl8168: soft mmio handoff PREPARE` · `g_fReady==0` · TE/RE off |
| P3 | Sole-owner | `linux_netdev_soft_mmio_sole_owner() != 0` · `ready()==2` · hostish `pci_dev` BAR/IRQ owner |
| P4 | Phase-3 path sets handoff | `g_fMmioHandoff=1` · `soft napi mmio_handoff=1` |
| P5 | Freestanding not live at try_open | else **FAULT** dual-drive |
| P6 | Layout confidence | hostish netdev **or** controlled ops table; diagnostic hostish ops ∈ r8169 load range |
| P7 | Option B gate **1** | `-DGJ_SOFT_R8169_KO_NDO_OPEN=1` **and** P1 |
| P8 | Soft ≠ product accepted | serial honesty; not G-AC-1 / bar3 |

Order: **gate1 sole-owner lab first** → hostish netdev / ops diagnostic PASS → only then consider a one-shot `ndo_open` lab.

---

## 5. Risks

| Risk | Why |
|------|-----|
| **Wrong ops pointer** | Soft `netdev_ops` @ 0x18 ≠ host @ 0x8; call through soft field may jump to garbage or skip real open |
| **Wrong ops table ABI** | Soft `LNDS` magic table vs host `net_device_ops` (`ndo_open` @ +0x10 not soft +0x8) |
| **Wrong this-call layout** | `ndo_open(dev)` reads/writes host field offsets; soft shape lies |
| **Priv / ring base wrong** | Host priv after 0xaf0; soft priv at +0x800 |
| **MMIO dual-drive** | If freestanding still TE/RE or rings live while `.ko` open programs BAR → undefined HW |
| **NAPI / IRQ** | Open may request IRQ / enable NAPI; soft IRQ path incomplete |
| **Firmware / PHY** | Open may need real `.fw` / PHY soft — still ENOENT class failures |
| **Irreversible lab hang** | Bad open can wedge NIC until reboot |

---

## 6. Recommendation

| Priority | Action |
|----------|--------|
| **Now** | Leave **both** gates **0** on default images. Run ops **diagnostic only** (below). |
| **Next lab** | Gate1 handoff: PREPARE → sole-owner → Option A soft-open; prove no dual-drive. |
| **Before Option B call** | Hostish `net_device` blob (size ≥ 0xaf0) + offset header for the **same** kver as staged `r8169.ko`; fill parent/MAC/mtu; re-run diagnostic on hostish primary. |
| **Option B call** | Only with handoff=1, sole-owner, freestanding ready=0, `g_fMmioHandoff=1`, Option B=1, hostish ops in r8169 text, and explicit lab consent. **Still Soft≠product.** |
| **Never** | Default-on Option B; claim product AC from `.ko` open; dual freestanding + soft TE/RE. |

**Recommended call policy:** hostish net_device **OR** only call open if **we** control the ops table (soft-owned `LNDS` table — already allowed, not r8169). Do **not** call host `.ko` open through soft field layout.

---

## 7. Soft ops diagnostic + hostish fill (implemented; no call)

Read-only probe: soft primary’s soft `netdev_ops` word **and** hostish word at **+0x8** / `ndo_open` at ops **+0x10**; dual hostish blob fill; test membership in soft-loaded `r8169` image range when module table has bounds.

| Item | Value |
|------|--------|
| Grep | `linux_netdev_soft: soft ops diagnostic` · `soft hostish net_device fill PASS` |
| When | Fill after `register_netdev`; diagnostic once from `linux_netdev_soft_open_primary` (refreshes fill) |
| Calls `ndo_open`? | **No** |
| Gate | Always safe to log; Option B remains DEFER |

Interpret:

| Pattern | Meaning |
|---------|---------|
| soft ops NULL, hostish non-NULL ∈ r8169 | `.ko` wrote host offset; soft field wrong — hostish recovery **working** |
| fill PASS `ops=` ∈ r8169, `ndo_open=` non-NULL | Option B **layout readiness** (still no call) |
| both NULL | probe did not set ops / wrong netdev / EMU path |
| soft ops non-NULL, `soft_magic=1` | soft-owned ops table only |
| hostish ∈ r8169 but freestanding ready | **do not** open — dual-drive |

---

## 8. Greppable markers

| Marker | Meaning |
|--------|---------|
| `linux_netdev_soft: soft open skip .ko ops` | Option A path refuses foreign ops |
| `linux_netdev_soft: soft ops diagnostic …` | Layout / range diagnostic (no call) |
| `linux_netdev_soft: soft hostish net_device fill PASS …` | Dual hostish blob filled; ops/ndo_open recovered (no call) |
| `linux_netdev_soft: soft mmio handoff phase3 soft-open only` | Option A done |
| `linux_netdev_soft: soft mmio handoff phase3 Option B … DEFER` | Option B gate on, still no call |
| `rtl8168: soft mmio handoff PREPARE\|FAULT` | Freestanding side |

---

## 9. Lab enable strings (do not ship defaults)

```text
# Default — freestanding path intact
# (no -D; both gates 0)

# Phase 1–3 Option A only:
make ... CFLAGS+='-DGJ_SOFT_R8169_MMIO_HANDOFF=1'

# Option B gate on — today DEFER only (still no .ko ndo_open):
make ... CFLAGS+='-DGJ_SOFT_R8169_MMIO_HANDOFF=1 -DGJ_SOFT_R8169_KO_NDO_OPEN=1'
```

---

## 10. Next lab step (safe order)

1. Boot default: confirm `soft hostish net_device fill PASS` + diagnostic after REAL+SOFT1 / soft open; record `ops` / `ndo_open` / `in_r8169`.
2. Gate1 sole-owner without dual-drive ([R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md)).
3. Option B **call** prototype only if fill PASS + hostish ops/ndo_open ∈ r8169 + freestanding down + both gates + lab consent. Prefer hostish blob as `dev` arg (not soft field layout).
4. Still Soft≠product; never default-on Option B.

---

*Dual MIT OR Apache-2.0. Soft ≠ product. G-AC-1: no `.ko` as product AC. No Linux GPL source in this document — host offsets are lab-measured numbers only.*
