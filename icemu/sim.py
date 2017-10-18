class Simulation:
    def _get_pin(self, pin_number):
        raise NotImplementedError()

    def pinset(self, pin_number, high):
        pin = self._get_pin(pin_number)
        pin.set(high)

    def pinishigh(pin_number):
        pin = self._get_pin(pin_number)
        return pin.ishigh()

    def delay(us):
        circuit.delay(us)

    def stop():
        global circuit, uiscreen
        uiscreen.stop()
        circuit = None
        uiscreen = None

    def int0_interrupt_check():
        if not circuit.serial_buffer.isempty():
            circuit.serial_buffer.pull()
            return True
        else:
            return False

    def set_timer0_target(ticks):
        circuit.timer0.set_target(ticks)

    def set_timer0_mode(mode):
        pass

    def timer0_interrupt_check():
        return circuit.timer0.check()
