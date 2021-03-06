#pragma once

/**
  1. clear() must be called either upon runtime init or with interrupts disabled
  2. rdptr and wrptr are only modified by reader and writer respectively
  3. FIFO always uses 1 byte less than actual buffer size
*/

class FIFO
{
public:
  u8 *addr;
  int size;
  int rdptr;
  int wrptr;
  bool overflow;
  bool underflow;

  void init(void *a, int s)
  {
    addr = (u8*)a;
    size = s;
    clear();
  }

  void clear()
  {
    rdptr = wrptr = 0;
    overflow = underflow = false;
  }

  int free()
  {
    int i = rdptr - wrptr;
    if (i < 1) i += size;
    return i - 1;
  }

  int used()
  {
    return size - free() - 1;
  }

  int free_vol()
  {
    int i = _vol(rdptr) - _vol(wrptr);
    if (i < 1) i += size;
    return i - 1;
  }

  int used_vol()
  {
    return _vol(size) - free_vol() - 1;
  }

  // Puts a byte into FIFO
  void put_byte_nocheck(u8 x)
  {
    *(addr + wrptr) = x;
    wrptr = (wrptr == (size - 1)) ? 0 : (wrptr + 1);
  }
  
  void put_byte(u8 x)
  {
    if (free())
      put_byte_nocheck(x);
    else
      overflow = true;
  }

  // Extracts a byte from FIFO
  u8 get_byte_nocheck()
  {
    u8 c = *(addr + rdptr);
    rdptr = (rdptr == (size - 1)) ? 0 : (rdptr + 1);
    return c;
  }
  
  u8 get_byte()
  {
    if (used())
      return get_byte_nocheck();
    else
      underflow = true;
  }

  // Reads first byte of FIFO without pushing it out
  u8 peek_byte()
  {
    return *(addr + rdptr);
  }

  // Puts a number of bytes into FIFO
  // Makes a check if there's enough place in FIFO
  bool put(void *a, int num)
  {
    if ((size - used()) < num) 
      return false;        // not enough free space

    u8 *buf = (u8*)a;
    while (num--) put_byte_nocheck(*buf++);
    return true;
  }

  // Extracts a number of bytes from FIFO
  // Makes a check if there's enough data in FIFO
  bool get(void *a, int num)
  {
    if (used() < num) 
      return false;        // not enough data in FIFO
    
    u8 *buf = (u8*)a;
    while (num--) *buf++ = get_byte_nocheck();
    return true;
  }
};
