#include<windows.h>
#include<stdio.h>
#include "Tlhelp32.h"
#include<ImageHlp.h>
#include <string.h>
#pragma comment(lib, "ImageHlp")
typedef int(WINAPI *PFN_MessageBoxA)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
PFN_MessageBoxA OldMessageBox = NULL;
WINUSERAPI

int

WINAPI

MessageBoxA(

    HWND hWnd ,

    LPCSTR lpText,

    LPCSTR lpCaption,

UINT uType);

int WINAPI My_MessageBoxA(
    HWND hWnd,
    LPCTSTR lpText,
    LPCTSTR lpCaption,
    UINT uType
){
    
    return OldMessageBox(NULL, "You Are Hooked", "Warning", MB_OK);
    
}

void hook_iat(){
    ULONG size;
 
    LPCSTR szLibName;

    HMODULE hModule=GetModuleHandle(NULL);

    PIMAGE_IMPORT_DESCRIPTOR pImportDesc; 

    PIMAGE_THUNK_DATA pThunk;
    DWORD dwOldProtect;
 
    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hModule,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&size);

    FARPROC pfnOrg = GetProcAddress(GetModuleHandle("USER32.dll"), 
                                        "MessageBoxA");
    printf("%p\n",pfnOrg );
    for( ; pImportDesc->Name; pImportDesc++ ){
        szLibName = (LPCSTR)((DWORD)hModule + pImportDesc->Name);
        printf("%s\n", szLibName);
        if( _stricmp(szLibName, "USER32.dll")==0){
        pThunk = (PIMAGE_THUNK_DATA)((DWORD)hModule + 
                                         pImportDesc->FirstThunk);
        for( ; pThunk->u1.Function; pThunk++ )
            {
                printf("u1.Function:%p\npfnOrg:%p\n",pThunk->u1.Function, pfnOrg);
                
                if(pThunk->u1.Function==(DWORD)pfnOrg )
                {


                    // 修改内存属性为 E R W
                    VirtualProtect((LPVOID)&pThunk->u1.Function, 
                                   4, 
                                   PAGE_EXECUTE_READWRITE, 
                                   &dwOldProtect);
                    OldMessageBox = pThunk->u1.Function;
                    // // 修改IAT值为新的函数地址
                    pThunk->u1.Function = &My_MessageBoxA;
                    
                    // // 恢复内存属性为只读
                    VirtualProtect((LPVOID)&pThunk->u1.Function, 
                                   4, 
                                   dwOldProtect, 
                                   &dwOldProtect);                      

                    printf("changed:%p\n",pThunk->u1.Function );
                }
            }
    }

    
    }
}

int main(int argc, char const *argv[])
{
    MessageBoxA(NULL, "This is a test function.", "Seem's ok", MB_OK);
    foreach_iat();
    
    
    MessageBoxA(NULL, "This is a test function.", "Seem's ok", MB_OK);
    return 0;
}