from burst import Status, Arrival

class Process():
    def __init__(self, pid, arr_time, cpu_burst, burst):
        self.basic_info = {
            'pid': pid,
            'arr_time': arr_time,
            'cpu_burst': cpu_burst
        }
        self.burst = burst # [Task]: list of CPU/IO
        # self.preempted = False

        # Note for RR: you may inherient this class if you found
        # more attributes or behavior needed to be override.

    def get_pid(self):
        return self.basic_info['pid']

    def get_arr_time(self):
        return self.basic_info['arr_time']

    def get_cpu_burst(self):
        return self.basic_info['cpu_burst']

    ''' 
        Tools for preemption
    '''
    # def is_preempted(self):
    #     return self.preempted

    # def reset_preempted(self):
    #     self.preempted = False
    
    # def set_preempted(self):
    #     if not self.preempted:
    #         self.preempted = True
    #         self.wait_turn += 1
    #     else:
    #         raise Exception("Process {} has preempted but tried to preemept again before resume. Maybe something goes wrong".format(self.get_pid()))
    
    ''' Representation '''
    def get_summary(self):
        return 'Process {} [NEW] (arrival time {} ms) {} CPU burst{}'.format(
            self.get_pid(), 
            self.get_arr_time(), 
            self.get_cpu_burst(),
            '' if self.get_cpu_burst() == 1 else 's'
        )
    
    def __str__(self):
        s = ''
        s += "{}\n".format(self.get_summary())
        for idx, val in enumerate(self.burst):
            s += "{}.{}\n".format(idx, str(val))

        return s

