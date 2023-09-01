
build/initcode.o:     file format elf64-littleaarch64


Disassembly of section .text:

0000000000000000 <start>:
   0:	58000201 	ldr	x1, 40 <argv+0x10>
   4:	58000222 	ldr	x2, 48 <argv+0x18>
   8:	d28000e0 	mov	x0, #0x7                   	// #7
   c:	d4000001 	svc	#0x0

0000000000000010 <exit>:
  10:	d2800040 	mov	x0, #0x2                   	// #2
  14:	d4000001 	svc	#0x0
  18:	17fffffe 	b	10 <exit>

000000000000001c <init>:
  1c:	696e692f 	.word	0x696e692f
  20:	0074      	.short	0x0074
  22:	00          	.byte	0x00
  23:	00          	.byte	0x00
  24:	d503201f 	nop
  28:	d503201f 	nop
  2c:	d503201f 	nop

0000000000000030 <argv>:
	...
