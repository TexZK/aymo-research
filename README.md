# Accelerated YaMaha Operators - Research Project

This is a personal research project to evaluate the porting of some *Yamaha* sound chip emulations from generic *C* implementation to *SIMD*-based architectures.

Currently the focus is on the *Yamaha OPL* series, especially the *Yamaha OPL3*, which runs 36 *sound operators* to a total of 18 *instrument channels*.
With such a high number of logically concurrent *DSPs*, it came to my mind whether they could be refactored so that SIMD processors can emulate them more efficiently.<br/>
My reference implementation is [Nuked OPL3](https://github.com/nukeykt/Nuked-OPL3), which seems to provide the most accurate sound emulation of the *Yamaha YMF262* up to now.

The current implementation under research is for the *Intel AVX2* standard, which should be widespread enough that almost any *x86* CPU of the latest decade supports them.
I'm using *Visual Studio Community 2022* so that I can run stuff on my native *Windows 10* machines, although I'd move to *\*nix* machines and toolchains after it's worth.

An initial estimation on some *AVX2* CPUs is shows a **speedup of around 2.5x**, where *AVX2* mostly executes 16x 16-bit operations per instruction.
Although this is far from the speedup obtained in other fields, I find this interesting enough to continue the research.<br/>
There are still some bugs to fix, and for sure some tuning to be made (I'm not a SIMD expert at all!), but I don't expect the speedup figure to change much for a possible final implementation.<br/>
Let's also consider that modern *x86* CPUs try their best to parallelize generic application code, with remarkable results: the *Nuked OPL3* program flow is inherently full of conditional branching, but the *out-of-order* execution of the *x86* CPUs make up for that astonishingly well!

If the outcome of this research is positive, the focus could move to the *ARM NEON*, to see if there are any benefits.
I expect results to be very different for an *ARM NEON* implementation, at least for the CPU I've got (the *TI AM335x - ARM Cortex-A8* of a *BeagleBone Black*).
If I recall correctly, that CPU executes only *in-order* code, so it should waste more cycles compared to the generic C implementation of *Nuked OPL3*, but let's see what the outcome is going to be.
Furthermore, the *ARM NEON ISA* looks much better than the *AVX2 ISA* for 16-bit operations, despite working with 128-bit words instead of 256-bit.

I'm loading this research project just for your information. If this project is going to get real, and not just a personal research, I might refactor it into a proper library.<br/>
As for now, this repository holds *Work-In-Progress* stuff which might not even compile; let's see it more like a notepad with debug fixtures than a real project.
If I had to invest time to make it properly, I'd do that after the outcome of this research.
