#include <stdio.h>

// ** DO NOT include mpi.h in this file **

//#include <iostream>
#include "rempi_record.h"
#include "rempi_event.h"
#include "rempi_event_list.h"
#include "rempi_io_thread.h"
#include "rempi_util.h"
#include "rempi_err.h"
#include "rempi_config.h"

//extern int rempi_mode;

rempi_event_list<rempi_event*> *event_list;
rempi_io_thread *record_thread, *read_record_thread;
//using namespace std;

int rempi_record_init(int *argc, char ***argv, int rank) 
{
  string id;

  //fprintf(stderr, "ReMPI: Function call (%s:%s:%d)\n", __FILE__, __func__, __LINE__);

  id = std::to_string((long long int)rank);
  event_list = new rempi_event_list<rempi_event*>(100000, 100);
  record_thread = new rempi_io_thread(event_list, id, rempi_mode); //0: recording mode
  record_thread->start();
  
  return 0;
}

int rempi_replay_init(int *argc, char ***argv, int rank) 
{
  string id;

  fprintf(stderr, "ReMPI: Function call (%s:%s:%d)\n", __FILE__, __func__, __LINE__);
  id = std::to_string((long long int)rank);
  event_list = new rempi_event_list<rempi_event*>(100000, 100);
  read_record_thread = new rempi_io_thread(event_list, id, rempi_mode); //1: replay mode
  read_record_thread->start();

  rempi_sleep(100);
  
  return 0;
}

int rempi_record_irecv(
   void *buf,
   int count,
   int datatype,
   int source,
   int tag,
   int comm, // The value is set by MPI_Comm_set_name in ReMPI_convertor
   void *request)
{
  //  event_list->add_event((int64_t)buf, count, datatype, source, tag, comm, (int64_t)request);
  //TODO: we need to record *request?
  //  event_list->push(new rempi_irecv_event(1, count, source, tag, comm, -1));
  // fprintf(stderr, "ReMPI: =>RECORD(IREC): buf:%p count:%d, datatype:%d, source:%d, tag:%d, comm:%d, request:%p\n", 
  //  	  buf, count, datatype, source, tag, comm, request);
  //  event_list.get_event((int64_t)request, source, tag, comm);
  //  fprintf(stderr, "ReMPI: Record: buf:%p count:%d, datatype:%d, source:%d, tag:%d, comm:%d, request:%p Function call (%s:%s:%d)\n", __FILE__, __func__, __LINE__);
  return 0;
}
int rempi_replay_irecv(
   void *buf,
   int count,
   int datatype,
   int source,
   int tag,
   int comm, // The value is set by MPI_Comm_set_name in ReMPI_convertor
   void *request)
{
  return 0;
}


int rempi_record_test(
    void *request,
    int *flag,
    int source,
    int tag
)
{
  int event_count = 1;
  int is_testsome = 0;
  int request_val = -1;

  //TODO: we need to record *request ?
  event_list->push(new rempi_test_event(event_count, is_testsome, request_val, *flag, source, tag));

  return 0;
}

int rempi_replay_test(
    void *request,
    int *flag,
    int *source,
    int *tag)
{
  rempi_event *test_event;
  test_event = event_list->pop();
  return 0;
}

int rempi_record_testsome(
    int incount,
    void *array_of_requests[],
    int *outcount,
    int array_of_indices[],
    void *array_of_statuses[])
{
  int event_count = 1;
  int is_testsome = 1;
  int request     = 0;
  int flag        = 0;
  int source      = -1;
  int tag         = -1;


  if (*outcount == 0) {
    event_list->push(new rempi_test_event(event_count, is_testsome, request, flag, source, tag));
    return 0;
  }

  flag = 1;
  for (int i = 0; i < *outcount; i++) {
    source = array_of_indices[i];
    //TODO:  tag,
    tag    = 0;
    
    event_list->push(new rempi_test_event(event_count, is_testsome, request, flag, source, tag));
  }

  //  fprintf(stderr, "ReMPI: Function call (%s:%s:%d)\n", __FILE__, __func__, __LINE__);
  return 0;
}

int rempi_replay_testsome(
    int incount,
    void *array_of_requests[],
    int *outcount,
    int array_of_indices[],
    void *array_of_statuses[])
{
  return 0;
 }

int rempi_record_finalize(void)
{

  event_list->push_all();
  record_thread->complete_flush();
  record_thread->join();

  //  fprintf(stderr, "ReMPI: Function call (%s:%s:%d)\n", __FILE__, __func__, __LINE__);
  return 0;
}

int rempi_replay_finalize(void)
{
  //TODO:
  //  fprintf(stderr, "ReMPI: Function call (%s:%s:%d)\n", __FILE__, __func__, __LINE__);
  return 0;
}

