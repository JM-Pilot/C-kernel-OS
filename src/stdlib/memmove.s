.globl memmove
memmove:
	push %ebp
	mov %esp, %ebp
	push %esi
	push %edi
	mov 8(%ebp), %edi
	mov 12(%ebp), %esi
	mov 16(%ebp), %ecx
	mov %edi, %eax
	cmp %esi, %edi
	je .Lmm0
	ja .Lmm1
.Lmm2:
	cld
	rep movsb
	jmp .Lmm0
.Lmm1:
	std
	add %ecx, %esi
	dec %esi
	add %ecx, %edi
	dec %edi
	rep movsb
	cld
.Lmm0:
	pop %edi
	pop %esi
	mov %ebp, %esp
	pop %ebp
	ret
