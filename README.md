# De Rebus Antiquis v6

This repo contains the utilities I used to create [De Rebus Antiquis](https://xerub.github.io/ios/iboot/2018/05/10/de-rebus-antiquis.html) ramdisks for iOS 6.

There are three utilities:
- iBootDebugger is a utility for adding breakpoints to iBoot for debugging.
- payloadGenerator is the utility which generates De Rebus Antiquis ramdisks.
- atropine is the hacked up version of the [atropine iBoot payload](https://github.com/synackuk/n1ghtshade/tree/master/atropine) used to generate nettoyeur.


## Writeup

My aim here is not to reiterate the explanation of the vulnerability and exploit explained in Xerub's [writeup](https://xerub.github.io/ios/iboot/2018/05/10/de-rebus-antiquis.html). Instead, I want to explain how to use the utilities in this repository to create new exploit ramdisks. I also want to thank [@xerub](https://x.com/xerub) for releasing the exploit, as well as [@dora2ios](https://x.com/dora2ios) and [@Pingzi610](https://x.com/Pingzi610) for their advice during my work.

### Prerequisites

- A UART cable. These can be found on [aliexpress](https://www.aliexpress.com/item/1005001868761413.html) for 30-pin devices.
- The ARM toolchain. On Mac this can be installed with `brew install --cask gcc-arm-embedded`.


### Building the utilities

To build either utility you need to symlink one of the targets to `target` and run make:

```bash
cd payloadGenerator
ln -s targets/iPod4,1/10B500 target
make
```

### iloader

Xerub created a program called iloader, capable of "running" iBoot in userland to help with building an exploit. Among other things iloader allowed the user to set breakpoints and check register values during iBoot execution, this is essential for exploitation. It has been known for years that iloader doesn't work on iOS 6, and although there has been some subsequent work by [@Pingzi610](https://x.com/Pingzi610) to fix this [here](https://github.com/Pingzi610/iloader6), this was not available when I began this work, and in any case it was not the approach I chose. Unlike Xerub, I had access to bootchain exploits (SHAtter and limera1n for the iPod touch 4G, checkm8 for the iPhone 4S) and firmware keys for the devices I was targeting. iBootDebugger is a utility which is capable of adding breakpoints to an iBoot image. Essentially, it overwrites the root certificate stored in iBoot with your breakpoints (small bits of shellcode that stash some registers and then `printf` some interesting values) and inserts branch instructions at locations you choose. The main function shows how breakpoints are added:

```C
int main(int argc, char *argv[]) {
    // ...
    add_breakpoint(READP_BP_OFF, readp_bkpt_handler, sizeof(readp_bkpt_handler), READP_SAFE_REG);
    add_breakpoint(MEMALIGN_START_BP_OFF, memalign_start_bkpt_handler, sizeof(memalign_start_bkpt_handler), MEMALIGN_START_SAFE_REG);
    add_breakpoint(MEMALIGN_LOOP_BP_OFF, memalign_loop_bkpt_handler, sizeof(memalign_loop_bkpt_handler), MEMALIGN_LOOP_SAFE_REG);
    add_breakpoint(MEMALIGN_END_BP_OFF, memalign_end_bkpt_handler, sizeof(memalign_end_bkpt_handler), MEMALIGN_END_SAFE_REG);
    // ...
}
```

Each breakpoint requires one "safe" register that may be clobbered. Targets have been provided for the iPhone 4S and iPod touch 4G which include the relevant breakpoint handlers for those devices. There are also some generic breakpoint handlers to aid in the exploit development process.

iBootDebugger also renames a couple of environment variables (`boot-ramdisk` and `boot-partition`), meaning you can test your exploit while allowing the device to still boot normally.

Once you have your patched iBoot, you can put your device into Pwned DFU mode and upload a patched iBSS (or simply enter kDFU mode) and boot your patched iBoot. You should see your breakpoints being hit:

```plaintext
_memalign: sp = 0x5ff4c8fc r8 = 0x5ff45bd0, sb = 0x0, r4 => 0x5ff45bf8 (0x6d04)
_memalign: sp = 0x5ff90b80, r0 = 0x5ff90b40, r2 = 0x2e, r3 = 0x5ff916c0, r4 = 0x5ff90b80, sb = 0x6, sl => (0x5ff409d0), r6 = 0x5ff17921 (0x50cf0)
_memalign: sp = 0x5ff4c8fc, r8 = 0x5ff17a55, lr = 0x5ff1561f
gBootArgs.commandLine = [ ]
```

Now, you can start working on creating your Ramdisk.


### Ramdisk

The objective of your ramdisk is to trigger the vulnerability using one of the strategies in Xerub's writeup under [2.1 - Burning the bridges](https://xerub.github.io/ios/iboot/2018/05/10/de-rebus-antiquis.html). The payloadGenerator already has support for both: set `#define FLAT_TREE 1` for the simpler first strategy and `#define COPY_ROOT 1` for the second one. If you're using the latter strategy, you also need to move `disk.dmg` to a suitable path. The safe way to do this is using [the hfsplus utility here](https://github.com/planetbeing/libdmg-hfsplus). For example, to move the disk to `/a/disk.dmg`:

```bash
hfsplus ramdisk.dmg mv /a/b/c/d/e/f/g/h/i/j/k/l/m/disk.dmg /a/disk.dmg
hfsplus ramdisk.dmg rmall /a/b
```

Note that you must not mount the ramdisk or you'll screw up the ramdisk template.

When you first run this ramdisk you'll either get stuck or crash at some SP offset. If the device simply gets stuck, it may be some kind of infinite loop caused by overwriting a specific stack variable (as in the iPod touch 4G case), or you may have overwritten a variable crucial to printf. For iOS 6, there appear to be two heap variables that must remain valid. You can identify these values by using the iBootDebugger to track down the offending variable.

Once you've found them you can setup your fix_printf function and call it from all the breakpoints:

```C
__attribute__((always_inline)) static inline void fix_printf() {
	/* These offsets must be fixed or printf calls will fail. */
    *(uint32_t*)0x9ff46ac4 = 0x9ff60a00;
    *(uint32_t*)0x9ff461e4 = 0x3;
}
```

Note you'll also need to make sure the breakpoints aren't called too early (by disabling your breakpoints when your SP is too high), otherwise setting these variables will cause your iBoot to crash.

Eventually, you should reach a point where you're successfully controlling the r0 register inside memalign. For example, from xerub's writeup:

```plaintext
_memalign: sp = 0x747ca8, r8 = 0x747c60, r3 = 0x9, r2 => 0x747cac (0xfffffffc)
_memalign: sp = 0x747ca8, r0 = 0x747b62, r1 = 0x5 (0x747ca2/0xbff47ca2), r2 = 0x747ba8, r3 = 0xc, r4 => (0x747b68/0xbff47b68), r9 = 0x4040 (0x13a)
_memalign: sp = 0x747ca8, r0 = 0x747b68, r1 = 0x101 (0x74bba8/0xbff4bba8), r2 = 0x747ba8, r3 = 0xc, r4 => (0x747b68/0xbff47b68), r9 = 0x4040 (0x4040)
_memalign: sp = 0x747ca8, r8 = 0x747c60
```

At this point, you need to start tweaking the `NODE_SIZE` until the value of r0 becomes one you control. On the iPod touch 4G, r0 will point to `0x5FF457B0`. At this point, exploitation works as described in [Xerub's guide](https://xerub.github.io/ios/iboot/2018/05/10/de-rebus-antiquis.html). You overwrite the LR on the stack with a pointer into your payload and jump to it:

```C
*(uint32_t*)(header + HEADER_R4_OFF) = HEADER_BUFFER + HEADER_SAFE_OFF; /* Point R0 into a memory region we can control. */
*(uint32_t*)(header + HEADER_SAFE_OFF + 0x4) = SUITABLE_BIN_SIZE >> 6; /* Set the size of the 'bin' to some suitable value that allows the loop to exit (getting to the STR) */
*(uint32_t*)(header + HEADER_SAFE_OFF + 0x40) = (EXTENTS_BUFFER + SAFE_EXTENTS_OFFSET) | 1; /* This is where we want to set the LR, a pointer into our payload. */
*(uint32_t*)(header + HEADER_SAFE_OFF + 0x44) = FINAL_SP + SP_LR_OFFSET; /* We write the offset at 0x40 to the address at 0x44, so point 0x44 to the LR. */
```

All of the code here is available in the `target.h` file, alongside extensive commenting.

#### The iPhone 4S

The iPhone 4S is somewhat more interesting. In that case, the value you control for R0 is the `EXTENTS_ROOT_NODE` value. By increasing the total nodes to `0xFFFFF` and using the address of the payload starting at 0x0 rather than the base address (0x9ff00000), you can still trigger your payload:

```C
/* For some devices (such as the iPhone 4S), this value becomes R0. It must be < TOTAL_NODES and LSB must == 0 (hence why we don't use the base address). */
#define EXTENTS_ROOT_NODE (HEADER_BUFFER + HEADER_SAFE_OFF) - 0x9ff00000

/* The total extent nodes. Must be > EXTENTS_ROOT_NODE. */
#define TOTAL_NODES (0xFFFFF)
```

### Nettoyeur

The final challenge is getting nettoyeur to work. Nettoyeur effectively restores iBoot's DATA section to match the DATA section on boot. This is essential for us to reboot iBoot cleanly. Xerub achieved this by dumping the copy of iBoot left in memory after the device booted with [kdumper](https://github.com/Serhatisb/kdumper/blob/master/kdumper.c). This approach doesn't appear to work properly with iOS 6, so instead I hacked up [atropine](https://github.com/synackuk/n1ghtshade/tree/master/atropine) by adding the following command:


```C
int diff_cmd(int argc, cmd_arg_t* argv) {
	for(uint32_t i = 0; i <= 0x00043000; i += 4) {
		uint32_t tru = *(uint32_t*)(((uint32_t)loadaddr) + i);
		uint32_t pru = *(uint32_t*)(((uint32_t)base_address) + i);
		if(tru != pru) {
			printf("*(uint32_t*)(BASE_ADDRESS + %p) = %p; /* %p -> %p */\n", i, tru, pru, tru);
		}
	}
	return 0;
}

ADD_COMMAND("diff", diff_cmd, "Diff iBoot with an uploaded iBoot. Used for building nettoyeur De Rebus Antiquis.");
```

The hacked version of atropine is included in the atropine folder. To use it, upload a decrypted, unpacked version of the running iBoot, and run `atropine diff`.

### Conclusion

Once you have your own payload running you can pretty much do what you like. The current payload patches iBoot's signature checks and forces it to run the 'diags' boot command. It then hacks up the diags routines to bootstrap a new iBoot from the NOR. Finally, it runs nettoyeur and restarts iBoot.