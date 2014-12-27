/*
 * event.h
 *
 *  Created on: Nov 7, 2014
 *      Author: sato5
 */

#ifndef REMPI_EVENT_H_
#define REMPI_EVENT_H_

#include <iostream>
#include <vector>

#define REMPI_MPI_EVENT_INPUT_NUM (6)
#define REMPI_MPI_EVENT_INPUT_INDEX_EVENT_COUNTS (0)
#define REMPI_MPI_EVENT_INPUT_INDEX_IS_TESTSOME  (1)
#define REMPI_MPI_EVENT_INPUT_INDEX_COMM_ID      (2)
#define REMPI_MPI_EVENT_INPUT_INDEX_FLAG         (3)
#define REMPI_MPI_EVENT_INPUT_INDEX_SOURCE       (4)
#define REMPI_MPI_EVENT_INPUT_INDEX_TAG          (5)

using namespace std;

class rempi_event
{
  public:
    static int max_size;
    vector<int> mpi_inputs;

    virtual void operator ++(int);
    virtual bool operator ==(rempi_event event);
    virtual void push(rempi_event* event);
    virtual rempi_event* pop();
    virtual int size();
    virtual char* serialize(size_t &size);
    
    virtual int get_event_counts();
    virtual int get_is_testsome();
    virtual int get_comm_id();
    virtual int get_flag();
    virtual int get_source();
    virtual int get_tag();
    void print();
};


class rempi_irecv_event : public rempi_event
{
  public:
    rempi_irecv_event(int event_counts, int count, int source, int tag, int comm, int request);
};

class rempi_test_event : public rempi_event
{
  public:
    rempi_test_event(int event_counts, int is_testsome, int request, int flag, int source, int tag);
};

#endif /* REMPI_EVENT_H_ */
