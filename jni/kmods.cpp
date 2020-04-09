#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <ctime>
#include <unistd.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>
#include "Log.h"
#include "Process.h"
#include "Mem.h"
#include "ELF/ElfReader.h"
#include "ELF/ElfRebuilder.h"

using namespace std;

const char* short_options = "hlmrfp:o:n:s:e:";
const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"lib", no_argument, NULL, 'l'},
        {"manual", no_argument, NULL, 'm'},
		{"raw", no_argument, NULL, 'r'},
		{"fast", no_argument, NULL, 'f'},
        {"package", required_argument, NULL, 'p'},
        {"output", required_argument, NULL, 'o'},
		{"name", required_argument, NULL, 'n'},
		{"start", required_argument, NULL, 's'},
		{"end", required_argument, NULL, 'e'},
		{NULL, 0, NULL, 0}
};

void Usage() {
	printf("MemDumper v0.4 <==> Made By KMODs(kp7742)\n");
	printf("Usage: memdumper -p <packageName> <option(s)>\n");
	printf("Dump Memory Segment From Process Memory and Rebuild So(Elf) Libraries\n");
	printf("-l for Library Mode, -m for Manual Dumping Mode, By Default Auto Dumping Mode\n");
	printf(" Options:\n");
	printf("--Auto Dump Args-------------------------------------------------------------------------\n");
	printf("  -n --name <segment_name>              Segment Name From proc maps\n");
	printf("--Manual Dump Args-----------------------------------------------------------------------\n");
	printf("  -m --manual                           Manual Dump Mode for Custom Address\n");
	printf("  -n --name <dump_name>                 Dumping File Name\n");
	printf("  -s --start <address>                  Starting Address\n");
	printf("  -e --end <address>                    Ending Address\n");
	printf("--Lib Dump Args-------------------------------------------------------------------------\n");
	printf("  -l --lib                              Dump So(Elf) Library from Memory\n");
	printf("  -n --name <lib_name>                  Library Name From proc maps\n");
	printf("  -r --raw(Optional)                    Output Raw Lib and Not Rebuild It\n");
	printf("--Other Args----------------------------------------------------------------------------\n");
	printf("  -f --fast(Optional)                   Enable Fast Dumping(May Miss Some Bytes in Dump)\n");
	printf("  -p --package <packageName>            Package Name of App\n");
	printf("  -o --output <outputPath>              File Output path(Default: /sdcard)\n");
	printf("  -h --help                             Display this information\n");
}

kaddr getHexAddr(const char* addr){
    auto is16Bit = [](const char* c) {
        auto len = strlen(c);
        if(len > 2) {
            if(c[0] == '0' & c[1] == 'x') return true;
        }
        bool is10bit = true;
        for(auto i = 0; i < len; i++) {
            if((c[i] > 'a' && c[i] < 'f') ||
               (c[i] > 'A' && c[i] < 'F')) {
                is10bit = false;
            }
        }
        return !is10bit;
    };
#ifndef __SO64__
    return (kaddr) strtoul(addr, nullptr, is16Bit(addr) ? 16: 10);
#else
    return (kaddr) strtoull(addr, nullptr, is16Bit(addr) ? 16: 10);
#endif
}

int main(int argc, char *argv[]) {
    int c;
    string pkg, name, outputpath("/sdcard");
    bool isValidArg = true, isManualDump = false, isLibDump = false, isFastDump = false, isRawDump = false;
    kaddr startAddr = 0, endAddr = 0;

    while((c = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
        switch (c) {
            case 'l':
                isLibDump = true;
                break;
            case 'm':
                isManualDump = true;
                break;
            case 'r':
                isRawDump = true;
                break;
            case 'f':
                isFastDump = true;
                break;
            case 'p':
                pkg = optarg;
                break;
            case 'o':
                outputpath = optarg;
                break;
            case 'n':
                name = optarg;
                break;
            case 's':
                startAddr = getHexAddr(optarg);
                break;
            case 'e':
                endAddr = getHexAddr(optarg);
                break;
            default:
                isValidArg = false;
                break;
        }
    }

    if(!isValidArg || name.empty() || outputpath.empty()) {
        printf("Wrong Arguments, Please Check!!\n");
        Usage();
        return -1;
    }

    //Find PID
    target_pid = find_pid(pkg.c_str());
    if (target_pid == -1) {
        cout << "Can't find the process" << endl;
        return -1;
    }
    cout << "Process name: " << pkg.data() << ", Pid: " << target_pid << endl;

    clock_t begin = clock();
    if(isManualDump){
        if(startAddr <= 0 || endAddr <= 0 || endAddr < startAddr){
            cout << "Start or End Address are Wrong!" << endl;
            return -1;
        }

        size_t dump_size = (endAddr - startAddr);
        cout << "Dump Size: " << dump_size << endl;

        ofstream mdump(outputpath + "/" + name, ofstream::out | ofstream::binary);
        if (mdump.is_open()) {
            if (isFastDump) {
                uint8_t *buffer = new uint8_t[dump_size];
                memset(buffer, '\0', dump_size);
                vm_readv((void *) startAddr, buffer, dump_size);
                mdump.write((char *) buffer, dump_size);
            } else {
                char *buffer = new char[1];
                while (dump_size != 0) {
                    vm_readv((void *) (startAddr++), buffer, 1);
                    mdump.write(buffer, 1);
                    --dump_size;
                }
            }
        } else {
            cout << "Error: Can't Output File" << endl;
            return -1;
        }
        mdump.close();
    } else if(isLibDump) {
        //Lib Base Address
        kaddr start_addr = get_module_base(name.c_str());
        if (start_addr == 0) {
            cout << "Can't find Library: " << name.c_str() << endl;
            return -1;
        }

        //Lib End Address
        kaddr end_addr = get_module_end(name.c_str());
        if (end_addr == 0) {
            cout << "Can't find End of Library: " << name.c_str() << endl;
            return -1;
        }

        cout << "Base Address of " << name.c_str() << " Found At " << setbase(16) << start_addr << setbase(10) << endl;
        cout << "End Address of " << name.c_str() << " Found At " << setbase(16) << end_addr << setbase(10) << endl;

        //Lib Dump
        size_t libsize = (end_addr - start_addr);
        cout << "Lib Size: " << libsize << endl;

        if(isRawDump){
            ofstream rdump(outputpath + "/" + name, ofstream::out | ofstream::binary);
            if (rdump.is_open()) {
                if (isFastDump) {
                    uint8_t *buffer = new uint8_t[libsize];
                    memset(buffer, '\0', libsize);
                    vm_readv((void *) start_addr, buffer, libsize);
                    rdump.write((char *) buffer, libsize);
                } else {
                    char *buffer = new char[1];
                    while (libsize != 0) {
                        vm_readv((void *) (start_addr++), buffer, 1);
                        rdump.write(buffer, 1);
                        --libsize;
                    }
                }
            } else {
                cout << "Can't Output File" << endl;
                return -1;
            }
            rdump.close();
        } else {
            ElfReader elf_reader;
            string tempPath = outputpath + "/KTemp.dat";

            ofstream ldump(tempPath, ofstream::out | ofstream::binary);
            if (ldump.is_open()) {
                if (isFastDump) {
                    uint8_t *buffer = new uint8_t[libsize];
                    memset(buffer, '\0', libsize);
                    vm_readv((void *) start_addr, buffer, libsize);
                    ldump.write((char *) buffer, libsize);
                } else {
                    char *buffer = new char[1];
                    while (libsize != 0) {
                        vm_readv((void *) (start_addr++), buffer, 1);
                        ldump.write(buffer, 1);
                        --libsize;
                    }
                }
            } else {
                cout << "Can't Output File" << endl;
                return -1;
            }
            ldump.close();

            //SoFixer Code//
			cout << "Rebuilding Elf(So)" << endl;
			
            elf_reader.setDumpSoFile(true);
            elf_reader.setDumpSoBaseAddr(start_addr);

            auto file = fopen(tempPath.c_str(), "rb");
            if (nullptr == file) {
                printf("source so file cannot found!!!\n");
                return -1;
            }
            auto fd = fileno(file);

            elf_reader.setSource(tempPath.c_str(), fd);

            if (!elf_reader.Load()) {
                printf("source so file is invalid\n");
                return -1;
            }

            ElfRebuilder elf_rebuilder(&elf_reader);
            if (!elf_rebuilder.Rebuild()) {
                printf("error occured in rebuilding elf file\n");
                return -1;
            }
            fclose(file);
            //SoFixer Code//

            ofstream redump(outputpath + "/" + name, ofstream::out | ofstream::binary);
            if (redump.is_open()) {
                redump.write((char*) elf_rebuilder.getRebuildData(), elf_rebuilder.getRebuildSize());
            } else {
                cout << "Can't Output File" << endl;
                return -1;
            }

            remove(tempPath.c_str());
            redump.close();
        }
    } else {
        //Segment Start Address
        kaddr start_addr = get_module_base(name.c_str());
        if (start_addr == 0) {
            cout << "Can't find Segment: " << name.c_str() << endl;
            return -1;
        }

        //Segment End Address
        kaddr end_addr = get_module_end(name.c_str());
        if (end_addr == 0) {
            cout << "Can't find End of Segment: " << name.c_str() << endl;
            return -1;
        }

        cout << "Base Address of " << name.c_str() << " Found At " << setbase(16) << start_addr << setbase(10) << endl;
        cout << "End Address of " << name.c_str() << " Found At " << setbase(16) << end_addr << setbase(10) << endl;

        //Segment Dump
        size_t seg_size = (end_addr - start_addr);
        cout << "Segment Size: " << seg_size << endl;

        ofstream sdump(outputpath + "/" + name, ofstream::out | ofstream::binary);
        if (sdump.is_open()) {
            if (isFastDump) {
                uint8_t *buffer = new uint8_t[seg_size];
                memset(buffer, '\0', seg_size);
                vm_readv((void *) startAddr, buffer, seg_size);
                sdump.write((char *) buffer, seg_size);
            } else {
                char *buffer = new char[1];
                while (seg_size != 0) {
                    vm_readv((void *) (startAddr++), buffer, 1);
                    sdump.write(buffer, 1);
                    --seg_size;
                }
            }
        } else {
            cout << "Error: Can't Output File" << endl;
            return -1;
        }
        sdump.close();
    }
    clock_t end = clock();

    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    printf("Dumped in %fS\n", elapsed_secs);
	return 0;
}
