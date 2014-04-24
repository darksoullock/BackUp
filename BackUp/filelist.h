const int MY_MAX_PATH = 1024;
#include <iostream>
#include <fstream>
#include <windows.h>
#include <list>
using namespace std;
bool strend(char*a, char*b)		//string a not ends with string 
{
    int l1,l2;
    l1 = strlen(a);
    l2 = strlen(b);
	if (l2>l1)
		return true;
    for (int i=0;i<l2;++i)
    {
        if (a[l1-l2+i]!=b[i])
            return true;
    }
    return false;
}
void __stdcall GetFileList(const char * cpath,int extc,char ** extv, list<char*> * r)
{
    


	if (GetFileAttributes(cpath)==-1)
		return;
	char path[MY_MAX_PATH];
	char pattern[MY_MAX_PATH];
	{							//path should ends with '\\'
		int l = strlen(cpath);
		strcpy_s<sizeof(path)>(path,cpath);
		if (path[l-1]!='\\')
		{
			path[l]='\\';       //strcat
			path[l+1]=0;
		}
	}
	if (GetFileAttributes(path)!=INVALID_FILE_ATTRIBUTES)
	{
		//SetCurrentDirectory(path);

		WIN32_FIND_DATA fd;
		BOOL b;
		strcpy_s<sizeof(pattern)>(pattern,path);
		strcat_s<sizeof(pattern)>(pattern,"*");
		HANDLE h = FindFirstFile(pattern,&fd);
		if (h!=INVALID_HANDLE_VALUE)
		{
			do
			{
				char cFileName[MY_MAX_PATH];
				strcpy_s<sizeof(cFileName)>(cFileName, path);
				strcat_s<sizeof(cFileName)>(cFileName, fd.cFileName);

				DWORD attr = GetFileAttributes(cFileName);
				
				if ((attr&FILE_ATTRIBUTE_DIRECTORY)&&(!(attr&FILE_ATTRIBUTE_SYSTEM))&&(!(attr&FILE_ATTRIBUTE_HIDDEN)))
				{
					if ((strend(cFileName,"..")&&strend(cFileName,".")))		//not current and parent folder
					{

						char buf[MY_MAX_PATH];
						strcpy_s<sizeof(buf)>(buf,cFileName);
						GetFileList(buf,extc, extv, r);		//recursion
					}
				}
				else
				{
					if ((attr&FILE_ATTRIBUTE_ARCHIVE)&&(!(attr&FILE_ATTRIBUTE_SYSTEM))&&(!(attr&FILE_ATTRIBUTE_HIDDEN)))
					{
						bool b = false;		//extension match ? 1 : 0
						for (int i=0;i< extc;++i)
						{
							if (!strcmp(extv[i],"*"))
							{
								b=true;
								break;
							}
							b = b || (!strend(cFileName,extv[i]));
						}
						if (b)	//if ok add to list
						{
							char *buf = new char[MY_MAX_PATH];
							strcpy_s(buf,MY_MAX_PATH,cFileName);
							r->push_front(buf);
						}
					}
				}
				//SetCurrentDirectory(path);
				b = FindNextFile(h,&fd);
			}
			while(b);
			FindClose(h);
		}
	}
}