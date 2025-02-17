#pragma once

#include "io/types.h"

PIM_C_BEGIN

bool Finder_IsOpen(Finder* fdr);
bool Finder_Begin(Finder* fdr, FinderData* data, const char* spec);
bool Finder_Next(Finder* fdr, FinderData* data);
// Always call End if you call Begin, or a leak occurs.
void Finder_End(Finder* fdr);

// Begin + Next + Close
bool Finder_Iterate(Finder* fdr, FinderData* data, const char* spec);

PIM_C_END
