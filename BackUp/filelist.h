#include <iostream>
#include <windows.h>
void printFiles(char * path)
{
    DWORD a = GetFileAttributes(path);
    if (a!=INVALID_FILE_ATTRIBUTES)
    {
        SetCurrentDirectory(path);
        WIN32_FIND_DATA fd;
        bool b;
        HANDLE h = FindFirstFile("*",&fd);
        if (h!=INVALID_HANDLE_VALUE)
            do
            {
                DWORD attr = GetFileAttributes(fd.cFileName);

                if (attr&FILE_ATTRIBUTE_DIRECTORY)
                {
                    if ((strcmp(fd.cFileName,"..")&&strcmp(fd.cFileName,".")))
                    {
                        char buf[MAX_PATH];
                        strcpy(buf,path);
                        strcat(buf,"\\");
                        strcat(buf,fd.cFileName);
                        printFiles(buf);
                    }
                }
                else
                    std::cout << path << '\\' << fd.cFileName << std::endl;
                SetCurrentDirectory(path);
                b = FindNextFile(h,&fd);
            }
            while(b);
        CloseHandle(h);
    }
}