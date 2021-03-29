
#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#include "Instruction.h"
#include ""

bool isKthBitSet(int n, int k)
{
    return n & (1 << (k - 1));
}

class Executor
{
public:
    void Execute(InstructionPtr& instr, Word ip) // _data, _addr, _nextIp / ALU - 
    {
        if(instr._src1Val && instr._src2Val) {
            Word _val1 = instr._src1Val;
            Word _val2 = instr._imm ? instr._imm : instr._src2Val;
            switch (instr._type) {
                case IType::St:
                case IType::Ld:
                    switch (instr._aluFunc): {
                        case AluFunc::Add:
                            instr._addr = _val1 + _val2;
                            break;
                        case AluFunc::Sub:
                            instr._addr = _val1 - _val2;
                            break;
                        case AluFunc::And:
                            instr._addr = _val1 & _val2;
                            break;
                        case AluFunc::Or:
                            instr._addr = _val1 | _val2;
                            break;
                        case AluFunc::Xor:
                            instr._addr = _val1 ^ _val2;
                            break;
                        case AluFunc::Slt:
                            if (CHECK_BIT(_val1, 0) && CHECK_BIT(_val1,0)) {
                                instr._addr = _val1 < _val2 ? _val1 : _val2;
                            }
                            else if (CHECK_BIT(_val1, 0) && !CHECK_BIT(_val2, 0)){
                                instr._addr = _val2;
                            } else if (!CHECK_BIT(_val1, 0) && CHECK_BIT(_val2, 0)) {
                                instr._addr = _val1;
                            }
                            break;
                        case AluFunc::Sltu:
                            instr._addr = (uint)_val1 < (uint)_val2;
                            break;
                        case AluFunc::Sll:
                            instr._addr = _val1  _val2;
                            break;
                        case AluFunc::Srl:
                            instr._addr = _val1  _val2;
                            break;
                        case AluFunc::Sra:
                            instr._addr = _val1  _val2;
                            break;
                        
                    } 
                    default:
                        break;
            }
        }

        switch (instr._type) {
            case IType::Csrr:
                instr._data = instr._csrVal;
                break;
            case IType::Csrw:
                instr._data = instr._src1Val;
                break;
            case IType::St:
                instr._data = instr._src2Val;
                break;
            case IType::j:
            case IType::Jr:
                instr._data = instr._addr + 4;
                break;
            case IType::Auipc:
                instr._data = instr._addr + instr._imm;
                break;
            default:
                break;
        }
    }

private:
    /* YOUR CODE HERE */

};

#endif // RISCV_SIM_EXECUTOR_H
