# cheat-loader-x64 - kernel-mode version

This project is abandoned but still perfectly fine to use.

This contains a x64 based manual mapper with a download logic & with pre authentification which returned a xor-key
which then was used to de-xor the dynamically generated bin for use in the injection process into the game.

VMProtect was used in this loader so you may want to remove that or implement the SDK & use it yourself.

This loader has some security features which I came up on the spot and never expanded upon even though I had multiple working POC's
which would have been a lot better lol.

This uses the Virtual box vuln. driver to load our kernel. (kernel project not included)

Also make sure you clear traces of the driver used.
(Timestamp of this driver 0x4840B58D)

Feel free to use this in whatever way you like.

Credits:
- @Google
- @VMProtect
- @Unknowncheats for the vuln. kernel
- @me
- @hde64 - disasm code
