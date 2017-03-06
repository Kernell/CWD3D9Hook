#include <list>

#include <d3d9.h>
#include <d3dx9core.h>

#include "../hook/detours.h"

typedef struct _LSA_UNICODE_STRING
{
  USHORT Length;
  USHORT MaximumLength;
  LPWSTR  Buffer;
}
LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;