## 'ps5_autoloader' folder
Put payloads in it. Then put this folder to `/mnt/USB?/` or `/data/`. Or implement it to the game savedata.

## 'autoload.txt' file
It's the script file to define payloads to run automatically by Autoloader. Edit this file according to the example in it.
**'Kstuff-lite' has to be the last one on the list.** Otherwise it would cause power-off. Use at your own risk.
(I think, it's the internal patching process of Kstuff wouldn't finish before ending the current running game. It would cause problem if any other payload was sent during this patching moment.)