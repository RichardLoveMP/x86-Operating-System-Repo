.global keyboard_helper


# function void keyboard_helper
# to help to keyboard to get the simultaneous input value
# void: return nothing
# SIDE EFFECT: NONE
keyboard_helper:

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
	
	# to call the target 
	call keyboard_handler
	
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

