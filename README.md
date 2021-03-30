## MemDumper
Dump Memory Segment From Process Memory and Rebuild So(Elf) Binaries

## Changelog
- v0.5:
    - 1) Added Option to Use PID instead of Package Name(Use new Option: -i or --pid)
    - 2) Added 64bit Elf Fixing(Please use 64bit dumper for 64bit processes)
    - 3) Few Things Fixed
	
## Features
- No need of Ptrace
- Bypass Anti Debugging
- Fix and Regenerate Elf Binaries
- Dumping of Lib from Memory of Process
- Auto Dumping With Segment Name
- Manual Dumping With Custom Memory Address
- Support Fast Dumping(May Miss some data due to limitations of syscalls)

## How to use
- You can Use latest precompiled Binaries from [HERE](https://github.com/kp7742/MemDumper/tree/master/libs) or You Can build your Own.
- Needs Root Access or Virtual Space
- Get Root Shell through Adb or Terminal Apps(type: 'su') or Normal Shell into Virtual Space via Terminal Apps
- For Usage Help: ./memdumper -h
- For General Usage: ./memdumper -p <packageName> <option(s)> -o <outputPath>
	```
	 Help: ./memdumper -h
	 
	 MemDumper v0.5 <==> Made By KMODs(kp7742)
	 Usage: ./memdumper -p <packageName> <option(s)>
	 Dump Memory Segment From Process Memory and Rebuild So(Elf) Libraries
	 -l for Library Mode, -m for Manual Dumping Mode, By Default Auto Dumping Mode
	 You can use either PID or Package Name, PID given priority over Package Name
	  Options:
	 --Auto Dump Args-------------------------------------------------------------------------
	   -n --name <segment_name>              Segment Name From proc maps
	 --Manual Dump Args-----------------------------------------------------------------------
	   -m --manual                           Manual Dump Mode for Custom Address
	   -n --name <dump_name>                 Dumping File Name
	   -s --start <address>                  Starting Address
	   -e --end <address>                    Ending Address
	 --Lib Dump Args-------------------------------------------------------------------------
	   -l --lib                              Dump So(Elf) Library from Memory
	   -n --name <lib_name>                  Library Name From proc maps
	   -r --raw(Optional)                    Output Raw Lib and Not Rebuild It
	 --Other Args----------------------------------------------------------------------------
	   -f --fast(Optional)                   Enable Fast Dumping(May Miss Some Bytes in Dump)
	   -i --pid <process-id>                 PID of Process
	   -p --package <packageName>            Package Name of App
	   -o --output <outputPath>              File Output path(Default: /sdcard)
	   -h --help                             Display this information
	  
	```
- For Dumping Libraries
	```
	 Dump Library: ./memdumper -p com.dts.freefireth -l -r -n libil2cpp.so -o /sdcard
	 Process name: com.dts.freefireth, Pid: 27077
	 Base Address of libil2cpp.so Found At b2dc4000
	 End Address of libil2cpp.so Found At b60b5000
	 Lib Size: 53415936
	 Dumped in 25.414995S
	```

## How to Build
- Clone this repo
- Install Android NDK, if not already.
- Open Shell/CMD in Project Folder
- Drag ndk-build from NDK in Shell or CMD and then Execute
- Output will be in libs Folder.

## Credits
- [SoFixer](https://github.com/F8LEFT/SoFixer): 32bit So(Elf) Rebuilding
- [elf-dump-fix](https://github.com/maiyao1988/elf-dump-fix): 64bit So(Elf) Rebuilding

## Technlogy Communication
> Email: patel.kuldip91@gmail.com
