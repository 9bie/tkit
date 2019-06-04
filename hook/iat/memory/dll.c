#include<windows.h>
#include<stdio.h>
#include<ImageHlp.h>
typedef int(WINAPI *PFN_MessageBoxA)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

int WINAPI My_MessageBoxA(
    HWND hWnd,
    LPCTSTR lpText,
    LPCTSTR lpCaption,
    UINT uType
){
    PFN_MessageBoxA OldMessageBox = NULL;
    return OldMessageBox(NULL, "You Are Hooked", "Warning", MB_OK);
    
}
BOOL hook_iat(LPCSTR szDllName, PROC pfnOrg, PROC pfnNew)
{
    HMODULE hMod;
    LPCSTR szLibName;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc; 
    PIMAGE_THUNK_DATA pThunk;
    DWORD dwOldProtect, dwRVA;
    PBYTE pAddr;
    ULONG size;

    hMod = GetModuleHandle(NULL);

    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hMod,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&size)

    for( ; pImportDesc->Name; pImportDesc++ )
    {

        szLibName = (LPCSTR)((DWORD)hMod + pImportDesc->Name);
        printf("%s\n", szLibName);
        if( !_stricmp(szLibName, szDllName) )
        {

            pThunk = (PIMAGE_THUNK_DATA)((DWORD)hMod + 
                                         pImportDesc->FirstThunk);


            for( ; pThunk->u1.Function; pThunk++ )
            {
                if( pThunk->u1.Function == (DWORD)pfnOrg )
                {

                    VirtualProtect((LPVOID)&pThunk->u1.Function, 
                                   4, 
                                   PAGE_EXECUTE_READWRITE, 
                                   &dwOldProtect);
                    pThunk->u1.Function = (DWORD)pfnNew; 
                    VirtualProtect((LPVOID)&pThunk->u1.Function, 
                                   4, 
                                   dwOldProtect, 
                                   &dwOldProtect);                      

                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH : 
            // // 获取original API的地址
            // g_pOrgFunc = GetProcAddress(GetModuleHandle(L"user32.dll"), 
            //                             "SetWindowTextW");
            // #ifdef _WIN64
            // ULONG_PTR TargetFunAddr = (ULONG_PTR)GetProcAddress(hModule, szFuncName);
            // PULONG_PTR lpAddr = NULL;
            // SIZE_T size = sizeof(PULONG_PTR);
            // #else
            // ULONG32  TargetFunAddr = (ULONG32)GetProcAddress(hModule, szFuncName);
            // PULONG32 lpAddr = NULL;
            // SIZE_T size = sizeof(PULONG32);
            // #endif
            // # hook
            // MySetWindowTextW并未定义成导出函数
            hook_iat("user32.dll", g_pOrgFunc, (PROC)My_MessageBoxA);
            break;

        case DLL_PROCESS_DETACH :
            // # unhook
            hook_iat("user32.dll", (PROC)My_MessageBoxA, g_pOrgFunc);
            break;
    }

    return TRUE;
}