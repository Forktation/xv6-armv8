// Mutual exclusion lock.
struct spinlock {
    uint locked;

    // For debugging:
    char* name;
     // The cpu holding the lock.
    struct cpu* cpu;
    // The call stack (an array of program counters) that locked the lock.
    // Why is it called a call stack, like a pointer or something to the core's actual stack?
    uint pcs[10];
};
