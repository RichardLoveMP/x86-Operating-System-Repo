.globl ret_handler

# function handle the return (iret)
# to help to handle return case
# void: return nothing
# SIDE EFFECT: calling iret
ret_handler:

	# push the ecx
	pushl 	%ecx

	# get the argument
	movl	4(%esp), %ecx
	movl	%ecx, (%eax)

	popl 	%ecx
	# pop the var
	addl	$4,%esp
	addl 	$4,%esp

	sti

	iret


