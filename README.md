# Y86-Simulator

### Installation

##### Cloning the Repository
`git clone https://github.com/brycegerdeman/Y86-Simulater.git`

### Building the Simulator
##### Using Makefile
```
cd Y86-Simulator
make run
```
### Running The Simulator
After successfully building the Y86 Simulator ('yess'), you can run it using '.yo'
files, which contain the Y86 assembly code for the simulator.
### Steps to Run 'yess' with a '.yo' File
#### 1. Ensure that 'yess' is Executable:
  - Use 'ls l' to check if 'yess' has execute permissions. if not, run 'chmod +x yess' to make it executable.
#### 2. Prepare your '.yo' File:
  - Create a '.yo' file containing Y86 assembly code. Here's an example format:
  ```
    0x000:                      | .pos 0
                              | 
  0x000: 30f00100000000000000 | irmovq 1, %rax
                              |
  0x00a: 10                   | nop
  0x00b: 10                   | nop
  0x00c: 10                   | nop
  0x00d: 6001                 | addq %rax, %rcx
  0x00f: 10                   | nop
  0x010: 10                   | nop
  0x011: 10                   | nop
  0x012: 6011                 | addq %rcx, %rcx
                              |
  0x014: 10                   | nop
  0x015: 10                   | nop
  0x016: 10                   | nop
  0x017: 6012                 | addq %rcx, %rdx
  0x019: 10                   | nop
  0x01a: 10                   | nop
  0x01b: 10                   | nop
  0x01c: 6022                 | addq %rdx, %rdx
                              |
  0x01e: 10                   | nop
  ...
  ```
  - The '.yo' file contains the assembly instructions for the simulator
#### 3. Run 'yess with the '.yo' File:
  -Execute the simulator with the '.yo' file as an argument:
  `./yess <yourfile.yo>`
  - Replace <yourfile.yo> with the path to you '.yo' file
#### 4. Interpreting Output:
  - After running, 'yess' will call Simulate.run() to run the simulation. The
    output produced is a detailed dump of the processors state at each cycle.
    This information includes:
    - Register Values
    - Processor Flags
    - Memory State: A hexadecimal dump of memory contents
    - Cycle Info: icode, ifun, stat, predPC
 
