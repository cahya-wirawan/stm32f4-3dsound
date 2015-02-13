# stm32f4-3dsound

STM32F4-3DSound is an application that runs on a STM32F4-Discovery board for playing a waveform file with 3D sound effects. It reads a mono sound waveform file from a USB stick, and processes the sound samples in real-time to produce a three-dimensional sound effect, then sends the output to stereo headphones. The 3D sound effect is controlled by the spatial alignment of the board, which uses the MEMS motion sensor to determine its position. If the board is turned to the left, the sound will come from the left side, and vice versa. Any angle in-between is of course handled as well. Currently only the horizontal angle (azimuth) is taken into account for the 3D sound effect calculation, but the next version will also include the vertical angle (elevation).

The application plays a waveform file from the root directory of a USB stick in a loop until the blue button is pressed, which plays the next waveform file. Only a mono sound file with sample rate of 44100 Hz will be accepted by the application, and headphones should be used to get the best result. Optionally, a serial connection (USART with TX=pin PA2, 115200 baud, 8N1) can be used to monitor the application (error messages, information about the waveform file to play, the angle position of the board, etc).

## Usage:

$ git clone git@github.com:cahya-wirawan/stm32f4-3dsound.git  
$ cd stm32f4-3dsound  
$ make

The binary (stm32f4_3dsound.bin, .hex, .elf) will be created in the build/ sub-directory. A pre-compiled binary can also be downloaded from this website.
