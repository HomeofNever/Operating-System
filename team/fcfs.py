from burst import Status, ContextSwitch
from fprocess import FProcess

# Class definition for FCFS algorithm
class FCFS():
    # Initializer
    def __init__(self, processes, ctx_time):
        self.processes = processes
        for key in self.processes:
            self.processes[key] = FProcess.get_fprocess(self.processes[key])
        self.ended_processes = []
        self.ctx_time = ctx_time / 2  # in and out
        self.queue = []
        self.time = 0  # Total simulation time
        self.current_process = None
        self.cpu_time = 0  # total burst time
        self.context_switch = 0

    # Used to output easier
    def getQueueStr(self):
        if len(self.queue) == 0:
            return "<empty>"
        else:
            return " ".join(self.queue)

    ''' Get next tasks, sorted '''

    def findNextTasks(self):
        ntasks = []
        for process in self.processes.values():
            ntasks.append(process.peak())

        if len(ntasks) == 0:
            return ntasks

        if self.current_process != None:
            # Remove all other CPU burst for now: they will have to wait
            ntasks = [i for i in ntasks if i.pid ==
                      self.current_process or i.my_type() != Status.CPU]

        maxTime = ntasks[0].time
        for i in ntasks:
            if i.time < maxTime:
                maxTime = i.time

        ntasks = [i for i in ntasks if i.time <= maxTime]
        ntasks.sort()
        # print("MaxTime" + str(maxTime) + ' '.join(str(x) for x in ntasks))

        return ntasks

    def cpu_handler(self, current_cpu):
        # Currently in progress
        ntype = current_cpu.my_type()
        npid = current_cpu.pid
        ntime = current_cpu.time
        if ntype == Status.CTX_SWITCH:
            next_task = self.processes[npid].peak(1)
            if next_task != None and next_task.my_type() == Status.CPU:
                # Join the CPU
                self.time += ntime
                self.processes[npid].dec_first_time(ntime)
                next_cpu_time = self.processes[npid].peak().time
                if self.time <= 999:
                    print("time {:.0f}ms: Process {} started using the CPU for {:.0f}ms burst [Q {}]".format(
                    self.time, npid, next_cpu_time, self.getQueueStr()))
                self.processes[npid].in_cpu = True
                # ctx only count once, as here we have divided into two parts
                self.context_switch += 1
            else:
                # Leaving the CPU
                self.processes[npid].dec_first_time(ntime)
                self.processes[npid].in_cpu = False
                self.current_process = None
                # Here we need to end the process, if this is it's last switch
                if self.processes[npid].peak() is None:
                    ### Termination Point ###
                    print("time {:.0f}ms: Process {} terminated [Q {}]".format(
                        self.time, npid, self.getQueueStr()))
                    self.processes[npid].time = self.time
                    self.ended_processes.append(self.processes[npid])
                    del self.processes[npid]
                # Now, record the time
                self.time += ntime
        # Active CPU time
        elif ntype == Status.CPU:
            self.time += ntime
            self.cpu_time += ntime
            self.processes[npid].dec_first_time(ntime)
            remain_burst = self.processes[npid].get_remain_burst()
            if remain_burst > 0:
                if self.time <= 999:
                    print("time {:.0f}ms: Process {} completed a CPU burst; {} burst{} to go [Q {}]".format(
                        self.time, npid, remain_burst,
                        '' if remain_burst == 1 else 's', self.getQueueStr()))
                io = self.processes[npid].peak()
                # Try if there is a IO followed, if nothing, it will switch out and terminate
                if io:
                    if self.time <= 999:
                        print("time {:.0f}ms: Process {} switching out of CPU; will block on I/O until time {:.0f}ms [Q {}]".format(
                            self.time, npid, self.time + self.ctx_time + io.time, self.getQueueStr()))
            self.processes[npid].set_prepend(
                ContextSwitch(self.ctx_time, npid))

    '''
        So our process is: 
        1) find the first arrival time/task time
        2) If it complete, search for the next one
        
        During the process:
        1) need to re-added task to the queue: IO may have finished
        2) remove processes when they have finished
    '''

    def run(self):
        print("time 0ms: Simulator started for FCFS [Q <empty>]")
        while len(self.processes) != 0:
            current_cpu = self.processes[self.current_process].peak(
            ) if self.current_process != None else None
            # Add CPU task if possible
            if current_cpu == None and len(self.queue) > 0:
                # Find a task to fill in, could be none
                task = self.processes[self.queue[0]].peak()
                npid = task.pid
                # Filling Task into CPU
                self.processes[npid].set_prepend(
                    ContextSwitch(self.ctx_time, npid))
                self.current_process = npid
                self.queue.pop(0)

            affected_pids = []
            ntasks = self.findNextTasks()
            ntime = ntasks[0].time
            # print(' '.join(str(x) for x in ntasks))
            # print("advance: {}".format(ntime))
            # As there could be only one CPU/CTX running at a time, add time only when there is a CPU
            cpu_task = [c for c in ntasks if c.my_type(
            ) == Status.CPU or c.my_type() == Status.CTX_SWITCH]
            for task in cpu_task:
                if task.pid == self.current_process:
                    self.cpu_handler(task)
                    affected_pids.append(task.pid)
            # Advance time if cpu_handler is not running
            if len(affected_pids) == 0:
                self.time += ntime

            for task in ntasks:
                npid = task.pid
                ntype = task.my_type()
                if npid not in affected_pids:
                    affected_pids.append(npid)
                    # Always decreasing
                    if ntype == Status.IO:
                        self.queue.append(npid)
                        if self.time <= 999:
                            print("time {:.0f}ms: Process {} completed I/O; placed on ready queue [Q {}]".format(
                                self.time, npid, self.getQueueStr()))
                    elif ntype == Status.ARRIVING:
                        self.queue.append(npid)
                        if self.time <= 999:
                            print("time {:.0f}ms: Process {} arrived; placed on ready queue [Q {}]".format(
                                self.time, npid, self.getQueueStr()))
                    self.processes[npid].dec_first_time(ntime)

            for pid, process in self.processes.items():
                if pid not in affected_pids:
                    process.dec_wait_time(ntime)

        print(
            "time {:.0f}ms: Simulator ended for FCFS [Q <empty>]".format(self.time))

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
                total_burst_time / self.time * 100)
