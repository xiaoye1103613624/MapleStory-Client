#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstdlib>

#import "D:\\software\\Microsoft Visual Studio\\18\\Community\\Common7\\IDE\\msdia140.dll" \
	no_namespace raw_interfaces_only rename("this", "This")

int wmain(int argc, wchar_t** argv)
{
	if (argc < 3) {
		wprintf(L"usage: dia_resolve.exe <pdb> <rva_hex>\n");
		return 1;
	}

	DWORD rva = static_cast<DWORD>(wcstoul(argv[2], nullptr, 16));
	CoInitialize(nullptr);

	IDiaDataSource* source = nullptr;
	HRESULT hr = CoCreateInstance(__uuidof(DiaSource), nullptr, CLSCTX_INPROC_SERVER,
		__uuidof(IDiaDataSource), (void**)&source);
	if (FAILED(hr)) {
		// registration-free
		HMODULE h = LoadLibraryW(L"D:\\software\\Microsoft Visual Studio\\18\\Community\\Common7\\IDE\\msdia140.dll");
		using DllGetClassObject_t = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, void**);
		auto pGet = reinterpret_cast<DllGetClassObject_t>(GetProcAddress(h, "DllGetClassObject"));
		IClassFactory* factory = nullptr;
		hr = pGet(__uuidof(DiaSource), IID_IClassFactory, (void**)&factory);
		if (FAILED(hr)) { wprintf(L"DllGetClassObject %08X\n", hr); return 2; }
		hr = factory->CreateInstance(nullptr, __uuidof(IDiaDataSource), (void**)&source);
		factory->Release();
		if (FAILED(hr)) { wprintf(L"CreateInstance %08X\n", hr); return 3; }
	}

	hr = source->loadDataFromPdb(argv[1]);
	if (FAILED(hr)) { wprintf(L"loadDataFromPdb %08X\n", hr); return 4; }

	IDiaSession* session = nullptr;
	hr = source->openSession(&session);
	if (FAILED(hr)) { wprintf(L"openSession %08X\n", hr); return 5; }

	IDiaSymbol* sym = nullptr;
	hr = session->findSymbolByRVA(rva, SymTagFunction, &sym);
	if (FAILED(hr) || !sym) {
		hr = session->findSymbolByRVA(rva, SymTagNull, &sym);
	}
	if (FAILED(hr) || !sym) {
		wprintf(L"findSymbolByRVA failed %08X\n", hr);
		return 6;
	}

	BSTR name = nullptr;
	sym->get_name(&name);
	DWORD symrva = 0;
	sym->get_relativeVirtualAddress(&symrva);
	wprintf(L"RVA 0x%X -> %s (symrva 0x%X, delta %+d)\n",
		rva, name ? name : L"(null)", symrva, (int)rva - (int)symrva);

	// Also line number
	IDiaEnumLineNumbers* lines = nullptr;
	if (SUCCEEDED(session->findLinesByRVA(rva, 1, &lines)) && lines) {
		IDiaLineNumber* line = nullptr;
		ULONG fetched = 0;
		if (SUCCEEDED(lines->Next(1, &line, &fetched)) && line) {
			DWORD ln = 0;
			line->get_lineNumber(&ln);
			IDiaSourceFile* sf = nullptr;
			if (SUCCEEDED(line->get_sourceFile(&sf)) && sf) {
				BSTR fn = nullptr;
				sf->get_fileName(&fn);
				wprintf(L"  %s:%u\n", fn ? fn : L"?", ln);
				if (fn) SysFreeString(fn);
				sf->Release();
			}
			line->Release();
		}
		lines->Release();
	}

	if (name) SysFreeString(name);
	sym->Release();
	session->Release();
	source->Release();
	return 0;
}
