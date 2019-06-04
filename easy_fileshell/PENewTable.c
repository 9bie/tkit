#include <windows.h>
#include <stdio.h>
char PATH[]="d:\\2.exe";
char shellcode[]="";
int Filelength(FILE *fp);
int main(int argc, char const *argv[])
{
    DWORD dwRead;
    HANDLE hFile;
    int FileSize;
    char * FileBuffer;
    hFile = CreateFile(PATH,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    FileSize=GetFileSize(hFile,NULL);
    FileBuffer = (char*)malloc(FileSize+1);
    if(!ReadFile(hFile,FileBuffer,FileSize,&dwRead,NULL))return 1;
    FileBuffer[FileSize+1]='\0';//多写一位，当结束标识

    PIMAGE_FILE_HEADER MFHeader;
    //这里是指针偏移,也就是绕过开头的DOS区块。DOS块从长度是0x3c,0x4是指向IMAGE_FILE_HEADER
    MFHeader = (PIMAGE_FILE_HEADER)(char *)(FileBuffer + *(int *)(FileBuffer + 0x3c) + 0x4);
    //或者使用另一种写法
    //fseek(hFile,0x3c,SEEK_SET)这种写法可能会更好些？
    PIMAGE_OPTIONAL_HEADER  MOptHeader;//定位可选PE头
    MOptHeader = (PIMAGE_OPTIONAL_HEADER)((char *)MFHeader + 0x14);
    //定位节表
    PIMAGE_SECTION_HEADER MSecHeader;
    MSecHeader = (PIMAGE_SECTION_HEADER)((char *)MOptHeader + MFHeader->SizeOfOptionalHeader);
    char * ImageBuffer = (char *)malloc(MOptHeader->SizeOfImage+0x1000);
    //内存中整个PE映像体的尺寸
    //模拟PE程序被加载
    //同理我们也可以使用MapViewOfFile
    ZeroMemory(ImageBuffer, MOptHeader->SizeOfImage+0x1000);
    if (ImageBuffer == NULL)return 1;
    memcpy(ImageBuffer, FileBuffer, MOptHeader->SizeOfHeaders);
    //拷贝原有节表的数据到我们的内存空间
    for (int i = 0; i < MFHeader->NumberOfSections; i++)
    {
        memcpy(ImageBuffer + MSecHeader->VirtualAddress, FileBuffer + MSecHeader->PointerToRawData, MSecHeader->SizeOfRawData);
        MSecHeader++;
    }
    //构建新表
    PIMAGE_SECTION_HEADER nSec;
    nSec = MSecHeader;
    MSecHeader--;
    //nSec->Name = ".xxx";
    //不用这种方式是因为C++后面字符会有\0作为截断字符也一并会被写进去
    nSec->Name[0] = '.';
    nSec->Name[1] = 'd';
    nSec->Name[2] = 'a';
    nSec->Name[3] = 't';
    nSec->Misc.VirtualSize = 0x1000;
    nSec->VirtualAddress = MSecHeader->VirtualAddress + MSecHeader->SizeOfRawData;
    nSec->SizeOfRawData = 0x1000;
    nSec->PointerToRawData = MSecHeader->PointerToRawData + MSecHeader->SizeOfRawData;
    nSec->Characteristics=(MSecHeader - (MFHeader->NumberOfSections-1))->Characteristics;
    MFHeader->NumberOfSections+=1;
    MOptHeader->SizeOfImage+0x1000;
    //新节构建完成
    


    return 0;
}
