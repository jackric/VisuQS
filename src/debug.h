#pragma once

#ifdef _DEBUG
//#define DEBUG(msg) std::cerr << #msg << std::endl;
#define DEBUG(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#else
#define DEBUG(...)
#endif
