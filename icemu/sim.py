import time


class Simulation:
    # this is the minimal time we want to wait between run loops.
    LOOP_PERIOD = 50 # in usec

    def __init__(self, usec_value=1):
        self._chips = []
        self.running = True
        self.usec_value = usec_value
        self._elapsed_usecs = 0
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
        return self._elapsed_usecs

    def run(self):
        usec_value_in_seconds = (1 / (10 ** 6)) * self.usec_value
        while self.running:
            try:
                timestamp = time.time()
                target_time = timestamp + (usec_value_in_seconds * self.LOOP_PERIOD)
                while time.time() < target_time:
                    time.sleep(0)
                new_timestamp = time.time()
                elapsed_usecs = round((new_timestamp - timestamp) / usec_value_in_seconds)
                self._elapsed_usecs += elapsed_usecs
                self.running_late = elapsed_usecs > (self.LOOP_PERIOD * 2)
                for chip in self._chips:
                    chip.tick(elapsed_usecs)
                self._process()
            except: # yes, even SysExit
                self.stop()
                raise

    def stop(self):
        for chip in self._chips:
            if hasattr(chip, 'stop'):
                chip.stop()
        self.running = False
