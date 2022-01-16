#ifndef __JOBS_HEADER__
#define __JOBS_HEADER__

void handler(int signum);
void child_handler(int signum, siginfo_t *siginfo, void* unused);
void set_mask(void);

#endif
