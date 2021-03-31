# Helper class for recording status
class Status():
    UNDEF = -1
    EMPTY = 0
    CPU = 1
    IO = 2
    ARRIVING = 3
    CTX_SWITCH = 4
    ## Flag
    RR_BEGINNING = 5
    RR_END = 6
    # Please add your own status here

class Task():
    ### pid reference helps when comparison
    def __init__(self, time, pid, id=0):
        self.id = id
        self.time = time
        self.total_time = time
        self.pid = pid
        self.enter_at = 0
        self.preempted = False

    '''
        This function decreases time by the amount provided, and return remaining
    '''
    def dec_time(self, time):
        self.time -= time

        if self.time < 0:
            raise Exception("Awww, too much time here... {} can't hold it! {}".format(self.whoami(), self.time))

        return self.time

    def whoami(self):
        return type(self).__name__
    
    def my_type(self):
        return Status.UNDEF

    def __str__(self):
        return '[{}{}]{}: {}ms'.format(self.pid, self.id, self.whoami(), self.time)

    ''' Comparison here intended to include pid '''
    def __gt__(self, other):
        if self.time == other.time:
            if self.my_type() == other.my_type():
                return self.pid > other.pid
            else:
                return self.my_type() > other.my_type()
            
        return self.time > other.time

    def __eq__(self, other):
        if other is None:
            return False
        return self.whoami == other.whoami and self.time == other.time and self.pid == other.pid

''' Subclass for each type of task '''

class CPUBurst(Task):
    def my_type(self):
        return Status.CPU


class IOBurst(Task):
    def my_type(self):
        return Status.IO

class Arrival(Task):
    def my_type(self):
        return Status.ARRIVING

class ContextSwitch(Task):
    def my_type(self):
        return Status.CTX_SWITCH

'''Test tool set'''
if __name__ == "__main__":
    task1 = Task(1, 'Z')
    print(task1)

    CPU1 = CPUBurst(100, 'A')
    print("{}, isCPU: {}".format(CPU1, CPU1.my_type()))

    IO1 = IOBurst(100, 'N')
    print("{}, isCPU: {}".format(IO1, IO1.my_type()))

    # Functions
    print('100ms - 10ms = {}'.format(CPU1.dec_time(10)))  # 90
    print('100ms > 1000ms: {}'.format(IO1.time > 1000))

    # Sort test
    CPU2 = CPUBurst(1000, 'A')
    CPU3 = CPUBurst(100, 'C')
    CPU4 = CPUBurst(100, 'D')
    ls = [CPU2, CPU3, CPU1, CPU4]
    print("Before: " + ' '.join(str(e) for e in ls))
    ls.sort()
    print("After: " + ' '.join(str(e) for e in ls))