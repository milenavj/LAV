================================
Software verification tool LAV
================================

This directory and its subdirectories contain source code for the software 
verification tool LAV. 

LAV is open source software. You may freely distribute it under the terms of
the license agreement found in LICENSE.txt.


--------------------------------------------------------------------
USAGE: LAV [options] <input bytecode>
--------------------------------------------------------------------

Use LAV --help to see details about all possible options. 

<input bytcode> is LLVM object code transformed from the original 
code by clang tool (LAV uses clang version 3.3) . 


--------------------------------------------------------------------
COMPILATION
--------------------------------------------------------------------

LAV works over the LLVM bytecode: 
 LLVM       - The LLVM Compiler Infrastructure (http://llvm.org)

LAV can use one of the following SMT solvers:
  yices     - Yices SMT solver (http://yices.csl.sri.com)
  boolector - Boolector SMT solver (http://fmv.jku.at/boolector/)
  mathsat   - MathSAT SMT solver (http://mathsat.itc.it/)
  z3        - Z3 SMT solver (https://github.com/Z3Prover/z3/releases)


LLVM and SMT solvers are not part of this distribution (due to
their specific licensing). 

In order to compile LAV you should download and install LLVM, 
see http://llvm.org/docs/GettingStarted.html
LAV uses version 3.3. 

Download one or more of the above SMT solvers from their web pages, 
place header files in
lav/solvers/<solver>/include

and library files into
lav/solvers/<solver>/lib

where <solver> is yices, boolector, mathsat or z3.
LAV is tested with the following versions of the solvers: 
boolector-1.1-IA-32, boolector-2.2.0, mathsat-4.2.8-linux-x86, yices-1.0.27
and z3 4.4, z3 4.0

Configure project  
$./configure --with-llvm=<path/to/llvm> --enable-<solver1> [--enable-<solver2>] [--enable-<solver3>] 
              [--enable-<solver4>]
This assumes that you compiled LLVM in-place. If you used a different directory for the object files then use:
$ ./configure --with-llvmsrc=path/to/llvm/src --with-llvmobj=path/to/llvm/obj 
              --enable-<solver1> [--enable-<solver2>] [--enable-<solver3>] 
              [--enable-<solver4>]
The LAV tool must be used in compliance with specific licence of each 
enabled solver. 

Build the project with 
$ make ENABLE_OPTIMIZED=1 
for Release version, or
$ make ENABLE_OPTIMIZED=0 
for Debug version.

Install the project with
$ make ENABLE_OPTIMIZED=1 install 

You can 
$ make doxygen 
if you want to get html documentation.

--------------------------------------------------------------------
For any further questions send an email to 
milena@matf.bg.ac.rs
--------------------------------------------------------------------




