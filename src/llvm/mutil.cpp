#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>
#include <execinfo.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/syscall.h>

#include <string>
#include <queue>

#include "mutil.h"

#define DEBUG_STDOUT stderr




using namespace std;
int mutil_my_rank = -1;
char mutil_hostname[256];
char header[128];
char message[2048];

static queue<char*> mutil_log_q;
static unsigned long mutil_total_alloc_size = 0;
static unsigned long mutil_total_alloc_count = 0;

static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MUTIL_OUTPUT(stream, label, fmt)        \
  do {                                          \
  va_list argp;                               \
  va_start(argp, fmt);                        \
  sprintf(header, "%s:%s:%s:%3d: ", MUTIL_PREFIX, label, mutil_hostname, mutil_my_rank); \
  vsprintf(message, fmt, argp); \
  va_end(argp);  \
  fprintf(stream, "%s%s\n", header, message);   \
  fflush(stream);				\
  } while(0)


void MUTIL_FUNC(err)(const char* fmt, ...)
{
#ifdef MUTIL_DBG_LOG
  mutil_dbg_log_print();
#endif
  MUTIL_OUTPUT(stderr, "** ERROR **", fmt);
  exit(15);
  return;
}

void MUTIL_FUNC(erri)(int r, const char* fmt, ...)
{
  if (mutil_my_rank != r) return;
  MUTIL_OUTPUT(stderr, "** ERROR **", fmt);
  exit(15);
  return;
}

void MUTIL_FUNC(alert)(const char* fmt, ...)
{
  MUTIL_OUTPUT(stderr, "ALERT", fmt);
  exit(1);
  return;
}

void MUTIL_FUNC(dbg)(const char* fmt, ...) {
  pthread_mutex_lock (&print_mutex);
  MUTIL_OUTPUT(stderr, "DEBUG", fmt);
  pthread_mutex_unlock (&print_mutex);
  return;
}

void MUTIL_FUNC(print)(const char* fmt, ...) {
  MUTIL_OUTPUT(stdout, "", fmt);
  return;
}

void MUTIL_FUNC(dbg_log_print)()
{
  pthread_mutex_lock (&print_mutex);
  fprintf(stdout, "%s:DEBUG:%s:%3d: Debug log dumping ... \n", MUTIL_PREFIX, mutil_hostname, mutil_my_rank);
  fflush(stdout);
  while (!mutil_log_q.empty()) {
    char* log = mutil_log_q.front();
    fprintf(DEBUG_STDOUT, "%s:DEBUG:%s:%3d: %s\n", MUTIL_PREFIX, mutil_hostname, mutil_my_rank, log);
    mutil_log_q.pop();
    free(log);
  }
  pthread_mutex_unlock (&print_mutex);
}

void MUTIL_FUNC(dbgi)(int r, const char* fmt, ...) {
  if (mutil_my_rank != r && -1 != r) return;
  va_list argp;
  pthread_mutex_lock (&print_mutex);

#ifdef MUTIL_DBG_LOG
  char *log;
  if (mutil_log_q.size() > MUTIL_DBG_LOG_BUF_LENGTH) {
    log = mutil_log_q.front();
    mutil_log_q.pop();
  } else {
    log = (char*)MUTIL_FUNC(malloc)(MUTIL_DBG_LOG_BUF_SIZE);
  }

  va_start(argp, fmt);
  vsnprintf(log, MUTIL_DBG_LOG_BUF_SIZE, fmt, argp);
  //  sprintf(log, "%s\0", log);
  va_end(argp);
  mutil_log_q.push(log);

#else
  fprintf(DEBUG_STDOUT, "%s:DEBUG:%s:%3d: ", MUTIL_PREFIX, mutil_hostname, mutil_my_rank);
  va_start(argp, fmt);
  vfprintf(DEBUG_STDOUT, fmt, argp);
  va_end(argp);
  fprintf(DEBUG_STDOUT, "\n");
#endif
  pthread_mutex_unlock (&print_mutex);
}


void MUTIL_FUNC(exit)(int no) {
  fprintf(stderr, "%s:DEBUG:%s:%d: == EXIT == sleep 1sec ...\n", MUTIL_PREFIX, mutil_hostname, mutil_my_rank);
  sleep(1);
  exit(no);
  return;
}





void MUTIL_FUNC(set_configuration)(int *argc, char ***argv)
{

  return;
}


void MUTIL_FUNC(init)(int r) {
  mutil_my_rank = r;
  if (gethostname(mutil_hostname, sizeof(mutil_hostname)) != 0) {
    MUTIL_FUNC(err)("gethostname fialed (%s:%s:%d)", __FILE__, __func__, __LINE__);
  }
  return;
}

char* steam_gethostname() {
  return mutil_hostname;
}


void MUTIL_FUNC(assert)(int b)
{
  assert(b);
  return;
}



// string MUTIL_FUNC(btrace_string)()
// {
//   int j, nptrs;
//   void *buffer[1024];
//   char **strings;
//   string trace_string;

//   nptrs = backtrace(buffer, 1024);

//   /* backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)*/
//   strings = backtrace_symbols(buffer, nptrs);
//   if (strings == NULL) {
//     perror("backtrace_symbols");
//     exit(EXIT_FAILURE);
//   }
//   /*
//     You can translate the address to function name by
//     addr2line -f -e ./a.out <address>
//   */
//   for (j = 0; j < nptrs; j++)
//     trace_string += strings[j] ;
//   free(strings);
//   return trace_string;
// }

void MUTIL_FUNC(btrace)() 
{
  int j, nptrs;
  void *buffer[100];
  char **strings;

  nptrs = backtrace(buffer, 100);

  /* backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)*/
  strings = backtrace_symbols(buffer, nptrs);
  if (strings == NULL) {
    perror("backtrace_symbols");
    exit(EXIT_FAILURE);
  }   

  /*
    You can translate the address to function name by
    addr2line -f -e ./a.out <address>
  */
  for (j = 0; j < nptrs; j++) {
    MUTIL_FUNC(dbg)(" #%d %s", j, strings[j]);
  }
  free(strings);
  return;
}



#if 0
void bt_sighandler(int sig, struct sigcontext ctx) {

  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;

  if (sig == SIGSEGV)
    printf("Got signal %d, faulty address is %p, "
           "from %p\n", sig, ctx.cr2, ctx.eip);
  else
    printf("Got signal %d\n", sig);

  trace_size = backtrace(trace, 16);
  /* overwrite sigaction with caller's address */
  trace[1] = (void *)ctx.eip;
  messages = backtrace_symbols(trace, trace_size);
  /* skip first stack frame (points here) */
  printf("[bt] Execution path:\n");
  for (i=1; i<trace_size; ++i)
    {
      printf("[bt] #%d %s\n", i, messages[i]);

      /* find first occurence of '(' or ' ' in message[i] and assume
       * everything before that is the file name. (Don't go beyond 0 though
       * (string terminator)*/
      size_t p = 0;
      while(messages[i][p] != '(' && messages[i][p] != ' '
            && messages[i][p] != 0)
        ++p;

      char syscom[256];
      sprintf(syscom,"addr2line %p -e %.*s", trace[i], p, messages[i]);
      //last parameter is the file name of the symbol
      system(syscom);
    }

  exit(0);
}
#endif

void MUTIL_FUNC(btracei)(int r) 
{
  if (mutil_my_rank != r) return;
  MUTIL_FUNC(btrace)();
  return;
}


void* MUTIL_FUNC(malloc)(size_t size) 
{
  void* addr;
  if ((addr = malloc(size)) == NULL) {
    MUTIL_FUNC(dbg)("Memory allocation returned NULL (%s:%s:%d)",  __FILE__, __func__, __LINE__);
    MUTIL_FUNC(assert)(0);
  }
  mutil_total_alloc_count++;

  //TODO: Manage memory consumption
  //  total_alloc_size += size;
  // pid_t tid;
  // tid = syscall(SYS_gettid);
  // MUTIL_DBG("malloc: %d, size: %lu", tid, size);
  //  mutil_dbg("malloc: done %d", total_alloc_size);
  return addr;
}


void* MUTIL_FUNC(realloc)(void *ptr, size_t size) 
{
  void* addr;
  if ((addr = realloc(ptr, size)) == NULL) {
    MUTIL_FUNC(dbg)("Memory reallocation returned NULL: ptr: %p, size: %lu (%s:%s:%d)", ptr, size,  __FILE__, __func__, __LINE__);
    MUTIL_FUNC(assert)(0);
  }
  if (ptr == NULL) {
    mutil_total_alloc_count++;
  }

  //TODO: Manage memory consumption
  //  total_alloc_size += size;
  // pid_t tid;
  // tid = syscall(SYS_gettid);
  // MUTIL_DBG("malloc: %d, size: %lu", tid, size);
  //  mutil_dbg("malloc: done %d", total_alloc_size);
  return addr;
}

void MUTIL_FUNC(free)(void* addr) 
{
  free(addr);
  mutil_total_alloc_count--;

  //TODO: Manage memory consumption
  //  total_alloc_size -= size;
  return;
}




double MUTIL_FUNC(get_time)(void)
{
  double t;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t = ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
  //  mutil_dbg(" -== > %f", t);
  return t;
}

void MUTIL_FUNC(dbg_sleep_sec)(int sec)
{
  MUTIL_FUNC(dbg)("Sleep: %d sec (%s:%s:%d)", sec, __FILE__, __func__, __LINE__);
  sleep(sec);
  return;
}
void MUTIL_FUNC(sleep_sec)(int sec)
{
  sleep(sec);
  return;
}

void MUTIL_FUNC(dbg_sleep_usec)(int usec)
{
  MUTIL_FUNC(dbg)("Sleep: %d usec (%s:%s:%d)\n", usec, __FILE__, __func__, __LINE__);
  usleep(usec);
  return;
}

void MUTIL_FUNC(sleep_usec)(int usec)
{
  usleep(usec);
  return;
}


unsigned int MUTIL_FUNC(hash)(unsigned int original_val, unsigned int new_val) {
  return ((original_val << 5) + original_val) + new_val;
}

unsigned int MUTIL_FUNC(hash_str)(const char* c, unsigned int length)
{
  unsigned int i;
  unsigned int hash = 15;
  for (i = 0; i < length; i++) {
    hash = MUTIL_FUNC(hash)(hash, (int)(c[i])) ;
  }
  return hash;
}


int MUTIL_FUNC(init_rand)(int seed) 
{
  srand(seed);
  return 0;
}

int MUTIL_FUNC(init_ndrand)() 
{
  srand((int)MUTIL_FUNC(get_time)());
  return 0;
}

int MUTIL_FUNC(get_rand)(int max)
{
  return rand() % max;
}

int MUTIL_FUNC(str_starts_with)(const char* base, const char* str)
{
  size_t base_len = strlen(base);
  size_t str_len  = strlen(str);
  if (base_len < str_len) return 0;
  if (strncmp(str, base, str_len) != 0) return 0;
  return 1;
}

