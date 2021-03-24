import sys
import math
from burst import Status, CPUBurst, IOBurst, Arrival
from process import Process
from fprocess import FProcess
from copy import deepcopy
from fcfs import FCFS
from rr import RR
from sjf import SJF
from srt import SRT

output_template = """Algorithm {}
-- average CPU burst time: {:.3f} ms
-- average wait time: {:.3f} ms
-- average turnaround time: {:.3f} ms
-- total number of context switches: {}
-- total number of preemptions: {}
-- CPU utilization: {:.3f}%
"""

# 48-bit random number generator, equivalence in C.
class Rand48(object):
    def __init__(self, seed = 0):
        self.n = seed
    
    def srand(self, seed):
        #sets the high order 32-bits of Xi to the argument seedval.  
        #sets the low order 16-bits are set to the arbitrary value 0x330E.        
        self.n = (seed << 16) + 0x330e
    
    def next(self):
        self.n = (25214903917 * self.n + 11) & (2**48 - 1)
        
        return self.n

    def drand(self):
        # give a number in range [0, 1)
        return self.next() / 2**48


if __name__ == "__main__":
    if len(sys.argv) < 8 or len(sys.argv) > 9:
        raise Exception("Error: invalid args")

    rr_behavior = Status.RR_END
    _, num_process, seed, lamb, rand_upper_bound, ctx_time, alpha, rr_time_slice = sys.argv
    if len(sys.argv) == 9:
        rr_behavior = Status.RR_END if sys.argv[8] == 'END' else Status.RR_BEGINNING 
    
    ### Get your parameters here ###
    num_process = min(int(num_process), 26)
    seed = int(seed)
    lamb = float(lamb) # avg ram value
    rand_upper_bound = int(rand_upper_bound)
    ctx_time = int(ctx_time) # always an even
    alpha = float(alpha) # estimates determined via exponential averaging
    rr_time_slice = int(rr_time_slice) 

    # Generate Processes
    rand_generator = Rand48()
    rand_generator.srand(seed)
    
    ### Helpers for desired number (with bound)
    def genFloor():
        tmp = math.floor(-math.log(rand_generator.drand()) / lamb)
        while tmp > rand_upper_bound:
            tmp = math.floor(-math.log(rand_generator.drand()) / lamb)
        
        return tmp

    def genCeil():
        tmp = math.ceil(-math.log(rand_generator.drand()) / lamb)
        while tmp > rand_upper_bound:
            tmp = math.ceil(-math.log(rand_generator.drand()) / lamb)
        
        return tmp

    processes = {}

    for i in range(num_process):
        pid = chr(ord('A') + i)
        burst = []
        arr_time = genFloor()
        cpu_burst = math.trunc(rand_generator.drand() * 100) + 1
        counter = 1
        for j in range(cpu_burst - 1):
            burst.append(CPUBurst(genCeil(), pid, counter))
            burst.append(IOBurst(genCeil() * 10, pid, counter))
            counter += 1
        burst.append(CPUBurst(genCeil(), pid, counter)) # add one more cpu time (than io)
        processes[pid] = Process(pid, arr_time, cpu_burst, burst)
    
    ### Print the process at each beginning
    def print_processes():
        for i in processes.values():
            print(i.get_summary())
    
    def print_tasks():
        for i in processes.values():
            print(i)
    
    # print_tasks()

    with open('simout.txt', 'w') as out_file:
        ## Now we have the processes, let's go

        ## Always remember the sequence! when out_file!
        ## name, AVG CPU Burst Time, Average Wait Time
        ## Average TurnAround Time, # of context switches
        ## # of preemptions, CPU utilization %
        ## Also, **copy the processes**

        ## Look, here comes the FCFS Algo!
        print_processes()
        fcfs_algo = FCFS(deepcopy(processes), ctx_time)
        avg_cpu_burst, avg_wait, avg_turnaround, num_context_switch, cpu_utilization = fcfs_algo.run()
        out_file.write(output_template.format(
            "FCFS",
            avg_cpu_burst,
            avg_wait,
            avg_turnaround,
            num_context_switch,
            0,
            cpu_utilization
        ))

        ### SJF
        print()
        # print_processes() ** controlled by method run **
        sjf_algo = SJF(deepcopy(processes), ctx_time, alpha, lamb)
        avg_cpu_burst, avg_wait, avg_turnaround, num_context_switch, _, cpu_utilization = sjf_algo.run()
        out_file.write(output_template.format(
            "SJF",
            avg_cpu_burst,
            avg_wait,
            avg_turnaround,
            num_context_switch,
            0,
            cpu_utilization
        ))
    
        # ### SRT
        print()
        # print_processes() ** controlled by method run **
        srt_algo = SRT(deepcopy(processes), ctx_time, alpha, lamb)
        avg_cpu_burst, avg_wait, avg_turnaround, num_context_switch, num_preemption, cpu_utilization = srt_algo.run()
        out_file.write(output_template.format(
            "SRT",
            avg_cpu_burst,
            avg_wait,
            avg_turnaround,
            num_context_switch,
            num_preemption,
            cpu_utilization
        ))

        ## RR
        print()
        print_processes()
        rr_algo = RR(deepcopy(processes), ctx_time, rr_time_slice, rr_behavior)
        avg_cpu_burst, avg_wait, avg_turnaround, num_context_switch, num_preemption, cpu_utilization = rr_algo.run()
        out_file.write(output_template.format(
            "RR",
            avg_cpu_burst,
            avg_wait,
            avg_turnaround,
            num_context_switch,
            num_preemption,
            cpu_utilization
        ))
    
    # Remove this statement when finished @TODO
    # This line is to ensure we have ended :) No way!


    

