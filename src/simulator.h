#ifndef SIMULATOR_H
#define SIMULATOR_H

// initialize simulation
extern void sim_start(int B, int S, int W);

// access one memory block
extern void sim_access(long long acc);

// return the number of misses
extern int sim_finish(void);

#endif