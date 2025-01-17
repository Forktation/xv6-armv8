#pragma once

struct buf {
  int        flags;
  uint       dev;
  uint       sector;

  // LRU cache list
  struct buf* prev;
  struct buf* next;
  // disk queue
  struct buf* qnext;
  uchar      data[512];
};

#define B_BUSY  0x1  // buffer is locked by some process
#define B_VALID 0x2  // buffer has been read from disk
#define B_DIRTY 0x4  // buffer needs to be written to disk
