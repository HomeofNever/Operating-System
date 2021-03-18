class Status():
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
    def __init__(self, time):
        self.time = time

    '''
        This function decreases time by the amount provided, and return remaining
    '''
    def dec_time(self, time):
        self.time -= time

        if self.time < 0:
            raise "Awww, too much time here... Can't hold it! {}".format(self.time)

        return self.time

    '''
        this function identify the time provided and see
        if time given is enough to consume
    '''
    def is_larger(self, time):
        return self.time > time

    def whoami(self):
        return type(self).__name__

    def __str__(self):
        return '{}: {}ms'.format(self.whoami(), self.time)


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
    task1 = Task(1)
    print(task1)

    CPU1 = CPUBurst(100)
    print("{}, isCPU: {}".format(CPU1, CPU1.my_type()))

    IO1 = IOBurst(100)
    print("{}, isCPU: {}".format(IO1, IO1.my_type()))

    # Functions
    print('100ms - 10ms = {}'.format(CPU1.dec_time(10)))  # 90
    print('100ms > 1000ms: {}'.format(IO1.is_larger(1000)))
