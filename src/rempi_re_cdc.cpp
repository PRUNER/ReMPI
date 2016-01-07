#include <stdlib.h>
#include <string.h>

#include <mpi.h>

#include "rempi_re.h"
#include "rempi_err.h"
#include "rempi_mem.h"
#include "rempi_config.h"
#include "clmpi.h"
#include "rempi_recorder.h"

#define  PNMPI_MODULE_REMPI "rempi"

using namespace std;


int rempi_re_cdc::init_clmpi()
{
  int err;
  // PNMPI_modHandle_t handle_rempi, handle_clmpi;
  // PNMPI_Service_descriptor_t serv;
  // /*Load clock-mpi*/
  // err=PNMPI_Service_GetModuleByName(PNMPI_MODULE_CLMPI, &handle_clmpi);
  // if (err!=PNMPI_SUCCESS) {
  //   REMPI_ERR("failed");
  //   return err;
  // }
  // /*Get clock-mpi service*/
  // err=PNMPI_Service_GetServiceByName(handle_clmpi,"clmpi_register_recv_clocks","pi",&serv);
  // if (err!=PNMPI_SUCCESS) {
  //   REMPI_ERR("failed");
  //   return err;
  // }
  // clmpi_register_recv_clocks=(PNMPIMOD_register_recv_clocks_t) ((void*)serv.fct);
  clmpi_register_recv_clocks = PNMPIMOD_register_recv_clocks;

  // /*Get clock-mpi service*/
  // err=PNMPI_Service_GetServiceByName(handle_clmpi,"clmpi_get_local_clock","p",&serv);
  // if (err!=PNMPI_SUCCESS) {
  //   REMPI_ERR("failed");
  //   return err;
  // }
  // clmpi_get_local_clock=(PNMPIMOD_get_local_clock_t) ((void*)serv.fct);
  clmpi_get_local_clock = PNMPIMOD_get_local_clock;

  // /*Get clock-mpi service*/
  // err=PNMPI_Service_GetServiceByName(handle_clmpi,"clmpi_collective_sync_clock","p", &serv);
  // if (err!=PNMPI_SUCCESS) {
  //   REMPI_ERR("failed");
  //   return err;
  // }
  // clmpi_collective_sync_clock=(PNMPIMOD_collective_sync_clock_t) ((void*)serv.fct);
  clmpi_collective_sync_clock = PNMPIMOD_collective_sync_clock;

  // /*Load own moduel*/
  // err=PNMPI_Service_GetModuleByName(PNMPI_MODULE_REMPI, &handle_rempi);
  // if (err!=PNMPI_SUCCESS) {
  //   REMPI_ERR("failed");
  //   return err;
  // }
  //  return PNMPI_SUCCESS;
  return MPI_SUCCESS;
}

unordered_map<string, int>matching_set_id_umap;
unordered_map<MPI_Request, int>request_to_matching_set_id_umap;
int next_matching_set_id_to_assign = 0;
int get_matching_set_id()
{
  if (!rempi_is_test_id) return 0;
  string matching_set_id_string;
  matching_set_id_string = rempi_btrace_string();
  if (matching_set_id_umap.find(matching_set_id_string) == matching_set_id_umap.end()) {
    matching_set_id_umap[matching_set_id_string] = next_matching_set_id_to_assign;
    next_matching_set_id_to_assign++;
  }
  return matching_set_id_umap[matching_set_id_string];
}

#ifdef REVERT1
int rempi_re_cdc::get_test_id()
{
  if (!rempi_is_test_id) return 0;
  string test_id_string;
  test_id_string = rempi_btrace_string();
  //TODO: get the binary name
  //  size_t pos = test_id_string.find("MCBenchmark");
  //  test_id_string = test_id_string.substr(pos);
  if (test_ids_map.find(test_id_string) == test_ids_map.end()) {
    test_ids_map[test_id_string] = next_test_id_to_assign;
    //    REMPI_DBGI(0, "global_test_id %d: %s", next_test_id_to_assign, test_id_string.c_str());
    next_test_id_to_assign++;
  }
  return test_ids_map[test_id_string];
}
#else
int rempi_re_cdc::get_test_id(MPI_Request *requests)
{
  if (!rempi_is_test_id) return 0;
  //TODO: get the binary name
  //  size_t pos = test_id_string.find("MCBenchmark");
  //  test_id_string = test_id_string.substr(pos);
  if (test_ids_map.find(requests) == test_ids_map.end()) {
    test_ids_map[requests] = next_test_id_to_assign;
    REMPI_DBGI(0, "global_test_id %d: %s", next_test_id_to_assign, test_id_string.c_str());
    next_test_id_to_assign++;
  }
  return test_ids_map[requests];
}
#endif

int rempi_re_cdc::re_init(int *argc, char ***argv)
{
  int ret;
  int provided;

  /*Init CLMPI*/
  init_clmpi();

  /*A CDC thread make MPI calls, so call PMPI_Init_thraed insted of PMPI_Init */
  // ret = PMPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
  // if (provided < MPI_THREAD_SERIALIZED) {
  //   REMPI_ERR("MPI supports only MPI_THREAD_SINGLE, and ReMPI does not work on this MPI");
  // }
  ret = PMPI_Init(argc, argv);
  //  REMPI_ERR("provided: %d", provided);
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

int rempi_re_cdc::re_init_thread(
			     int *argc, char ***argv,
			     int required, int *provided)
{
  int ret;
  /*Init CLMPI*/
  init_clmpi();

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

int rempi_re_cdc::re_isend(
		       void *buf,
		       int count,
		       MPI_Datatype datatype,
		       int dest,
		       int tag,
		       MPI_Comm comm,
		       MPI_Request *request)
{
  int ret;
#ifdef REMPI_DBG_REPLAY
  size_t clock;
  clmpi_get_local_clock(&clock);
#endif
  if (comm != MPI_COMM_WORLD) {
    REMPI_ERR("Current ReMPI does not multiple communicators");
  }
  ret = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
  if (rempi_mode == REMPI_ENV_REMPI_MODE_REPLAY) {
    recorder->replay_isend(request);
  }

#ifdef REMPI_DBG_REPLAY
    REMPI_DBG("  Send: request: %p dest: %d, tag: %d, clock: %d, count: %d", *request, dest, tag, clock, count);
#endif

  return ret;
}


int rempi_re_cdc::re_irecv(
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
  int comm_id_int;
  int resultlen;
  if (comm != MPI_COMM_WORLD) {
    REMPI_ERR("Current ReMPI does not multiple communicators");
  }

  PMPI_Comm_get_name(MPI_COMM_WORLD, comm_id, &resultlen);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    ret = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    //TODO: Get Datatype,
    recorder->record_irecv(buf, count, datatype, source, tag, (int)comm_id[0], request);
  } else {
    /*TODO: Really need datatype ??*/
    recorder->replay_irecv(buf, count, datatype, source, tag, (int)comm_id[0], &comm, request);
  }
#if 1
  {
    int matching_set_id;
    matching_set_id  = get_matching_set_id();
    request_to_matching_set_id_umap[*request] = matching_set_id;
    REMPI_DBGI(0, "matching_set_id: %d, source: %d, tag: %d, count: %d, request: %p", matching_set_id, source, tag, (int)comm_id[0], *request);
  }
#endif
  return ret;
}
  
int rempi_re_cdc::re_test(
		    MPI_Request *request, 
		    int *flag, 
		    MPI_Status *status)
{
  int ret;
  size_t clock;
#ifdef REVERT1
  int test_id = get_test_id();
#else
  int test_id = get_test_id(request);
#endif

#if 1
  int matching_set_id = request_to_matching_set_id_umap[*request];
  REMPI_DBGI(0, "test_id: %d, request: %p (matching_set_id: %d)", test_id, *request, matching_set_id);
#endif


  if (status == NULL) {
    /*TODO: allocate array_of_statuses in ReMPI instead of the error below*/
    REMPI_ERR("ReMPI requires status in MPI_Test");
  }

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    clmpi_register_recv_clocks(&clock, 1);
    ret = PMPI_Test(request, flag, status);
    /*If recoding mode, record the test function*/
    /*TODO: change froi void* to MPI_Request*/
    if(clock != PNMPI_MODULE_CLMPI_SEND_REQ_CLOCK) {
// #ifdef REMPI_DBG_REPLAY
//   REMPI_DBGI(REMPI_DBG_REPLAY, "  Test: (count: %d, with_next: %d, flag: %d, source: %d, tag: %d, clock: %d): test_id: %d",
// 	     1, 0, *flag, status->MPI_SOURCE, status->MPI_TAG, clock, test_id);
// #endif
       // REMPI_DBG( "Record  : (count: %d, with_next: %d, flag: %d, source: %d, tag: %d, clock: %d)",
       // 		 1, 0, *flag,
       // 		  status->MPI_SOURCE, status->MPI_TAG, clock);
      recorder->record_test(request, flag, status->MPI_SOURCE, status->MPI_TAG, clock, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);

    }
  } else {
    recorder->replay_test(request, flag, status, test_id);
  }

  return ret;
}

int rempi_re_cdc::re_cancel(MPI_Request *request)
{
  int ret;
  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    ret = PMPI_Cancel(request);
  } else {
    ret = recorder->replay_cancel(request);
  }
  return ret;
}


int rempi_re_cdc::re_testsome(
			  int incount, 
			  MPI_Request array_of_requests[],
			  int *outcount, 
			  int array_of_indices[], 
			  MPI_Status array_of_statuses[])
{
  int ret;
  size_t *clocks;
  int is_with_next = REMPI_MPI_EVENT_WITH_NEXT;
#ifdef REVERT1
  int test_id = get_test_id();
#else
  int test_id = get_test_id(array_of_requests);
#endif
  
  if (array_of_statuses == NULL) {
    /*TODO: allocate array_of_statuses in ReMPI instead of the error below*/
    REMPI_ERR("ReMPI requires array_of_statues in MPI_Testsome");
  }

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    clocks = (size_t*)rempi_malloc(sizeof(size_t) * incount);
    clmpi_register_recv_clocks(clocks, incount);
#if 1
    for (int i = 0; i < incount; i++) {
      int matching_set_id = request_to_matching_set_id_umap[array_of_requests[i]];
      REMPI_DBGI(0, "test_id: %d, matching_set_id: %d", test_id, matching_set_id);
    }
#endif
    ret = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);

    if (*outcount == 0) {
      int flag = 0;
      /*TODO: 
	PNMPI_MODULE_CLMPI_SEND_REQ_CLOCK does not mean it's send request, but mean it's not recv request.
	It means that if the request is not called by Irecv, ReMPI regard the request as send request.
	currently if at least one of requests is NOT PNMPI_MODULE_CLMPI_SEND_REQ_CLOCK 
	(which means one of request is called by Irecv), 
        then ReMPI regard the rest of the request are recv requests that is not called by Irecv.
	Thus, ReMPI record the events as unmatched event
      */
#if 1
      int is_all_send_req = 1;
      for (int i = 0; i < incount; i++) {
	if(clocks[i] != PNMPI_MODULE_CLMPI_SEND_REQ_CLOCK) {	
	  is_all_send_req = 0;
	}
      }

      if (!is_all_send_req) {
	recorder->record_test(NULL, &flag, -1, -1, -1, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);
      } else {
#ifdef REMPI_DBG_REPLAY
	REMPI_DBGI(REMPI_DBG_REPLAY, "Test send: test_id: %d: incount: %d outcount: %d", test_id, incount, *outcount);
#endif
      }
#else
      if(clocks[0] != PNMPI_MODULE_CLMPI_SEND_REQ_CLOCK) {
	REMPI_PRTI(0, "outcount: %d", *outcount);
	//	REMPI_DBG("aaa");
	recorder->record_test(NULL, &flag, -1, -1, -1, REMPI_MPI_EVENT_NOT_WITH_NEXT, test_id);
      } else {
	REMPI_PRTI(0, "skip outcount: %d", *outcount);
#if 0	
	REMPI_DBGI(1, "skipp flag = 0, clock: %d (incount: %d)", clocks[0], incount);
#endif
      }
#endif
    } else {
      for (int i = 0; i < *outcount; i++) {
	int flag = 1;
	int index = array_of_indices[i];
	if(clocks[index] != PNMPI_MODULE_CLMPI_SEND_REQ_CLOCK) {
	  if (i == *outcount - 1) {
	    is_with_next = REMPI_MPI_EVENT_NOT_WITH_NEXT;
	  }


// #ifdef REMPI_DBG_REPLAY
// 	  REMPI_DBGI(REMPI_DBG_REPLAY, "  Test: index: %d (incount: %d, outcount: %d, with_next: %d, flag: %d, source: %d, tag: %d, clock: %d): test_id: %d",     index, incount, *outcount, is_with_next, flag,
// 		     array_of_statuses[i].MPI_SOURCE, array_of_statuses[i].MPI_TAG, clocks[index], test_id);
// #endif
	  /*array_of_statuses only contain statuese for completed messages (i.e. length of array_of_indices == *outcount)
	    so I use [i] insted of [index]
	   */
	  recorder->record_test(&array_of_requests[index], &flag, array_of_statuses[i].MPI_SOURCE, array_of_statuses[i].MPI_TAG, clocks[index], is_with_next, test_id);
	}  else {
#if 0	
	  REMPI_DBGI(1, "skipp flag = 1, clock: %d (%d/%d)", clocks[i], i, *outcount);
#endif
	}
      }
    }
    free(clocks);
  } else {
    //    ret = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
// #ifdef REMPI_DBG_REPLAY
//     REMPI_DBGI(REMPI_DBG_REPLAY, "testsome call");
// #endif
    recorder->replay_testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, test_id, REMPI_MF_FLAG_1_TEST, REMPI_MF_FLAG_2_SOME);
// #ifdef REMPI_DBG_REPLAY
//     REMPI_DBGI(REMPI_DBG_REPLAY, "testsome end");
// #endif
  }

#if 0
  for (int i = 0; i < *outcount; i++) {
    int source;
    int index = array_of_indices[i];
    source =    array_of_statuses[index].MPI_SOURCE;
    // REMPI_DBG("out: (%lu |%d|) read index: %d", clocks[index], source, index);
    //    REMPI_DBG("clock %lu, source: %d (%d/%d); incount:%d", clocks[index], source, index, *outcount, incount);
    REMPI_DBG(" => Testsome: clock %lu |%d|", clocks[index], source);
    //    REMPI_DBG("source: %d (%d/%d); incount:%d", source, index, *outcount, incount);
  }
#endif


  //  REMPI_DBG(" ----------- ");

  return ret;
}


int array_of_indices_waitall[16]; 
int rempi_re_cdc::re_waitall(
			  int incount, 
			  MPI_Request array_of_requests[],
			  MPI_Status array_of_statuses[])
{
  int ret;
  size_t *clocks;
  int is_with_next = REMPI_MPI_EVENT_WITH_NEXT;
#ifdef REVERT1
  int test_id = get_test_id();
#else
  int test_id = get_test_id(array_of_requests);
#endif

  if (array_of_statuses == NULL) {
    /*TODO: allocate array_of_statuses in ReMPI instead of the error below*/
    REMPI_ERR("ReMPI requires array_of_statues in MPI_Testsome");
  }

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    clocks = (size_t*)rempi_malloc(sizeof(size_t) * incount);
    clmpi_register_recv_clocks(clocks, incount);
    ret = PMPI_Waitall(incount, array_of_requests, array_of_statuses);

    for (int i = 0; i < incount; i++) {
      int flag = 1;
      int index = i;
      if(clocks[index] != PNMPI_MODULE_CLMPI_SEND_REQ_CLOCK) {
	if (i == incount - 1) {
	  is_with_next = REMPI_MPI_EVENT_NOT_WITH_NEXT;
	}
	recorder->record_test(&array_of_requests[index], &flag, array_of_statuses[i].MPI_SOURCE, array_of_statuses[i].MPI_TAG, clocks[index], is_with_next, test_id);
      }
    }
    free(clocks);
  } else {
    int  outcount;
    /*TODO: This is to avoid malloc overhead. But to fix this limitation*/
    if (incount > 16) {
      REMPI_ERR("Current ReMPI only support incount <= 16 in MPI_Waitall");
    }    
    recorder->replay_testsome(incount, array_of_requests, &outcount, array_of_indices_waitall, array_of_statuses, test_id, 
			      REMPI_MF_FLAG_1_WAIT, REMPI_MF_FLAG_2_ALL);
    if (incount != outcount) {
      REMPI_ERR("incount != outcount in MPI_Waitall: incount = %d outcount = %d", incount, outcount);
    }
  }
  return ret;
}


int rempi_re_cdc::re_comm_split(MPI_Comm arg_0, int arg_1, int arg_2, MPI_Comm *arg_3)
{
  int ret;
  REMPI_DBG("MPI_Comm_split is not implemented yet");
  ret = PMPI_Comm_split(arg_0, arg_1, arg_2, arg_3);
  return ret;
}
  
int rempi_re_cdc::re_comm_create(MPI_Comm arg_0, MPI_Group arg_1, MPI_Comm *arg_2)
{
  int ret;
  REMPI_DBG("MPI_Comm_create is not implemented yet");
  ret = PMPI_Comm_create(arg_0, arg_1, arg_2);
  return ret;
}

int rempi_re_cdc::re_comm_dup(MPI_Comm arg_0, MPI_Comm *arg_2)
{
  int ret;   
  REMPI_DBG("MPI_Comm_create is not implemented yet");
  ret = PMPI_Comm_dup(arg_0, arg_2);
  return ret;
}


int rempi_re_cdc::collective_sync_clock(MPI_Comm comm) 
{
  int ret;
  if (rempi_mode == REMPI_ENV_REMPI_MODE_REPLAY) {
    recorder->set_fd_clock_state(1);
  }
  /*collectively sync clock*/
  ret = clmpi_collective_sync_clock(comm);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_REPLAY) {
    recorder->set_fd_clock_state(0);    
  }
  return ret;
}


int rempi_re_cdc::re_allreduce(const void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5)
{
  int ret;
  collective_sync_clock(arg_5);
  ret = PMPI_Allreduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
  recorder->fetch_and_update_local_min_id();
  return ret;  
}

int rempi_re_cdc::re_reduce(const void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, int arg_5, MPI_Comm arg_6)
{
  int ret;
  collective_sync_clock(arg_6);
  ret = PMPI_Reduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  recorder->fetch_and_update_local_min_id();
  return ret;
}

int rempi_re_cdc::re_scan(const void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5)
{
  int ret;
  collective_sync_clock(arg_5);
  ret = PMPI_Scan(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
  recorder->fetch_and_update_local_min_id();
  return ret; 
}

int rempi_re_cdc::re_allgather(const void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6)
{
  int ret;
  collective_sync_clock(arg_6);
  ret = PMPI_Allgather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  recorder->fetch_and_update_local_min_id();
  return ret;
}

int rempi_re_cdc::re_gatherv(const void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, const int *arg_4, const int *arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8)
{
  int ret;
  collective_sync_clock(arg_8);
  ret = PMPI_Gatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
  recorder->fetch_and_update_local_min_id();
  return ret;
}

int rempi_re_cdc::re_reduce_scatter(const void *arg_0, void *arg_1, const int *arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5)
{
  int ret;
  collective_sync_clock(arg_5);
  ret = PMPI_Reduce_scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
  recorder->fetch_and_update_local_min_id();
  return ret; 
}

int rempi_re_cdc::re_scatterv(const void *arg_0, const int *arg_1, const int *arg_2, MPI_Datatype arg_3, void *arg_4, int arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8)
{
  int ret;
  collective_sync_clock(arg_8);
  ret = PMPI_Scatterv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
  recorder->fetch_and_update_local_min_id();
  return ret;
}

int rempi_re_cdc::re_allgatherv(const void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, const int *arg_4, const int *arg_5, MPI_Datatype arg_6, MPI_Comm arg_7)
{
  int ret;
  collective_sync_clock(arg_7);
  ret = PMPI_Allgatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
  recorder->fetch_and_update_local_min_id();
  return ret; 
}

int rempi_re_cdc::re_scatter(const void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7)
{
  int ret;
  collective_sync_clock(arg_7);
  ret = PMPI_Scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
  recorder->fetch_and_update_local_min_id();
  return ret;
}

int rempi_re_cdc::re_bcast(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, MPI_Comm arg_4)
{
  int ret;
  collective_sync_clock(arg_4);
  ret = PMPI_Bcast(arg_0, arg_1, arg_2, arg_3, arg_4);
  recorder->fetch_and_update_local_min_id();
  return ret; 
}

int rempi_re_cdc::re_alltoall(const void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6)
{
  int ret;
  collective_sync_clock(arg_6);
  ret = PMPI_Alltoall(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  recorder->fetch_and_update_local_min_id();
  return ret;
}

int rempi_re_cdc::re_gather(const void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7)
{
  int ret;
  collective_sync_clock(arg_7);
  ret = PMPI_Gather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
  recorder->fetch_and_update_local_min_id();
  return ret;
}

int rempi_re_cdc::re_barrier(MPI_Comm arg_0)
{
  int ret;
  collective_sync_clock(arg_0);
  ret = PMPI_Barrier(arg_0);
  recorder->fetch_and_update_local_min_id();
  return ret;  
}

int rempi_re_cdc::re_finalize()
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



