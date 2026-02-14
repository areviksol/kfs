code a stack,
and integrate it with the GDT.

The GDT ("Global Descriptor Table") is a data structure used to define the different
memory areas: the base address, the size and access privileges like execute and write.
These memory areas are called "segments"


In a GDT, you can find:
•Kernel code, used to store the executable binary code
•Kernel data
•Kernel stack, used to store the call stack during kernel execution
•User code, used to store the executable binary code for user programs
•User program data
•User stack, used to store the call stack during execution in userland

Next, the stack. I’m sure you all know what a stack is, but here’s a friendy reminder:
In computer science, a stack is an abstract data type that serves as a
collection of elements, with two principal operations: push, which adds an
element to the collection, and pop, which removes the most recently added
element that was not yet removed. The order in which elements come off a
stack gives rise to its alternative name, LIFO (for last in, first out)


In this subject, you will have to create, fill and link a Global Descriptor Table into
your Kernel.
Yup, that’s it. Not so much, eh ?
Actually, you will have to understand how "memory" really works in a system, how the
Stack and RAM works, how to use it, how to fill it and how to link it with the BIOS.
Yeah, the BIOS. Thanks to GRUB, it will help you a lot !
Good thing you already installed it.


Furthermore, all of the features of a language cannot be used in a basic kernel. Let’s
take an example with C++ :
This language uses ’new’ to make allocation, class and structures declaration. But in
your kernel, you don’t have a memory interface (yet), so you can’t use those features
now

Flags
In order to boot your kernel without any dependencies, you must compile your code
with the following flags (Adapt the flags for your language, those ones are for C++, for
instance):
•-fno-builtin
•-fno-exception
•-fno-stack-protector
•-fno-rtti
•-nostdlib
•-nodefaultlibs
Pay attention to -nodefaultlibs and -nostdlib. Your Kernel will be compiled on a
host system, yes, but cannot be linked to any existing library on that host, otherwise it
will not be executed.


Linking
You cannot use an existing linker in order to link your kernel. As written above, your
kernel will not boot. So, you must create a linker for your kernel.
Be carefull, you CAN use the ’ld’ binary available on your host, but it is FORBIDDEN
to use the .ld file of your host.
IV.4 Architecture
The i386 (x86) architecture is mandatory (you can thank me later).
IV.5 Documentation
There is a lot of documentation available, good and bad. I personnaly think the OSDev
wiki is one of the best.
IV.6 Base code
In this subject, you have to take your precedent KFS code, and work from it !
Or not... and rewrite all from the beginning. Your call !


et’s sum this up:
•You must create a Global Descriptor Table.
•Your GDT must contain:
◦Kernel Code
◦Kernel Data
◦Kernel stack
◦User code
◦User data
◦User stack
◦Your work should not exceed 10 MB.
•You must declare your GDT to the BIOS.
•The GDT must be set at address 0x00000800.
When this is done, you have to code a tool to print the kernel stack, in a human-friendly
way. (Tip: If you haven’t made a printk yet, now is a good time !)



