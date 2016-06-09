#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "rempi_err.h"
#include "rempi_config.h"

using namespace std;

int rempi_lite;
int rempi_mode = 0;
string rempi_record_dir_path = ".";
int rempi_encode;
int rempi_gzip;
int rempi_is_test_id;
int rempi_max_event_length;

void rempi_set_configuration(int *argc, char ***argv)
{
  char* env;
  int env_int;

#ifdef REMPI_LITE
  rempi_lite = 1;
#else
  rempi_lite = 0;
#endif

  if (NULL == (env = getenv(REMPI_ENV_NAME_MODE))) {
    rempi_dbg("getenv failed: Please specify REMPI_MODE (%s:%s:%d)", __FILE__, __func__, __LINE__);
    exit(0);
  }
  env_int = atoi(env);
  if (env_int == 0) {
    rempi_mode = 0;
  } else if (env_int == 1) {
    rempi_mode = 1;
  } else {
    rempi_dbg("Invalid value for REMPI_MODE: %s (%s:%s:%d)", env, __FILE__, __func__, __LINE__);
    exit(0);
  }

  if (NULL == (env = getenv(REMPI_ENV_NAME_DIR))) {
    // defoult: current dir
    rempi_record_dir_path =  ".";
  } else {
    rempi_record_dir_path = env;
  }

  if (NULL == (env = getenv(REMPI_ENV_NAME_ENCODE))) {
    rempi_dbg("getenv failed: Please specify REMPI_ENCODE (%s:%s:%d)", __FILE__, __func__, __LINE__);
    exit(0);
  } else {
    env_int = atoi(env);
    rempi_encode = env_int;
   }

  if (NULL == (env = getenv(REMPI_ENV_NAME_GZIP))) {
    rempi_dbg("getenv failed: Please specify REMPI_GZIP (%s:%s:%d)", __FILE__, __func__, __LINE__);
    exit(0);
  } else {
    env_int = atoi(env);
    rempi_gzip = env_int;
  }

  if (NULL == (env = getenv(REMPI_ENV_NAME_TEST_ID))) {
    rempi_dbg("getenv failed: Please specify REMPI_TEST_ID (%s:%s:%d)", __FILE__, __func__, __LINE__);
    exit(0);
  } else {
    env_int = atoi(env);
    rempi_is_test_id = env_int;
  }

  if (NULL == (env = getenv(REMPI_ENV_NAME_MAX_EVENT_LENGTH))) {
    rempi_max_event_length = REMPI_DEFAULT_MAX_EVENT_LENGTH;
  } else {
    env_int = atoi(env);
    rempi_max_event_length = env_int;
  }

  // if (rempi_mode == 1 && rempi_is_test_id == 1) {
  //   REMPI_DBG("REMPI_MODE=1 & REMPI_IS_TEST_IS=1 is not supported yet");
  // }


  return;
}


