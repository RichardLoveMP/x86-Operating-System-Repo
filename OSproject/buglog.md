## ECE 391 MP3 BugLog

*ECE 391 MP3, From 2019.10.19 to 2019.12.08, BP, TQ, AY, KS*

#### Interrupt_Descriptor_Table.c:57

Discription: The vm just boot again and again quickly, also, there is just a small bunch of words printed.(may be because the fast reboot, so no time to show more words)  Deal with the 1 to 20 exception's reserved bits and present.(we just set them all to 0 at first) 

*Found by: Kaiwei Sun*

*Fixed by: Kaiwe Sun*



#### paging.c:59

*(algorithmic error)*

Description: Page directory has the attribute "page_table_base_address". This suggests where the page tables should be, and should not be a specific address, but rather an index, i.e., the lowest 12 bits of the address are useless, and thus should be right shift for 12 bits.

Originally we forgot to do this shift, and the program get crushed, or have some weird behaviours. It takes about 3 hours to fix.

*Found by: Bo Pang*

*Fixed by: Bo Pang*

#### paging.h:20

*(algorithmic error)*

Here we have an implicit error: we wrote the bits of the union "pde" wrongly. the union has 32 bits with 12 bits work as flags and other 20 bits as address index. I wrote a wrong order of these bits... Cost 4 hours.

*Found by: Kaiwei Sun, Bo Pang*

*Fixed by: Bo Pang*



#### rtc.c

Description: Many little bugs here. e.g., output an instruction to the wrong port, cost half hour to fix; not set the initial frequency properly, cost half hour to fix; slave PIC not enabled after initialize PIC cost half hour to fix; output an instruction to the wrong port, cost half hour to fix; test function can not actually test rtc functionality, cost 10 minutes to fix.

*Found by: Anbang Ye*

Fixed by: Anbang Ye



#### keyboard.c

*Logic Bug*

Description: I messed up the send code and the corresponding characters when I was creating the lookup table while I was writing the keyboard. Therefore, the keyboard outputon the screen is different from the actual key I had pressed. Solution, when I referenced the document on devOS, I found that there are three types of table of the input-output table. However, I did not pay attention to the type of the lookup table.

*Found by: Tianzuo Qin, Bo Pang, Kaiwei Sun*

*Fixed by: Tianzuo Qin, Anbang Ye*



#### keyboard.c

*Algorithm Bug*

Description: After the keyboard could work, we found that if we only pressed one key at one time, it worked very well. Nevertheless, if we pressed two or more keys at one time, the system crashed. Solution is, according to the exception, we knew that it might be the protrection of the kernel that multiple interrupts or abrupt massive inputs may cause the handler cannot have time to handle the calling convention in C. Therefore, we added Assembly code to help the origin C code.

*Found by: Tianzuo Qin*

*Fixed by: Tianzuo Qin*



#### terminal.c/h

Bug time: 2019.10.27

Bug Type: Algorithm Bug

Description: When I input the character to the bottom and perform a scroll up, there is a blue screen which causes the system crash and was denoted as Page-Fault.

Solution: When I see the source code of my own terminal, I found that when I add the cursor by 1 (Temporarily the cursor is out of the bound of the video memory). That is to say, If I set this cursor to the screen, it will obviously cause a Page-Fault. However, I should update the cursor position but I do not update it. Therefore, I checked the source code and added the update process to the cursor. Then when I test it again, it works.

#### terminal.c/h

Bug time: 2019.10.28

Bug Type: Logic Bug

Bug Location: terminal.c/h

Description: I am not sure how to handle '\n' in my code because we are only told to handle 128 characters. However, I did not know whether we should consider '\n' in the total 128 characters or handle it separately.  

Solution: When I asked it with TA and referred to piazza, I knew that the '\n' should be counted into 128 characters and we only to handle 127 "normal" characters in the buffer. Therefore, revised the version and wrote my own version of the put. Then, from my perspective, I think this bug is fixed.

#### ROfilesystem.c/h

Bug time: 2019.10.27

Bug Location:  ROfilesystem.c in read_data

Description: Page fault, for the wrong value of  num_datablock=(length-lengthhalfway)/datablocks_size; because I define datablocks_size as 4* 1024, it will divide 4 first and *1024

Solution: define datablocks_size directly to 4096 rather than 4*1024

---



#### system_call.c

*(algorithmic error, probrably)*

Description: In the function execute(), we keeps having PAGE_FAULT, we suspect that this is because the register operations were having some problems... But we will never know, cause we rewrited this function :(



#### syscall_helper.S

Description: This is a painful typo. The logic was perfectly fine, only one line was missing: we fogot to "popl edi" after stored it. This leaded to some *mysterious* mistakes... It takes about 3 hours...



#### terminal.c

Descripiton: Some input characters doesn't show up on screen when press keyboard continuously. We found that this is because that we didn't call sti() in some branches (i.e., the "branch coverage" of sti() is not 100%. Please refer to line 299 for ref).



####cat/grep

For terminal write, initially we used a rather easy approach: stop when we encountered any "\0". However this is not very correct, because this approch is too simple, and sometimes naive: when you try to call the user-program "cat" or "grep", you might find that not all contents is printed on the screen. (An example might be: '\4C' '\4A' '\53' '\0' '\42', which *should* print "LJSB", but with such implementation, only "LJS" will be printed to the screen)

We changed the stopping codition, now a '\0' will no longer lead to a stop of terminal writing.



#### Keyboard input to other terminal

When we input something very fast and try to switch terminals simutaneously, some characters filped to another terminal while we are typing. It is a bug.

The internal real problem is: When we are trying to process the writing character which should be showed on this terminal, the pit may change the "active terminal" because it is a part of scheduling. Before we fix it, it could print to the "active terminal" but not "visible terminal".

Solution: We try to fix it by using cli and sti to limit the behavior when the system is trying to print the character on the terminal. 

#### PIT Handler

PIT does not work, but there is no page fault or crashing incident. It simply does not work.

Problem: We get wrong order of the parameter of outb. When we inverted it, it works well.

