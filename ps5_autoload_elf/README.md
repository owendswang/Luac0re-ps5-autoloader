## ps5_autoload.elf

Replacement of 'autoload.lua' function, which could run after the game closed to avoid conflicts between Kstuff initialization and other payloads. So Kstuff does **NOT** have to be the last one on the list 'autoload.txt'.

## Building

Built with '[ps5-payload-dev/pacbrew-repo](https://github.com/ps5-payload-dev/pacbrew-repo)'. (You might need to rebuild it by the source code of it. Because current release v0.32 is not built with the latest SDK which supports upto FW 12.00. Go check the 'Actions' section of it for detailed building procedure.)
