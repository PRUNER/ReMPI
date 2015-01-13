#include <vector>
#include <set>
#include <map>



#include "../src/rempi_message_manager.h"
#include "../src/rempi_clock_delta_compression.h"
#include "../src/rempi_compress.h"
#include "../src/rempi_err.h"


void test_simple() {
  vector<rempi_message_identifier*> msg_ids;
  char* compressed_data;
  size_t compressed_size;
  msg_ids.push_back(new rempi_message_identifier(2, 1, 1));
  msg_ids.push_back(new rempi_message_identifier(1, 1, 1));
  msg_ids.push_back(new rempi_message_identifier(3, 1, 1));
  msg_ids.push_back(new rempi_message_identifier(1, 1, 1));
  msg_ids.push_back(new rempi_message_identifier(2, 1, 1));
  msg_ids.push_back(new rempi_message_identifier(3, 1, 1));
  msg_ids.push_back(new rempi_message_identifier(1, 1, 1));
  msg_ids.push_back(new rempi_message_identifier(3, 1, 1));
  //  compressed_data = rempi_compress::binary_compress(msg_ids, compressed_size);
}


void test_clock_delta() {
  map<int, rempi_message_identifier*> msg_ids_clocked;
  vector<rempi_message_identifier*> msg_ids_observed;
  char* compressed_data;
  size_t compressed_size;
  rempi_message_identifier *msg_id;
  int id = 0;
#define add_identifier(a, b, c, d)			\
  msg_id = new rempi_message_identifier(a, b, c, d);	\
  msg_ids_clocked[msg_id->clock] = msg_id;              \
  msg_ids_observed.push_back(msg_id);

  add_identifier(2, 1, 1, 40);
  add_identifier(1, 1, 1,  0);
  add_identifier(3, 1, 1, 60);
  add_identifier(1, 1, 1, 20);
  add_identifier(2, 1, 1, 30);
  add_identifier(3, 1, 1, 70);
  add_identifier(1, 1, 1, 10);
  add_identifier(3, 1, 1, 50);
#undef add_identifier
  // for (int i = 0; i < msg_ids_observed.size(); i++) {
  //   REMPI_DBG("%d\n", msg_ids_observed[i]);
  // }
  compressed_data = rempi_clock_delta_compression::compress(msg_ids_clocked, msg_ids_observed, compressed_size);
}


int main(int argc, char* argv[]) {

  test_clock_delta();
  return 0;
}


