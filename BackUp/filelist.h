#include <iostream>
#include <windows.h>
#include <list>
using namespace std;
void GetFileList(const char * cpath, list<char*> * r)
{
    char path[MAX_PATH];
	{							//path should ends with '\\'
		int l = strlen(cpath);
        strcpy(path,cpath);
        if (path[l-1]!='\\')
        {
            path[l]='\\';       //strcat
            path[l+1]=0;
        }
    }
    if (GetFileAttributes(path)!=INVALID_FILE_ATTRIBUTES)
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
                        strcat(buf,fd.cFileName);
                        GetFileList(buf,r);
                    }
                }
                else
                {
                    if (attr&FILE_ATTRIBUTE_ARCHIVE)    //only changed files need
                    {
                        char *buf = new char[MAX_PATH];
                        strcpy(buf,path);
                        strcat(buf,fd.cFileName);
                        r->push_front(buf);
                    }
                }
                SetCurrentDirectory(path);
                b = FindNextFile(h,&fd);
            }
            while(b);
        CloseHandle(h);
    }
}
/*int main()
{
    list<char*> *a = new list<char*>();
    GetFileList("F:\\",a);
    list<char*>::iterator it;
    for (it = a->begin(); it!=a->end(); ++it)
        cout << *it << endl;
    return 0;
}*/
