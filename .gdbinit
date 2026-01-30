# ~/LearnixMiniOs/.gdbinit
set architecture i386
target remote localhost:1234
symbol-file build/kernel.elf
define k
    info registers
    x/5i $eip
end