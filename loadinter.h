#ifndef LOADINTER_H
#define LOADINTER_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct interface_header_tag {
  unsigned long HeaderSize;
} INTERFACE_HEADER;

typedef __declspec(dllexport) int (__stdcall  *LOADINTERFACE)(INTERFACE_HEADER *h);
__declspec(dllexport) int __stdcall  ProvideInterface(INTERFACE_HEADER *h);
// return 0 if ok


#ifdef __cplusplus
}
#endif

template<typename T> int tLoadDLL(LPCSTR dllname, HMODULE &h, T *bi)
{
  int i = 1;
  h = LoadLibrary(dllname);
  if (h) {
    LOADINTERFACE pi = (LOADINTERFACE)GetProcAddress(h,
        "ProvideInterface");
    if (!pi)
      pi = (LOADINTERFACE)GetProcAddress(h, "_ProvideInterface@4");
    if (pi) {
      bi->Header.HeaderSize = sizeof(T);
      i = (*pi)((INTERFACE_HEADER*)bi);
    }

    if (i) {
       FreeLibrary(h);
       h = NULL;
       return i;
    }
  }
  else
    i = GetLastError();
  return i;
}

#endif


