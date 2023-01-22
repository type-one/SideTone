# SideTone
Portable C++ voice capture and low latency feedback

https://github.com/type-one/SideTone


# What
Small test program written in C++17 to have voice feedback with reasonably short latency. 
The code is portable and use the miniaudio framework (https://miniaud.io/).

# Why
The purpose was to test a simple double buffer structure with frame locking, with a 
capture thread producing samples and a playing thread replaying what was captured.
The double buffer itself is implemented in a cache-friendly way by splitting in two parts
the internal contiguous buffer of an std::vector.  The swapping of the buffers is also 
lock-free.
 
It was also fun to let my young children play with it and discover their own voice with
a small delay producing an echo.

This small framework can be a start for more complex sound processing on the feedback voice.

Note: miniaudio has some full-duplex features that allow latency optimization on this 
scenario, but I didn't want to rely on that for the test.

# How
Can be compiled and Linux and Windows, and should be easily
adapted for other platforms supported by miniaudio (Mac, Android, BSD).

On Linux, just use cmake .
On Windows, just use cmake-gui to generate a Visual Studio solution

Tested on Windows 10 and Ubuntu Linux.  Should work also on Raspberry Pi.
It just use default capture and playback devices.

Launch in a shell using "sidetone.exe" or "sidetone"
Press "enter" to exit

You can play with different parameters in the audio/audio_context.hpp
file: format, sample rate, channels, period.

Enjoy

# Author
Laurent Lardinois / Type One (TFL-TDV)

https://be.linkedin.com/in/laurentlardinois

https://demozoo.org/sceners/19691/
