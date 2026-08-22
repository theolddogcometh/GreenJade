# xhci_udx — clean-room UDX USB host residual (Intel PCH `8086:a12f`)

| Field | Value |
|-------|--------|
| **Path** | `user/drivers/xhci_udx/` |
| **License** | **MIT OR Apache-2.0** - dual; **no GPL**, no Linux xHCI paste |
| **Match** | PCI `8086:a12f` (G752VT 100 Series / C230 xHCI) · optional class `0x0c0330` |
| **Runtime** | Links **libudx** (`user/udx/`) - Linux-shaped `probe` / `quiesce` / `remove` |
| **Dual DoD A** | **OPEN** — RS-off product program; BOT/MSC stick **not** closed |
| **Scratchpad clamp** | `XHCI_PROD_SPAD_PAGES_CLAMP` **64** (a12f typically 31). **USBCMD.RS=0**. Dual DoD A **OPEN** (`need=usbcmd_rs`). |
| **Fly bar** | Match kernel `GJ_IMAGE_VERSION` (**STATUS (static) v0.1.184**; packed, not host-probed). Dual DoD **A/B OPEN**. Never `USBCMD.RS=1`. **0.2.0** reserved |
| **Program** | real_ddi+gate: halt if RS/HCH running, public xECP walk + USBLEGSUP OS handshake (missing → `usblegsup=0` continue; timeout → SKIP), then scratchpad DCBAA[0] + once RS-off `CONFIG`/`DCBAAP`/`CRCR`/`ERST*` + `IMAN.IE` IR0 + post-IMAN IRQ_BIND + command-ring doorbell name/fail-close + once-read public PORTSC CCS (`product program PASS`); never set `USBCMD.RS`; never ring doorbell while halted; never write PORTSC; `doorbell=OPEN` · `need=usbcmd_rs` |
| **Freestanding MSC** | **SKIP** default (`GJ_XHCI_MSC_PROBE=0`) — not product |

## Honesty split (read this first)

| Path | Where | Claim |
|------|--------|--------|
| **Freestanding lab MSC** | `abandoned/kernel/drv/xhci_msc.c` | Soft / lab scaffold only - **default OFF** (`GJ_XHCI_MSC_PROBE=0`); **not linked**; **not** product; **do not thrash** from this tree |
| **This tree** | `user/drivers/xhci_udx/` | **Userspace Linux-shaped driver host** against UDX - soft open/MAP + **prefer real DDI bind** (8086:a12f; inject fallback) + **DMA ring residual** + **IOMMU grant residual** + cap/params/ports + **op residual** + **cap-ext residual** + BOT stub + **EP0/config/CDB residual** + **DDI caps residual** + **product residual catalog/densify** + **program gate honesty** (`never_program=1` unless proven real_ddi+gate) + **residual lean once-lamps** (Dual DoD A C1/C2 lean; **OPEN**) |
| **In-kernel `.ko` USB** | Soft module path eng only | **G-AC-1**: no `usb_storage.ko` (or any `.ko`) init in kernel as product AC |
| **BOT / stick log / MSC** | Neither path here as product | Soft residual lamps **!=** CBW/CSW / GET_DESC wire / SET_CONFIG / SCSI CDB / stick log |

### Product model (normative)

| Rule | Meaning |
|------|---------|
| **G-AC-1** | No Linux `.ko` binary runs in the kernel as product |
| **Dual DoD A product direction** | Linux-shaped **userspace** USB host over **DDI/UDX** (this tree + soft ksym eng) - **product=UDX+ABI**; **need=DDI caps** |
| **Not product** | In-kernel `usb_storage.ko` init · freestanding MSC as T1 close |
| **Freestanding MSC default** | **`GJ_XHCI_MSC_PROBE=0`** - freestanding MSC **SKIP** at bring-up; set `1` only for targeted lab residual |
| **Soft!=product** | Soft open/MAP/op/EP0/config/CDB catalog PASS != stick PASS; once-lamps preferred (no stamp storms; no version stamp) |

GreenJade product I/O direction remains ABI-first + virtio T0 + future DDI grants.
In-kernel freestanding xHCI stages stay **soft / optional** (see [docs/G752VT_LINUX_HWTEST.md](../../../docs/G752VT_LINUX_HWTEST.md),
[docs/DDI_SOFT.md](../../../docs/DDI_SOFT.md), [docs/UDX_LINUX_PORTER.md](../../../docs/UDX_LINUX_PORTER.md),
[docs/LAPTOP_LINUX_DRIVER_HOST.md](../../../docs/LAPTOP_LINUX_DRIVER_HOST.md) wave D,
[docs/TODO.md](../../../docs/TODO.md) dual DoD A).

On many lab hosts `xhci_pci` / `xhci_hcd` are **kernel builtin** (no plain `.ko`
to embed). Collect reports `xhci_pci=BUILTIN`; freestanding STATUS soft-skips
the xHCI module path - [docs/LINUX_MODULE_PATH.md](../../../docs/LINUX_MODULE_PATH.md).

## What the soft skeleton does (lean residual)

1. **Match** id table: exact `8086:a12f` and optional PCI class `0x0c0330` (USB xHCI).
2. **Soft catalog**: Dual DoD A surface list (`soft catalog`) - no product claim.
3. **Soft open**: `pci_enable` -> `request_regions` - **open path honesty** (MAP still ahead; `product_mint=0`).
4. **Soft MAP**: **ioremap BAR0** - **map path honesty** (EP0 program still OPEN).
5. **Soft DMA ring residual**: `udx_dma_alloc_coherent` cmd+evt TRB windows
   (layout zero; **never** CRCR/DCBAAP/ERST program) + **`udx_dma_iommu_grant`**
   residual note (`soft ring residual` / `soft dma residual` / `soft iommu residual`).
6. **Stage 1 cap**: soft-read **CAPLENGTH** / **HCIVERSION** + **cap residual**
   major/minor once-lamp (xHCI Spec offsets only).
7. **Stage 2 params**: soft-read **HCSPARAMS1** + **HCSPARAMS2** + **HCSPARAMS3**
   observe (`soft params residual` MaxScratchpadBufs + U1/U2 exit latency names -
   soft residual **never** scratchpad alloc / LPM program).
8. **Stage 3 ports**: soft PORTSC walk at `op + 0x400 + (n-1)*0x10` (op = CAPLENGTH);
   count CCS; lamp first connected port + speed + **PLS** residual name;
   **ports residual** change-bit *names* only (**never** PORTSC W1C / port reset / PLS write).
9. **Soft op residual**: soft-read **USBCMD/USBSTS/PAGESIZE/DNCTRL/CRCR_lo/DCBAAP_lo/CONFIG**
   + USBSTS bit *names* (HSE/EINT/PCD/CNR/HCE) + PAGESIZE page-bit residual
   (observe only - **never** RS/HCRST/ring program / USBSTS W1C).
10. **Soft cap-ext residual**: soft-read **HCCPARAMS1 / DBOFF / RTSOFF** with
    AC64/BNC/CSZ/xECP field names (**soft path never** xECP walk / USBLEGSUP /
    doorbell/runtime program — product program walks xECP separately).
11. **Stage 4 stub**: soft BOT **progress** lamp only - catalogs next product steps
    without CBW/CSW wire.
12. **Stage 5 EP0 residual**: clean-room catalog steps 1..4 - Enable Slot /
    Address Device / GET_DESCRIPTOR(DEVICE,18) Setup fields /
    GET_DESCRIPTOR(CONFIG,hdr) Setup fields + GET_STATUS residual note - **no Setup TRB**.
13. **Stage 6 config residual**: clean-room catalog steps 1..5 -
    SET_CONFIGURATION Setup / parse IF (08/06/50) / claim bulk EPs /
    GET_MAX_LUN / BOT CBW·CSW signature residual - **no SET_CONFIG write / no BOT**.
14. **Stage 7 CDB residual (lean)**: clean-room SCSI opcode catalog steps 1..5 -
    TEST UNIT READY / REQUEST SENSE / INQUIRY / READ CAPACITY(10) / READ(10) +
    MODE SENSE(6)/START STOP residual *notes* - **no CBW / no wire**.
15. **Soft DDI caps residual (lean; need=DDI_caps)**: 3-step OPEN catalog -
    MMIO_FRAME / IRQ Notification / DMA window - **never mints** product caps.
16. **Stage 8 product residual (lean C2)**: 6-step OPEN catalog -
    run_stop / rings / irq_db / slot_addr / ep0_cfg / bot_stick -
    **never programs HW**; `soft product step=` / `soft product catalog PASS` +
    **`soft product densify`** (prefer_real_ddi / MAP_BAR / dma_ring / iommu /
    program_gate once-lamps; `never_program=1`).
17. **Product program (RS-off)**: when real_ddi + program_gate, if
    `USBCMD.RS=1` or `USBSTS.HCH=0` fail-closed halt (`USBCMD.RS=0` only;
    bounded HCH wait; **never** HCRST unless halt timeout + documented need).
    Still running → `product program SKIP reason=controller_running`.
    Then public xECP walk (`HCCPARAMS1.xECP` + Next) for **USBLEGSUP** (ID=1):
    missing → lamp `usblegsup=0` and continue; present → OS/BIOS semaphore
    handshake (xHCI 7.1, bounded wait); timeout or OS bit not sticky →
    `product program SKIP`. Then if `HCSPARAMS2` MaxScratchpadBufs==0 lamp
    `scratchpad=0` and continue; if >0 alloc FORCE32 array+pages (clamp 64),
    write `DCBAA[0]`=array PA; alloc fail or need>clamp → `product program SKIP`.
    Then once write public `CONFIG.MaxSlotsEn` → `DCBAAP` → `CRCR.RCS` →
    `ERSTSZ`/`ERSTBA`/`ERDP` → optional public `IMODI=4000` (1 ms) →
    `IMAN.IE` for IR0 when RTSOFF known. Prefer `IMAN.IE` only (do **not**
    set `USBCMD.INTE`). Then DDI `IRQ_BIND` once on retained handle
    (`irq_bind=PASS/FAIL/OPEN`). Then name + fail-close the public
    command-ring doorbell (`DBOFF` from cap; `doorbell[0]` = slot 0 /
    Host Controller Command Ring). Range-check `DBOFF`; **read** (never
    write) `doorbell[0]`. Missing/OOR → `product program doorbell SKIP`
    reason, continue (never invent offsets). **Do not ring** while halted
    (`never_ring_while_halted=1`). Then once-read public PORTSC for
    ports 1..MaxPorts (HCSPARAMS1); decode CCS only (xHCI 5.4.8).
    **Never write PORTSC** (no PR/WPR/PP/PED/CSC W1C). Op/port range
    OOR → `product program portsc SKIP reason=portsc_oor`, continue.
    MAP miss or DMA alloc fail → `product program SKIP`.
    **Never set** `USBCMD.RS=1`. **Never write doorbell if RS=0**.
    Dual DoD A remains **OPEN** (`need=usbcmd_rs`).
18. **Honesty catalog**: soft-ready flags + open/MAP honesty + `soft bind_ne_stick` +
    path bits + `soft probe residual` / `soft residual` vs product-OPEN steps;
    product model (`product=UDX+ABI` · `need=DDI_caps` · freestanding MSC SKIP;
    **dual_dod_a=OPEN**).
19. **Residual lean (once-lamps)**: Dual DoD A soft probe/map/cap catalog +
    `soft residual lean cap` (cap/params/ports/op/cap-ext + hcs3/pagesize/pls) +
    `soft residual lean catalog` (ep0/cfg/cdb/ddi/prod depth) +
    `soft residual lean ddi` + honesty + rollup
    (no stamp storms; no version stamp).
20. **quiesce**: mark soft idle (no bus-master product arm in this skeleton).
21. **remove**: free DMA ring → `iounmap` → release regions → disable.

Host demo **prefers** `udx_host_bind_by_id(8086,a12f)` (GJ_SYS_DDI SCAN/GET/OPEN/MAP_BAR)
over inject; host-linux typically soft-SKIPs DDI, then injects a synthetic function,
seeds soft BAR0 capability dword0 + HCSPARAMS1 + PORTSC[1] (CCS|PED|PP|SS) +
DBOFF/RTSOFF + HCCPARAMS1(AC64) + USBSTS(HCH), and re-registers so probe walks
stages 1→8 (+ DMA ring / product densify residual).

## Soft open / MAP path honesty

| Soft step | Greppable | Product claim |
|-----------|-----------|---------------|
| enable + regions | `soft open PASS` · `soft open path` · `soft open honesty` | **None** - not CNode mint |
| ioremap BAR0 | `soft map PASS` · `soft map path` · `soft map honesty` · `soft open+map PASS` | **None** - not product MMIO_FRAME |
| open + map together | `open=1 map=1` · `soft open/map honesty` | Soft only; EP0/BOT still OPEN |
| residual step counts | `ep0_steps=N/4` · `cfg_steps=N/5` · `cdb_steps=N/5` · `ddi_steps=N/3` · `prod_steps=N/6` · `path=0x...` | Catalog depth only |

Soft open alone != MAP. Soft MAP != EP0 program. Soft EP0 catalog != GET_DESC on wire.
Soft config catalog != SET_CONFIG / BOT. Soft CDB catalog != CBW / stick.
Soft DDI caps catalog != product MMIO/IRQ/DMA mint. Soft product catalog != product USB host / stick PASS.
**Dual DoD A remains OPEN** until userspace UDX owns lab USB datapath with real DDI caps.

## Greppable markers

| Marker | Meaning |
|--------|---------|
| `xhci_udx: soft open PASS` / `soft open path` / `soft open honesty` | Soft enable+regions (MAP still ahead) |
| `xhci_udx: soft map PASS` / `soft map path` / `soft map honesty` | BAR0 ioremap soft MAP |
| `xhci_udx: soft open+map PASS` | open + MAP both soft-ok |
| `xhci_udx: soft open/map honesty` | open!=map!=ep0!=config!=stick rollup + step counts |
| `xhci_udx: soft cap ver=` | CAPLENGTH + HCIVERSION soft snapshot |
| `xhci_udx: soft cap residual` | CAPLENGTH + HCIVERSION major/minor residual once-lamp |
| `xhci_udx: soft bot stage=1 cap` | Soft BOT progress: capability ok |
| `xhci_udx: soft params ... maxports=` | HCSPARAMS1 soft snapshot |
| `xhci_udx: soft params residual` | HCSPARAMS2/3 observe · MaxScratchpadBufs + U1/U2 latency names - soft residual never spad alloc / LPM |
| `xhci_udx: soft bot stage=2 params` | Soft BOT progress: params ok |
| `xhci_udx: soft port N ccs=1 ...` | Per-port PORTSC soft CCS hit (+ pls residual name) |
| `xhci_udx: soft ports n=... ccs=...` | Soft port walk summary |
| `xhci_udx: soft ports residual` | PORTSC change-bit / PLS *names* - **never W1C / port reset / PLS write** |
| `xhci_udx: soft bot stage=3 ports` | Soft BOT progress: ports walked |
| `xhci_udx: soft bot would_try_port=` | Soft next-step note (no Enable Slot) |
| `xhci_udx: soft catalog` | Dual DoD A surface list - **!= product** · dual_dod_a=OPEN |
| `xhci_udx: soft op residual` | USBCMD/USBSTS/DNCTRL/... observe - **never RS/HCRST/ring program** |
| `xhci_udx: soft op residual bits` | USBSTS HSE/EINT/PCD/CNR/HCE + PAGESIZE page-bit residual |
| `xhci_udx: soft cap-ext residual` | HCCPARAMS1(AC64/xECP)/DBOFF/RTSOFF - **soft never xECP walk / doorbell** |
| `xhci_udx: soft bot stage=4 stub` | Soft BOT catalog complete |
| `xhci_udx: soft bot stub PASS` | Soft progress path finished - **!= product BOT** |
| `xhci_udx: soft ep0 residual` / `soft ep0 step=` / `soft ep0 setup` / `soft ep0 catalog PASS` | EP0 residual scaffolding (4 steps) - **!= GET_DESC wire** |
| `xhci_udx: soft config residual` / `soft config step=` / `soft config setup` / `soft config catalog PASS` | Config residual scaffolding (5 steps) - **!= SET_CONFIG product** |
| `xhci_udx: soft cdb residual` / `soft cdb step=` / `soft cdb catalog PASS` | MSC CDB residual scaffolding (5 steps) - **!= CBW/SCSI wire** |
| `xhci_udx: soft cdb residual note` | MODE SENSE(6)/START STOP residual *names* (not counted steps) |
| `xhci_udx: soft ddi caps residual` / `soft caps residual` / `soft caps step=` / `soft caps catalog PASS` | DDI caps residual (3 steps: mmio/irq/dma) - **!= product mint** |
| `xhci_udx: soft product residual` / `soft product step=` / `soft product catalog PASS` | Product OPEN catalog (6 steps) - **!= stick / BOT program** |
| `xhci_udx: soft product densify` / `soft product densify prefer_real_ddi=` / `map_bar=` / `dma_ring=` / `iommu=` / `program_gate=` | Product residual densify (prefer real DDI + MAP_BAR + DMA/IOMMU + gate honesty; **never_program=1**) |
| `xhci_udx: soft dma residual` / `soft ring residual` / `soft iommu residual` | Soft cmd/evt ring layout + `udx_dma_iommu_grant` residual - **never** CRCR/DCBAAP/ERST write |
| `xhci_udx: product program` / `PASS` / `SKIP` | RS-off public CONFIG/DCBAAP/CRCR/ERST + IMAN.IE when real_ddi+gate; `rs=0`; `irq_bind=OPEN`; MAP/DMA fail → SKIP |
| `xhci_udx: product program iman` / `ie=` / `usbcmd_inte=0` | IR0 `IMAN.IE` after ERDP (prefer IMAN only; never USBCMD.INTE / RS) |
| `xhci_udx: product program imod` / `conservative=1` | Optional public IMODI=4000 (1 ms); skip claim if readback miss |
| `xhci_udx: product program irq_bind` / `irq_bind=OPEN` | DDI IRQ_BIND once on retained handle; PASS/FAIL/OPEN; never invent MSI-X |
| `xhci_udx: product program doorbell` / `doorbell=OPEN` / `never_ring_while_halted=1` / `need=usbcmd_rs` | Name + fail-close command-ring doorbell (`DBOFF`; `doorbell[0]`=slot 0). Read-only. Missing/OOR → `SKIP reason=dboff_missing\|dboff_oor`, continue. Never ring while halted. Never write if RS=0. |
| `xhci_udx: product program portsc` / `ports=` / `ccs=` / `ccs_n=` / `never_portsc_write=1` / `reason=portsc_oor` | After PASS: once-read PORTSC[1..MaxPorts] CCS only (xHCI 5.4.8). `ccs=`=any stick present; `ccs_n=`=CCS count. Op/port OOR → SKIP reason, continue. Never write PORTSC. Serial lamp + hold3 `ccs=0\|1\|?` (glass, no-COM1). |
| `xhci_udx: product program status hold` / `hold3=` / `UDX xhci PASS` / `UDX xhci SKIP` | Once-pin STATUS hold3 after PORTSC observe: `UDX xhci PASS rs=0 iman=N irq=WORD ccs=N` (or SKIP `reason=…` unchanged). Never hold0/2/6/14/15. |
| `xhci_udx: product program scratchpad` / `scratchpad=` / `dcbaa0=` | HCSPARAMS2 MaxScratchpadBufs: 0 continue; >0 FORCE32 array+pages + DCBAA[0]; alloc fail / need>64 → SKIP |
| `xhci_udx: product program halt` / `rs_was=` / `HCH_after=` | Fail-closed halt before silicon writes (`USBCMD.RS=0` only; bounded HCH; never HCRST this gate) |
| `xhci_udx: product program usblegsup` / `usblegsup=` / `xecp_walk=` | Public xECP walk + USBLEGSUP OS handshake (missing=`usblegsup=0` continue; present=OS/BIOS semaphores; timeout → SKIP) |
| `xhci_udx: soft ddi prefer` / `soft prefer real DDI` / `prefer_real_ddi=1` | Prefer GJ_SYS_DDI bind_by_id over inject-only (`chain=SCAN,GET,OPEN,MAP_BAR`) |
| `xhci_udx: soft bind_ne_stick` | bind/open/MAP != stick honesty (Dual DoD A C1) |
| `xhci_udx: soft ready open=... map=... op_res=... cap_ext=... ep0_res=... cfg_res=... cdb_res=... ddi_caps=... prod_cat=... path=` | Soft-ready honesty rollup |
| `xhci_udx: soft probe residual` / `soft residual` | Path-bit residual rollup - **!= stick PASS** · dual_dod_a=OPEN |
| `xhci_udx: soft residual lean` | Dual DoD A once-lamp probe/map/cap catalog residual |
| `xhci_udx: soft residual lean cap` | cap/params/ports/op/cap-ext residual lean once-lamp (+ hcs3/pagesize/pls) |
| `xhci_udx: soft residual lean catalog` | ep0/cfg/cdb/ddi/prod step depth once-lamp |
| `xhci_udx: soft residual lean ddi` | DDI caps residual lean once-lamp · need=DDI_caps · dual_dod_a=OPEN |
| `xhci_udx: soft residual lean honesty` | freestanding MSC SKIP · **product=UDX+ABI** · **need=DDI_caps** · Soft!=product |
| `xhci_udx: soft residual lean rollup` | lean path/step rollup · dual_dod_a=OPEN · no version stamp |
| `xhci_udx: soft product model` | Userspace UDX direction · product=UDX+ABI · need=DDI_caps · MSC SKIP · dual_dod_a=OPEN |
| `xhci_udx: soft probe PASS` | probe completed (bot_stage=8 on host demo) |
| `xhci_udx: soft quiesce` | DDI-style quiesce ran |
| `xhci_udx: soft remove` | remove completed |
| `xhci_udx: host soft demo done` | host main verified soft seed + residual flags |

```sh
make -C user/drivers/xhci_udx
./user/drivers/xhci_udx/build/xhci_udx | tee /tmp/xhci_udx.log
grep -E 'xhci_udx: soft (probe PASS|probe residual|residual lean|catalog|open |map |open/map|op residual|cap residual|cap-ext|bot stage=|bot stub PASS|ep0 |config |cdb |ddi caps|caps residual|caps step=|product residual|product step=|product catalog|product densify|dma residual|ring residual|iommu residual|bind_ne_stick|ports |params |product model|residual |ddi prefer|prefer real)|product program|product program scratchpad|product program usblegsup|product program iman|product program imod|product program irq_bind|product program doorbell|product program portsc|doorbell=OPEN|never_ring_while_halted=1|need=usbcmd_rs|never_portsc_write=1|ports=|ccs=|ccs_n=|reason=portsc_oor|dboff=|db0=|usblegsup=|xecp_walk=|scratchpad=|irq_bind=OPEN|prefer_real_ddi|never_program|product=UDX\+ABI|need=DDI|Soft!=product|dual_dod_a=OPEN|soft residual lean (cap|catalog|ddi|honesty|rollup)' /tmp/xhci_udx.log
```

## Build

### Product freestanding (Dual DoD A — embed / stage-esp)

```sh
# from repo root — static user.ld + libudx-fs + libgj (no UDX_HOST_LIBC)
make drivers-udx
# → build/user/drivers/xhci_udx  (ENTRY _start → freestanding main →
#    xhci_udx_freestanding_start → prefer udx_host_bind_by_id(8086:a12f) →
#    product_program_try; keep_live park after soft bind PASS)
file build/user/drivers/xhci_udx   # expect: statically linked
```

### Host-libc soft lab (inject 8086:a12f; product program SKIP)

```sh
make udx
make -C user/drivers/xhci_udx
./user/drivers/xhci_udx/build/xhci_udx
# optional staged host binary:
# make build/user/drivers/xhci_udx.host
```

Host path **prefers** `udx_host_bind_by_id(8086,a12f)` then **inject fallback**
(seed BAR0) when DDI soft-SKIPs. Freestanding product ELF uses
`xhci_udx_freestanding_start` / `_start` via **GJ_SYS_DDI** SCAN/GET/OPEN/MAP_BAR
— still **no** product BOT/EP0/MSC claim; `product_program_try` does
USBLEGSUP handshake then writes RS-off scratchpad `DCBAA[0]` +
`CONFIG`/`DCBAAP`/`CRCR`/`ERST*` + `IMAN.IE` when real_ddi+gate, then
names the command-ring doorbell fail-closed and once-reads PORTSC CCS
(never `USBCMD.RS`; never ring while halted; never write PORTSC).
Soft DDI open/MAP + soft DMA ring != product cap mint. **Dual DoD A remains OPEN.** Soft!=product.

## Public registers / USB names (clean-room)

Offsets relative to **BAR0** unless noted. Soft path only:

| Offset | Name | Width | Soft use |
|--------|------|-------|----------|
| `0x00` | **CAPLENGTH** | 8-bit | Length of capability registers |
| `0x02` | **HCIVERSION** | 16-bit | Interface Version Number (major/minor residual) |
| `0x04` | **HCSPARAMS1** | 32-bit | MaxSlots / MaxIntrs / MaxPorts |
| `0x08` | **HCSPARAMS2** | 32-bit | Soft observe MaxScratchpadBufs; product program DCBAA[0] |
| `0x0C` | **HCSPARAMS3** | 32-bit | Soft observe U1/U2 Device Exit Latency names only |
| `0x10` | **HCCPARAMS1** | 32-bit | Soft observe AC64/BNC/CSZ/xECP; product program walks xECP for USBLEGSUP |
| `xECP + Next` | **USBLEGSUP** (ID=1) | 32-bit | Product OS/BIOS semaphores (bits 24/16); missing → `usblegsup=0` continue |
| `0x14` | **DBOFF** | 32-bit | Cap doorbell-array offset (bits 31:2). Product: range-check + read `doorbell[0]` only |
| `(DBOFF&~3)+0` | **doorbell[0]** | 32-bit | Host Controller Command Ring (slot 0). Named/fail-closed; **never write if RS=0** |
| `0x18` | **RTSOFF** | 32-bit | Soft observe only (never runtime program) |
| `CAPLENGTH + 0x00` | **USBCMD** | 32-bit | Soft observe only (never RS/HCRST) |
| `CAPLENGTH + 0x04` | **USBSTS** | 32-bit | Soft observe only (HCH + HSE/EINT/PCD/CNR/HCE names; host seeds HCH) |
| `CAPLENGTH + 0x08` | **PAGESIZE** | 32-bit | Soft observe (+ page-bit residual decode) |
| `CAPLENGTH + 0x14` | **DNCTRL** | 32-bit | Soft observe only |
| `CAPLENGTH + 0x18` | **CRCR** | 64-bit | Soft observe lo; product program writes RCS+ptr (RS-off) |
| `CAPLENGTH + 0x30` | **DCBAAP** | 64-bit | Soft observe lo; product program writes DCBAA ptr |
| `CAPLENGTH + 0x38` | **CONFIG** | 32-bit | Soft observe MaxSlotsEn; product program writes clamp-N |
| `RTSOFF + 0x20` | **IMAN** | 32-bit | Product program sets IE for IR0 after ERDP (never USBCMD.INTE) |
| `RTSOFF + 0x24` | **IMOD** | 32-bit | Optional public IMODI=4000 (1 ms conservative default) |
| `RTSOFF + 0x28` | **ERSTSZ** | 32-bit | Product program writes 1 (interrupter 0) |
| `RTSOFF + 0x30` | **ERSTBA** | 64-bit | Product program writes ERST ptr |
| `RTSOFF + 0x38` | **ERDP** | 64-bit | Product program writes event-ring dequeue |
| `CAPLENGTH + 0x400 + (n-1)*0x10` | **PORTSC[n]** | 32-bit | Soft CCS/PED/PP/speed/PLS snapshot; product after PASS: CCS-only once-read (never write) |

USB Ch.9 / MSC / SCSI names used in **residual catalog only** (never issued on wire here):

| Name | Value (public) | Soft residual |
|------|----------------|---------------|
| GET_DESCRIPTOR | `bRequest=0x06` | EP0 setup residual DEVICE/CONFIG |
| SET_ADDRESS (name) | `bRequest=0x05` | EP0 Address Device residual note |
| SET_CONFIGURATION | `bRequest=0x09` | config setup residual |
| GET_MAX_LUN | `bRequest=0xFE` | config max_lun residual |
| DT_DEVICE / DT_CONFIG / DT_IF / DT_EP | `0x01` / `0x02` / `0x04` / `0x05` | catalog type fields |
| MSC BOT class triple | `08/06/50` | config would_msc_bind lamp |
| CBW / CSW sig | `USBC` / `USBS` | config bot_ready residual only |
| TEST UNIT READY | SCSI opc `0x00` | CDB residual only |
| REQUEST SENSE | SCSI opc `0x03` | CDB residual only |
| INQUIRY | SCSI opc `0x12` | CDB residual only |
| READ CAPACITY(10) | SCSI opc `0x25` | CDB residual only |
| READ(10) | SCSI opc `0x28` | CDB residual name only |
| MODE SENSE(6) / START STOP | SCSI opc `0x1A` / `0x1B` | CDB residual *notes* only (not counted steps) |
| DDI caps residual steps 1..3 | mmio / irq / dma | need=DDI_caps catalog only - **never mint** |
| product residual steps 1..6 | run_stop / rings / irq_db / slot_addr / ep0_cfg / bot_stick | OPEN catalog only + densify lamps |
| soft DMA ring residual | cmd TRBs + evt TRBs (16B public TRB size) | Layout + IOMMU grant residual; **never** CRCR/DCBAAP/ERST |
| product program (RS-off) | real_ddi + open/map/ddi/ring/op | halt if RS=1 or HCH=0 (`USBCMD.RS=0` only); xECP USBLEGSUP handshake (missing continue / timeout SKIP); scratchpad DCBAA[0]; then CONFIG/DCBAAP/CRCR/ERST + IMAN.IE once; optional IMODI=4000; IRQ_BIND; doorbell name/fail-close (read `db0`, never ring); PORTSC CCS once-read (`ccs=`/`ccs_n=`; never write PORTSC); **never set USBCMD.RS**; fail-closed MAP/DMA/spad |

Host seed writes:

- dword0 at `0x00`: `CAPLENGTH | (HCIVERSION << 16)`
- dword at `0x04`: soft MaxSlots/MaxIntrs/MaxPorts
- dword at `0x10`: soft HCCPARAMS1 (AC64=1; xECP=0)
- dword at `0x14` / `0x18`: soft DBOFF / RTSOFF (observe residual)
- dword at `CAPLENGTH+USBSTS`: soft HCH=1 (halted residual)
- dword at PORTSC[1]: CCS|PED|PP|speed=SS

Product program (real_ddi+gate only) walks xECP for USBLEGSUP (missing=`usblegsup=0` continue), then writes RS-off scratchpad `DCBAA[0]` + `CONFIG`/`DCBAAP`/`CRCR`/`ERST*` + `IMAN.IE` (optional IMODI=4000), then names the command-ring doorbell (read-only) and once-reads PORTSC CCS (read-only).
**Never** `USBCMD.RS`, `USBCMD.INTE`, doorbell **write**, PORTSC **write**, EP0 TRB, SET_CONFIGURATION, CBW/CSW, or SCSI wire. `doorbell=OPEN` · `need=usbcmd_rs`.

## Clean-room rules

- Specs + public PCI IDs / manuals only (xHCI Spec + USB 2.0 Ch.9 names).
- **Do not** paste Linux `xhci-*.c` / `xhci_hcd` / `usb-storage` or any GPL tree.
- **Do not** edit `abandoned/kernel/drv/xhci_msc.c` from this dual-license host path.
- Caps, IOMMU, hard IRQ stay inside UDX - never in this driver `.c`.
- Soft host inject != product MMIO/IRQ/DMA grants from devmgr.
- Soft open/MAP / DMA ring / IOMMU / op residual / bot stage / EP0 / config / CDB / DDI caps / product densify catalog **PASS** != product stick access.
- Soft product residual densify holds **never_program=1**. Product program (real_ddi+gate) writes RS-off public init + IMAN.IE only; names doorbell fail-closed; once-reads PORTSC CCS; **never USBCMD.RS**; **never doorbell write while halted**; **never PORTSC write**.
- **Not** product AC via in-kernel `usb_storage.ko` init (**G-AC-1**).
- Freestanding MSC remains optional lab (`GJ_XHCI_MSC_PROBE`; **default 0** / **SKIP**); **not product**.
- Product direction: **product=UDX+ABI** · **need=DDI caps** · prefer real DDI bind · Soft!=product · residual lean once-lamps.
- **Dual DoD A = OPEN** honesty (soft residual deepen != DoD close; agent!=close).


---

**Project:** GreenJade · Soft≠product · Dual DoD A **OPEN** until host USB path · Dual DoD B **OPEN** until interactive SSH login. [root README](../../../README.md). Support: [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog).
