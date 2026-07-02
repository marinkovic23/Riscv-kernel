.global supervisor_trap
.type supervisor_trap @function
supervisor_trap:
    #saving all the registers before calling the handletrap
    addi sp, sp, -256
    .irp index, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    # pass pointer to saved registers
    mv a0, sp

    call trap_handler


    # this is where registers are restored

    .irp index, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256
    sret

#console_trap:
#   addi sp, sp, -256
#   .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
#    sd x\index, \index * 8(sp)
#    .endr
#
#    call _ZN5RiscV17handleConsoleTrapEv
#
#    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
#    ld x\index, \index * 8(sp)
#    .endr
#    addi sp, sp, 256
#    sret

