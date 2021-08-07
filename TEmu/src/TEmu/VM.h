#ifndef _TEVM_H_
#define _TEVM_H_

namespace TEmu {
	template<typename DATA_TYPE>
	class VM {
	protected:
		DATA_TYPE* memory;
		DATA_TYPE* reg;

		DATA_TYPE pc; //Program Counter

		void loadBuffer(const unsigned int start, char* buffer, unsigned int size);
	public:
		VM(int ram, int regCount);
		~VM();
	};
}

#endif // !_TEVM_H_
