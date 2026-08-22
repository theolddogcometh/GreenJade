# Abandoned in-kernel drivers / Linux .ko-soft

These sources are **not product** and are **not built** into `greenjade.elf`.

- Product Dual DoD A / B = userspace UDX (`xhci_udx` / `rtl8168_udx`) over
  hot+cold ABI. Not in-kernel rtl8168, not xhci_msc, not Linux `.ko` soft.
- G-AC-1: no GPL Linux driver as product AC. This tree is historical only.

## How to look at old sources

Paths mirror the old live layout:

- `abandoned/kernel/drv/` — freestanding rtl8168, xhci_msc, PCI class probes
- `abandoned/kernel/mm/` — linux_ksym / linux_module / linux_*_soft
- `abandoned/kernel/include/gj/` — matching headers + hostish offset tables
- `abandoned/kernel/proc/` — r8169 / usb-storage / rtl_nic firmware embeds;
  `sshd_embed.S` (sshd_gj NATIVE :22 blob — not linked)
- `abandoned/user/sshd/` — `sshd_gj` NATIVE lab probe (not product SSH)
- `abandoned/kernel/` — one-shot expand / deepen scripts

They are not on `C_SRCS` / `S_SRCS`. Do not re-link them.

## hwtest image / collect

`make collect-linux-drivers` and `scripts/make-hwtest-img.sh` may still stage
a host-collected `r8169.ko` onto ESP/GJ-PERSIST as optional media. That is
not an in-kernel embed. If the blob is absent, the packer already SKIPs;
it does not hard-require `r8169_mod_blob.S` (now here, not linked).
