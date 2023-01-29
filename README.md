VICE Emulator emitting bap-frames traces
========================================

This is a patched version of the VICE C64/VIC20/... emulator that can emit
traces in the [bap-frames](https://github.com/BinaryAnalysisPlatform/bap-frames)
format, primarily to be used in combination with
[rz-tracetest](https://github.com/rizinorg/rz-tracetest) for testing 6502 lifting.

Status and quirks
-----------------

* Currently only x64sc (the standard C64 emulator) is patched for tracing, but
  the others could be added easily.
* Because there is no trivial way to determine which registers are read or
  written during the execution of an instruction, the pre-operands always
  contain all registers and post-operands contain only the registers that have
  changed. The register information is thus not sufficient for comparing
  exact events, but only for pre/post contents.

Building
--------

First, make sure the bap-frames submodule is up to date:
```
git submodule update --init
```

Then:
```
cd vice
./autogen.sh
./configure --enable-bap-frames --prefix=...
make
make install # optional
```

Usage
-----

Start x64sc, then to start tracing enter the monitor and type e.g.:
```
bapo "mytrace.frames"
```

After exiting the monitor, every instruction will be recorded to the file.
Later, to close the trace from the monitor, use:
```
bapc
```

Hint: It's convenient to enable the remote tcp monitor and connect with `nc`
instead of using the internal interface.
