// initialize arduino, will only be able to do 4 bits of address space. 1 bit for CE, OE, WE. 8 bits for data inputs

// Read a hex file 1 byte at a time and set each byte to the data lines

//  function to set address pins, set data pins, sequence to start a write
#include <avr/io.h>
#include <iostream>
#include <fstream>
#include <vector>

// Define symbolic names for pins and ports
#define SIG_PORT PORTC
#define WE PINC0
#define CE PINC1
#define OE PINC2
#define DATA_BUS PORTD
#define ADDR_BUS PORTB

enum PROGRAM_STATUS {
    P_SUCCESS,
    P_FAILURE
};

void clear_pin(volatile uint8_t* port ,volatile uint8_t pin) {
    *port &= ~(1 << pin);
}

void set_pin(volatile uint8_t* port, uint8_t pin) {
    *port |= (1 << pin);
}

int init_ard() {
    DDRC |= (1 << WE) | (1 << CE) | (1 << OE);
    DDRD = 0xFF;  // All pins in PORTD as outputs
    DDRB = 0xFF;  // All pins in PORTB as outputs

    // Set initial states for signal pins
    SIG_PORT |= (1 << WE);
    SIG_PORT |= (1 << CE);
    SIG_PORT |= (1 << OE);

    return P_SUCCESS;
}

int clear_buffer(std::vector<uint8_t>& buffer) {
    // TODO not implemented
    buffer.clear();
    return P_SUCCESS;
}

int make_buffer(std::vector<uint8_t>& buffer, bool clear=true) {
    if (clear) {
        clear_buffer(buffer);
    }
    std::fstream fs;
    fs.open("input.hex", std::fstream::in | std::fstream::binary);

    char byte;
    while (fs >> byte) {
        buffer.push_back(byte);
    }

    return P_SUCCESS;
}

int write_byte(uint8_t addr, uint8_t byte) {
    DATA_BUS = byte; // put the byte on the data lines
    set_pin(&SIG_PORT, OE);  // turn off eeprom output 
    ADDR_BUS = addr;
    clear_pin(&SIG_PORT, CE); // activate chip enable
    clear_pin(&SIG_PORT, WE); // write enable signal
    // need to at least delay 100 ns

    // write sequence is over reset pins
    set_pin(&SIG_PORT, WE); 
    set_pin(&SIG_PORT, CE);
    clear_pin(&SIG_PORT, OE);

    return P_SUCCESS;
}


int write_buffer(std::vector<uint8_t>& buffer) {
    for (uint8_t byte : buffer) {
        write_byte(byte)
    }
}