
# Additional notes

## Apple Silicon 

The Apple Silicon processors' speeds are reported by 
MacOS's utility program `/usr/bin/powermetrics`.

## Windows

Running `bandwidth` on Windows can only occur after:

1. You install Cygwin 64-bit and you include gcc, make, and nasm.
2. You put Windows in performance mode.
3. You compile & run `bandwidth` within Cygwin.

## Android

The primary way to run `bandwidth` on Android is inside the *Termux* app,
which sets up a Linux-like environment in which you can run 
aarch64 ELF executables natively. This is one of the unusual
benefits of using an Android device.

Termux is downloaded from here and then you will side-load it.
* https://f-droid.org/packages/com.termux/

Actually there is a second way to run `bandwidth` on Android,
which is to statically compile `bandwidth64` in an aarch64
environment like a Docker container or on a Raspberry Pi, then:

* adb cp bandwidth64 /data/local/tmp
* adb shell /data/local/tmp/bandwidth64

You can install `adb` on Linux with: apt install adb

