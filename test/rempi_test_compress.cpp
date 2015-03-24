#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <algorithm>
#include <utility>

#include <zlib.h>

#include "../src/rempi_message_manager.h"
#include "../src/rempi_clock_delta_compression.h"
#include "../src/rempi_compress.h"
#include "../src/rempi_compression_util.h"
#include "../src/rempi_err.h"
#include "../src/rempi_util.h"

using namespace std;

string file;

void test_simple() {
  vector<rempi_message_identifier*> msg_ids;
  char* compressed_data;
  size_t compressed_size;
  // msg_ids.push_back(new rempi_message_identifier(2, 1, 1));
  // msg_ids.push_back(new rempi_message_identifier(1, 1, 1));
  // msg_ids.push_back(new rempi_message_identifier(3, 1, 1));
  // msg_ids.push_back(new rempi_message_identifier(1, 1, 1));
  // msg_ids.push_back(new rempi_message_identifier(2, 1, 1));
  // msg_ids.push_back(new rempi_message_identifier(3, 1, 1));
  // msg_ids.push_back(new rempi_message_identifier(1, 1, 1));
  // msg_ids.push_back(new rempi_message_identifier(3, 1, 1));
  //  compressed_data = rempi_compress::binary_compress(msg_ids, compressed_size);
}


// void test_clock_delta() {
//   map<int, rempi_message_identifier*> msg_ids_clocked;
//   vector<rempi_message_identifier*> msg_ids_observed;
//   char* compressed_data;
//   size_t compressed_size;
//   rempi_message_identifier *msg_id;
//   int id = 0;
//   double s_add, e_add, s_comp, e_comp;
// #define add_identifier(a, b, c, d)			\
//   msg_id = new rempi_message_identifier(a, b, c, d);	\
//   msg_ids_clocked[msg_id->clock] = msg_id;              \
//   msg_ids_observed.push_back(msg_id);

//   s_add = rempi_get_time();
// #if 1
//   for(int i = 0; i < 10000; i++) {
//     int delay = (i % 100 == 0)? 100:0;
//     add_identifier(i, 1, 1, i - delay);
//   }
// #else
//   add_identifier(2, 1, 1, 40);
//   add_identifier(1, 1, 1,  0);
//   add_identifier(3, 1, 1, 60);
//   add_identifier(1, 1, 1, 20);
//   add_identifier(2, 1, 1, 30);
//   add_identifier(3, 1, 1, 70);
//   add_identifier(1, 1, 1, 10);
//   add_identifier(3, 1, 1, 50);
// #endif
//   e_add = rempi_get_time();
//   REMPI_DBG("ADD: %f", e_add - s_add);

// #undef add_identifier
//   // for (int i = 0; i < msg_ids_observed.size(); i++) {
//   //   REMPI_DBG("%d\n", msg_ids_observed[i]);
//   // }
//   s_comp = rempi_get_time();
//   compressed_data = rempi_clock_delta_compression::compress(msg_ids_clocked, msg_ids_observed, compressed_size);
//   e_comp = rempi_get_time();
//   REMPI_DBG("COM: %f", e_comp - s_comp);

// #if 1
//   {
//     //    REMPI_DBG("size:%d", compressed_size);
//     int i;
//     int *compressed_data_int = (int*)compressed_data;
//     for (i = 0; i < compressed_size/4; i++) {
//       REMPI_DBG("  %d", compressed_data_int[i]);
//     }
//   }
// #endif
// }

// void test_clock_delta3() {
//   map<int, rempi_message_identifier*> msg_ids_clocked;
//   vector<rempi_message_identifier*> msg_ids_observed;
//   char* compressed_data;
//   size_t compressed_size;
//   rempi_message_identifier *msg_id;
//   int id = 0;
//   double s_add, e_add, s_comp, e_comp;
// #define add_identifier(a, b, c, d)			\
//   msg_id = new rempi_message_identifier(a, b, c, d);	\
//   msg_ids_clocked[msg_id->clock] = msg_id;              \
//   msg_ids_observed.push_back(msg_id);

//   s_add = rempi_get_time();
// #if 1
//   for(int i = 0; i < 10000; i++) {
//     int delay = (i % 100 == 0)? 100:0;
//     add_identifier(i, 1, 1, i - delay);
//   }
// #else
//   add_identifier(2, 1, 1, 40);
//   add_identifier(1, 1, 1,  0);
//   add_identifier(3, 1, 1, 60);
//   add_identifier(1, 1, 1, 20);
//   add_identifier(2, 1, 1, 30);
//   add_identifier(3, 1, 1, 70);
//   add_identifier(1, 1, 1, 10);
//   add_identifier(3, 1, 1, 50);
// #endif
//   e_add = rempi_get_time();
//   REMPI_DBG("ADD: %f", e_add - s_add);

// #undef add_identifier
//   // for (int i = 0; i < msg_ids_observed.size(); i++) {
//   //   REMPI_DBG("%d\n", msg_ids_observed[i]);
//   // }
//   s_comp = rempi_get_time();
//   compressed_data = rempi_clock_delta_compression::compress(msg_ids_clocked, msg_ids_observed, compressed_size);
//   e_comp = rempi_get_time();
//   REMPI_DBG("COM: %f", e_comp - s_comp);

// #if 1
//   {
//     //    REMPI_DBG("size:%d", compressed_size);
//     int i;
//     int *compressed_data_int = (int*)compressed_data;
//     for (i = 0; i < compressed_size/4; i++) {
//       REMPI_DBG("  %d", compressed_data_int[i]);
//     }
//   }
// #endif
// }


void split(std::vector<std::string> &v, const std::string &input_string, const std::string &delimiter)
{
  std::string::size_type index = input_string.find_first_of(delimiter);

  if (index != std::string::npos) {
    v.push_back(input_string.substr(0, index));
    split(v, input_string.substr(index + 1), delimiter);
  } else {
    v.push_back(input_string);
  }
}


bool test_clock_compare(const pair<int, int> &id1,
			const pair<int, int> &id2)
{
  if (id1.first == id2.first) {
    return id1.second < id2.second;
  }
  return id1.first < id2.first;
}

// void test_clock_delta2() {
//   map<int, rempi_message_identifier*> msg_ids_clocked;
//   vector<rempi_message_identifier*> msg_ids_observed;
//   char* compressed_data;
//   size_t compressed_size;
//   rempi_message_identifier *msg_id;
//   ifstream ifs(file);
//   string line;
//   int id = 0;
//   double s_add, e_add, s_comp, e_comp;
//   unordered_map<int, vector<pair<int,int>>*> map_rank_clock_order;

//   if (!ifs) {
//     REMPI_DBG("open error !");
//   }

//   while (getline(ifs, line)) {
//     vector<string> v1, v2;
//     int clock;
//     int source;
//     int rank;
//     split(v1, line, "\t");
//     split(v2, v1[4], "|");

//     clock = atoi(v1[0].c_str());
//     source = atoi(v1[1].c_str());
//     rank  = atoi(v2[1].c_str());

//     if (map_rank_clock_order.count(rank) == 0) {
//       map_rank_clock_order[rank] = new vector<pair<int, int>>();
//     }
// 	map_rank_clock_order[rank]->push_back(pair<int, int>(clock, source));
//       v1.clear();
//       v2.clear();
//   }

	
//   unordered_map<int, vector<pair<int,int>>*>::iterator m_it;
//   map<pair<int, int>, int> map_part_to_id;
//   for (m_it  = map_rank_clock_order.begin();
//        m_it != map_rank_clock_order.end();
//        m_it++) {
//     int rank = m_it->first;
//     vector<pair<int,int>> *vv = m_it->second;
//     vector<pair<int,int>> vv_sort;
//     /*Copy vector*/
//     for (int i = 0; i < vv->size(); i++) {
//       vv_sort.push_back(vv->at(i));
//     }
//     sort(vv_sort.begin(), vv_sort.end(), test_clock_compare);
//     /*Put ordering number*/
//     for (int i = 0; i < vv_sort.size(); i++) {
//       map_part_to_id.insert(make_pair(vv_sort[i] , i));
//     }
//     /*change from Clock to ordering number*/
//     for (int i = 0; i < vv->size(); i++) {
//       vv->at(i).first = map_part_to_id.at(make_pair(vv->at(i).first, vv->at(i).second));
//     }

//     // for (int i = 0; i < vv->size(); i++) {
//     //   REMPI_DBG("%d vv search %d", rank, vv->at(i).first);
//     // }	   
//   }


// #define add_identifier(a, b, c, d)			\
//   msg_id = new rempi_message_identifier(a, b, c, d);	\
//   msg_ids_clocked[msg_id->clock] = msg_id;              \
//   msg_ids_observed.push_back(msg_id);


//   for (m_it  = map_rank_clock_order.begin();
//        m_it != map_rank_clock_order.end();
//        m_it++) {
//     msg_ids_clocked.clear();
//     msg_ids_observed.clear();

//     int rank = m_it->first;
//     vector<pair<int,int>> *vv = m_it->second;
//     //s_add = rempi_get_time();  
//     for (int i = 0; i < vv->size(); i++) {
//       add_identifier(-1, -1, -1, vv->at(i).first);
//     }
//     //  e_add = rempi_get_time();
//     //  REMPI_DBG("ADD: %f", e_add - s_add);
//     s_comp = rempi_get_time();
//     //    REMPI_DBG("Compress search %d", rank);
//     compressed_data = rempi_clock_delta_compression::compress(msg_ids_clocked, msg_ids_observed, compressed_size);
//     e_comp = rempi_get_time();
//     //  REMPI_DBG("COM: %f", e_comp - s_comp);
//     {
//       ofstream fd(file + "." + to_string((long long)rank) + ".rempi-cdc", ofstream::binary);
//       fd.write(compressed_data, compressed_size);
//       fd.close();
//     }
// #if 1
//     {
//       REMPI_DBG("rank %d, size:%d (%d)", rank, compressed_size, msg_ids_observed.size() * 4);
//       int *compressed_data_int = (int*)compressed_data;
//       for (int i = 0; i < compressed_size/4; i++) {
// 	//	REMPI_DBG("  %d", compressed_data_int[i]);
//       }
//     }
// #endif

//   }


// #undef add_identifier


// }

void test_zlib(size_t length)
{
  vector<char*> input_vec, output_vec, output_vec2;
  vector<size_t>  input_size_vec, output_size_vec, output_size_vec2;
  size_t num_array = 8;
  size_t total_size = 0;

  rempi_compression_util<int> *cutil = new rempi_compression_util<int>();

  for (int i = 0; i < num_array; i++) {
    size_t size = sizeof(int) * length;
    int *int_array =  (int*)malloc(sizeof(int) * length);
    for (int j = 0; j < length; j++) {
      int_array[j] = rand() / 1000;
    }
    input_vec.push_back((char*)int_array);
    input_size_vec.push_back(size);
    total_size += size;
  }
  int *array = (int*)input_vec[0];
  size_t sum = 0;
  for (int i = 0; i < input_size_vec.size(); i++) {
    sum += input_size_vec[i];
  }
  //  fprintf(stderr, "Original size  : %10lu %d %lu\n", total_size, array[0], sum);
  cutil->compress_by_zlib_vec(input_vec, input_size_vec, output_vec, output_size_vec, total_size);
  sum = 0;
  for (int i = 0; i < output_size_vec.size(); i++) {
    sum += output_size_vec[i];
  }
  //  fprintf(stderr, "compressed size: %10lu length: %lu %lu\n", total_size, output_vec.size(), sum);
  // for (int i = 0; i < output_size_vec.size(); i++) {
  //   fprintf(stderr, "%lu\n", output_size_vec[i]);
  // }
  //  fprintf(stderr, "               : %p %lu\n", output_vec.front(). output_size_vec.front());
  // char*  a = output_vec.front();
  // size_t s = output_size_vec.front();
  // fprintf(stderr, "               : %p %lu\n", a, s);
  cutil->decompress_by_zlib_vec(output_vec, output_size_vec, output_vec2, output_size_vec2, total_size);
  array = (int*)output_vec[0];
  sum = 0;
  for (int i = 0; i < output_size_vec2.size(); i++) {
    sum += output_size_vec2[i];
  }
  //  fprintf(stderr, "Original size  : %10lu %d %lu %lu\n", total_size, array[0], sum, output_vec2.size());
  // char *final_char = (char*)malloc(total_size);
  // for (int i = 0; i < output_vec2.size(); i++) {
  //   memcpy(final_int, output_vec2[i], output_size_vec2[i]);
  //   final_int += output_size_vec2[i];
  // }

  // vector<>
  


  return;
}

int test_lp(int length)
{
  vector<int> test_vec;
  vector<int> test_vec2;
  // test_vec.push_back(1);
  // test_vec.push_back(3);
  // test_vec.push_back(2);
  // test_vec.push_back(5);
  for (int i = 0; i < length; i++) {
    test_vec.push_back(rand()/1000);
  }
  test_vec2 = test_vec;
  rempi_compression_util<int> *cutil = new rempi_compression_util<int>();  
  cutil->compress_by_linear_prediction(test_vec);
  cutil->decompress_by_linear_prediction(test_vec);
  for (int i = 0; i < test_vec.size(); i++) {
    if(test_vec2[i] != test_vec[i]) {
      fprintf(stderr, "Error in index: %d\n: should be %d but %d\n", i, test_vec2[i], test_vec[i]);
    }
  }
  return 0;
}

int test_bin(int length)
{
  vector<int> test_vec;
  vector<int> test_vec2;
  unsigned char* bin;
  size_t size;
  rempi_compression_util<int> *cutil = new rempi_compression_util<int>();  

   for (int i = 0; i < length; i++) {
        test_vec.push_back(rand() % 2);
   }
   test_vec2 = test_vec;
   bin = cutil->compress_by_zero_one_binary(test_vec, size);
   test_vec.clear();
   cutil->decompress_by_zero_one_binary(bin, test_vec2.size(), test_vec);

  for (int i = 0; i < test_vec2.size(); i++) {
    if(test_vec2[i] != test_vec[i]) {
      fprintf(stderr, "Error in index: %d: should be %d but %d\n", i, test_vec2[i], test_vec[i]);
    }
  }
  return 0;
}


int main(int argc, char* argv[]) {
#if 0
 if (argc != 2) {
   REMPI_ERR("a.out <file>");
 }
 file = string(argv[1]);
 // test_clock_delta2();
 // test_clock_delta3();
#endif
 for (int i = 1; i < 1024 * 1024; i = i*4) {
   for (int j = -3; j <= 3; j++) {
     //     REMPI_DBG("i: %d", i);
     int s = i + j;
     if (s < 1) continue;
     test_zlib(s);
   }
 }
 REMPI_DBG("Zib OK");
 for (int i = 1; i < 1000; i++) {
   test_lp(i);
 }
 REMPI_DBG("LP OK");
 for (int i = 1; i < 1000; i++) {
   test_bin(i);
 }
 REMPI_DBG("BIN OK");

 return 0;
}


