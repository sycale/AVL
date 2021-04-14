
#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H

#include "Instruction.h"

class Executor
{
public:
    void Execute(InstructionPtr& instr, Word ip)
    {
        Word result;
        Word first_operand = instr->_src1Val;
        Word second_operand;
        
        if (instr->_imm)
        {
            second_operand = instr->_imm.value();
        }
        else
        {
            second_operand = instr->_src2Val;
        }

        switch(instr->_aluFunc)
        {
            case AluFunc::Add:
                result = first_operand + second_operand;
                break;
            case AluFunc::Sub:
                result = first_operand - second_operand;
                break;
            case AluFunc::And:
                result = first_operand & second_operand;
                break;
            case AluFunc::Or:
                result = first_operand | second_operand;
                break;
            case AluFunc::Xor:
                result = first_operand ^ second_operand;
                break;
            case AluFunc::Slt:
                result = (int)first_operand < (int)second_operand;
                break;
            case AluFunc::Sltu:
                result = first_operand < second_operand;
                break;
            case AluFunc::Sll:
                result = first_operand << (second_operand % 32);
                break;
            case AluFunc::Srl:
                result = first_operand >> (second_operand % 32);
                break;
            case AluFunc::Sra:
                result = (int)first_operand >> (second_operand % 32);
                break;
        }

        switch(instr->_type)
        {
            case IType::Csrr:
                instr->_data = instr->_csrVal;
                break;
            case IType::Csrw:
                instr->_data = instr->_src1Val;
                break;
            case IType::St:
                instr->_data = instr->_src2Val;
                instr->_addr = result;
                break;
            case IType::J:
            case IType::Jr:
                instr->_data = ip + 4;
                break;
            case IType::Auipc:
                instr->_data = ip + instr->_imm.value();
                break;
            case IType::Ld:
                instr->_addr = result;
                break;
            default:
                instr->_data = result;
                break;
        }

        bool branch = false;

        switch (instr->_brFunc)
        {
            case BrFunc::Eq:
                branch = instr->_src1Val == instr->_src2Val;
                break;
            case BrFunc::Neq:
                branch = instr->_src1Val != instr->_src2Val;
                break;
            case BrFunc::Lt:
                branch = (int)instr->_src1Val < (int)instr->_src2Val;
                break;
            case BrFunc::Ltu:
                branch = instr->_src1Val < instr->_src2Val;
                break;
            case BrFunc::Ge:
                branch = (int)instr->_src1Val >= (int)instr->_src2Val;
                break;
            case BrFunc::Geu:
                branch = instr->_src1Val >= instr->_src2Val;
                break;
            case BrFunc::AT:
                branch = true;
                break;
            case BrFunc::NT:
                branch = false;
                break;
        }

        if ((instr->_type == IType::Br || instr->_type == IType::J) && branch)
        {
            instr->_nextIp = ip + instr->_imm.value();
        }
        else if (instr->_type == IType::Jr && branch)
        {
            instr->_nextIp = instr->_imm.value() + instr->_src1Val;
        }
        else
        {
            instr->_nextIp = ip + 4;
        }
    }
};

#endif // RISCV_SIM_EXECUTOR_H