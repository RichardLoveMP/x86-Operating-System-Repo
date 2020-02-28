.globl system_call_helpler


# system_call_helpler
# do the dispatch of the system call
# INPUT: ecx
# OUTPUT/RETURN VALUE: NONE
# SIDE EFFECT: dispatch it to another system call 
# by using system call call_table

system_call_helpler:
	#push registers
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%edi
	pushl	%ebp
	pushl	%esi
	

	# set data segment
	pushl 	%eax
	movl	$0, %eax
	movw 	KERNEL_DS, %ax	#user segment magic number
	movw 	%ax, %ds
	popl 	%eax

	#sanity check
	cmpl	$10, %eax
	ja		error
	cmpl	$1, %eax
	jb		error

	#push stack 
	pushl 	%edx
	pushl 	%ecx
	pushl 	%ebx

	call 	*( call_table-4)(,%eax,4)	#jump to corresponding system call
	addl	$12, %esp
	cmpl	$0, %eax
	jge		return_	

error:
	movl	$-1, %eax
	
return_:
	movl	$0, %ebx
	movw 	USER_DS, %bx	#user segment magic number
	movw 	%bx, %ds
	
	popl %esi
	popl %ebp
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	
	iret
	

USER_DS:
.word 0x2B

KERNEL_DS:
.word 0x18


 call_table:
.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn




