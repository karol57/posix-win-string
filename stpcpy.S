.file	"stpcpy.S"
.intel_syntax noprefix
.text
.p2align 4,,15
.globl	stpcpy_asm
.def	stpcpy_asm;	.scl	2;	.type	32;	.endef
.seh_proc	stpcpy_asm
stpcpy_asm:
.seh_endprologue
  sub  rcx, rdx
  test dl, 7
jz     .aligned_stpcpy
jmp    .unalgn_loop

.p2align 4,,15
.unalgn_finish:
  add  rcx, rdx
  mov  rax, rcx
ret

.p2align 4,,15
.unalgn_loop_inc:
  inc  rdx
.unalgn_loop:
  mov  al, [rdx]
  mov  [rcx+rdx], al
  test al, al
jz     .unalgn_finish
  test dl, 7
jnz    .unalgn_loop_inc

.p2align 4,,15
.aligned_stpcpy:
mov r10, 0x0101010101010101
mov r11, 0x8080808080808080
jmp    .aligned_loop

.p2align 4,,15
.aligned_loop_inc:
  mov  [rcx+rdx], rax
  add  rdx, 8
.aligned_loop:
  mov  rax, [rdx]
  mov  r8, rax
  mov  r9, rax
  sub  r8, r10
  not  r9
  and  r8, r9
  test r8, r11
jz     .aligned_loop_inc
  mov  [rcx+rdx], al
  test al, al
jz     .aligned_finish
  inc  rdx
  mov  [rcx+rdx], ah
  test ah, ah
jz     .aligned_finish
  inc  rdx
  shr  rax, 16
  mov  [rcx+rdx], al
  test al, al
jz     .aligned_finish
  inc  rdx
  mov  [rcx+rdx], ah
  test ah, ah
jz     .aligned_finish
  inc  rdx
  shr  rax, 16
  mov  [rcx+rdx], al
  test al, al
jz     .aligned_finish
  inc  rdx
  mov  [rcx+rdx], ah
  test ah, ah
jz     .aligned_finish
  inc  rdx
  shr  eax, 16
  mov  [rcx+rdx], al
  test al, al
jz     .aligned_finish
  inc  rdx
  mov  [rcx+rdx], ah
.p2align 4,,15
.aligned_finish:
  add  rcx, rdx
  mov  rax, rcx
ret
.seh_endproc