#include <iostream>
#include <bitset>

#include "memory_bus.hpp"
#include "uartlite.hpp"
#include "ram.hpp"
#include "rv_core.hpp"
#include "rv_systembus.hpp"
#include "rv_clint.hpp"

int main() {
    rv_systembus system_bus;
    
    uartlite uart;
    rv_clint<1> clint;
    ram dram(4096l*1024l*1024l,"../opensbi/build/platform/generic/firmware/fw_payload.bin");

    assert(system_bus.add_dev(0x60100000,1024*1024,&uart));
    assert(system_bus.add_dev(0x2000000,0x10000,&clint));
    assert(system_bus.add_dev(0x80000000,2048l*1024l*1024l,&dram));

    rv_core rv(system_bus,0);
    rv.jump(0x80000000);
    while (1) {
        clint.tick();
        rv.step(false,clint.m_s_irq(0),clint.m_t_irq(0),uart.irq());
        while (uart.exist_tx()) {
            char c = uart.getc();
            if (c != '\r') std::cout << c;
        }
        std::cout.flush();
    }
    return 0;
}