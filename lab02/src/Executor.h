
#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H

#include "Instruction.h"

class Executor
{
public:
    void Execute(InstructionPtr& instr, Word ip)
    {
        Word result;

        Word f_operand = instr->_src1Val;

        Word s_operand;

        if (instr->_imm)
        {
            s_operand = instr->_imm.value();
        }
        else
        {
            s_operand = instr->_src2Val;
        }

        switch(instr->_aluFunc)
        {
            case AluFunc::Add:

                result = f_operand + s_operand;

                break;

            case AluFunc::Sub:

                result = f_operand - s_operand;

                break;

            case AluFunc::And:

                result = f_operand & s_operand;

                break;

            case AluFunc::Or:

                result = f_operand | s_operand;

                break;

            case AluFunc::Xor:

                result = f_operand ^ s_operand;

                break;

            case AluFunc::Slt:

                result = (int)f_operand < (int)s_operand;

                break;

            case AluFunc::Sltu:

                result = f_operand < s_operand;

                break;

            case AluFunc::Sll:

                result = f_operand << (s_operand % 32);

                break;

            case AluFunc::Srl:

                result = f_operand >> (s_operand % 32);

                break;

            case AluFunc::Sra:

                result = (int)f_operand >> (s_operand % 32);

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

        bool br = false;

        switch (instr->_brFunc)
        {
            case BrFunc::Eq:

                br = instr->_src1Val == instr->_src2Val;

                break;

            case BrFunc::Neq:

                br = instr->_src1Val != instr->_src2Val;

                break;

            case BrFunc::Lt:

                br = (int)instr->_src1Val < (int)instr->_src2Val;

                break;

            case BrFunc::Ltu:

                br = instr->_src1Val < instr->_src2Val;

                break;

            case BrFunc::Ge:

                br = (int)instr->_src1Val >= (int)instr->_src2Val;

                break;

            case BrFunc::Geu:

                br = instr->_src1Val >= instr->_src2Val;

                break;

            case BrFunc::AT:
                br = true;
                break;
            case BrFunc::NT:
                br = false;
                break;
        }

        if ((instr->_type == IType::Br || instr->_type == IType::J) && br)
        {
            instr->_nextIp = ip + instr->_imm.value();
        }
        else if (instr->_type == IType::Jr && br)
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