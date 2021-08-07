#include "VM.h"

using namespace TEmu;

template<typename DATA_TYPE>
void VM<DATA_TYPE>::loadBuffer(const unsigned int start, char* buffer, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++) {
		memory[start + i] = buffer[i];
	}
}

template<typename DATA_TYPE>
VM<DATA_TYPE>::VM(int ram, int regCount)
	:memory(new DATA_TYPE[ram])
	,reg(new DATA_TYPE[regCount])
{
}
template<typename DATA_TYPE>
VM<DATA_TYPE>::~VM()
{
	delete[] memory;
	delete[] reg;
}