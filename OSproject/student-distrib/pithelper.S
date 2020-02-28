.globl pithelper

# function void pit_helper
# to help to pit to get the correct value
# void: return nothing
# SIDE EFFECT: NONE

pithelper:

	
	# to push the  general registers
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esp
    pushl %ebp
    pushl %esi
    pushl %edi

    # to push the flag registers
	pushfl

	movw 	KERNEL_DS, %cx
	movw 	%cx, %ds
	call 	pit_handler
	movw 	USER_DS, %cx
	movw 	%cx, %ds
	

	# pop the flag register
    popfl

    # to pop the normal registers
    popl %edi
    popl %esi
    popl %ebp
    popl %esp
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax 

	iret


USER_DS:
.word 0x2B

KERNEL_DS:
.word 0x18




