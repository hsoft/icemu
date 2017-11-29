# ICemu - Emulate Integrated Circuits

`icemu` is a C library that emulates integrated circuits at the logic level. Its goal is to
facilitate the debugging of software being written for microcontrollers being plugged into the
emulated circuit.

Yup, that's it! With a few shims, you can take the code that targets your MCU and run the **exact
same code** in an emulated circuit, on your computer. In the simulation, you can:

* See the state of every pin in the circuit.
* Pause the simulation.
* Slow the simulation down.
* Advance time "tick-by-tick", a "tick" being one pass of the main loop.
* Add triggers on some pins and make the simulation stop when the pin state change.

By testing the soundness of your software before sending it to the MCU, you can save significant
setup time and do introspection that you couldn't do on your hardware. Of course, the simulation
will always be an approximation, but it's still a good first step to weed out oubvious bugs
before debugging the really hairy stuff.

## See it in action

Here's a little video of the simulated `seg7` example (see `examples` folder):

[![asciinema](https://asciinema.org/a/LLNB2fvP0nPhn1CazSQ1y9ymT.png)](https://asciinema.org/a/LLNB2fvP0nPhn1CazSQ1y9ymT)

and here is the video of the **exact same code** running in real life!

[![vimeo](https://i.vimeocdn.com/video/662736875_677x.jpg)](https://vimeo.com/239693641)

## Comparison with Verilog/VHDL

Being new to the world of electronics, I don't know much about full blown simulation solutions.
However, from what I read about Verilog and VHDL, these tools seem to be about helping to design
**circuits**.

ICemu's goal is not that! Its goal is to help you debug the software you're going to flash on your
MCU. You can, with a little abstraction layer, directly run your code on the simulator and debug it
there.

What I've read about simulations on Verilog/VHDL simulators is that you supply it with a series of
inputs you want to send to your circuits. That's insufficient! What I want to do is run my whole,
complex software and have **it** supply the inputs and react to the outputs of my simulated circuit.

There's a possibility that my newbie-ness made me create a tool that already exists, however, and
if that happened, please tell me so I can stop working on useless tools.

## Where's the Python implementation

`icemu` was initially written in Python. I underestimated how quickly speed would become an issue
and, soon enough, it was. I rewrote the whole thing in C. The old python implementation is still
available in the `python` branch of the repo.

## Requirements

* C compiler
* GNU make
* ncurses
* CUnit for tests

## How to use

For now, the API is not stable so it's not a library meant to be installed. What you're going to
do is to add `icemu` as a subrepo build is along with your project. You can look at the `examples`
folder for example, or at my [seg7-multiplex][seg7-multiplex] for a real-world integration example.

Then, you need to recreate your prototype's logic along with IO shims in a small C program that
uses `icemu` and configure it with your program's runloop. Again, look at examples.

Then, compile, run and enjoy!

## Examples

There are examples in the `examples` folder. Follow instructions in the README file of each example.

## Documentation

Documentation is in the header files of the source. `sim.h`, `pin.h` and `chip.h` are good
starting point after having looked at the examples.

## License

LGPLv3, Copyright 2017 Virgil Dupras

[seg7-multiplex]: https://github.com/hsoft/seg7-multiplex
