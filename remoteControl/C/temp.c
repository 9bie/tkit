#include <stdio.h>
#include <Winsock2.h>
 
int main()
{
    char szPath[128] = "";
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    gethostname(szPath, sizeof(szPath));
    printf("%s\n", szPath);
    WSACleanup();
    system("pause");
    return 0;
}
