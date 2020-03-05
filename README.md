## MemDumper
Dump Memory of Process in Android With Root Access

## Features
- No need of Ptrace
- Fast Dumping

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
- ./memdumper [PID] [segement-name] [start-addr] [end-addr]
- Output will be [segement-name].dump in Same Directory where executable is.

## Technlogy Communication
> Email: patel.kuldip91@gmail.com
