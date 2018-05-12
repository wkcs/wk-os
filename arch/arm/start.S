.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb

.global	g_pfnVectors

/* start address for the initialization values of the .data section.
defined in linker script */
.word	_sidata
/* start address for the .data section. defined in linker script */
.word	_sdata
/* end address for the .data section. defined in linker script */
.word	_edata
/* start address for the .bss section. defined in linker script */
.word	_sbss
/* end address for the .bss section. defined in linker script */
.word	_ebss

.word __irq_handler

.equ  BootRAM,        0xF1E0F85F
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

    .section	.text.reset_randler
	.weak	reset_randler
	.type	reset_randler, %function
reset_randler:

/* Copy the data segment initializers from flash to SRAM */
  movs	r1, #0
  b	LoopCopyDataInit

CopyDataInit:
	ldr	r3, =_sidata
	ldr	r3, [r3, r1]
	str	r3, [r0, r1]
	adds	r1, r1, #4

LoopCopyDataInit:
	ldr	r0, =_sdata
	ldr	r3, =_edata
	adds	r2, r0, r1
	cmp	r2, r3
	bcc	CopyDataInit
	ldr	r2, =_sbss
	b	LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
	movs	r3, #0
	str	r3, [r2], #4

LoopFillZerobss:
	ldr	r3, = _ebss
	cmp	r2, r3
	bcc	FillZerobss

/* Call the clock system intitialization function.*/
    bl  SystemInit
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
	bl	wk_os_start
	bx	lr
.size	reset_randler, .-reset_randler

 	.section	.isr_vector,"a",%progbits
	.type	g_pfnVectors, %object
	.size	g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
	.word	_estack
	.word	reset_randler
	.word	__irq_handler
	.word	__irq_handler + 4
	.word	__irq_handler + 8
	.word	__irq_handler + 12
	.word	__irq_handler + 16
	.word	__irq_handler + 20
	.word	__irq_handler + 24
	.word	__irq_handler + 28
	.word	__irq_handler + 32
	.word	__irq_handler + 36
	.word	__irq_handler + 40
	.word	__irq_handler + 44
	.word	__irq_handler + 48
	.word	__irq_handler + 52
	.word	__irq_handler + 56
	.word	__irq_handler + 60
	.word	__irq_handler + 64
	.word	__irq_handler + 68
	.word	__irq_handler + 72
	.word	__irq_handler + 76
	.word	__irq_handler + 80
	.word	__irq_handler + 84
	.word	__irq_handler + 88
	.word	__irq_handler + 92
	.word	__irq_handler + 96
	.word	__irq_handler + 100
	.word	__irq_handler + 104
	.word	__irq_handler + 108
	.word	__irq_handler + 112
	.word	__irq_handler + 116
	.word	__irq_handler + 120
	.word	__irq_handler + 124
	.word	__irq_handler + 128
	.word	__irq_handler + 132
	.word	__irq_handler + 136
	.word	__irq_handler + 140
	.word	__irq_handler + 144
	.word	__irq_handler + 148
	.word	__irq_handler + 152
	.word	__irq_handler + 156
	.word	__irq_handler + 160
	.word	__irq_handler + 164
	.word	__irq_handler + 168
	.word	__irq_handler + 172
	.word	__irq_handler + 176
	.word	__irq_handler + 180
	.word	__irq_handler + 184
	.word	__irq_handler + 188
	.word	__irq_handler + 192
	.word	__irq_handler + 196
	.word	__irq_handler + 200
	.word	__irq_handler + 204
	.word	__irq_handler + 208
	.word	__irq_handler + 212
	.word	__irq_handler + 216
	.word	__irq_handler + 220
	.word	__irq_handler + 224
	.word	__irq_handler + 228
	.word	__irq_handler + 232
	.word	__irq_handler + 236
	.word	__irq_handler + 240
	.word	__irq_handler + 244
	.word	__irq_handler + 248
	.word	__irq_handler + 252
	.word	__irq_handler + 256
	.word	__irq_handler + 260
	.word	__irq_handler + 264
	.word	__irq_handler + 268
	.word	__irq_handler + 272
	.word	__irq_handler + 276
	.word	__irq_handler + 280
	.word	__irq_handler + 284
	.word	__irq_handler + 288
	.word	__irq_handler + 292
	.word	__irq_handler + 296
	.word	__irq_handler + 300
	.word	__irq_handler + 304
	.word	__irq_handler + 308
	.word	__irq_handler + 312
	.word	__irq_handler + 316
	.word	__irq_handler + 320
	.word	__irq_handler + 324
	.word	__irq_handler + 328
	.word	__irq_handler + 332
	.word	__irq_handler + 336
	.word	__irq_handler + 340
	.word	__irq_handler + 344
	.word	__irq_handler + 348
	.word	__irq_handler + 352
	.word	__irq_handler + 356
	.word	__irq_handler + 360
	.word	__irq_handler + 364
	.word	__irq_handler + 368
	.word	__irq_handler + 372
	.word	__irq_handler + 376
	.word	__irq_handler + 380
	.word	__irq_handler + 384
	.word	__irq_handler + 388
	.word	__irq_handler + 392
	.word	__irq_handler + 396
	.word	__irq_handler + 400
	.word	__irq_handler + 404
	.word	__irq_handler + 408
	.word	__irq_handler + 412
	.word	__irq_handler + 416
	.word	__irq_handler + 420
	.word	__irq_handler + 424
	.word	__irq_handler + 428
	.word	__irq_handler + 432
	.word	__irq_handler + 436
	.word	__irq_handler + 440
	.word	__irq_handler + 444
	.word	__irq_handler + 448
	.word	__irq_handler + 452
	.word	__irq_handler + 456
	.word	__irq_handler + 460
	.word	__irq_handler + 464
	.word	__irq_handler + 468
	.word	BootRAM       /* @0x1E0. This is for boot in RAM mode for
                         STM32F10x High Density devices. */