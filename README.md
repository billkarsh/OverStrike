OverStrike
==========

### What

OverStrike is a postprocessing adjunct to SpikeGLX.

OverStrike overwrites noisy regions in binary data with zeros:

- Zero any range of samples or times.
- Zero all channels or a specified subset.

### Who

OverStrike is developed by [Bill Karsh](https://www.janelia.org/people/bill-karsh)
of the [Tim Harris Lab](https://www.janelia.org/lab/harris-lab-apig) at
HHMI/Janelia Research Campus.

### Building in Windows

I build using Qt 5.12.0 (MinGW 64-bit). Compiled releases include the
app and supporting DLLs from that Qt version.

### Building in Linux

I build (under WSL2 on my Windows laptop) using ubuntu 16.04 and Qt
5.9.9 (MinGW 64-bit). Compiled releases include the app and supporting
DLLs from those versions of ubuntu and Qt, to make the package complete.
I build against old versions for (hopefully) better portability.

### Compiled Software

Official release software and support materials are here:
[**SpikeGLX Download Page**](http://billkarsh.github.io/SpikeGLX).

### Licensing

Use is subject to Janelia Research Campus Software Copyright 1.2 license terms:
[http://license.janelia.org/license](http://license.janelia.org/license).


_fin_

