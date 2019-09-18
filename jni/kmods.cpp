#include <cstdio>
#include <cstdlib>
#include <cstddef>
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
	kaddr start_addr = strtoul(argv[3], nullptr, 16);
	kaddr end_addr = strtoul(argv[4], nullptr, 16);

	libbase = get_module_base(lib_name.c_str());
	if (libbase < 1 && start_addr < 1) {
		printf("Can't find module\n");
		return -1;
	}

	if (end_addr < start_addr) {
		printf("Start Address Should be Smaller\n");
		return -1;
	}

	if (libbase != start_addr) {
		printf("Warning: Start Address is not Module Address\n");
	}

	int size = (end_addr - start_addr);
	printf("----------------------------\n");
	printf("Base Address of %s Found At %lx\n", lib_name.c_str(), libbase);
	printf("Dumping %d Size of Data\n", size);
	uint8* buffer = new uint8[size];
	memset(buffer, '\0', size);
	vm_readv((void*) start_addr, buffer, size);
	ofstream sdk(lib_name + ".dump", ofstream::out | ofstream::binary);
	if (sdk.is_open()) {
		sdk.write((char*)buffer, size);
	}
	sdk.close();
	printf("----------------------------\n");
	return 0;
}






