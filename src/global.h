#ifdef _DEBUG

// We don't use _CRTDBG_MAP_ALLOC for this to avoid redefining strdup, which
// base uses as base::strdup. At the bottom of the file we do define it still
// though which avoids things that other headers do.
#define   malloc(s)             _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   calloc(c, s)          _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   realloc(p, s)         _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _recalloc(p, c, s)    _recalloc_dbg(p, c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _expand(p, s)         _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   free(p)               _free_dbg(p, _NORMAL_BLOCK)
#define   _msize(p)             _msize_dbg(p, _NORMAL_BLOCK)
#define   _aligned_msize(p, a, o)                   _aligned_msize_dbg(p, a, o)
#define   _aligned_malloc(s, a)                     _aligned_malloc_dbg(s, a, __FILE__, __LINE__)
#define   _aligned_realloc(p, s, a)                 _aligned_realloc_dbg(p, s, a, __FILE__, __LINE__)
#define   _aligned_recalloc(p, c, s, a)             _aligned_recalloc_dbg(p, c, s, a, __FILE__, __LINE__)
#define   _aligned_offset_malloc(s, a, o)           _aligned_offset_malloc_dbg(s, a, o, __FILE__, __LINE__)
#define   _aligned_offset_realloc(p, s, a, o)       _aligned_offset_realloc_dbg(p, s, a, o, __FILE__, __LINE__)
#define   _aligned_offset_recalloc(p, c, s, a, o)   _aligned_offset_recalloc_dbg(p, c, s, a, o, __FILE__, __LINE__)
#define   _aligned_free(p)  _aligned_free_dbg(p)

#define   _malloca(s)        _malloca_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _freea(p)          _freea_dbg(p, _NORMAL_BLOCK)

#define   _strdup(s)         _strdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wcsdup(s)         _wcsdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _mbsdup(s)         _strdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _tempnam(s1, s2)   _tempnam_dbg(s1, s2, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wtempnam(s1, s2)  _wtempnam_dbg(s1, s2, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _fullpath(s1, s2, le)     _fullpath_dbg(s1, s2, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wfullpath(s1, s2, le)    _wfullpath_dbg(s1, s2, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _getcwd(s, le)      _getcwd_dbg(s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wgetcwd(s, le)     _wgetcwd_dbg(s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _getdcwd(d, s, le)  _getdcwd_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wgetdcwd(d, s, le) _wgetdcwd_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _getdcwd_nolock(d, s, le)     _getdcwd_lk_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wgetdcwd_nolock(d, s, le)    _wgetdcwd_lk_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _dupenv_s(ps1, size, s2)      _dupenv_s_dbg(ps1, size, s2, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wdupenv_s(ps1, size, s2)     _wdupenv_s_dbg(ps1, size, s2, _NORMAL_BLOCK, __FILE__, __LINE__)

#include <stdlib.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC

#endif
