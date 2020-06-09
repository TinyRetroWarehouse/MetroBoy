#include "Serial.h"
#include "Constants.h"
#include <assert.h>

//-----------------------------------------------------------------------------

void Serial::reset() {
  sb = 0x00; // FF01
  sc = 0x7E; // FF02
}

//-----------------------------------------------------------------------------

void Serial::tock(int phase, const Req& req) {
  if (req.write) {
    if (req.addr == ADDR_SB) sb = (uint8_t)req.data;
    if (req.addr == ADDR_SC) sc = (uint8_t)req.data | 0b01111110;
  }
}

void Serial::tick(int phase, const Req& req, Ack& ack) const {
  if (req.read && ((req.addr == ADDR_SB) || (req.addr == ADDR_SC))) {
    ack.addr = req.addr;
    ack.data = req.addr == ADDR_SB ? sb : sc;
    ack.read++;
  }
}

//-----------------------------------------------------------------------------

void Serial::dump(std::string& d) {
  sprintf(d, "\002--------------SERIAL-----------\001\n");
  sprintf(d, "SB 0x%02x\n", sb);
  sprintf(d, "SC 0x%02x\n", sc);
}

//-----------------------------------------------------------------------------
