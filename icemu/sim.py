import time


class Simulation:
    # this is the minimal time we want to wait between run loops.
    LOOP_PERIOD = 50 # in usec

    def __init__(self, usec_value=1):
        self._chips = []
        self.running = True
        self.usec_value = usec_value
        self._ticks = 0
        # whether the simulated time is going too fast for the computer's capabilities.
        # If it's true, the simulation might begin to lose timing accuracy. You should slow it
        # down with `usec_value` or increase TIME_RESOLUTION (do in on the C side as well!)
        self.running_late = False

    def _process(self):
        pass # override with code you want to execute in the runloop

    # add a chip that will have its tick() method called at each tick of the sim
    # if it exists, stop() will also be called when the sim stops.
    def add_chip(self, chip):
        self._chips.append(chip)
        return chip

    def elapsed_usecs(self):
        return self._ticks * self.LOOP_PERIOD

    def run(self):
        usec_value_in_seconds = (1 / (10 ** 6)) * self.usec_value
        tick_value_in_seconds = usec_value_in_seconds * self.LOOP_PERIOD
        timestamp = time.time()
        epoch = timestamp
        while self.running:
            try:
                for chip in self._chips:
                    chip.tick(self.LOOP_PERIOD)
                self._process()
                new_timestamp = time.time()
                self._ticks += 1
                target = epoch + self._ticks * tick_value_in_seconds
                self.late_by = new_timestamp - target
                self.running_late = self.late_by > 0
                if not self.running_late:
                    time.sleep(-self.late_by)
            except: # yes, even SysExit
                self.stop()
                raise

    def stop(self):
        for chip in self._chips:
            if hasattr(chip, 'stop'):
                chip.stop()
        self.running = False
