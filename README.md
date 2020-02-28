# x86 Operating System Construction
Operating System Construction with Intel 8086 Architecture

***Author: Tianzuo Qin***

***Time: Dec 10th 2019***

#### Project Overview:

Design a core on af operating system from setting the interrupt descriptor table (IDT) to scheduling. 

The structure of this project is listed:

1. Processor Initialization:
   1. Load the GDT
   2. Initialize the IDT
   3. Initialize the Devices
   4. Initialize Paging
2. Device Drivers:
   1. Create a Terminal Driver
   2. Parse the Read-only File System
   3. The Real-Time Clock Driver
3. System Calls Part I
   1. Support System Calls (halt, execute, read, write)
   2. Tasks
   3. Support A Loader
   4. Executing User-level Code
   5. Process Control Block
4. System Calls Part II
   1. Advanced System Calls (vidmap)
5. Scheduling
   1. Multiple Terminals and Active Tasks
   2. Scheduling
6. Sound Driver



#### Kernel Statics:

Single Core

4MB Memory
