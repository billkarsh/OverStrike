
Purpose:
+ Overwrite noise with zeros in imec probe data.
Run messages are appended to OverStrike.log in the current working directory.

Install:
1) Copy OverStrike-linux to your machine, cd into folder.
2) If needed, > chmod +x install.sh
3) > ./install.sh
4) Read notes in runit.sh wrapper script (required).

Compatibility:
- Included libraries are from Ubuntu 16.04 (Xenial).
- Tested with Ubuntu 20.04 and 20.10.
- Tested with Scientific Linux 7.3.
- Tested with Oracle Linux Server 8.3.
- Let me know if it runs on other distributions.

Usage:
>runit.sh -file=path/file.bin <-samps=a,b | -secs=a,b> [ options ]

# Enclosing whole parameter list in quotes is recommended, like this:
# >runit.sh '-file=path/file.bin <-samps=a,b | -secs=a,b> [ options ]'

Options:
-chans=0,3:5    ;only zero these channels (acquisition indices)

Notes:
- Use -file to set path and name of the .bin file.
- The .meta file must also exist in the same directory as the .bin.
- The named .bin is modified in place. The .meta is not changed.
- The .bin file length is not changed.
- The SY (sync) channel, if present, is never zeroed.
- The time span can be given in samples relative to file start.
- The time span can alternatively be given as seconds from file start.
- If -samps and -secs are both given, -secs is ignored.
- Ranges you enter are automatically bound to [0,file-length).
- Specify the whole file length as follows: -secs=0,1E9.
- OverStrike does one time span per call.
- Omit -chans=list to zero all channels (except SY).
- Use -chans=list to zero only selected channels.
- The channel indices must be original acquisition stream indices.
- AP indices are usually in range [0,383] and LF in range [384,767].
- Acquisition indices are displayed in the SpikeGLX FileViewer.
- Channel SY is not zeroed, even if in -chans list.


Change Log
----------
Version 1.0
- Initial release.


