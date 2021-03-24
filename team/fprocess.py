from process import Process
from burst import Status, Arrival

class FProcess(Process):
    ''' Overwrite Process'''
    def __init__(self, pid, arr_time, cpu_burst, burst):
        self.basic_info = {
            'pid': pid,
            'arr_time': arr_time,
            'cpu_burst': cpu_burst
        }
        self.prepend = Arrival(arr_time, pid) # this is used for arrival time and context switch
        self.burst = burst # [Task]: list of CPU/IO
        self.basic_info['total_burst_time'] = self.get_total_burst_time()
        self.preempted = False
        self.wait_turn = 0 # num of wait turn
        self.time = 0 # used to calculate turnaround time
        self.wait_time = 0 # time where process in the queue
        self.in_cpu = False # Identify current wait state
        self.switching = False # Identify the process is in switching state

    '''
        Quick Peak of first task
        level: number of item behind
        None if cannot each the level
    '''
    def peak(self, level=0):
        if self.prepend != None:
            if level == 0:
                return self.prepend
            else:
                return self.peak_burst(level - 1)
        else:
            return self.peak_burst(level)
    
    def peak_burst(self, level=0):
        if len(self.burst) > level:
                return self.burst[level]
        return None

    def get_remain_burst(self):
        return int((len(self.burst) + 1) / 2)
    
    def get_wait_time(self):
        return self.wait_time

    def get_total_burst_time(self):
        total = self.basic_info.get('total_burst_time')
        if total != None:
            return total
        else:
            total = 0

        for i in self.burst:
            if i.my_type() == Status.CPU:
                total += i.time
        
        return total

    '''Mutator'''

    def set_prepend(self, task):
        if self.prepend != None:
            raise Exception("Process {} has a prepend {} waiting, empty it before set another one".format(self.get_pid(), str(self.prepend)))

        self.prepend = task

    '''
        This function takes a time and Status
        to consume. It only consume if the process current peak()
        allows to be consume.
    '''
    def dec_wait_time(self, time):
        curr = self.peak()
        if curr:
            the_type = curr.my_type()
            # Always decreasing arriving time or IO time
            # May add more types here
            if the_type is Status.CTX_SWITCH and not self.in_cpu:
                # it is about to get in the CPU, but used to be in the ready queue
                # so we need to add this time
                self.dec_first_time(time)
                self.wait_time += time
            elif the_type is Status.ARRIVING or the_type is Status.IO or the_type is Status.CTX_SWITCH:
                self.dec_first_time(time)
            elif self.in_cpu == True and the_type == Status.CPU:
                # If we are in CPU, we should decrease as well
                self.dec_first_time(time)
            elif self.in_cpu == False:
                # Aww, we are waiting
                self.wait_time += time
        return curr
    
    def dec_first_time(self, time):
        curr = self.peak()
        the_type = curr.my_type()
        res = curr.dec_time(time)
        if res == 0:
            if the_type == Status.ARRIVING or the_type == Status.CTX_SWITCH:
                self.prepend = None
            else: 
                self.burst.pop(0)
    
    @staticmethod
    def get_fprocess(process):
        return FProcess(process.get_pid(), process.get_arr_time(), process.get_cpu_burst(), process.burst)