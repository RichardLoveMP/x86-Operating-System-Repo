.global rtc_helper

# function void rtc_helper
# to help to rtc to get the correct value
# void: return nothing
# SIDE EFFECT: NONE

rtc_helper:
	# to push the  general registers
    pushal

    # to push the flag registers
	pushfl
	
    # to call the rtc helper
	call rtc_irq_handler
	
	# pop the flag register
    popfl

    # to pop the normal registers
    popal
	
	iret
	






