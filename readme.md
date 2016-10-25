#ALIAneS Emulation Project#
* Michael Kafarowski
* Project start date : March 10th 2015

###Current Progress and To do's###

CPU Complete! Bronze goal achieved! 

We have basic scrolling and greyscale video. Some glitches with the mirroring though. Getting scrolling working has partly broken Donkey Kong along the way...

The PPU will have to be re-written because it's becoming a hacky compatibility layer rather than an emulator, and it's impacting the accuracy. This will likely take quite a while because I've forgotten a lot of how my own code works!

###Project Goals###

I have a tier'd goal structure for this project:

- Bronze: Achieve CPU emulation - DONE!
- Silver: Achieve PPU emulation. Games should run at >5 frames per second. 
- Gold: Add very basic mapper support. Games should approach comfortable speeds if not already.
- Platinum: Add more mapper support and APU emulation. Games should run at or higher than full speed.


###Terms###

* CPU - Central Processing Unit : The chip that takes care of reading and executing instructions that appear in memory.
* PPU - Pixel Processing Unit : The chip that accepts commands from the CPU and using special tables in its own memory, takes care of drawing sprites and backgrounds to the screen.
* APU - Audio Processing Unit : Takes care of the game sound generation.
* More defined terms coming soon.

###About###

This is a project I am undertaking to learn more about a lot of things: advanced C++ coding, emulation, microprocessors, assembly language... etc. I am having a lot of fun with the development of this software as well, and regardless of how the final product turns out, this has already proven to be an amazing learning experience.
