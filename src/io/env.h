#pragma once

#include "common/macro.h"

PIM_C_BEGIN

bool Env_Search(const char* filename, const char* varname, char* dst);
const char* Env_Get(const char* varname);
bool Env_Set(const char* varname, const char* value);

PIM_C_END
