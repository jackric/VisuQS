#pragma once

#ifdef _DEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#else
#define DEBUG(...) for(;;){{}
#endif
