#include "PointerRingBuffer.h"

PointerRingBuffer::PointerRingBuffer(int p_size)
{
    size = p_size + 1;
    buf = new char[size];
    addr_w = 0;
    addr_r = 0;
}

PointerRingBuffer::~PointerRingBuffer()
{
    delete [] buf;
}

int PointerRingBuffer::put(char dat)
{
    int next;

    next = (addr_w + 1) % size;
    if (next == addr_r) {
        return -1;
    }
    buf[addr_w] = dat;
    addr_w = next;
    return -1;
}

int PointerRingBuffer::put(char *dat, int len)
{
    int next, i;


    for (i = 0; i < len; i ++) {
        next = (addr_w + 1) % size;
        if (next == addr_r) {
            break;
        }
        buf[addr_w] = dat[i];
        addr_w = next;
    }
    return i;
}

int PointerRingBuffer::get(char *dat)
{
    if (addr_r == addr_w) {
        return -1;
    }
    *dat = buf[addr_r];
    addr_r = (addr_r + 1) % size;
    return 0;
}

int PointerRingBuffer::get(char *dat, int len)
{
    int i;

    for (i = 0; i < len; i ++) {
        if (addr_r == addr_w) {
            break;
        }
        dat[i] = buf[addr_r];
        addr_r = (addr_r + 1) % size;
    }
    return i;
}

int PointerRingBuffer::available()
{
    if (addr_w < addr_r) {
        return addr_r - addr_w - 1;
    } else {
        return (size - addr_w) + addr_r - 1;
    }
}

int PointerRingBuffer::use()
{
    return size - available() - 1;
}

void PointerRingBuffer::clear()
{
    addr_w = 0;
    addr_r = 0;
}
