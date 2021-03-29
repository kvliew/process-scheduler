**COMP30023 Project 1 2021**  
**Khin Vyn LIEW**  
khinl@student.unimelb.edu.au

**Code Attribution**  
I attempted the 2020 version of COMP30023 but later withdrew due to personal reasons. I completed the process scheduling project from last year. Please note that several sections of my submission for this year contains code re-used from last year. Below is a summary of the code I re-used from my previous submission for your reference:  

**allocate.c**
- File reading lines 32-67 are re-used code.
- Simulation loop beginning line 124 was written based on the simulation loop from my previous submission. Modifications were made to cater for multiple processor simulations.

**scheduling.c**
- Code in step() and challengeStep() functions were created based on the step() function from my previous submission. Modifications were made to cater for multiple processor simulations.  
- The calculatePerformance() function (used to calculate required performance statistic) is set up the same why as in my previous submission, with throughput calculations omitted.

**queue.c**
- enQueue() and deQueue() functions are re-used functions from the queue used in my previous submission. For enQueue(), several modifications were made, including utilisation of qsort() for sorting processes.