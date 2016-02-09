#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <mpi.h>

#include "rempi_re.h"
#include "rempi_err.h"
#include "rempi_mem.h"
#include "rempi_config.h"
#include "rempi_type.h"
#include "rempi_status.h"
#include "rempi_util.h"
#include "rempi_recorder.h"
#include "rempi_request_mg.h"


using namespace std;

#define CHECK_MPI_STATUS(count, statuses)	\
do { \
  if (statuses == NULL) {						\
    /*TODO: allocate array_of_statuses in ReMPI instead of the error below*/ \
    statuses = (MPI_Status*)malloc(count * sizeof(MPI_Status));	\
    REMPI_DBG("ReMPI requires status in MPI_Test");	       \
  } \
} while(0) \


#define HANDLE_NON_RECV_REQUESTS(incount, array_of_requests, matching_func, func) \
do { \
  int recv_request_count; \
  int null_request_count; \
  int ret; \
  REMPI_DBGI(9, "MF call: %s: incount: %d", func, incount);			\
  recv_request_count = rempi_reqmg_get_recv_request_count(incount, array_of_requests); \
  REMPI_DBGI(9, "MDD MF call: %s: incount: %d", func, incount);	\
  null_request_count = rempi_reqmg_get_null_request_count(incount, array_of_requests); \
  REMPI_DBGI(9, "END MF call: %s: incount: %d", func, incount);	\
  if  (null_request_count > 0) { \
    REMPI_DBGI(9, "Warning: MPI_REQUEST_NULL is used: %p/%d (at %s)", MPI_REQUEST_NULL, incount, func); \
    if (recv_request_count == 0) { \
      REMPI_DBGI(9, "Warning: Calling with MPI_REQUEST_NULL");	\
        return matching_func;		       \
    } \
  } else if(recv_request_count == 0) { \
    if (incount > 0)  REMPI_DBGI(9, "Untraced request call: %p (at %s)", array_of_requests[0], func); \
    ret = matching_func; \
    REMPI_DBGI(9, " ... Done");			\
    return ret; \
  } else if (recv_request_count != incount) {		\
    REMPI_DBG("ReMPI requires array_of_requests to be all send requests OR all recv requests: " \
              "recv_req_count: %d, incount: %d", recv_request_count, incount); \
    REMPI_ASSERT(1 == 0); \
  } \
} while(0)			\


int rempi_re::init_after_pmpi_init(int *argc, char ***argv)
{
  signal(SIGSEGV, SIG_DFL);

  char comm_id[REMPI_COMM_ID_LENGTH];
  comm_id[0] = 0;
  PMPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  PMPI_Comm_set_name(MPI_COMM_WORLD, comm_id);

  rempi_err_init(my_rank);
  rempi_set_configuration(argc, argv);

  return 0;
}

int rempi_re::re_init(int *argc, char ***argv)
{
  int ret;
  //  int provided;

  /*Init CLMPI*/
  ret = PMPI_Init(argc, argv);
  /*Init from configuration and for valiables for errors*/
  init_after_pmpi_init(argc, argv);
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    recorder->record_init(argc, argv, my_rank);
  } else {
    //TODO: Check if inputs are same as values in recording run
    //TODO: Check if this is same run as the last recording run
    recorder->replay_init(argc, argv, my_rank);
  }
  return ret;
}

int rempi_re::re_init_thread(
			     int *argc, char ***argv,
			     int required, int *provided)
{
  int ret;
  /*Init CLMPI*/
  ret = PMPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, provided);


  if (*provided < MPI_THREAD_SERIALIZED) {
    REMPI_ERR("MPI supports only MPI_THREAD_SINGLE, and ReMPI does not work on this MPI");
  }
  /*Init from configuration and for valiables for errors*/
  init_after_pmpi_init(argc, argv);
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    recorder->record_init(argc, argv, my_rank);
  } else {
    //TODO: Check if inputs are same as values in recording run
    //TODO: Check if this is same run as the last recording run
    recorder->replay_init(argc, argv, my_rank);
  }
  return ret;
}

int rempi_re::re_isend(
		       void *buf,
		       int count,
		       MPI_Datatype datatype,
		       int dest,
		       int tag,
		       MPI_Comm comm,
		       MPI_Request *request)
{
  int ret;
  if (comm != MPI_COMM_WORLD) {
    REMPI_ERR("Current ReMPI does not multiple communicators");
  }
  ret = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
  rempi_reqmg_register_send_request(request, dest, tag, comm);

  return ret;
}


int rempi_re::re_irecv(
		 void *buf,
		 int count,
		 MPI_Datatype datatype,
		 int source,
		 int tag,
		 MPI_Comm comm,
		 MPI_Request *request)
{
  int ret;
  char comm_id[REMPI_COMM_ID_LENGTH];
  //  int comm_id_int;
  int resultlen;
  if (comm != MPI_COMM_WORLD) {
    REMPI_ERR("Current ReMPI does not multiple communicators");
  }

  //  PMPI_Comm_get_name(MPI_COMM_WORLD, comm_id, &resultlen);
  

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // ret = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    // rempi_reqmg_register_recv_request(request, source, tag, (int)comm_id[0]);
    ret = recorder->record_irecv(buf, count, datatype, source, tag, (int)comm_id[0], comm, request);
  } else {
    /*TODO: Really need datatype ??*/
    ret = recorder->replay_irecv(buf, count, datatype, source, tag, (int)comm_id[0], comm, request);
  }

  return ret;
}

  
int rempi_re::re_test(
		    MPI_Request *request, 
		    int *flag, 
		    MPI_Status *status)
{
  int ret;
  int test_id;
  int status_flag;


  status = rempi_status_allocate(1, status, &status_flag);

  *flag = 0;
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // HANDLE_NON_RECV_REQUESTS(1, request, 
    // 			     PMPI_Test(request, flag, status), __func__);

    // test_id = rempi_reqmg_get_test_id(request, 1);
    // ret = PMPI_Test(request, flag, status);
    recorder->record_mf(1, request, flag, NULL, status, test_id, REMPI_MPI_TEST);
  } else {
    recorder->replay_testsome(1, request, flag, NULL, status,
			      test_id, REMPI_MF_FLAG_1_TEST, REMPI_MF_FLAG_2_SINGLE);
  }

  if (status_flag) rempi_status_free(status);

  return ret;
}

int rempi_re::re_testany(int count, MPI_Request array_of_requests[],
		       int *index, int *flag, MPI_Status *status)
{ 
  int ret;
  int test_id;
  int status_flag;


  status = rempi_status_allocate(count, status, &status_flag);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // HANDLE_NON_RECV_REQUESTS(count, array_of_requests,
    // 			     PMPI_Testany(count, array_of_requests, index, flag, status), __func__);
    // test_id = rempi_reqmg_get_test_id(array_of_requests, count);
    // ret = PMPI_Testany(count, array_of_requests, index, flag, status);
    recorder->record_mf(count, array_of_requests, flag, index, status, test_id, REMPI_MPI_TESTANY);
  } else {
    recorder->replay_testsome(count, array_of_requests, flag, index, status, test_id, REMPI_MF_FLAG_1_TEST, REMPI_MF_FLAG_2_ANY);
  }

  if (status_flag) rempi_status_free(status);

  return MPI_SUCCESS;
}



int rempi_re::re_testsome(
			  int incount, 
			  MPI_Request array_of_requests[],
			  int *outcount, 
			  int array_of_indices[], 
			  MPI_Status array_of_statuses[])
{
  int ret;
  //  size_t *clocks;
  //  int is_with_next = REMPI_MPI_EVENT_WITH_NEXT;
  int test_id;
  int status_flag;  
  
  array_of_statuses = rempi_status_allocate(incount, array_of_statuses, &status_flag);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // HANDLE_NON_RECV_REQUESTS(incount, array_of_requests, 
    // 			     PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses), __func__);


    // test_id = rempi_reqmg_get_test_id(array_of_requests, incount);
    // //    int flag;
    // ret = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    recorder->record_mf(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, test_id, REMPI_MPI_TESTSOME);

    // if (*outcount == 0) {
    //   flag = 0;
    //   recorder->record_test(NULL, &flag, -1, -1, -1, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);
    // } else {
    //   flag = 1;
    //   for (int i = 0; i < *outcount; i++) {
    // 	int index = array_of_indices[i];
    // 	if (i == *outcount - 1) {
    // 	  is_with_next = REMPI_MPI_EVENT_NOT_WITH_NEXT;
    // 	}
    // 	recorder->record_test(&array_of_requests[index], &flag, array_of_statuses[i].MPI_SOURCE, 
    // 			      array_of_statuses[i].MPI_TAG, 0, is_with_next, test_id);
    //   }
    // }
  } else {
    recorder->replay_testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, 
			      test_id, REMPI_MF_FLAG_1_TEST, REMPI_MF_FLAG_2_SOME);
  }

  if (status_flag) rempi_status_free(array_of_statuses);

  return ret;
}


int rempi_re::re_testall(int count, MPI_Request array_of_requests[],
		       int *flag, MPI_Status array_of_statuses[])
{
  int ret;
  //  size_t *clocks;
  //  int is_with_next = REMPI_MPI_EVENT_WITH_NEXT;
  int test_id;
  int status_flag;


  array_of_statuses = rempi_status_allocate(count, array_of_statuses, &status_flag);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // REMPI_DBGI(9, "Testall: handle");
    // HANDLE_NON_RECV_REQUESTS(count, array_of_requests, 
    // 			     PMPI_Testall(count, array_of_requests, flag, array_of_statuses), __func__);

    // test_id = rempi_reqmg_get_test_id(array_of_requests, count);
    // REMPI_DBGI(9, "Testall: flag before: %d", *flag);
    // ret = PMPI_Testall(count, array_of_requests, flag, array_of_statuses);
    // REMPI_DBGI(9, "Testall: flag after: %d", *flag);
    recorder->record_mf(count, array_of_requests, flag, NULL, array_of_statuses, test_id, REMPI_MPI_TESTALL);

    // if (*flag == 0) {
    //   recorder->record_test(NULL, flag, -1, -1, -1, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);
    //   return MPI_SUCCESS;
    // } 

    // for (int i = 0; i < count; i++) {
    //   if (i == count - 1) {
    // 	is_with_next = REMPI_MPI_EVENT_NOT_WITH_NEXT;
    //   }
    //   recorder->record_test(&array_of_requests[i], flag, array_of_statuses[i].MPI_SOURCE, 
    // 			    array_of_statuses[i].MPI_TAG, 0, is_with_next, test_id);
    // }
  } else {
    int outcount;
    //    int a[300];
    recorder->replay_testsome(count, array_of_requests, &outcount, NULL, array_of_statuses, 
			      test_id, REMPI_MF_FLAG_1_TEST, REMPI_MF_FLAG_2_ALL);
    if (outcount == 0) {
      *flag = 0;
    } else {
      *flag = 1;
    }
    //    *flag = (outcount == 0)? 0:1;
  }

  if (status_flag) rempi_status_free(array_of_statuses);

  return ret;
}

int rempi_re::re_wait(
		    MPI_Request *request,
		    MPI_Status *status)
{
  int ret;
  int test_id;
  int flag = 1;
  int status_flag;

  status = rempi_status_allocate(1, status, &status_flag);
  
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // REMPI_DBGI(9, "wait called: req: %p %p", *request, request);
    // HANDLE_NON_RECV_REQUESTS(1, request, PMPI_Wait(request, status), __func__);
    // REMPI_DBGI(9, "wait called ... done: %p %p", *request, request);

    // test_id = rempi_reqmg_get_test_id(request, 1);
    // ret = PMPI_Wait(request, status);
    // //    recorder->record_test(request, &flag, status->MPI_SOURCE, status->MPI_TAG, 0, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);
    recorder->record_mf(1, request, NULL, NULL, status, test_id, REMPI_MPI_WAIT);
  } else {
    int index;
    recorder->replay_testsome(1, request, &flag, &index, status,
			      test_id, REMPI_MF_FLAG_1_WAIT, REMPI_MF_FLAG_2_SINGLE);
  }

  if (status_flag) rempi_status_free(status);
  return ret;
}
 
  
int rempi_re::re_waitany(
		       int count, MPI_Request array_of_requests[],
		       int *index, MPI_Status *status)
{
  int ret;
  int test_id;
  int flag = 1;
  int status_flag;

  status = rempi_status_allocate(count, status, &status_flag);
  
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {

    // HANDLE_NON_RECV_REQUESTS(count, array_of_requests,
    // 			     PMPI_Waitany(count, array_of_requests, index, status), __func__);
    // test_id = rempi_reqmg_get_test_id(array_of_requests, count);
    // ret = PMPI_Waitany(count, array_of_requests, index, status);
    recorder->record_mf(count, array_of_requests, NULL, index, status, test_id, REMPI_MPI_WAITANY);
    //    recorder->record_test(&array_of_requests[*index], &flag, status->MPI_SOURCE, status->MPI_TAG, 0, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);
  } else {
    recorder->replay_testsome(count, array_of_requests, &flag, index, status, test_id, REMPI_MF_FLAG_1_WAIT, REMPI_MF_FLAG_2_ANY);
  }
  if (status_flag) rempi_status_free(status);
  return ret;
}
  
int rempi_re::re_waitsome(int incount, MPI_Request array_of_requests[],
			  int *outcount, int array_of_indices[],
			  MPI_Status array_of_statuses[])
{
  int ret;
  //  size_t *clocks;
  //int is_with_next = REMPI_MPI_EVENT_WITH_NEXT;
  int test_id;
  int status_flag;
  
  array_of_statuses = rempi_status_allocate(incount, array_of_statuses, &status_flag);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // HANDLE_NON_RECV_REQUESTS(incount, array_of_requests, 
    // 			     PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses), __func__);


    // test_id = rempi_reqmg_get_test_id(array_of_requests, incount);
    // //    int flag;
    // ret = PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    recorder->record_mf(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, test_id, REMPI_MPI_WAITSOME);
    // if (*outcount == 0) {
    //   flag = 0;
    //   recorder->record_test(NULL, &flag, -1, -1, -1, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);
    // } else {
    //   flag = 1;
    //   for (int i = 0; i < *outcount; i++) {
    // 	int index = array_of_indices[i];
    // 	if (i == *outcount - 1) {
    // 	  is_with_next = REMPI_MPI_EVENT_NOT_WITH_NEXT;
    // 	}
    // 	recorder->record_test(&array_of_requests[index], &flag, array_of_statuses[i].MPI_SOURCE, 
    // 			      array_of_statuses[i].MPI_TAG, 0, is_with_next, test_id);
    //   }
    // }
  } else {
    recorder->replay_testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, 
			      test_id, REMPI_MF_FLAG_1_WAIT, REMPI_MF_FLAG_2_SOME);
  }
  if (status_flag) rempi_status_free(array_of_statuses);

  return ret;
}
  

int array_of_indices_waitall[16]; 
int rempi_re::re_waitall(
			  int incount, 
			  MPI_Request array_of_requests[],
			  MPI_Status array_of_statuses[])
{
  int ret;
  //  int is_with_next = REMPI_MPI_EVENT_WITH_NEXT;
  int test_id;
  int status_flag;

  array_of_statuses = rempi_status_allocate(incount, array_of_statuses, &status_flag);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // HANDLE_NON_RECV_REQUESTS(incount, array_of_requests,
    // 			     PMPI_Waitall(incount, array_of_requests, array_of_statuses), __func__);
    // test_id = rempi_reqmg_get_test_id(array_of_requests, incount);
    // //int flag = 1;
    // ret = PMPI_Waitall(incount, array_of_requests, array_of_statuses);
    recorder->record_mf(incount, array_of_requests, NULL, NULL, array_of_statuses, test_id, REMPI_MPI_WAITALL);

    // for (int i = 0; i < incount; i++) {
    //   int index = i;
    //   if (i == incount - 1) is_with_next = REMPI_MPI_EVENT_NOT_WITH_NEXT;
    //   recorder->record_test(&array_of_requests[index], &flag, array_of_statuses[i].MPI_SOURCE, 
    // 			    array_of_statuses[i].MPI_TAG, 0, is_with_next, test_id);
    // }
  } else {
    int  outcount;
    recorder->replay_testsome(incount, array_of_requests, &outcount, NULL, array_of_statuses, test_id, 
			      REMPI_MF_FLAG_1_WAIT, REMPI_MF_FLAG_2_ALL);
    if (incount != outcount) {
      REMPI_ERR("incount: %d != outcount: %d", incount, outcount)
    }
    REMPI_ASSERT(incount == outcount);
  }
  if (status_flag) rempi_status_free(array_of_statuses);
  return ret;
}


int rempi_re::re_probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
  int ret;
  int test_id;
  int flag;
  char comm_id[REMPI_COMM_ID_LENGTH];
  int resultlen;
  int status_flag;

  PMPI_Comm_get_name(MPI_COMM_WORLD, comm_id, &resultlen);

  status = rempi_status_allocate(1, status, &status_flag);  

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // rempi_reqmg_register_recv_request(&req, source, tag, (int)comm_id[0]);
    // test_id = rempi_reqmg_get_test_id(&req, 1);
    // ret = PMPI_Probe(source, tag, comm, status);
    // recorder->record_mf(1, NULL, NULL, NULL, status, test_id, REMPI_MPI_PROBE);
    recorder->record_pf(source, tag, comm, NULL, status, REMPI_MPI_PROBE);
  } else {
    recorder->replay_iprobe(source, tag, comm, &flag, status, (int)comm_id[0]);
  }
  if (status_flag) rempi_status_free(status);
  return ret;
}

int rempi_re::re_iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
  int ret;
  int test_id;
  char comm_id[REMPI_COMM_ID_LENGTH];
  int resultlen;
  int status_flag;

  status = rempi_status_allocate(1, status, &status_flag);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    // MPI_Request req = NULL;
    // rempi_reqmg_register_recv_request(&req, source, tag, (int)comm_id[0]);
    // test_id = rempi_reqmg_get_test_id(&req, 1);
    // ret = PMPI_Iprobe(source, tag, comm, flag, status);
    // recorder->record_mf(1, NULL, NULL, flag, status, test_id, REMPI_MPI_IPROBE);
    recorder->record_pf(source, tag, comm, flag, status, REMPI_MPI_IPROBE);
  } else {
    recorder->replay_iprobe(source, tag, comm, flag, status, (int)comm_id[0]);
  }
  if (status_flag) rempi_status_free(status);

  return ret;
}


int rempi_re::re_cancel(MPI_Request *request)
{
  int ret;
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    ret = PMPI_Cancel(request);
  } else {
    ret = recorder->replay_cancel(request);
  }
  return ret;
}

int rempi_re::re_comm_split(MPI_Comm arg_0, int arg_1, int arg_2, MPI_Comm *arg_3)
{
  int ret;
  ret = PMPI_Comm_split(arg_0, arg_1, arg_2, arg_3);
  return ret;
}
  
int rempi_re::re_comm_create(MPI_Comm arg_0, MPI_Group arg_1, MPI_Comm *arg_2)
{
  int ret;
  ret = PMPI_Comm_create(arg_0, arg_1, arg_2);
  return ret;
}

int rempi_re::re_comm_dup(MPI_Comm arg_0, MPI_Comm *arg_2)
{
  int ret;   
  ret = PMPI_Comm_dup(arg_0, arg_2);
  return ret;
}




int rempi_re::re_allreduce(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5)
{
  int ret;
  ret = PMPI_Allreduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
  return ret;  
}

int rempi_re::re_reduce(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, int arg_5, MPI_Comm arg_6)
{
  int ret;
  ret = PMPI_Reduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  return ret;
}

int rempi_re::re_scan(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5)
{
  int ret;
  ret = PMPI_Scan(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
  return ret; 
}

int rempi_re::re_allgather(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6)
{
  int ret;
  ret = PMPI_Allgather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  return ret;
}

int rempi_re::re_gatherv(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, rempi_mpi_version_int *arg_4, rempi_mpi_version_int *arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8)
{
  int ret;
  ret = PMPI_Gatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
  return ret;
}

int rempi_re::re_reduce_scatter(rempi_mpi_version_void *arg_0, void *arg_1, rempi_mpi_version_int *arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5)
{
  int ret;
  ret = PMPI_Reduce_scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
  return ret; 
}

int rempi_re::re_scatterv(rempi_mpi_version_void *arg_0, rempi_mpi_version_int *arg_1, rempi_mpi_version_int *arg_2, MPI_Datatype arg_3, void *arg_4, int arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8)
{
  int ret;
  ret = PMPI_Scatterv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
  return ret;
}

int rempi_re::re_allgatherv(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, rempi_mpi_version_int *arg_4, rempi_mpi_version_int *arg_5, MPI_Datatype arg_6, MPI_Comm arg_7)
{
  int ret;
  ret = PMPI_Allgatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
  return ret; 
}

int rempi_re::re_scatter(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7)
{
  int ret;
  ret = PMPI_Scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
  return ret;
}

int rempi_re::re_bcast(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, MPI_Comm arg_4)
{
  int ret;
  ret = PMPI_Bcast(arg_0, arg_1, arg_2, arg_3, arg_4);
  return ret; 
}

int rempi_re::re_alltoall(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6)
{
  int ret;
  ret = PMPI_Alltoall(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  return ret;
}

int rempi_re::re_gather(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7)
{
  int ret;
  ret = PMPI_Gather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
  return ret;
}

int rempi_re::re_barrier(MPI_Comm arg_0)
{
  int ret;
  ret = PMPI_Barrier(arg_0);
  return ret;  
}

int rempi_re::re_finalize()
{
  int ret;
  ret = PMPI_Finalize();
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    ret = recorder->record_finalize();
  } else {
    ret = recorder->replay_finalize();
  }
  return ret;
}



