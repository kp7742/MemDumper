## MemDumper
Dump Memory Segment From Process Memory and Rebuild So(Elf) Libraries

## Features
- No need of Ptrace
- Bypass Anti Debugging
- Dumping of Lib from Memory of Process
- Auto Dumping With Segment Name
- Manual Dumping With Custom Memory Address
- Fix and Regenerate Elf File
- Support Fast Dumping(May Miss some data)

## How to use
- You can Use latest precompiled Binaries from [HERE](https://github.com/kp7742/MemDumper/tree/master/libs) or You Can build your Own.
- Needs Root Access or Virtual Space
- Get Root Shell through Adb or Terminal Apps(type: 'su') or Normal Shell into Virtual Space via Terminal Apps
- For Usage Help: memdumper -h
- For General Usage: memdumper -p <packageName> <option(s)> -o <outputPath>
	```
	 Help: ./memdumper -h
	 
	 MemDumper v0.4 <==> Made By KMODs(kp7742)
	 Usage: memdumper -p <packageName> <option(s)>
	 Dump Memory Segment From Process Memory and Rebuild So(Elf) Libraries
	 -l for Library Mode, -m for Manual Dumping Mode, By Default Auto Dumping Mode
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
	   -p --package <packageName>            Package Name of App
	   -o --output <outputPath>              File Output path(Default: /sdcard)
	   -h --help                             Display this information
	  
	```
- For Dumping Libraries
	```
	 Dump Library: ./memdumper -p com.dts.freefireth -l -n libil2cpp.so -o /sdcard
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
- Drag ndk-build of NDK in Shell or CMD an then Execute
- Output will be in libs Folder.

## Credits
- [SoFixer](https://github.com/F8LEFT/SoFixer): So(Elf) Rebuilding

## Technlogy Communication
> Email: patel.kuldip91@gmail.com
