#pragma once

#include "common/macro.h"

PIM_C_BEGIN

PIM_FWD_DECL(GLFWwindow);

void UiSys_Init(GLFWwindow* window);
void UiSys_BeginFrame(void);
void UiSys_EndFrame(void);
void UiSys_Shutdown(void);

PIM_C_END
