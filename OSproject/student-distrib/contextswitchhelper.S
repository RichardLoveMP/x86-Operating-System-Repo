.globl contextswitchhelper


# contextswitchhelper
# FUNCTION: help to use the contextswitchhelper
# save/restore some necessary register
# RETURN VALUE: NONE
# SIDE EFFECT: NONE

contextswitchhelper:

	#saving eip for previous process
	movl	(%esp), %ebx
	movl	16(%esp), %eax
	
	movl	%ebx, (%eax)


	# get the context
	movl 	4(%esp), %ecx
	movl	12(%esp), %ebp
	
	movl	8(%esp), %esp

	pushl 	%ecx
	
    ret


   


   

