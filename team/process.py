from burst import Status, Arrival

class Process():
    def __init__(self, pid, arr_time, cpu_burst, burst):
        self.basic_info = {
            'pid': pid,
            'arr_time': arr_time,
            'cpu_burst': cpu_burst
        }
        self.prepend = Arrival(arr_time) # this is used for arrival time and context switch
        self.burst = burst # [Task]: list of CPU/IO
        self.preempted = False
        self.wait_turn = 0 # num of wait turn

        # Note for RR: you may inherient this class if you found
        # more attributes or behavior needed to be override.

    '''
        Quick Peak of first task
    '''
    def peak(self):
        if self.prepend != None:
            return self.prepend
        if len(self.burst) != 0:
            return self.burst[0]

        return None

    def get_remain_burst(self):
        return len(self.burst + 1) / 2

    '''Mutator'''

    '''
        This function takes a time and Status
        to consume. It only consume if the process current peak()
        match the given type.
    '''
    def dec_time(self, time, taskType):
        curr = self.peak()
        if curr and (curr.myType() == taskType or curr.myType() == Status.ARRIVING):
            res = curr.dec_time(time)
            if res == 0:
                if curr is self.prepend:
                    self.prepend = None
                else:
                    self.burst.pop()
            
        return curr
            

    ''' 
        Tools for preemption
    '''
    def is_preempted(self):
        return self.preempted

    def reset_preempted(self):
        self.preempted = False
    
    def set_preempted(self):
        if not self.preempted:
            self.preempted = True
            self.wait_turn += 1
        else:
            raise "Process {} has preempted but tried to preemept again before resume. Maybe something goes wrong".format(self.basic_info['pid'])
    
    def get_summary(self):
        return 'Process {} [NEW] (arrival time {} ms) {} CPU bursts'.format(
            self.basic_info['pid'], 
            self.basic_info['arr_time'], 
            self.basic_info['cpu_burst']
        )
    
    def __str__(self):
        s = ''
        s += "{}\n".format(self.get_summary())
        for idx, val in enumerate(self.burst):
            s += "{}.{}\n".format(idx, str(val))

        return s

