#ifndef KMODS_H
#define KMODS_H

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <dirent.h>
#include <unistd.h>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>

#include "Log.h"
#include "Process.h"
#include "Mem.h"

#if defined(__LP64__)
#include "ELF64/fix.h"
#else
#include "ELF/ElfReader.h"
#include "ELF/ElfRebuilder.h"
#endif

#endif
