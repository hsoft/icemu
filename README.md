# ICemu - Emulate Integrated Circuits

`icemu` is a Python library that emulates integrated circuits at the logic level. For example,
if you want to simulate a circuit with a decoder driving the clock pin of two shift registers,
it would look like this:

    dec = SN74HC138()
    sr1 = CD74AC164()
    sr2 = CD74AC164()
    mcu_pin = OutputPin('PB4')

    sr1.pin_CP.wire_to(dec.pin_Y0)
    sr2.pin_CP.wire_to(dec.pin_Y1)
    sr1.pin_DS1.wire_to(mcu_pin)
    sr2.pin_DS1.wire_to(mcu_pin)

    print(dec.asciiart())
         _______
       A>|- U +|>Y7
       B>|-   +|>Y6
       C>|-   +|>Y5
     G2A>|-   +|>Y4
     G2B>|-   +|>Y3
      G1>|+   +|>Y2
      Y0<|-___+|>Y1

You could then play with pins at your heart contents and have them "propagate" through wires and IC
logic automatically.

## See it in action

Here's a little video of `icemu` used in my [seg7-multiplex][seg7-multiplex] project:

[![asciinema](https://asciinema.org/a/WsYhXc1VcgfmkKZ8SAT18xYjv.png)](https://asciinema.org/a/WsYhXc1VcgfmkKZ8SAT18xYjv)

This board drives an array of 7-segments displays through a ATtiny45 MCU and a couple of ICs to
drive down pin count. The software that runs on the MCU is real, but with a couple of ifdefs, can
run on a completely simulated environment with the help of `icemu`.

## What is it for

The goal of this library is to facilitate the testing and debugging of embedded software. When we
run software on an embedded prototype, it's often hard to debug failures because we don't even
know if the problem comes from hardware (wiring, it's always the wiring!) or software. Moreover,
testing directly on a prototype often involves significant setup time.

With emulation, we have a quick setup time, introspection capabilities, all this stuff. We can then
confirm the soundness of our logic before sending it to our prototype.

## Comparison with Verilog/VHDL

Being new to the world of electronics, I don't know much about full blown simulation solutions.
However, from what I read about Verilog and VHDL, these tools seem to be about helping to design
**circuits**.

ICemu's goal is not that! Its goal is to help you debug the software you're going to flash on your
MCU. Python being easily hooked to C, you can, with a little abstraction layer, directly run your
code on the simulator and debug it there.

What I've read about simulations on Verilog/VHDL simulators is that you supply it with a series of
inputs you want to send to your circuits. That's insufficient! What I want to do is run my whole,
complex software and have **it** supply the inputs and react to the outputs of my simulated circuit.

There's a possibility that my newbie-ness made me create a tool that already exists, however, and
if that happened, please tell me so I can stop working on useless tools.

## Why Python

Because it's used for debugging purposes, speed is not essential. Also, Python is easy to glue
with C.

I've tried writing quick `icemu` prototype in C and Rust, but they were needlessly complicated.
With Python, it's easy to write the software and add new chips. Because there's gonna be a *lot*
of these chips to add, we might as well make this process as fast as possible.

## How to use

You can install `icemu` with pip on python **3.4+**:

    $ pip install --user icemu

Then, you need to recreate your prototype's logic in a small Python program that uses `icemu` and
wrap that into easy to use functions. Those functions should be designed to receive pin state
change from the MCU and apply the logic change into your circuit. Make that program print relevant
information so that you can assert your logic's soundness.

Then, write yourself a small Hardware Abstraction Layer at the pin/register level, embed your
Python program like a regular C application would do, make your `ifdef`ed functions call helper
functions you've written in your Python program, compile and run!

## Examples

You can find small examples is the `tests` folders, but the best way to get the grand idea is to
look at a project that uses it such as my [seg7-multiplex][seg7-multiplex] or my 
[solar-timer][solar-timer]. Read the Simulation part of the README to get started.

## License

LGPLv3, Copyright 2017 Virgil Dupras

[solar-timer]: https://github.com/hsoft/solar-timer
[seg7-multiplex]: https://github.com/hsoft/seg7-multiplex
