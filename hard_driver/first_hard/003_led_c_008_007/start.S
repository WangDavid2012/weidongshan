
.text
.global _start

_start:

	/* 设置内存: sp 栈 */
	// 当nand启动时，0地址对应的是4K的片内内存，将栈空间设置成这4K内存的首地址
	// S3C2440有4K的片内RAM
	ldr sp, =4096  /* nand启动 */
	//如果以nor启动，则片内内存的地址是0x40000000+4096
//	ldr sp, =0x40000000+4096  /* nor启动 */

	/* 调用main */
	// 跳转到main，并且将返回地址保存 bl表示
	bl main

halt:
	b halt
	
