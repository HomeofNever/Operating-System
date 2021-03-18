from burst import Status, ContextSwitch

class FCFS():
    def __init__(self, processes, ctx_time):
        self.processes = processes
        self.ctx_time = ctx_time / 2 # in and out
        self.queue = []
        self.time = 0
    
    def getQueueStr(self):
        if len(self.queue) == 0:
            return "<empty>"
        else:
            return " ".join(self.queue)

    def findNextTask(self):
        npid, ntask = None, None
        for pid, process in self.processes:
            task = process.peak()
            if task:
                if ntime is None:
                    npid, ntask = pid, task
                elif ntask.time > task.time:
                    npid, ntime = pid, task

        return npid, ntask

    def apply_decrement(self, time):
        for pid, process in self.processes:
            task = process.dec_time(time)
            if task.time == 0:
                if task.my_type() == Status.CPU:
                    print("time {}ms: Process {} completed a CPU burst; {} bursts to go [Q {}]".format(self.time, pid, process.get_remain_burst(), self.getQueueStr()))
                    io = process.peak()
                   

    def run(self):
        print("time 0ms: Simulator started for FCFS [Q <empty>]")
        '''
            So our process is: 
            1) find the first arrvial time/task time
            2) If it complete, search for the next one
            
            During the process:
            1) need to re-added task to the queue: IO may have finished
            2) delete all processes time
        '''
        while len(self.processes) != 0:
            npid, ntask = self.findNextTask()
            ntime = ntask.time
            ntype = ntask.my_type()

            self.time += ntime
            if ntype == Status.ARRIVING:
                self.queue.append(npid)
                print("time {}ms: Process {} arrived; placed on ready queue [Q {}]".format(self.time, npid, self.getQueueStr()))
                self.processes[npid].prepend = ContextSwitch(self.ctx_time)
            elif ntype == Status.CPU:
                if self.queue[0] and self.queue[0] == npid:
                    ### Join the CPU
                    self.queue.pop()
                    print("time {}ms: Process {} started using the CPU for {}ms burst [Q {}]".format(self.time, npid, ntime, self.getQueueStr()))
                else:
                    ### Leaving the CPU
                    io = self.processes[npid].peak()
                    print("time {}ms: Process {} switching out of CPU; will block on I/O until time {}ms [Q {}]".format(self.time, npid, self.time + io.time, self.getQueueStr()))
                    self.processes[npid].prepend = ContextSwitch(self.ctx_time)
            elif ntype == Status.IO:
                ### Join the Queue
                print("time 2507ms: Process A completed I/O; placed on ready queue [Q A]")

            
            self.apply_decrement(ntime)
        






# Process A [NEW] (arrival time 9 ms) 16 CPU bursts
# time 0ms: Simulator started for FCFS [Q <empty>]
# time 9ms: Process A arrived; placed on ready queue [Q A]
# time 11ms: Process A started using the CPU for 56ms burst [Q <empty>]
# time 67ms: Process A completed a CPU burst; 15 bursts to go [Q <empty>]
# time 67ms: Process A switching out of CPU; will block on I/O until time 299ms [Q <empty>]
# time 299ms: Process A completed I/O; placed on ready queue [Q A]
# time 301ms: Process A started using the CPU for 60ms burst [Q <empty>]
# time 361ms: Process A completed a CPU burst; 14 bursts to go [Q <empty>]
# time 361ms: Process A switching out of CPU; will block on I/O until time 1893ms [Q <empty>]
# time 1893ms: Process A completed I/O; placed on ready queue [Q A]
# time 1895ms: Process A started using the CPU for 39ms burst [Q <empty>]
# time 1934ms: Process A completed a CPU burst; 13 bursts to go [Q <empty>]
# time 1934ms: Process A switching out of CPU; will block on I/O until time 2436ms [Q <empty>]
# time 2436ms: Process A completed I/O; placed on ready queue [Q A]
# time 2438ms: Process A started using the CPU for 47ms burst [Q <empty>]
# time 2485ms: Process A completed a CPU burst; 12 bursts to go [Q <empty>]
# time 2485ms: Process A switching out of CPU; will block on I/O until time 2507ms [Q <empty>]
# time 2507ms: Process A completed I/O; placed on ready queue [Q A]
# time 2509ms: Process A started using the CPU for 137ms burst [Q <empty>]
# time 2646ms: Process A completed a CPU burst; 11 bursts to go [Q <empty>]
# time 2646ms: Process A switching out of CPU; will block on I/O until time 2888ms [Q <empty>]
# time 2888ms: Process A completed I/O; placed on ready queue [Q A]
# time 2890ms: Process A started using the CPU for 64ms burst [Q <empty>]
# time 2954ms: Process A completed a CPU burst; 10 bursts to go [Q <empty>]
# time 2954ms: Process A switching out of CPU; will block on I/O until time 3106ms [Q <empty>]
# time 3106ms: Process A completed I/O; placed on ready queue [Q A]
# time 3108ms: Process A started using the CPU for 68ms burst [Q <empty>]
# time 3176ms: Process A completed a CPU burst; 9 bursts to go [Q <empty>]
# time 3176ms: Process A switching out of CPU; will block on I/O until time 3818ms [Q <empty>]
# time 3818ms: Process A completed I/O; placed on ready queue [Q A]
# time 3820ms: Process A started using the CPU for 113ms burst [Q <empty>]
# time 3933ms: Process A completed a CPU burst; 8 bursts to go [Q <empty>]
# time 3933ms: Process A switching out of CPU; will block on I/O until time 4095ms [Q <empty>]
# time 4095ms: Process A completed I/O; placed on ready queue [Q A]
# time 4097ms: Process A started using the CPU for 11ms burst [Q <empty>]
# time 4108ms: Process A completed a CPU burst; 7 bursts to go [Q <empty>]
# time 4108ms: Process A switching out of CPU; will block on I/O until time 5730ms [Q <empty>]
# time 5730ms: Process A completed I/O; placed on ready queue [Q A]
# time 5732ms: Process A started using the CPU for 24ms burst [Q <empty>]
# time 5756ms: Process A completed a CPU burst; 6 bursts to go [Q <empty>]
# time 5756ms: Process A switching out of CPU; will block on I/O until time 6928ms [Q <empty>]
# time 6928ms: Process A completed I/O; placed on ready queue [Q A]
# time 6930ms: Process A started using the CPU for 76ms burst [Q <empty>]
# time 7006ms: Process A completed a CPU burst; 5 bursts to go [Q <empty>]
# time 7006ms: Process A switching out of CPU; will block on I/O until time 7288ms [Q <empty>]
# time 7288ms: Process A completed I/O; placed on ready queue [Q A]
# time 7290ms: Process A started using the CPU for 122ms burst [Q <empty>]
# time 7412ms: Process A completed a CPU burst; 4 bursts to go [Q <empty>]
# time 7412ms: Process A switching out of CPU; will block on I/O until time 7744ms [Q <empty>]
# time 7744ms: Process A completed I/O; placed on ready queue [Q A]
# time 7746ms: Process A started using the CPU for 9ms burst [Q <empty>]
# time 7755ms: Process A completed a CPU burst; 3 bursts to go [Q <empty>]
# time 7755ms: Process A switching out of CPU; will block on I/O until time 8437ms [Q <empty>]
# time 8437ms: Process A completed I/O; placed on ready queue [Q A]
# time 8439ms: Process A started using the CPU for 62ms burst [Q <empty>]
# time 8501ms: Process A completed a CPU burst; 2 bursts to go [Q <empty>]
# time 8501ms: Process A switching out of CPU; will block on I/O until time 9273ms [Q <empty>]
# time 9273ms: Process A completed I/O; placed on ready queue [Q A]
# time 9275ms: Process A started using the CPU for 200ms burst [Q <empty>]
# time 9475ms: Process A completed a CPU burst; 1 burst to go [Q <empty>]
# time 9475ms: Process A switching out of CPU; will block on I/O until time 9587ms [Q <empty>]
# time 9587ms: Process A completed I/O; placed on ready queue [Q A]
# time 9589ms: Process A started using the CPU for 197ms burst [Q <empty>]
# time 9786ms: Process A terminated [Q <empty>]
# time 9788ms: Simulator ended for FCFS [Q <empty>]