	.file	"rsum.c"
	.text
	.globl	rsum
	.type	rsum, @function
# start in %rdi, count in %rsi
rsum:
	xorq %rax,%rax
	andq %rsi,%rsi
	je .L3
	pushq %rbx
	mrmovq (%rdi),%rbx
	irmovq $8,%r8
	irmovq $1,%r9
	addq %r8,%rdi
	subq %r9,%rsi
	call rsum
	addq %rbx,%rax
	popq %rbx
.L3:
	ret
