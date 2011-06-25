.global iap_entry_inner

iap_entry_inner:
	push {lr}
	mov r3,#0x7ffffff1 @IAP entry point according to LPC manual
	mov lr,pc
	bx r3
	pop {pc}
