    .section .text
    .align 2
    .global halt
    .type  halt, @function

halt:
1:  wfi            # sleep until interrupt (low power)
    j 1b           # forever