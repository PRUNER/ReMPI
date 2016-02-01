#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

#include "rempi_re.h"
#include "rempi_err.h"
#include "rempi_type.h"

#ifndef _EXTERN_C_
#ifdef __cplusplus
#define _EXTERN_C_ extern "C"
#else /* __cplusplus */
#define _EXTERN_C_
#endif /* __cplusplus */
#endif /* _EXTERN_C_ */

#ifdef MPICH_HAS_C2F
_EXTERN_C_ void *MPIR_ToPointer(int);
#endif // MPICH_HAS_C2F

#ifdef PIC
/* For shared libraries, declare these weak and figure out which one was linked
   based on which init wrapper was called.  See mpi_init wrappers.  */
#pragma weak pmpi_init
#pragma weak PMPI_INIT
#pragma weak pmpi_init_
#pragma weak pmpi_init__
#endif /* PIC */

_EXTERN_C_ void pmpi_init(MPI_Fint *ierr);
_EXTERN_C_ void PMPI_INIT(MPI_Fint *ierr);
_EXTERN_C_ void pmpi_init_(MPI_Fint *ierr);
_EXTERN_C_ void pmpi_init__(MPI_Fint *ierr);

#define PNMPI_MODULE_REMPI "rempi"



rempi_re *rempi_record_replay;


// int PNMPI_RegistrationPoint()
// {
//   int err;
//   // PNMPI_Service_descriptor_t service;
//   // PNMPI_Global_descriptor_t global;

//   /* register this module and its services */  
//   err = PNMPI_Service_RegisterModule(PNMPI_MODULE_REMPI);
//   if (err!=PNMPI_SUCCESS) {
//     return MPI_ERROR_PNMPI;
//   }

//   return err;
// }

void init_rempi() {
  //rempi_record_replay = new rempi_re_cdc();
  rempi_record_replay = new rempi_re();
  return;
}
// MPI_Init does all the communicator setup
//
/* ================== C Wrappers for MPI_Init ================== */
_EXTERN_C_ int PMPI_Init(int *arg_0, char ***arg_1);
_EXTERN_C_ int MPI_Init(int *arg_0, char ***arg_1)
{ 
  int _wrap_py_return_val = 0;
  init_rempi();
  _wrap_py_return_val = rempi_record_replay->re_init(arg_0, arg_1);
  return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Init_thread ================== */
_EXTERN_C_ int PMPI_Init_thread(int *arg_0, char ***arg_1, int arg_2, int *arg_3);
_EXTERN_C_ int MPI_Init_thread(int *arg_0, char ***arg_1, int arg_2, int *arg_3)
{ 
  int _wrap_py_return_val = 0;
  init_rempi();
  _wrap_py_return_val = rempi_record_replay->re_init_thread(arg_0, arg_1, arg_2, arg_3);
  return _wrap_py_return_val;
}


/* ================== C Wrappers for MPI_Recv ================== */
_EXTERN_C_ int PMPI_Recv(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Status *arg_6);
_EXTERN_C_ int MPI_Recv(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Status *arg_6) {
  int _wrap_py_return_val = 0;
  {
    /*TODO: Implementation witouh using MPI_Irecv*/
    MPI_Request req;

    //    _wrap_py_return_val = MPI_Irecv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, &req);
    _wrap_py_return_val = rempi_record_replay->re_irecv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, &req);
    REMPI_DBGI(9, "request: %p %p at %s", req, &req, __func__);
    //    _wrap_py_return_val = MPI_Wait(&req, arg_6);
    _wrap_py_return_val = rempi_record_replay->re_wait(&req, arg_6);

  } 
  return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Irecv ================== */
_EXTERN_C_ int PMPI_Irecv(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6);
_EXTERN_C_ int MPI_Irecv(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6)
{ 
  int _wrap_py_return_val = 0;
  _wrap_py_return_val = rempi_record_replay->re_irecv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  REMPI_DBGI(9, "request: %p %p at %s", *arg_6, arg_6, __func__);

  // int rank;
  // PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //  fprintf(stderr, "rank %d: irecv request: %p\n", rank, *arg_6);
  return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Test ================== */
_EXTERN_C_ int PMPI_Test(MPI_Request *arg_0, int *arg_1, MPI_Status *arg_2);
_EXTERN_C_ int MPI_Test(MPI_Request *arg_0, int *arg_1, MPI_Status *arg_2)
{ 
  int _wrap_py_return_val = 0;
  _wrap_py_return_val = rempi_record_replay->re_test(arg_0, arg_1, arg_2);
  return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Testany ================== */
_EXTERN_C_ int PMPI_Testany(int arg_0, MPI_Request *arg_1, int *arg_2, int *arg_3, MPI_Status *arg_4);
_EXTERN_C_ int MPI_Testany(int arg_0, MPI_Request *arg_1, int *arg_2, int *arg_3, MPI_Status *arg_4) {
  int _wrap_py_return_val = 0;
  {  

    _wrap_py_return_val = rempi_record_replay->re_testany(arg_0, arg_1, arg_2, arg_3, arg_4);
  }    return _wrap_py_return_val;
}

/* kento================== C Wrappers for MPI_Testsome ================== */
_EXTERN_C_ int PMPI_Testsome(int arg_0, MPI_Request *arg_1, int *arg_2, int *arg_3, MPI_Status *arg_4);
_EXTERN_C_ int MPI_Testsome(int arg_0, MPI_Request *arg_1, int *arg_2, int *arg_3, MPI_Status *arg_4)
{ 
  int _wrap_py_return_val = 0;
  _wrap_py_return_val = rempi_record_replay->re_testsome(arg_0, arg_1, arg_2, arg_3, arg_4);
  return _wrap_py_return_val;
}


/* ================== C Wrappers for MPI_Testall ================== */
_EXTERN_C_ int PMPI_Testall(int arg_0, MPI_Request *arg_1, int *arg_2, MPI_Status *arg_3);
_EXTERN_C_ int MPI_Testall(int arg_0, MPI_Request *arg_1, int *arg_2, MPI_Status *arg_3) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = rempi_record_replay->re_testall(arg_0, arg_1, arg_2, arg_3);
    REMPI_DBGI(9, "======= testall: flag: %d", *arg_2);
  }    return _wrap_py_return_val;
}



/* ================== C Wrappers for MPI_Wait ================== */
_EXTERN_C_ int PMPI_Wait(MPI_Request *arg_1, MPI_Status *arg_2);
_EXTERN_C_ int MPI_Wait(MPI_Request *arg_1, MPI_Status *arg_2) {
  int _wrap_py_return_val = 0;
  _wrap_py_return_val = rempi_record_replay->re_wait(arg_1, arg_2);
  return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Waitany ================== */
_EXTERN_C_ int PMPI_Waitany(int arg_0, MPI_Request *arg_1, int *arg_2, MPI_Status *arg_3);
_EXTERN_C_ int MPI_Waitany(int arg_0, MPI_Request *arg_1, int *arg_2, MPI_Status *arg_3) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = rempi_record_replay->re_waitany(arg_0, arg_1, arg_2, arg_3);
  } 
  return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Waitsome ================== */
_EXTERN_C_ int PMPI_Waitsome(int arg_0, MPI_Request *arg_1, int *arg_2, int *arg_3, MPI_Status *arg_4);
_EXTERN_C_ int MPI_Waitsome(int arg_0, MPI_Request *arg_1, int *arg_2, int *arg_3, MPI_Status *arg_4) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = rempi_record_replay->re_waitsome(arg_0, arg_1, arg_2, arg_3, arg_4);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Waitall ================== */
_EXTERN_C_ int PMPI_Waitall(int arg_0, MPI_Request *arg_1, MPI_Status *arg_2);
_EXTERN_C_ int MPI_Waitall(int arg_0, MPI_Request *arg_1, MPI_Status *arg_2) {
  int _wrap_py_return_val = 0;
  _wrap_py_return_val = rempi_record_replay->re_waitall(arg_0, arg_1, arg_2);
  return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Probe ================== */
_EXTERN_C_ int PMPI_Probe(int arg_0, int arg_1, MPI_Comm arg_2, MPI_Status *arg_3);
_EXTERN_C_ int MPI_Probe(int arg_0, int arg_1, MPI_Comm arg_2, MPI_Status *arg_3) {
  int _wrap_py_return_val = 0;
  {  
    _wrap_py_return_val = rempi_record_replay->re_probe(arg_0, arg_1, arg_2, arg_3);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iprobe ================== */
_EXTERN_C_ int PMPI_Iprobe(int arg_0, int arg_1, MPI_Comm arg_2, int *flag, MPI_Status *arg_4);
_EXTERN_C_ int MPI_Iprobe(int arg_0, int arg_1, MPI_Comm arg_2, int *flag, MPI_Status *arg_4) {
  int _wrap_py_return_val = 0;
  {   
    _wrap_py_return_val = rempi_record_replay->re_iprobe(arg_0, arg_1, arg_2, flag, arg_4);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cancel ================== */
_EXTERN_C_ int PMPI_Cancel(MPI_Request *arg_0);
_EXTERN_C_ int MPI_Cancel(MPI_Request *arg_0) {
  int _wrap_py_return_val = 0;
  {
    /*Message pooling is needed, and arg_0 is not used internal. so ignore this cancel*/
    //    REMPI_ERR("MPI_Cancel called");
    _wrap_py_return_val = rempi_record_replay->re_cancel(arg_0);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Request_free ================== */
_EXTERN_C_ int PMPI_Request_free(MPI_Request *arg_0);
_EXTERN_C_ int MPI_Request_free(MPI_Request *arg_0) {
  int _wrap_py_return_val = 0;
  {
    //    _wrap_py_return_val = PMPI_Request_free(arg_0);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_commit ================== */
_EXTERN_C_ int PMPI_Type_commit(MPI_Datatype *arg_0);
_EXTERN_C_ int MPI_Type_commit(MPI_Datatype *arg_0) {
  int _wrap_py_return_val = 0;
  {   
    _wrap_py_return_val = PMPI_Type_commit(arg_0);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_contiguous ================== */
_EXTERN_C_ int PMPI_Type_contiguous(int arg_0, MPI_Datatype arg_1, MPI_Datatype *arg_2);
_EXTERN_C_ int MPI_Type_contiguous(int arg_0, MPI_Datatype arg_1, MPI_Datatype *arg_2) {
  int _wrap_py_return_val = 0;
  {  
   
    _wrap_py_return_val = PMPI_Type_contiguous(arg_0, arg_1, arg_2);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_struct ================== */
#if MPI_VERSION == 3 && !defined(OPEN_MPI)
_EXTERN_C_ int PMPI_Type_struct(int arg_0, const int *arg_1, const MPI_Aint *arg_2, const MPI_Datatype *arg_3, MPI_Datatype *arg_4);
_EXTERN_C_ int MPI_Type_struct(int arg_0, const int *arg_1, const MPI_Aint *arg_2, const MPI_Datatype *arg_3, MPI_Datatype *arg_4) {
#else
_EXTERN_C_ int PMPI_Type_struct(int arg_0, int *arg_1, MPI_Aint *arg_2, MPI_Datatype *arg_3, MPI_Datatype *arg_4);
_EXTERN_C_ int MPI_Type_struct(int arg_0, int *arg_1, MPI_Aint *arg_2, MPI_Datatype *arg_3, MPI_Datatype *arg_4) {
#endif
  int _wrap_py_return_val = 0;
  {  
   
    _wrap_py_return_val = PMPI_Type_struct(arg_0, arg_1, arg_2, arg_3, arg_4);
  }    return _wrap_py_return_val;
}


/* ================== C Wrappers for MPI_Comm_split ================== */
_EXTERN_C_ int PMPI_Comm_split(MPI_Comm arg_0, int arg_1, int arg_2, MPI_Comm *arg_3);
_EXTERN_C_ int MPI_Comm_split(MPI_Comm arg_0, int arg_1, int arg_2, MPI_Comm *arg_3) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = rempi_record_replay->re_comm_split(arg_0, arg_1, arg_2, arg_3);
    //_wrap_py_return_val = PMPI_Comm_split(arg_0, arg_1, arg_2, arg_3);
  }    return _wrap_py_return_val;
}


/* ================== C Wrappers for MPI_Comm_create ================== */
_EXTERN_C_ int PMPI_Comm_create(MPI_Comm arg_0, MPI_Group arg_1, MPI_Comm *arg_2);
_EXTERN_C_ int MPI_Comm_create(MPI_Comm arg_0, MPI_Group arg_1, MPI_Comm *arg_2) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = rempi_record_replay->re_comm_create(arg_0, arg_1, arg_2);
    //    _wrap_py_return_val = PMPI_Comm_create(arg_0, arg_1, arg_2);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_dup ================== */
 _EXTERN_C_ int PMPI_Comm_dup(MPI_Comm arg_0, MPI_Comm *arg_1);
 _EXTERN_C_ int MPI_Comm_dup(MPI_Comm arg_0, MPI_Comm *arg_1) {
   int _wrap_py_return_val = 0;
   {
     _wrap_py_return_val = PMPI_Comm_dup(arg_0, arg_1);
   }    return _wrap_py_return_val;
 }


/* ================== C Wrappers for MPI_Finalize ================== */
_EXTERN_C_ int PMPI_Finalize();
_EXTERN_C_ int MPI_Finalize()
{ 
  int _wrap_py_return_val = 0;
  _wrap_py_return_val = rempi_record_replay->re_finalize();
  return _wrap_py_return_val;
}


/* ================== C Wrappers for MPI_Isend ================== */
_EXTERN_C_ int PMPI_Isend(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6);
_EXTERN_C_ int MPI_Isend(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = rempi_record_replay->re_isend(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
    REMPI_DBGI(9, "buf: %p, request: %p %p at %s", arg_0, *arg_6, arg_6, __func__);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Send ================== */
_EXTERN_C_ int PMPI_Send(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5);
_EXTERN_C_ int MPI_Send(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = PMPI_Send(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
  }
  return _wrap_py_return_val;
}


/* ==================================================================== */
/* ================== C Wrappers for MPI_Collectives ================== */
/* ==================================================================== */


/* ================== C Wrappers for MPI_Allreduce ================== */
_EXTERN_C_ int PMPI_Allreduce(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5);
_EXTERN_C_ int MPI_Allreduce(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5)
{
  int _wrap_py_return_val = 0;
  {
    //    fprintf(stderr, "======= %s =======\n", __func__);
    //    _wrap_py_return_val = PMPI_Allreduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
    _wrap_py_return_val = rempi_record_replay->re_allreduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
    //    fprintf(stderr, "======= %s end =======\n", __func__);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Reduce ================== */
_EXTERN_C_ int PMPI_Reduce(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, int arg_5, MPI_Comm arg_6);
_EXTERN_C_ int MPI_Reduce(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, int arg_5, MPI_Comm arg_6)
{
  int _wrap_py_return_val = 0;
  {
    //    _wrap_py_return_val = PMPI_Reduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
    _wrap_py_return_val = rempi_record_replay->re_reduce(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Scan ================== */
_EXTERN_C_ int PMPI_Scan(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5);
_EXTERN_C_ int MPI_Scan(rempi_mpi_version_void *arg_0, void *arg_1, int arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5) {
  int _wrap_py_return_val = 0;
  {
    //    _wrap_py_return_val = PMPI_Scan(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
    _wrap_py_return_val = rempi_record_replay->re_scan(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
    
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Allgather ================== */
_EXTERN_C_ int PMPI_Allgather(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6);
_EXTERN_C_ int MPI_Allgather(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6)
 {
  int _wrap_py_return_val = 0;
  { 
    //    fprintf(stderr, "======= %s =======\n", __func__); 
    //    _wrap_py_return_val = PMPI_Allgather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
    _wrap_py_return_val = rempi_record_replay->re_allgather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
    //    fprintf(stderr, "======= %s end =======\n", __func__);
  }    return _wrap_py_return_val;
}



/* ================== C Wrappers for MPI_Gatherv ================== */
 _EXTERN_C_ int PMPI_Gatherv(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, rempi_mpi_version_int *arg_4, rempi_mpi_version_int *arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8);
 _EXTERN_C_ int MPI_Gatherv(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, rempi_mpi_version_int *arg_4, rempi_mpi_version_int *arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Gatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
     _wrap_py_return_val = rempi_record_replay->re_gatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
   }    return _wrap_py_return_val;
 }

 /* ================== C Wrappers for MPI_Reduce_scatter ================== */
 _EXTERN_C_ int PMPI_Reduce_scatter(rempi_mpi_version_void *arg_0, void *arg_1, rempi_mpi_version_int *arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5);
 _EXTERN_C_ int MPI_Reduce_scatter(rempi_mpi_version_void *arg_0, void *arg_1, rempi_mpi_version_int *arg_2, MPI_Datatype arg_3, MPI_Op arg_4, MPI_Comm arg_5) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Reduce_scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
     _wrap_py_return_val = rempi_record_replay->re_reduce_scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
   }    return _wrap_py_return_val;
 }

 /* ================== C Wrappers for MPI_Scatterv ================== */
 _EXTERN_C_ int PMPI_Scatterv(rempi_mpi_version_void *arg_0, rempi_mpi_version_int *arg_1, rempi_mpi_version_int *arg_2, MPI_Datatype arg_3, void *arg_4, int arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8);
 _EXTERN_C_ int MPI_Scatterv(rempi_mpi_version_void *arg_0, rempi_mpi_version_int *arg_1, rempi_mpi_version_int *arg_2, MPI_Datatype arg_3, void *arg_4, int arg_5, MPI_Datatype arg_6, int arg_7, MPI_Comm arg_8) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Scatterv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
     _wrap_py_return_val = rempi_record_replay->re_scatterv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8);
   }    return _wrap_py_return_val;
 }

 /* ================== C Wrappers for MPI_Allgatherv ================== */
 _EXTERN_C_ int PMPI_Allgatherv(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, rempi_mpi_version_int *arg_4, rempi_mpi_version_int *arg_5, MPI_Datatype arg_6, MPI_Comm arg_7);
 _EXTERN_C_ int MPI_Allgatherv(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, rempi_mpi_version_int *arg_4, rempi_mpi_version_int *arg_5, MPI_Datatype arg_6, MPI_Comm arg_7) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Allgatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
     _wrap_py_return_val = rempi_record_replay->re_allgatherv(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
   }    return _wrap_py_return_val;
 }

 /* ================== C Wrappers for MPI_Scatter ================== */
 _EXTERN_C_ int PMPI_Scatter(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7);
 _EXTERN_C_ int MPI_Scatter(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
     _wrap_py_return_val = rempi_record_replay->re_scatter(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
   }    return _wrap_py_return_val;
 }


/* ================== C Wrappers for MPI_Bcast ================== */
_EXTERN_C_ int PMPI_Bcast(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, MPI_Comm arg_4);
_EXTERN_C_ int MPI_Bcast(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, MPI_Comm arg_4) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Bcast(arg_0, arg_1, arg_2, arg_3, arg_4);
     _wrap_py_return_val = rempi_record_replay->re_bcast(arg_0, arg_1, arg_2, arg_3, arg_4);
   }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Alltoall ================== */
 _EXTERN_C_ int PMPI_Alltoall(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6);
 _EXTERN_C_ int MPI_Alltoall(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, MPI_Comm arg_6) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Alltoall(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
     _wrap_py_return_val = rempi_record_replay->re_alltoall(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
   }    return _wrap_py_return_val;
 }

 /* ================== C Wrappers for MPI_Gather ================== */
 _EXTERN_C_ int PMPI_Gather(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7);
 _EXTERN_C_ int MPI_Gather(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, void *arg_3, int arg_4, MPI_Datatype arg_5, int arg_6, MPI_Comm arg_7) {
   int _wrap_py_return_val = 0;
   {
     //     _wrap_py_return_val = PMPI_Gather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
     _wrap_py_return_val = rempi_record_replay->re_gather(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
   }    return _wrap_py_return_val;
 }


/* ================== C Wrappers for MPI_Barrier ================== */
_EXTERN_C_ int PMPI_Barrier(MPI_Comm arg_0);
_EXTERN_C_ int MPI_Barrier(MPI_Comm arg_0) {
   int _wrap_py_return_val = 0;
   {
     //     fprintf(stderr, "======= %s =======\n", __func__);
     _wrap_py_return_val = rempi_record_replay->re_barrier(arg_0);
     //     fprintf(stderr, "======= %s end =======\n", __func__);
   }    return _wrap_py_return_val;
}



/* ================== C Wrappers for MPI_Recv_init ================== */
 _EXTERN_C_ int PMPI_Recv_init(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6);
 _EXTERN_C_ int MPI_Recv_init(void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6) {
   int _wrap_py_return_val = 0;
   {
     _wrap_py_return_val = PMPI_Recv_init(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
     REMPI_DBGI(9, "request: %p at %s", *arg_6, __func__);
   }    return _wrap_py_return_val;
 }

/* ================== C Wrappers for MPI_Send_init ================== */
_EXTERN_C_ int PMPI_Send_init(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6);
_EXTERN_C_ int MPI_Send_init(rempi_mpi_version_void *arg_0, int arg_1, MPI_Datatype arg_2, int arg_3, int arg_4, MPI_Comm arg_5, MPI_Request *arg_6) {
  int _wrap_py_return_val = 0;
  {
     _wrap_py_return_val = PMPI_Send_init(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
     REMPI_DBGI(9, "request: %p at %s", *arg_6, __func__);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Start ================== */
_EXTERN_C_ int PMPI_Start(MPI_Request *arg_0);
_EXTERN_C_ int MPI_Start(MPI_Request *arg_0) {
  int _wrap_py_return_val = 0;
  {
    _wrap_py_return_val = PMPI_Start(arg_0);
     REMPI_DBGI(9, "request: %p at %s", *arg_0, __func__);
  }    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Startall ================== */
 _EXTERN_C_ int PMPI_Startall(int arg_0, MPI_Request *arg_1);
 _EXTERN_C_ int MPI_Startall(int arg_0, MPI_Request *arg_1) {
   int _wrap_py_return_val = 0;
   {
     _wrap_py_return_val = PMPI_Startall(arg_0, arg_1);
     REMPI_DBGI(9, "request: %p at %s", *arg_1, __func__);
   }    return _wrap_py_return_val;
 }








