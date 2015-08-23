#ALIAneS Emulation Project#
* Michael Kafarowski
* Project start date : March 10th 2015

###Current Progress and To do's###

- Filling in operations!
- Add a verbose debug option that enables extra information about the operation of each opcode.

###August Shell Update###
After a couple months of my computer being out of commission, plus a new co-op job, I haven't had much time to work on the project. But I've made some progress: the August shell update is my first major release in a couple months and it includes a new "front end" that can be used to run the emulator as normal, or in a test mode. In this test mode you can write to blank memory and execute at different locations.

###Testing the Emulator###
First, download the latest version of the code and compile it. If you can use make and gcc, type `make` then `make run` to start. Otherwise compile as you would normally. Run the bin in a terminal. Typing `start` will search for a "dk.nes" file in the /res directory, you must supply this yourself. `Test` will initialize the CPU and allow you to `read` and `write` to memory, and either `decode` and execute an opcode at a location, or `exevute` an opcode, which will use the subsequent memory. `end` finishes this mode, or just type `quit` to end the program. Not much error checking for inputs has been implemented, so be careful.

###Project Goals###

I have a tier'd goal structure for this project:

- Bronze: Achieve CPU emulation
- Silver: Achieve PPU emulation. Games should run at >5 frames per second
- Gold: Add very basic mapper support. Games should approach comfortable speeds if not already.
- Platinum: Add more mapper support and APU emulation. Games should run at or higher than full speed.


###Terms###

* CPU - Central Processing Unit : The chip that takes care of reading and executing instructions that appear in memory.
* PPU - Pixel Processing Unit : The chip that accepts commands from the CPU and using special tables in its own memory, takes care of drawing sprites and backgrounds to the screen.
* APU - Audio Processing Unit : Takes care of the game sound generation.
* More defined terms coming soon.

###About###

This is a project I am undertaking to learn more about a lot of things: advanced C++ coding, emulation, assembly language... etc. I am having a lot of fun with the development of this software as well, and regardless of how the final product turns out, this has already proven to be an amazing learning experience.
