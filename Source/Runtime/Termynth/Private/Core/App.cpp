#include "Core/App.h"

bool App::s_bUseFixedDeltaTime = true;
double App::s_FixedDeltaTime = 0;
double App::s_CurrentTime = 0;
double App::s_LastTime = 0;
double App::s_DeltaTime = 0;

double App::s_DeltaTimeAccumulator = 0;