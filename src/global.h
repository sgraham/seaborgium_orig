#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// base::strdup fails if this is |#define|d.
#undef strdup

#endif
