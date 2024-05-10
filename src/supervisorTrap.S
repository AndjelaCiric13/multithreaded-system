//Vezbe 7

.extern _ZN5Riscv17mem_alloc_handlerEv;
.extern _ZN5Riscv16mem_free_handlerEv
.align 4
.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:

   j start
   j  handleTimer
   j somethingCrazy
   j somethingCrazy
   j somethingCrazy
   j somethingCrazy
   j somethingCrazy
   j somethingCrazy
   j somethingCrazy
   j handleConsole
   start:
   //ld t0, ssp
   //sd sp, -32 * 8(t0)
   //addi sp, t0, 0
   addi sp,sp,-256
   sd x0, 0*8(sp)
   sd x1, 1*8(sp)
   sd x2, 2*8(sp)
   sd x3, 3*8(sp)
   sd x4, 4*8(sp)
   sd x5, 5*8(sp)
   sd x6, 6*8(sp)
   sd x7, 7*8(sp)
   sd x8, 8*8(sp)
   sd x9, 9*8(sp)
   sd x10, 10*8(sp)
   sd x11, 11*8(sp)
   sd x12, 12*8(sp)
   sd x13, 13*8(sp)
   sd x14, 14*8(sp)
   sd x15, 15*8(sp)
   sd x16, 16*8(sp)
   sd x17, 17*8(sp)
   sd x18, 18*8(sp)
   sd x19, 19*8(sp)
   sd x20, 20*8(sp)
   sd x21, 21*8(sp)
   sd x22, 22*8(sp)
   sd x23, 23*8(sp)
   sd x24, 24*8(sp)
   sd x25, 25*8(sp)
   sd x26, 26*8(sp)
   sd x27, 27*8(sp)
   sd x28, 28*8(sp)
   sd x29, 29*8(sp)
   sd x30, 30*8(sp)
   sd x31, 31*8(sp)
   //sd sp, 0 * 8(t0)
   call _ZN5Riscv20handleSupervisorTrapEv

   sd a0, 10 * 8(sp)
   .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
   ld x\index, \index * 8(sp)
   .endr
   addi sp,sp,256
   sret  //uzima ono iz sepc

    handleTimer:
    addi sp, sp, -256
        .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        sd x\index, \index * 8(sp)
        .endr
        call _ZN5Riscv16timerTrapHandlerEv
        .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        ld x\index, \index * 8(sp)
        .endr
        addi sp,sp,256
        sret

somethingCrazy:
    call _ZN5Riscv21somethingCrazyHandlerEv

    handleConsole:
    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr
    call _ZN5Riscv18consoleTrapHandlerEv
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp,sp,256
    sret