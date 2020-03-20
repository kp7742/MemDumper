#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <ctime>
#include <unistd.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Log.h"
#include "Process.h"
#include "Memory.h"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc <= 0) {
		printf("Not Enough Args, quitting\n");
		return 0;
	}

	if (getuid() != 0) {
		printf("Not root, quitting\n");
		return -1;
	}

	target_pid = atoi(argv[1]);
	if (target_pid == -1) {
		printf("Can't find the process\n");
		return -1;
	}
	printf("Pid: %d\n", target_pid);

	string lib_name = string(argv[2]);
	kaddr start_addr = get_module_base(lib_name.c_str());
	if (start_addr < 1) {
		printf("Can't find module\n");
		return -1;
	}
	
	kaddr end_addr = get_module_end(lib_name.c_str());
	if (end_addr < start_addr) {
		printf("Start Address Should be Smaller\n");
		return -1;
	}

	int size = (end_addr - start_addr);
	printf("----------------------------\n");
	printf("Base Address of %s Found At %lx\n", lib_name.c_str(), start_addr);
	printf("End Address of %s Found At %lx\n", lib_name.c_str(), end_addr);
	printf("Dumping %d Size of Data, Please Wait...\n", size);
	
	clock_t begin = clock();
	ofstream dmp(lib_name, ofstream::out | ofstream::binary);
	if (dmp.is_open()) {
		char* buffer = new char[1];
        while(size != 0){
            vm_readv((void*) (start_addr++), buffer, 1);
            dmp.write(buffer, 1);
            --size;
        }
	}
	dmp.close();
	clock_t end = clock();
	
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	printf("Lib Dumped in %fS\n", elapsed_secs);	
	printf("----------------------------\n");
	return 0;
}
