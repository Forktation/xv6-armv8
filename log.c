#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "fs.h"
#include "buf.h"

// Contents of the header block, used for both the on-disk header block
// and to keep track in memory of logged sector #s before commit.
struct logheader {
  int n;
  int sector[LOGSIZE];
};

struct log {
  struct spinlock lock;
  int start;
  int size;
  int busy; // a transaction is active
  int dev;
  struct logheader lh;
};
struct log log;

static void recover_from_log(void);

void initlog(void) {
  struct superblock sb;

  if (sizeof(struct logheader) >= BSIZE) {
    panic("initlog: too big logheader");
  }

  initlock(&log.lock, "log");
  readsb(ROOTDEV, &sb);
  log.start = sb.size - sb.nlog;
  log.size = sb.nlog;
  log.dev = ROOTDEV;
  recover_from_log();
}

// Copy committed blocks from log to their home location
static void install_trans(void) {
  int tail;
  struct buf* lbuf;
  struct buf* dbuf;

  for (tail = 0; tail < log.lh.n; tail++) {
    lbuf = bread(log.dev, log.start + tail + 1); // read log block
    dbuf = bread(log.dev, log.lh.sector[tail]); // read dst

    memmove(dbuf->data, lbuf->data, BSIZE);  // copy block to dst

    bwrite(dbuf);  // write dst to disk
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
static void read_head(void) {
  struct buf* buf;
  struct logheader* lh;
  int i;

  buf = bread(log.dev, log.start);
  lh = (struct logheader*)(buf->data);
  log.lh.n = lh->n;

  for (i = 0; i < log.lh.n; i++) {
    log.lh.sector[i] = lh->sector[i];
  }

  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
static void write_head(void) {
  struct buf* buf;
  struct logheader* hb;
  int i;

  buf = bread(log.dev, log.start);
  hb = (struct logheader*)(buf->data);

  hb->n = log.lh.n;

  for (i = 0; i < log.lh.n; i++) {
    hb->sector[i] = log.lh.sector[i];
  }

  bwrite(buf);
  brelse(buf);
}

static void recover_from_log(void) {
  read_head();
  install_trans(); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

void begin_trans(void) {
  acquire(&log.lock);

  while (log.busy) {
    sleep(&log, &log.lock);
  }

  log.busy = 1;
  release(&log.lock);
}

void commit_trans(void) {
  if (log.lh.n > 0) {
    write_head();    // Write header to disk -- the real commit
    install_trans(); // Now install writes to home locations
    log.lh.n = 0;
    write_head();    // Erase the transaction from the log
  }

  acquire(&log.lock);
  log.busy = 0;
  wakeup(&log);
  release(&log.lock);
}

// Caller has modified b->data and is done with the buffer.
// Append the block to the log and record the block number,
// but don't write the log header (which would commit the write).
// log_write() replaces bwrite(); a typical use is:
//   bp = bread(...)
//   modify bp->data[]
//   log_write(bp)
//   brelse(bp)
void log_write(struct buf* b) {
  struct buf* lbuf;
  int i;

  if (log.lh.n >= LOGSIZE || log.lh.n >= log.size - 1) {
    panic("too big a transaction");
  }

  if (!log.busy) {
    panic("write outside of trans");
  }

  for (i = 0; i < log.lh.n; i++) {
    // log absorption or something
    if (log.lh.sector[i] == b->sector) {
      break;
    }
  }

  log.lh.sector[i] = b->sector;
  lbuf = bread(b->dev, log.start + i + 1);

  memmove(lbuf->data, b->data, BSIZE);
  bwrite(lbuf);
  brelse(lbuf);

  if (i == log.lh.n) {
    log.lh.n++;
  }

  b->flags |= B_DIRTY; // XXX prevent eviction
}

//PAGEBREAK!
// Blank page.

