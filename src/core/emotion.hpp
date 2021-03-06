#ifndef EMOTION_HPP
#define EMOTION_HPP
#include <cstdint>
#include "cop0.hpp"
#include "cop1.hpp"

class Emulator;
class BIOS_HLE;

class EmotionEngine
{
    private:
        BIOS_HLE* bios;
        Emulator* e;

        Cop0 cp0;
        Cop1 fpu;

        //Each register is 128-bit
        uint8_t gpr[32 * sizeof(uint64_t) * 2];
        uint32_t PC, new_PC;
        uint64_t LO, LO1, HI, HI1;
        uint64_t SA;

        bool int1_signal;

        bool increment_PC;
        bool branch_on;
        bool can_disassemble;
        int delay_slot;

        uint8_t scratchpad[1024 * 16];

        uint32_t get_paddr(uint32_t vaddr);
        void handle_exception(uint32_t new_addr, uint8_t code);
    public:
        EmotionEngine(BIOS_HLE* b, Emulator* e);
        static const char* REG(int id);
        void reset();
        void run();
        void print_state();
        template <typename T> T get_gpr(int id, int offset = 0);
        template <typename T> void set_gpr(int id, T value, int offset = 0);
        uint32_t get_PC();
        uint8_t read8(uint32_t address);
        uint16_t read16(uint32_t address);
        uint32_t read32(uint32_t address);
        uint64_t read64(uint32_t address);
        //void set_gpr_lo(int index, uint64_t value);

        void set_PC(uint32_t addr);
        void write8(uint32_t address, uint8_t value);
        void write16(uint32_t address, uint16_t value);
        void write32(uint32_t address, uint32_t value);
        void write64(uint32_t address, uint64_t value);

        void jp(uint32_t new_addr);
        void branch(bool condition, int offset);
        void branch_likely(bool condition, int offset);
        void ctc(int cop_id, int reg, int cop_reg);
        void mfc(int cop_id, int reg, int cop_reg);
        void mtc(int cop_id, int reg, int cop_reg);
        void lwc1(uint32_t addr, int index);
        void swc1(uint32_t addr, int index);

        void mfhi(int index);
        void mflo(int index);
        void mfhi1(int index);
        void mflo1(int index);
        void mfsa(int index);
        void set_LO_HI(uint64_t a, uint64_t b, bool hi = false);

        void hle_syscall();
        void syscall_exception();
        void int0();
        void int1();
        void set_int1_signal(bool value);

        void eret();
        void ei();
        void di();

        void fpu_cop_s(uint32_t instruction);
        void fpu_bc1(int32_t offset, bool test_true, bool likely);
        void fpu_cvt_s_w(int dest, int source);
};

template <typename T>
inline T EmotionEngine::get_gpr(int id, int offset)
{
    return *(T*)&gpr[(id * sizeof(uint64_t) * 2) + (offset * sizeof(T))];
}

template <typename T>
inline void EmotionEngine::set_gpr(int id, T value, int offset)
{
    if (id)
        *(T*)&gpr[(id * sizeof(uint64_t) * 2) + (offset * sizeof(T))] = value;
}

#endif // EMOTION_HPP
