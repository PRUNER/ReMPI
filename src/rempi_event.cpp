#include <iostream>
#include <vector>

#include "rempi_event.h"
#include "rempi_err.h"


using namespace std;

int rempi_event::max_size = sizeof(int) * REMPI_MPI_EVENT_INPUT_NUM;

void rempi_event::operator ++(int) {
  mpi_inputs[0]++;
}

bool rempi_event::operator ==(rempi_event event)
{
    if (this->mpi_inputs.size() != event.mpi_inputs.size()) {
      cerr << "something wrong !" << endl;
    }
    for (unsigned int i = 1; i < mpi_inputs.size(); i++) {
      if (this->mpi_inputs[i] != event.mpi_inputs[i]) {
	return false;
      }
    }
    return true;
}

char* rempi_event::serialize(size_t &size)
{
    int *serialized_data = new int[mpi_inputs.size()];
    this->print();
    fprintf(stderr, "\n");
    for (unsigned int i = 0; i < mpi_inputs.size(); i++) {
      serialized_data[i] = mpi_inputs[i];
    }
    size = mpi_inputs.size() * sizeof(int);
    return (char*)serialized_data;
}

void rempi_event::print() 
{
  fprintf(stderr, "<Event: ");
  for (int i = 0; i < mpi_inputs.size(); i++) {
    fprintf(stderr, "%d \t", mpi_inputs[i]);
  }
  fprintf(stderr, ">");
  return;
}


/*====== child class constructures ======*/

rempi_irecv_event::rempi_irecv_event(int event_counts, int count, int source, int tag, int comm, int request) {
    mpi_inputs.push_back(event_counts);
    mpi_inputs.push_back(count);
    mpi_inputs.push_back(source);
    mpi_inputs.push_back(tag);
    mpi_inputs.push_back(comm);
    mpi_inputs.push_back(request);
}

rempi_test_event::rempi_test_event(int event_counts, int is_testsome, int request, int flag, int source, int tag) {
    mpi_inputs.push_back(event_counts);
    mpi_inputs.push_back(is_testsome);
    //    mpi_inputs.push_back(request);
    mpi_inputs.push_back(flag);
    mpi_inputs.push_back(source);
    mpi_inputs.push_back(tag);
}
