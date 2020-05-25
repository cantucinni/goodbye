#include <iostream>

typedef unsigned char byte;
typedef unsigned short word;

#define INTERNAL_MEMORY_SIZE 0x10000
#define INTERNAL_STACK_SIZE 0x1000
#define MESSAGE_OFFSET_END 666

#define PUSH_W 0x00
#define PUSH_WPTR 0x01
#define POP_WPTR 0x02
#define ADD 0x03
#define CMP_JEQ 0x04
#define XOR 0x05
#define AND 0x06
#define OR 0x07
#define NEG 0x09
#define NOP 0xcc
#define END 0xff

class CStackVM
{
public:
	void SetEIP(word ip);
	void LoadToMemory(byte *pbData, word uCnt);
	bool Execute();
	CStackVM();

private:
	byte bInternalMemory[INTERNAL_MEMORY_SIZE];
	word wStack[INTERNAL_STACK_SIZE];
	word eip;
	word sp;

};

bool CStackVM::Execute()
{
	switch (bInternalMemory[this->eip++])
	{
	case PUSH_W:
		this->sp--;
		wStack[this->sp] = *(word*)&bInternalMemory[this->eip];
		this->eip += 2;
		break;
	case PUSH_WPTR:
		this->sp--;
		wStack[this->sp] = *(word*)&bInternalMemory[*(word*)&bInternalMemory[this->eip]];
		this->eip += 2;
		break;
	case POP_WPTR:
		*(word*)&bInternalMemory[*(word*)&bInternalMemory[this->eip]] = wStack[this->sp];
		this->sp++;
		this->eip += 2;
		break;
	case ADD:
		wStack[this->sp + 1] = wStack[this->sp + 1] + wStack[this->sp];
		wStack[this->sp] = 0xffff;
		this->sp++;
		break;
	case XOR:
		wStack[this->sp + 1] = wStack[this->sp + 1] ^ wStack[this->sp];
		wStack[this->sp] = 0xffff;
		this->sp++;
		break;
	case AND:
		wStack[this->sp + 1] = wStack[this->sp + 1] & wStack[this->sp];
		wStack[this->sp] = 0xffff;
		this->sp++;
		break;
	case OR:
		wStack[this->sp + 1] = wStack[this->sp + 1] | wStack[this->sp];
		wStack[this->sp] = 0xffff;
		this->sp++;
		break;
	case NEG:
		wStack[this->sp] = wStack[this->sp] * -1;
		break;
	case CMP_JEQ:
		if (wStack[this->sp] == wStack[this->sp + 1])
		{
			word wTmp = *(word*)&bInternalMemory[this->eip];
			if (wTmp & 0x8000)
				this->eip -= wTmp;
			else
				this->eip += wTmp;
			this->eip--;
		}
		else
		{
			this->eip += 2;
		}
		this->sp += 2;
		break;
	case NOP:
		break;

	case END:
		return false;

	default:
		return false;
	}

	return true;
}

CStackVM::CStackVM()
{
	this->eip = 0;
	this->sp = INTERNAL_STACK_SIZE - 1;
	memset(wStack, 0xff, sizeof(wStack));
	memset(bInternalMemory, 0xff, sizeof(bInternalMemory));
}

void CStackVM::SetEIP(word eip)
{
	this->eip = eip;
}

void CStackVM::LoadToMemory(byte *pbData, word uCnt)
{
	memcpy_s(bInternalMemory, INTERNAL_MEMORY_SIZE, pbData, uCnt);
}

#define W(x) x & 0xff, (x) >> 8

int main()
{
	static const byte cInstructions[] = {

		PUSH_W, W(0),
		PUSH_W, W(0),
		CMP_JEQ, W(7),
		0x13, 0x37, 0xc0, 0xde,
		NOP,
		PUSH_W, W(0x1234),
		PUSH_WPTR, W(0x9),
		PUSH_W, W(0x2600),
		ADD,
		ADD,
		POP_WPTR, W(MESSAGE_OFFSET_END),
		NOP,
		PUSH_WPTR, W(0xb),
		PUSH_W, W(0xb0a9),
		XOR,
		POP_WPTR, W(MESSAGE_OFFSET_END + 2),
		NOP,
		PUSH_W, W(0x2167),
		PUSH_W, W(0x32cf),
		AND,
		POP_WPTR, W(MESSAGE_OFFSET_END + 4),
		NOP,
		PUSH_W, W(0x6050),
		PUSH_W, W(0x0f04),
		OR,
		POP_WPTR, W(MESSAGE_OFFSET_END + 6),
		NOP,
		PUSH_WPTR, W(0xb),
		PUSH_W, W(0x93e0),
		XOR,
		POP_WPTR, W(MESSAGE_OFFSET_END + 8),
		NOP,
		PUSH_W, W(0x1337),
		PUSH_W, W(0xc0de),
		AND,
		PUSH_W, W(0xacad),
		NEG,
		ADD,
		POP_WPTR, W(MESSAGE_OFFSET_END + 10),
		NOP,
		PUSH_W, W(0x8fcc),
		PUSH_W, W(0x6f79),
		NEG,
		ADD,
		POP_WPTR, W(MESSAGE_OFFSET_END + 12),
		NOP,
		PUSH_W, W(0x6f78),
		PUSH_W, W(0x0001),
		ADD,
		POP_WPTR, W(MESSAGE_OFFSET_END + 14),
		NOP,
		PUSH_W, W(0xfffe),
		PUSH_W, W(0xffff),
		CMP_JEQ, W(0x5),
		PUSH_W, W(0xadde),
		PUSH_WPTR, W(0x9),
		XOR,
		PUSH_W, W(0x85a8),
		ADD,
		POP_WPTR, W(MESSAGE_OFFSET_END + 16),
		NOP,
		PUSH_W, W(0xefbe),
		PUSH_WPTR, W(0xb),
		ADD,
		PUSH_W, W(0x9de3),
		ADD,
		POP_WPTR, W(MESSAGE_OFFSET_END + 18),
		NOP,
		PUSH_W, W(0x6969),
		PUSH_W, W(6969),
		PUSH_W, W(0x6336),
		NEG,
		ADD,
		ADD,
		POP_WPTR, W(MESSAGE_OFFSET_END + 20),
		PUSH_W, W(0xffff),
		POP_WPTR, W(MESSAGE_OFFSET_END + 20),
		NOP,
		END
	};

	CStackVM c;
	c.LoadToMemory((byte *)cInstructions, _countof(cInstructions));
	c.SetEIP(0);
	while (c.Execute());
	
	return 0;
}