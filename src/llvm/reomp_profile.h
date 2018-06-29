#ifndef REOMP_PROFILE_H_
#define REOMP_PROFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <omp.h>
#include <mpi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/limits.h>

#include <unordered_map>

#define REOMP_PROFILE_ENABLE
#ifdef REOMP_PROFILE_ENABLE
#define REOMP_PROFILE(profile_func)	    \
  do {					    \
    profile_func;			    \
  } while(0)
#else
#define REOMP_PROFILE(profile_func)
#endif

using namespace std;

typedef struct {
  unordered_map<size_t, size_t> *rr_type_umap;
  unordered_map<size_t, size_t> *lock_id_to_pre_rr_type_umap;
  size_t load_seq_count = 0;
  size_t store_seq_count = 0;
} reomp_profile_t;

void reomp_profile_init();
void reomp_profile_rr_type(size_t rr_type);
void reomp_profile_rr_rw_seq(size_t rr_type, size_t lock_id);
void reomp_profile_print();
void reomp_profile_finalize();

#endif

