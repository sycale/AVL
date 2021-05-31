
#ifndef RISCV_SIM_CPU_H
#define RISCV_SIM_CPU_H

#include "Memory.h"
#include "Decoder.h"
#include "RegisterFile.h"
#include "CsrFile.h"
#include "Executor.h"

class Cpu
{
public:
    Cpu(Memory& mem)
        : _mem(mem)
    {

    }

    void ProcessInstruction()
    {
        Word с_word = _mem.Request(_ip);
        InstructionPtr i_ptr = _decoder.Decode(с_word);
        _rf.Read(i_ptr);
        _csrf.Read(i_ptr);
        _exe.Execute(i_ptr, _ip);
        _mem.Request(i_ptr);
        _rf.Write(i_ptr);
        _csrf.Write(i_ptr);
        _csrf.InstructionExecuted();
        _ip = i_ptr->_nextIp;
    }

    void Reset(Word ip)
    {
        _csrf.Reset();
        _ip = ip;
    }

    std::optional<CpuToHostData> GetMessage()
    {
        return _csrf.GetMessage();
    }

private:
    Reg32 _ip;
    Decoder _decoder;
    RegisterFile _rf;
    CsrFile _csrf;
    Executor _exe;
    Memory& _mem;
};


#endif //RISCV_SIM_CPU_H