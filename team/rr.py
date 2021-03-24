from burst import Status, ContextSwitch
from fprocess import FProcess

class RR():
    def __init__(self, processes, ctx_time, rr_time_slice, rr_behavior):
        self.processes = processes
        for key in self.processes:
            self.processes[key] = FProcess.get_fprocess(self.processes[key])
        self.ended_processes = []
        self.ctx_time = ctx_time // 2
        self.time_slice = rr_time_slice
        self.behavior = rr_behavior
        self.queue = []
        self.time = 0  # Total simulation time
        self.current_burst = None
        self.current_burst_time = 0
        self.cpu_time = 0  # total burst time
        self.context_switch = 0
        self.preemption = 0
        self.switching = self.ctx_time
        self.io = []

    def output_queue(self):
        if len(self.queue) == 0:
            return "<empty>"
        seq = filter(lambda pid: not self.processes[pid].switching, self.queue)
        return ' '.join(seq)
    
    def burst_completion(self):
        if self.current_burst != None:
            # Case where switch out
            if self.current_burst.time == 0:
                current_process = self.current_burst.pid
                self.switching = self.ctx_time * 2
                # Pop this CPU burst
                burst = self.processes[current_process].get_burst()
                pid = burst.pop(0).pid
                num_remaining_burst = len(burst) // 2
                # If this process have no further burst, terminate it
                if num_remaining_burst == 0:
                    if self.time <= 999:
                        print("time {}ms: Process {} terminated [Q {}]".\
                            format(self.time, pid, self.output_queue()))
                    self.ended_processes.append(self.processes[pid])
                    self.processes.pop(pid)
                else: # we have io to do
                    if self.time <= 999:
                        print("time {}ms: Process {} completed a CPU burst; {} {} to go [Q {}]".\
                            format(self.time, pid, num_remaining_burst, ["bursts","burst"][num_remaining_burst==1], self.output_queue()))
                    io = burst.pop(0)
                    if self.time <= 999:
                        print("time {}ms: Process {} switching out of CPU; will block on I/O until time {}ms [Q {}]".\
                            format(self.time, pid, self.time+self.ctx_time+io.time, self.output_queue()))
                    io.time += self.ctx_time
                    self.io.append(io)
                self.current_burst_time = 0
                self.current_burst = None
            # Case where preemption occurs
            if self.current_burst_time == self.time_slice:
                # Case that queue is empty, so no preemption
                if len(self.queue) == 0:
                    if self.time <= 999:
                        print("time {}ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]".\
                            format(self.time))
                    self.current_burst_time = 0
                else:
                    self.preemption += 1
                    self.current_burst.preempted = True
                    pid = self.current_burst.pid
                    if self.time <= 999:
                        print("time {}ms: Time slice expired; process {} preempted with {}ms to go [Q {}]".\
                            format(self.time, pid, self.current_burst.time, self.output_queue()))
                    if self.behavior == Status.RR_END:
                        self.queue.append(self.current_burst.pid)
                    else:
                        self.queue.insert(1, self.current_burst.pid)
                    self.processes[self.current_burst.pid].wait_time -= self.ctx_time
                    self.current_burst = None
                    self.current_burst_time = 0
                    self.switching = self.ctx_time * 2
        elif self.switching == 0 and len(self.queue) > 0: # Case we can perform a new CPU burst
            pid = self.queue.pop(0)
            process = self.processes[pid]
            process.switching = False
            self.context_switch += 1
            self.current_burst = process.get_burst()[0]
            self.current_burst_time = 0
            if not self.current_burst.preempted:
                if self.time <= 999:
                    print("time {}ms: Process {} started using the CPU for {}ms burst [Q {}]".\
                        format(self.time, pid, self.current_burst.time, self.output_queue()))
            else:
                if self.time <= 999:
                    print("time {}ms: Process {} started using the CPU with {}ms burst remaining [Q {}]".\
                        format(self.time, pid, self.current_burst.time, self.output_queue()))

    def io_completion(self):
        finished_pid = []
        index = 0
        while index < len(self.io):
            io = self.io[index]
            if io.time == 0:
                self.io.pop(index)
                pid = io.pid
                finished_pid.append(pid)
            else:
                index += 1
        finished_pid.sort()
        for pid in finished_pid:
            self.queue.append(pid)
            if self.time <= 999:
                print("time {}ms: Process {} completed I/O; placed on ready queue [Q {}]".\
                    format(self.time, pid, self.output_queue()))

    def new_arrival(self):
        for pid in self.processes:
            process = self.processes[pid]
            arr_time = process.get_arr_time()
            if arr_time == self.time:
                self.queue.append(pid)
                if self.time <= 999:
                    print("time {}ms: Process {} arrived; placed on ready queue [Q {}]".\
                        format(self.time, pid, self.output_queue()))
    
    def decrease_io(self):
        for io in self.io:
            io.time -= 1
    
    def increase_queue(self):
        if len(self.queue) == 0:
            return
        for pid in self.queue:
            if not self.processes[pid].switching:
                self.processes[pid].wait_time += 1

    def run(self):
        # Start our simulation :)
        print("time 0ms: Simulator started for RR with time slice {}ms and rr_add to {} [Q <empty>]".\
            format(self.time_slice, ["END", "BEGINNING"][self.behavior == Status.RR_BEGINNING]))
        while len(self.processes) != 0:
            self.burst_completion()
            self.io_completion()
            self.new_arrival()

            self.time += 1
            if self.current_burst != None:
                self.current_burst_time += 1
                self.current_burst.time -= 1
            elif len(self.queue) > 0:
                self.switching -= 1
                if self.switching < self.ctx_time:
                    self.processes[self.queue[0]].switching = True
            elif self.switching > self.ctx_time:
                self.switching -= 1

            self.decrease_io()
            self.increase_queue()

        # End of our simulation :)
        self.time = self.time-1+self.ctx_time
        print("time {}ms: Simulator ended for RR [Q <empty>]".format(self.time))

        total_burst_time = 0
        total_wait_time = 0
        total_burst = 0

        for i in self.ended_processes:
            total_burst_time += i.get_total_burst_time()
            total_wait_time += i.get_wait_time()
            total_burst += i.get_cpu_burst()
        
        return (total_burst_time / total_burst,
                total_wait_time / total_burst,
                (total_burst_time + self.context_switch * self.ctx_time * 2 + total_wait_time) / total_burst,
                self.context_switch,
                self.preemption,
                total_burst_time / self.time * 100)
    
