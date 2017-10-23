import time


class Simulation:
    TIME_RESOLUTION = 50 # in usecs

    def __init__(self, usec_value=1):
        self._chips = []
        self.running = True
        self.usec_value = usec_value
        self.ticks = 0
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
        return self.ticks * self.TIME_RESOLUTION

    def run(self):
        one_tick_in_seconds = (1 / (1000 * (1000 / self.TIME_RESOLUTION))) * self.usec_value
        target_time = time.time() + one_tick_in_seconds
        while self.running:
            try:
                for chip in self._chips:
                    chip.tick()
                self._process()
                self.running_late = time.time() > (target_time + one_tick_in_seconds)
                while time.time() < target_time:
                    time.sleep(0)
                target_time += one_tick_in_seconds
                self.ticks += 1
            except: # yes, even SysExit
                self.stop()
                raise

    def stop(self):
        for chip in self._chips:
            if hasattr(chip, 'stop'):
                chip.stop()
        self.running = False
