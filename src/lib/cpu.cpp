#include "cpu.hpp"

auto CPU::execute(const Instruction& instruction) -> u8
{
    bool jump_condition = false;
    u8 cycle = 0;
    
    auto arithmetic_op = [&](auto reg, auto func)
    {
        reg = func(reg);
    };

    auto push_inst = [&](u16 value)
    {
        u16 sp = registers->get_SP();
        registers->set_SP(sp - 2);

        registers->get_bus()->write_byte(registers->get_SP() + 1, static_cast<u8>((value >> 8) & 0xFF));
        registers->get_bus()->write_byte(registers->get_SP(), static_cast<u8>(value & 0xFF));
    };

    auto pop_inst = [&]() -> u16 {
        // u16 value = static_cast<u16>(registers->get_bus()->read_byte(SP)) | (static_cast<u16>(registers->get_bus()->read_byte(SP + 1)) << 8);
        u16 value = static_cast<u16>(registers->get_bus()->read_byte(registers->get_SP())) |
                    (static_cast<u16>(registers->get_bus()->read_byte(registers->get_SP() + 1)) << 8);
        // SP += 2;
        u16 sp = registers->get_SP();
        registers->set_SP(sp + 2);
        return value;
    };

    switch (instruction.get_inst_type())
    {
        case InstructionType::ADD:
            arithmetic_op(registers->get_register(instruction.get_arithmetic_target()), [this](u8 value) { return inst->add_inst(value); });
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::ADDHL:
            u16 hl_value;
            hl_value = registers->get_HL();
            arithmetic_op(hl_value, [this](u16 value) { return inst->addhl_inst(value); });
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::ADC:
            arithmetic_op(registers->get_register(instruction.get_arithmetic_target()), [this](u8 value) { return inst->adc_inst(value); });
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SUB:
            arithmetic_op(registers->get_register(instruction.get_arithmetic_target()), [this](u8 value) { return inst->sub_inst(value); });
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SBC:
            arithmetic_op(registers->get_register(instruction.get_arithmetic_target()), [this](u8 value) { return inst->sbc_inst(value); });
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::AND:
            arithmetic_op(registers->get_register(instruction.get_arithmetic_target()), [this](u8 value) { return inst->and_inst(value); });
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::OR:
            arithmetic_op(registers->get_register(instruction.get_arithmetic_target()), [this](u8 value) { return inst->or_inst(value); });
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::XOR:
            arithmetic_op(registers->get_register(instruction.get_arithmetic_target()), [this](u8 value) { return inst->xor_inst(value); });
            // cout << "Zero flag after XOR case: " << boolalpha << registers->get_flag()->zero << endl;
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::CP:
            inst->cp_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::INC:
            if (instruction_byte == 0x03 || instruction_byte == 0x13 ||
                instruction_byte == 0x23 || instruction_byte == 0x33)
            {
                switch (instruction_byte)
                {
                    case 0x03:
                        registers->set_BC(registers->get_BC() + 1);
                        break;

                    case 0x13:
                        registers->set_DE(registers->get_DE() + 1);
                        break;

                    case 0x23:
                        registers->set_HL(registers->get_HL() + 1);
                        break;

                    case 0x33:
                        registers->set_SP(registers->get_SP() + 1);
                        break;

                    default:
                        throw runtime_error("Unkown case in INC(CPU): " + to_string(static_cast<int>(instruction_byte)));
                }
            }
            else
            {
                inst->inc_inst(registers->get_register(instruction.get_arithmetic_target()));
            }
            cycle = inst->get_cycle_value();
            return cycle;
        
        case InstructionType::DEC:
            if (instruction_byte == 0x0B || instruction_byte == 0x1B ||
                instruction_byte == 0x2B || instruction_byte == 0x3B)
            {
                switch (instruction_byte)
                {
                    case 0x03:
                        registers->set_BC(registers->get_BC() - 1);
                        break;

                    case 0x13:
                        registers->set_DE(registers->get_DE() - 1);
                        break;

                    case 0x23:
                        registers->set_HL(registers->get_HL() - 1);
                        break;

                    case 0x33:
                        registers->set_SP(registers->get_SP() - 1);
                        break;

                    default:
                        throw runtime_error("Unkown case in DEC(CPU): " + to_string(static_cast<int>(instruction_byte)));
                }
            }
            else
            {
                inst->dec_inst(registers->get_register(instruction.get_arithmetic_target()));
            }
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::JP:
            jump_condition = inst->check_jump_condition(instruction.get_jump_condition());
            inst->jp_inst(jump_condition);
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::JR:
            // cout << "Zero flag before JR case: " << boolalpha << registers->get_flag()->zero << endl;
            jump_condition = inst->check_jump_condition(instruction.get_jump_condition());
            inst->jr_inst(jump_condition);
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::JPI:
            inst->jpi_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::CCF:
            inst->ccf_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SCF:
            inst->scf_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RRA:
            inst->rra_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RLA:
            inst->rla_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RRCA:
            inst->rrca_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RLCA:
            inst->rlca_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::CPL:
            inst->cpl_inst();
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::BIT:
            inst->bit_inst(instruction.bit, registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RES:
            inst->res_inst(instruction.bit, registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SET:
            inst->set_inst(instruction.bit, registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SRL:
            inst->srl_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RR:
            inst->rr_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RL:
            inst->rl_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RRC:
            inst->rrc_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RLC:
            inst->rlc_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SRA:
            inst->sra_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SLA:
            inst->sla_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::SWAP:
            inst->swap_inst(registers->get_register(instruction.get_arithmetic_target()));
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::LD:
            inst->ld_inst(instruction);
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::PUSH:
            push_inst(registers->get_register_pair(instruction.get_arithmetic_target()));
            return cycle;

        case InstructionType::POP:
            registers->set_register_pair(instruction.get_arithmetic_target(), pop_inst());
            return cycle;

        case InstructionType::CALL:
            jump_condition = inst->check_jump_condition(instruction.get_jump_condition());
            if (jump_condition)
            {
                push_inst(registers->get_PC() + 2);
                registers->set_PC(registers->read_next_world());
            }
            else
            {
                registers->set_PC(registers->get_PC() + 3);
            }
            return cycle;

        case InstructionType::RET:
            jump_condition = inst->check_jump_condition(instruction.get_jump_condition());
            if (jump_condition)
            {
                registers->set_PC(pop_inst());
            }
            else
            {
                registers->set_PC(registers->get_PC() + 1);
            }
            return cycle;

        case InstructionType::NOP:
            registers->set_PC(registers->get_PC() + 1);
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::HALT:
            registers->set_PC(registers->get_PC() + 1);
            // this->is_halted = true;
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::EI:
            inst->IME = 0;
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::DI:
            inst->IME = 1;
            cycle = inst->get_cycle_value();
            return cycle;

        case InstructionType::RST: {
            u16 return_address = registers->get_PC() + 1;
            push_inst(return_address);
            switch (instruction_byte)
            {
                case 0xCF:
                    registers->set_PC(0x0008);
                    registers->set_PC(registers->get_PC() - 1);     //Prevent inc in cpu step
                    break;
                    
                case 0xDF:
                    registers->set_PC(0x0018);
                    registers->set_PC(registers->get_PC() - 1);     //Prevent inc in cpu step
                    break;
                
                case 0xEF:
                    registers->set_PC(0x0028);
                    registers->set_PC(registers->get_PC() - 1);     //Prevent inc in cpu step
                    break;
                    
                case 0xFF:
                    registers->set_PC(0x0038);
                    registers->set_PC(registers->get_PC() - 1);     //Prevent inc in cpu step
                    break;
            
                default:
                    throw runtime_error("Unknown instruction_byte found at step: 0x" + to_string(instruction_byte));
                    break;
            }
            cycle = inst->get_cycle_value();
            return cycle;
        }

        default:
            cerr << "Unknown instruction at execute" << endl;
            cerr << "Instruction type: " << static_cast<u16>(instruction.get_inst_type()) << endl;
            cerr << "PC: " << hex << registers->get_PC() << dec << endl;
            cerr << "SP: " << hex << registers->get_SP() << dec << endl;
            throw runtime_error("Unknown instruction at execute");
            break;
    }
}

auto CPU::step() -> void {
    u8 cycle = 0;
    instruction_byte = registers->get_bus()->read_byte(registers->get_PC());
    bool prefixed = (instruction_byte == 0xCB);
    if (prefixed)
    {
        instruction_byte = registers->get_bus()->read_byte(registers->get_PC() + 1);
    }

    if (optional<Instruction> instruction = Instruction::from_byte(instruction_byte, prefixed))
    {
        log_state("Before execute", instruction_byte, prefixed);
        cycle = execute(*instruction);
        registers->set_PC(registers->get_PC() + 1);
        log_state("After execute", instruction_byte, prefixed);

    }
    else
    {
        throw runtime_error("Unknown instruction found at step: 0x" + to_string(registers->get_PC()));
    }

    //Implement cycles in cpu(step)
    timer(cycle);
    interrupts();    

    if(registers->get_PC() > 256)
    {
        exit(0);
    }
}

auto CPU::timer(u8 cycle) -> void
{
    //Update DIV register (increment every 256 cycles)
    div_clocksum += cycle;
    if (div_clocksum >= 256)
    {
        div_clocksum -= 256;
        registers->get_bus()->write_byte(registers->get_bus()->read_byte(0xFF04), 1);
    }

    //Check if timer is enabled (TAC bit 2)
    if ((registers->get_bus()->read_byte(0xFF07) >> 2) & 0x1)
    {
        timer_clocksum += cycle * 4;

        // u32 freq_lut[4] = {4096, 262144, 65536, 16384}; // Should be
        u32 freq_lut[4] = {262144, 4096, 65536, 16384};
        u32 freq = freq_lut[registers->get_bus()->read_byte(0xFF07) & 3];

        //Increment TIMA when enough cycles have passed
        while (timer_clocksum >= (4194304 / freq))
        {
            u8 tima = registers->get_bus()->read_byte(0xFF05);
            registers->get_bus()->write_byte(0xFF05, tima + 1);

            //Check for overflow (TIMA == 0 after increment)
            if (registers->get_bus()->read_byte(0xFF05) == 0)
            {
                //Trigger Timer Overflow interrupt
                registers->get_bus()->write_byte(0xFF0F, registers->get_bus()->read_byte(0xFF0F) | 4);
                //Reload TIMA from TMA
                registers->get_bus()->write_byte(0xFF05, registers->get_bus()->read_byte(0xFF06));
            }
            timer_clocksum -= (4194304 / freq);
        }
    }
}

auto CPU::interrupts() -> void
{
    if (inst->IME)
    {
        u8 IE = registers->get_bus()->read_byte(0xFFFF);
        u8 IF = registers->get_bus()->read_byte(0xFF0F);
        u8 pending_interrupts = IE & IF;

        if (pending_interrupts)
        {
            //V-Blank interrupt (Bit 0)
            if (pending_interrupts & (1 << 0))
            {
                //Push PC to stack
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() >> 8);
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() & 0xFF);

                //Set PC to V-Blank interrupt vector
                registers->set_PC(0x0040);

                //Clear the V-Blank interrupt flag in IF
                registers->get_bus()->write_byte(0xFF0F, IF & ~(1 << 0));
                inst->IME = 0;
                return;
            }

            //LCD STAT interrupt (Bit 1)
            if (pending_interrupts & (1 << 1)) 
            {
                //Push PC to stack
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() >> 8);
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() & 0xFF);

                //Set PC to LCD STAT interrupt vector
                registers->set_PC(0x0048);

                //Clear the LCD STAT interrupt flag in IF
                registers->get_bus()->write_byte(0xFF0F, IF & ~(1 << 1));
                inst->IME = 0;
                return;
            }

            //Timer Overflow interrupt (Bit 2)
            if (pending_interrupts & (1 << 2)) 
            {
                //Push PC to stack
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() >> 8);
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() & 0xFF);

                //Set PC to Timer Overflow interrupt vector
                registers->set_PC(0x0050);

                //Clear the Timer Overflow interrupt flag in IF
                registers->get_bus()->write_byte(0xFF0F, IF & ~(1 << 2));
                inst->IME = 0;
                return;
            }

            //Serial Transfer Completion interrupt (Bit 3)
            if (pending_interrupts & (1 << 3)) 
            {
                //Push PC to stack
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() >> 8);
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() & 0xFF);

                //Set PC to Serial Transfer Completion interrupt vector
                registers->set_PC(0x0058);

                //Clear the Serial Transfer Completion interrupt flag in IF
                registers->get_bus()->write_byte(0xFF0F, IF & ~(1 << 3));
                inst->IME = 0;
                return;
            }

            //Joypad Input interrupt (Bit 4)
            if (pending_interrupts & (1 << 4)) 
            {
                //Push PC to stack
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() >> 8);
                registers->set_SP(registers->get_SP() - 1);
                registers->get_bus()->write_byte(registers->get_SP(), registers->get_PC() & 0xFF);

                //Set PC to Joypad interrupt vector
                registers->set_PC(0x0060);

                //Clear the Joypad interrupt flag in IF
                registers->get_bus()->write_byte(0xFF0F, IF & ~(1 << 4));
                inst->IME = 0;
                return;
            }
        }
    }
}

auto CPU::log_state(const string &stage, u8 instruction_byte, bool prefixed) -> void
{
    //Debug output
    cout << "[" << stage << "] PC: 0x" << hex << setw(4) << setfill('0') << static_cast<u16>(registers->get_PC())
         << " | SP: 0x" << hex << setw(4) << setfill('0') << static_cast<u16>(registers->get_SP())
         << " | Instruction: 0x" << hex << setw(2) << setfill('0') << static_cast<u16>(instruction_byte)
         << " | Prefix: 0x" << hex << setw(1) << setfill('0') << static_cast<u16>(prefixed)
         << std::endl;

    // cout << "Memory write to: " << address << " | Value: " << value << endl;

    cout << "Registers: A = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_a())
         << ", F = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_f())
         << ", B = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_b())
         << ", C = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_c())
         << ", D = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_d())
         << ", E = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_e())
         << ", H = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_h())
         << ", L = " << hex << setw(2) << setfill('0') << static_cast<u16>(registers->get_l())
         << " | Zero: " << registers->get_flag()->zero
         << " | Subtract: " << registers->get_flag()->subtract
         << " | Half-Carry: " << registers->get_flag()->half_carry
         << " | Carry: " << registers->get_flag()->carry
         << std::endl;
    //Debug output
}