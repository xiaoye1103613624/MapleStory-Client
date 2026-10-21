#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#import "D:\\software\\Microsoft Visual Studio\\18\\Community\\Common7\\IDE\\msdia140.dll" no_namespace raw_interfaces_only

int wmain(int argc, wchar_t** argv) {
  DWORD rva = (DWORD)wcstoul(argv[2], 0, 16);
  CoInitialize(0);
  HMODULE h = LoadLibraryW(L"D:\\software\\Microsoft Visual Studio\\18\\Community\\Common7\\IDE\\msdia140.dll");
  using F = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, void**);
  auto get = (F)GetProcAddress(h, "DllGetClassObject");
  IClassFactory* f=0; get(__uuidof(DiaSource), IID_IClassFactory, (void**)&f);
  IDiaDataSource* src=0; f->CreateInstance(0, __uuidof(IDiaDataSource), (void**)&src); f->Release();
  src->loadDataFromPdb(argv[1]);
  IDiaSession* s=0; src->openSession(&s);
  IDiaEnumLineNumbers* lines=0;
  s->findLinesByRVA(rva, 8, &lines);
  IDiaLineNumber* line=0; ULONG n=0;
  while (lines && SUCCEEDED(lines->Next(1,&line,&n)) && n) {
    DWORD ln=0, lrva=0; line->get_lineNumber(&ln); line->get_relativeVirtualAddress(&lrva);
    IDiaSourceFile* sf=0; line->get_sourceFile(&sf);
    BSTR fn=0; if(sf) sf->get_fileName(&fn);
    wprintf(L"line %u rva 0x%X %s\n", ln, lrva, fn?fn:L"?");
    if(fn) SysFreeString(fn); if(sf) sf->Release(); line->Release();
  }
  // Also print locals? skip
  return 0;
}
