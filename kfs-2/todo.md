 * code a stack, and integrate it with the GDT.
 - The GDT ("Global Descriptor Table") is a data structure used to define the different memory areas: the base address, the size and access privileges like execute and write.
 * in a GDT, you can find:
    • Kernel code, used to store the executable binary code
    • Kernel data
    • Kernel stack, used to store the call stack during kernel execution
    • User code, used to store the executable binary code for user programs
    • User program data
    • User stack, used to store the call stack during execution in userland
 * In computer science, a stack is an abstract data type that serves as a collection of elements, with two principal operations: push, which adds an element to the collection, and pop, which removes the most recently added element that was not yet removed. The order in which elements come off a stack gives rise to its alternative name, LIFO (for last in, first out).
 * In this subject, you will have    to create, fill and link a Global Descriptor Table into your Kernel.
    Actually, you will have to understand how "memory" really works in a system, how the
    Stack and RAM works, how to use it, how to fill it and how to link it with the BIOS.
    Yeah, the BIOS. Thanks to GRUB, it will help you a lot !
    Good thing you already installed it.
Let’s sum this up:
• You must create a Global Descriptor Table.
• Your GDT must contain:
◦ Kernel Code
◦ Kernel Data
◦ Kernel stack
◦ User code
◦ User data
◦ User stack
◦ Your work should not exceed 10 MB.
• You must declare your GDT to the BIOS.
• The GDT must be set at address 0x00000800.
When this is done, you have to code a tool to print the kernel stack, in a human-friendly
way. (Tip: If you haven’t made a printk yet, now is a good time !)
