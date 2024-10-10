#include "registers.hpp"

auto Registers::get_PC() const noexcept -> u16
{
    return PC;
}

auto Registers::get_SP() const noexcept -> u16
{
    return SP;
}

auto Registers::set_PC(u16 value) -> void
{
    PC = value;
}

auto Registers::set_SP(u16 value) -> void
{
    SP = value;
}

auto Registers::get_a() const noexcept -> u8
{
    return a;
}
auto Registers::get_b() const noexcept -> u8
{
    return b;
}
auto Registers::get_c() const noexcept -> u8
{
    return c;
}
auto Registers::get_d() const noexcept -> u8
{
    return d;
}
auto Registers::get_e() const noexcept -> u8
{
    return e;
}
auto Registers::get_f() const noexcept -> u8
{
    return f;
}
auto Registers::get_h() const noexcept -> u8
{
    return h;
}
auto Registers::get_l() const noexcept -> u8
{
    return l;
}

auto Registers::set_a(u8 value) -> void
{
    a = value;
}
auto Registers::set_b(u8 value) -> void
{
    b = value;
}
auto Registers::set_c(u8 value) -> void
{
    c = value;
}
auto Registers::set_d(u8 value) -> void
{
    d = value;
}
auto Registers::set_e(u8 value) -> void
{
    e = value;
}
auto Registers::set_f(u8 value) -> void
{
    f = value;
}
auto Registers::set_h(u8 value) -> void
{
    h = value;
}
auto Registers::set_l(u8 value) -> void
{
    l = value;
}

auto Registers::get_AF() const noexcept -> u16
{
    return static_cast<u16>(a) << 8 | f;
}

auto Registers::get_BC() const noexcept -> u16
{
    return static_cast<u16>(b) << 8 | c;
}

auto Registers::get_DE() const noexcept -> u16
{
    return static_cast<u16>(d) << 8 | e;
}

auto Registers::get_HL() const noexcept -> u16
{
    return static_cast<u16>(h) << 8 | l;
}

auto Registers::set_AF(u16 value) -> void
{
    a = static_cast<u8>((value & 0xFF00) >> 8);
    f = static_cast<u8>(value & 0xFF);
}

auto Registers::set_BC(u16 value) -> void
{
    b = static_cast<u8>((value & 0xFF00) >> 8);
    c = static_cast<u8>(value & 0xFF);
}

auto Registers::set_DE(u16 value) -> void
{
    d = static_cast<u8>((value & 0xFF00) >> 8);
    e = static_cast<u8>(value & 0xFF);
}

auto Registers::set_HL(u16 value) -> void
{
    h = static_cast<u8>((value & 0xFF00) >> 8);
    l = static_cast<u8>(value & 0xFF);
}

auto Registers::read_next_byte() -> u8
{
    PC += 1;
    u8 byte = get_bus()->read_byte(PC);
    return byte;
}

auto Registers::read_next_world() -> u16
{
    u8 lower_byte = get_bus()->read_byte(PC);
    u8 upper_byte = get_bus()->read_byte(PC + 1);
    PC += 2;
    return (static_cast<u16>(upper_byte) << 8) | lower_byte;
}

auto Registers::get_register(ArithmeticTarget target) const -> u8
{
    switch (target)
    {
        case ArithmeticTarget::A: return a;
        case ArithmeticTarget::B: return b;
        case ArithmeticTarget::C: return c;
        case ArithmeticTarget::D: return d;
        case ArithmeticTarget::E: return e;
        case ArithmeticTarget::H: return h;
        case ArithmeticTarget::L: return l;
        case ArithmeticTarget::HLI: {
            static u8 temp;
            temp = bus->read_byte(get_HL());
            return temp;
        }
        default: {
            throw runtime_error("Unknown target at get_register: " + to_string(static_cast<u16>(target)));
        }
    }
}

auto Registers::set_register(ArithmeticTarget target, u8 value) -> void
{
    switch (target)
    {
        case ArithmeticTarget::A: a = value; break;
        case ArithmeticTarget::B: b = value; break;
        case ArithmeticTarget::C: c = value; break;
        case ArithmeticTarget::D: d = value; break;
        case ArithmeticTarget::E: e = value; break;
        case ArithmeticTarget::H: h = value; break;
        case ArithmeticTarget::L: l = value; break;
        case ArithmeticTarget::HLI: bus->write_byte(get_HL(), value); break;
        default:
            throw runtime_error("Unknown target at set_register: " + to_string(static_cast<int>(target)));
    }
}

auto Registers::get_register_pair(ArithmeticTarget target) const -> u16
{
    switch (target)
    {
        case ArithmeticTarget::BC: return get_BC();
        case ArithmeticTarget::DE: return get_DE();
        case ArithmeticTarget::HL: return get_HL();
        case ArithmeticTarget::AF: return get_AF();
        case ArithmeticTarget::SP: return get_SP();
        default:
            throw runtime_error("Unknown target at get_register_pair: " + to_string(static_cast<int>(target)));
    }
}

auto Registers::set_register_pair(ArithmeticTarget target, u16 value) -> void
{
    switch (target)
    {
        case ArithmeticTarget::BC: set_BC(value); break;
        case ArithmeticTarget::DE: set_DE(value); break;
        case ArithmeticTarget::HL: set_HL(value); break;
        case ArithmeticTarget::AF: set_AF(value); break;
        default:
            throw runtime_error("Unknown target at set_register_pair: " + to_string(static_cast<int>(target)));
    }
}