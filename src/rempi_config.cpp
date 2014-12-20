#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "rempi_err.h"
#include "rempi_config.h"

using namespace std;

int rempi_mode = 0;
string rempi_record_dir_path = ".";

void rempi_set_configuration(int *argc, char ***argv)
{
  char* env;
  int env_int;

  if (NULL == (env = getenv(REMPI_ENV_NAME_MODE))) {
    rempi_dbgi(0, "getenv failed: REMPI_ENV_NAME_MODE (%s:%s:%d)", __FILE__, __func__, __LINE__);
    exit(0);
  }
  env_int = atoi(env);
  if (env_int == 0) {
    rempi_mode = 0;
  } else if (env_int == 1) {
    rempi_mode = 1;
  } else {
    rempi_dbgi(0, "No such value for REMPI_ENV_NAME_MODE: %s (%s:%s:%d)", env, __FILE__, __func__, __LINE__);
    exit(0);
  }

  if (NULL == (env = getenv(REMPI_ENV_NAME_DIR))) {
    // defoult: current dir
    rempi_record_dir_path =  ".";
  } else {
    rempi_record_dir_path = env;
  }

  return;
}


