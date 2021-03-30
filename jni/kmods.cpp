#include "kmods.h"

using namespace std;

const char* short_options = "hlmrfi:p:o:n:s:e:";
const struct option long_options[] = {
		{"help", no_argument, nullptr, 'h'},
		{"lib", no_argument, nullptr, 'l'},
        {"manual", no_argument, nullptr, 'm'},
		{"raw", no_argument, nullptr, 'r'},
		{"fast", no_argument, nullptr, 'f'},
        {"pid", required_argument, nullptr, 'i'},
        {"package", required_argument, nullptr, 'p'},
        {"output", required_argument, nullptr, 'o'},
		{"name", required_argument, nullptr, 'n'},
		{"start", required_argument, nullptr, 's'},
		{"end", required_argument, nullptr, 'e'},
		{nullptr, 0, nullptr, 0}
};

void Usage() {
	printf("MemDumper v0.5 <==> Made By KMODs(kp7742)\n");
	printf("Usage: ./memdumper -p <packageName> <option(s)>\n");
	printf("Dump Memory Segment From Process Memory and Rebuild So(Elf) Libraries\n");
	printf("-l for Library Mode, -m for Manual Dumping Mode, By Default Auto Dumping Mode\n");
    printf("You can use either PID or Package Name, PID given priority over Package Name\n");
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
	printf("  -i --pid <process-id>                 PID of Process\n");
    printf("  -p --package <packageName>            Package Name of App\n");
	printf("  -o --output <outputPath>              File Output path(Default: /sdcard)\n");
	printf("  -h --help                             Display this information\n");
}

kaddr getHexAddr(const char* addr){
#ifndef __SO64__
    return (kaddr) strtoul(addr, nullptr, 16);
#else
    return (kaddr) strtoull(addr, nullptr, 16);
#endif
}

int main(int argc, char *argv[]) {
    int c, pid = -1;
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
            case 'i':
                pid = atoi(optarg);
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
    target_pid = pid > 0 ? pid :find_pid(pkg.c_str());
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
                auto *buffer = new uint8_t[dump_size];
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
                    auto *buffer = new uint8_t[libsize];
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
            string tempPath = outputpath + "/KTemp.dat";

            ofstream ldump(tempPath, ofstream::out | ofstream::binary);
            if (ldump.is_open()) {
                if (isFastDump) {
                    auto *buffer = new uint8_t[libsize];
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

#if defined(__LP64__)
            string outPath = outputpath + "/" + name;

            fix_so(tempPath.c_str(), outPath.c_str(), start_addr);
#else
            ElfReader elf_reader;

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
            redump.close();
#endif
            cout << "Rebuilding Complete" << endl;
            remove(tempPath.c_str());
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
                auto *buffer = new uint8_t[seg_size];
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
