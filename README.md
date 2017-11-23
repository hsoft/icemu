# icemuc

This is a prototype of a pure C implementation of [icemu](https://github.com/hsoft/icemu). After
a some quick prototypes in Rust, C and Python, I opted for Python because I assumed that speed
wasn't very important. However, as I try more and more complex simulations, I see that speed is
quickly becoming an issue.

I'm trying out C a little harder and see if it leads to something that isn't of an overwhelming
complexity. So far it's looking good...

# Requirements

* C compiler
* GNU make
* ncurses

# Usage

Not usable right now, but there's a test suite. `make test` runs it.

I've started populating the `example` folder. They're barely working, but still. `cd` into it then
run `make sim`.
