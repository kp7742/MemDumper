#ifndef MEMORY_H
#define MEMORY_H

#include <dirent.h>
#include "Process.h"

using namespace std;

typedef unsigned long kaddr;

// Unsigned base types.
typedef unsigned char 		uint8;
typedef unsigned short int	uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

// Signed base types.
typedef	signed char			int8;
typedef signed short int	int16;
typedef signed int	 		int32;
typedef signed long long	int64;

// Character types.
typedef char				ANSICHAR;
typedef wchar_t				WIDECHAR;
typedef uint8				CHAR8;
typedef uint16				CHAR16;
typedef uint32				CHAR32;
typedef WIDECHAR			TCHAR;

static kaddr libbase = 0;

pid_t find_pid(const char* process_name) {
	int id;
	pid_t pid = -1;
	DIR* dir;
	FILE* fp;
	char filename[32];
	char cmdline[256];

	struct dirent* entry;
	if (process_name == NULL) {
		return -1;
	}
	dir = opendir("/proc");
	if (dir == NULL) {
		return -1;
	}
	while ((entry = readdir(dir)) != NULL) {
		id = atoi(entry->d_name);
		if (id != 0) {
			sprintf(filename, "/proc/%d/cmdline", id);
			fp = fopen(filename, "r");
			if (fp) {
				fgets(cmdline, sizeof(cmdline), fp);
				fclose(fp);

				if (strcmp(process_name, cmdline) == 0) {
					/* process found */
					pid = id;
					break;
				}
			}
		}
	}

	closedir(dir);
	return pid;
}

kaddr get_module_base(const char *module_name) {
    FILE *fp;
    kaddr addr = 0;
    char filename[32], buffer[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", target_pid);
    fp = fopen(filename, "rt");
    if (fp != nullptr) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strstr(buffer, module_name)) {
                addr = (uintptr_t)strtoul(buffer, nullptr, 16);
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}

kaddr getRealOffset(kaddr offset) {
    if (libbase == 0) {
        LOGW("Error: Can't Find Base Addr for Real Offset");
        return 0;
    }
    return (libbase + offset);
}

template <typename T>
T Read(kaddr address) {
    T data;
    vm_readv(reinterpret_cast<void*>(address), reinterpret_cast<void*>(&data), sizeof(T));
    return data;
}

template <typename T>
void Write(kaddr address, T data) {
    vm_writev(reinterpret_cast<void*>(address), reinterpret_cast<void*>(&data), sizeof(T));
}

template <typename T>
T* ReadArr(kaddr address, unsigned int size) {
    T data[size];
    T* ptr = data;
    vm_readv(reinterpret_cast<void*>(address), reinterpret_cast<void*>(ptr), (sizeof(T) * size));
    return ptr;
}

kaddr getPtr(kaddr address) {
    return Read<kaddr>(address);
}

#endif //MEMORY_H
