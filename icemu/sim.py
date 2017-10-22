import time

TIME_RESOLUTION = 50 # in usecs

class Simulation:
    def __init__(self, usec_value=1):
        self.mcus = []
        self.running = True
        self.usec_value = usec_value
        self.ticks = 0
        # whether the simulated time is going too fast for the computer's capabilities.
        # If it's true, the simulation might begin to lose timing accuracy. You should slow it
        # down with `usec_value` or increase TIME_RESOLUTION (do in on the C side as well!)
        self.running_late = False

    def _process(self):
        pass # override with code you want to execute in the runloop

    def add_mcu(self, mcu):
        self.mcus.append(mcu)

    def elapsed_usecs(self):
        return self.ticks * TIME_RESOLUTION

    def run(self):
        one_tick_in_seconds = (1 / (1000 * (1000 / TIME_RESOLUTION))) * self.usec_value
        target_time = time.time() + one_tick_in_seconds
        while self.running:
            for mcu in self.mcus:
                mcu.process_msgout()
                mcu.tick()
            self._process()
            self.running_late = time.time() > (target_time + one_tick_in_seconds)
            while time.time() < target_time:
                time.sleep(0)
            target_time += one_tick_in_seconds
            self.ticks += 1

    def stop(self):
        for mcu in self.mcus:
            mcu.stop()
        self.running = False
