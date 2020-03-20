## MemDumper
Dump Memory of Process in Android With Root Access

## Features
- No need of Ptrace
- Bypass Anti Debugging
- ~~Fast~~ Slow Dumping :P

## How to Build
- Install Android NDK, if not.
- Open Shell in This Folder
- Use and Execute ndk-build of NDK Here
- Output will be in libs Folder.
 
## How to use
- Needs Root Access
- Get Root Shell through Adb or Terminal Apps
- Get PID of Desired Process
- Get Memory Segement from /proc/[PID]/maps
- ./memdumper [PID] [segement-name]
- Output will be [segement-name] in Same Directory where executable is.
- eg: ./memdumper 1337 libil2cpp.so

## Note
- Use 64bit and 32bit binary for respected type of process.
- Due to Some Issues, I have reduced dumping speed.

## Technlogy Communication
> Email: patel.kuldip91@gmail.com
