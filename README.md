# esc-tool-for-blheli

Automatically exported from code.google.com/p/esc-tool-for-blheli

## Building the EscTool

### On Ubuntu Linux
1. Install build-essential
   * ```sudo apt-get install build-essential```
2. Install Qt
   * ```sudo apt-get install libqt4-dev```
3. Optionally install qt-creator for developing on this project.
   * ```sudo apt-get install qt-creator```
4. Download the Code and open a console. Switch to the code-directory
   * ```qmake && make```
5. you can execute the EscTool by typing:
   * ```./escdude```

### Windows
TODO

## Building the firmware

## On Ubuntu Linux
1. Install the build chain
   * ```sudo apt-get install avr-libc binutils-avr gcc-avr avrdude```
2. Download the Code and open a console. Switch to the code-subdirectory firmware
   * ```./build```
   * edit upload file ... replace ```/dev/ttyACM0``` with the port which your Arduino uses and replace the baud rate with the one which fits for your Arduino
   * ```./upload```

## Connections on Arduino

D2 interface | atmega328p | arduino
--- | --- | ---
C2D | PORTB4 | 12
C2CK | PORTB3 | 11

