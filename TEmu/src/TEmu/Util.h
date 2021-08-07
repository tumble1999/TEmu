#ifndef _TE_UTIL_H_
#define _TE_UTIL_H_

#include <cstdint>
#include <fstream>

namespace TEmu {

	struct FileBin {
		char* buffer{};
		std::streampos size;
	};

	FileBin GetFileBin(const char* path) {
		printf("Opening file %s\n", path);
		std::ifstream file(path, std::ios::binary | std::ios::ate);

		if (!file.is_open()) {
			printf("Error opening file %s\n", path);
			exit(1);
		}

		FileBin binary;
		binary.size = file.tellg();
		binary.buffer = new char[binary.size];

		file.seekg(0, std::ios::beg);
		file.read(binary.buffer, binary.size);
		file.close();
		return binary;
	}
}

#endif // !_TE_UTIL_H_