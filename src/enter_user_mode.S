.global enter_user_mode
.type enter_user_mode, @function

#a0 = body
#a1 = arg
enter_user_mode:
    #sepc = user_thread_wrapper
    la t0, user_thread_wrapper
    csrw sepc, t0

    #clear SPP bit, bit 8
    li t0, 0x100
    csrc sstatus, t0

    #set SPIE bit, bit 5
    li t0, 0x20
    csrs sstatus, t0

    sret