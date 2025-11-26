# General instructions
IV.1 Code and Execution
IV.1.1 Emulation
The following part is not mandatory, you’re free to use any virtual manager you want to,
however, i suggest you to use KVM. It’s a Kernel Virtual Manager, and have advanced
execution and debugs functions. All the example below will use KVM.
IV.1.2 Language
The C language is not mandatory, you can use any language you want for this suit of
projects.
Keep in mind that all language are not kernel friendly, you could code a kernel with
Javascript, but are you sure it’s a good idea ?
Also, a lot of the documentation are in C, you will have to ’translate’ the code all along
if you choose a different language.
Furthermore, all of the features of a language cannot be used in a basic kernel. Let’s
take an example with C++ :
This language uses ’new’ to make allocation, class and structures declaration. But in
your kernel, you don’t have a memory interface (yet), so you can’t use those features
now.
A lot of language can be used instead of C, like C++, Rust, Go, etc. You can even code
your entire kernel in ASM !
IV.2 Compilation
IV.2.1 Compilers
You can choose any compilers you want. I personnaly use gcc and nasm. A Makefile
must be added to your repo.
IV.2.2 Flags
In order to boot your kernel without any dependencies, you must compile your code
with the following flags (Adapt the flags for your language, those ones are for C++, for
instance):
• -fno-builtin
• -fno-exception
• -fno-stack-protector
• -fno-rtti
• -nostdlib
• -nodefaultlibs
Pay attention to -nodefaultlibs and -nostdlib. Your Kernel will be compiled on a
host system, yes, but cannot be linked to any existing library on that host, otherwise it
will not be executed.     
IV.3 Linking
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
