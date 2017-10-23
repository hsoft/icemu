import curses
import time

from .sim import Simulation

class UIElement:
    def __init__(self, label, outputfunc):
        self.label = label
        self.outputfunc = outputfunc

class UIAction:
    def __init__(self, key, label, func):
        self.key = key
        self.label = label
        self.func = func

class UIScreen:
    def __init__(self, simulation, refresh_rate=0.3): # 300ms
        self.simulation = simulation
        self.stdscr = curses.initscr()
        self.refresh_rate = refresh_rate
        curses.noecho()
        curses.cbreak()
        curses.curs_set(0)
        self.stdscr.nodelay(True)
        self.elements = []
        self.actions = []
        self.key2actions = {}
        self.last_ch = -1
        self.elements_win = curses.newwin(1, 42, 0, 0)
        self.action_win = curses.newwin(1, 42, 1, 1)
        self.add_action(
            'q', "Quit",
            self.simulation.stop,
        )

    def _element_lines(self):
        lines = []
        for elem in self.elements:
            lines += [elem.label]
            lines += elem.outputfunc().splitlines()
        return lines

    def _read_key(self):
        ch = self.stdscr.getch()
        if ch > 0 and ch != self.last_ch:
            key = chr(ch)
            if key in self.key2actions:
                self.key2actions[key].func()
        self.last_ch = ch

    def _refresh_actions(self):
        win = self.action_win
        win.erase()
        _, maxw = win.getmaxyx()
        actionlines = ["{} - {}".format(action.key, action.label) for action in self.actions]
        actionw = max(len(line) for line in actionlines) + 2
        win.addnstr(0, 0, "Menu:", actionw)
        for i, line in enumerate(actionlines):
            win.addnstr(i+1, 0, line, actionw)

        x = actionw + 1
        w = maxw - x
        win.addnstr(0, x, "Stats:", w)
        win.addnstr(1, x, "Time: %1.1f s" % (self.simulation.elapsed_usecs() / (1000 * 1000)), w)
        win.addnstr(2, x, "Slowdown modifier: %d x" % (self.simulation.usec_value), w)
        if self.simulation.running_late:
            win.addnstr(3, x, "Running late!", w)

        win.refresh()

    def _refresh_elements(self):
        win = self.elements_win
        win.erase()
        y = 0
        x = 0
        acc_y = 0
        maxh, maxw = win.getmaxyx()
        for elem in self.elements:
            lines = [elem.label] + elem.outputfunc().splitlines()
            elemw = max(len(line) for line in lines) + 2
            if elemw + x >= maxw:
                x = 0
                y += acc_y
                acc_y = 0
            elemh = len(lines)
            acc_y = max(elemh, acc_y)
            for i, line in enumerate(lines):
                win.addnstr(y+i, x, line, elemw)
            x += elemw
        win.refresh()

    def _resize_windows(self):
        maxh, maxw = self.stdscr.getmaxyx()
        acth, _ = self._win_actions_size()
        acth += 2
        self.action_win.mvwin(maxh-acth, 0)
        self.action_win.resize(acth, maxw)
        self.elements_win.resize(maxh-acth-1, maxw)
        self.last_refresh = 0
        self.refresh()

    def _win_actions_size(self):
        h = min(len(self.actions) + 1, 4)
        # 5 chars are for key prefixes
        w = max((len(a.label) for a in self.actions), default=1) + 5
        return (h, w)

    def stop(self):
        curses.nocbreak()
        curses.echo()
        curses.endwin()
        curses.curs_set(1)

    def add_element(self, label, outputfunc):
        self.elements.append(UIElement(label, outputfunc))
        self._resize_windows()

    def add_action(self, key, label, func):
        self.actions.append(UIAction(key, label, func))
        self.key2actions = {a.key: a for a in self.actions}
        self._resize_windows()
        self._refresh_actions()

    def refresh(self):
        if self.last_refresh + self.refresh_rate < time.time():
            self.last_refresh = time.time()
            self._refresh_elements()
            self._refresh_actions()
            self.stdscr.refresh()

        self._read_key()


class SimulationWithUI(Simulation):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.uiscreen = UIScreen(self)

    def _process(self):
        self.uiscreen.refresh()

    def stop(self):
        super().stop()
        self.uiscreen.stop()

