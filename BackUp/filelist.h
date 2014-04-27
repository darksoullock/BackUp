const int MY_MAX_PATH = 1024;
#include <iostream>
#include <fstream>
#include <windows.h>
#include <list>
using namespace std;
bool strend(TCHAR *a, TCHAR*b)		//string a not ends with string 
{
    int l1,l2;
    l1 = _tcslen(a);
    l2 = _tcslen(b);
	if (l2>l1)
		return true;
    for (int i=0;i<l2;++i)
    {
        if (a[l1-l2+i]!=b[i])
            return true;
    }
    return false;
}
void __stdcall GetFileList(const TCHAR * cpath,int extc,TCHAR ** extv, list<TCHAR*> * r)
{
    


	if (GetFileAttributes(cpath)==-1)
		return;
	TCHAR path[MY_MAX_PATH];
	TCHAR pattern[MY_MAX_PATH];
	{							//path should ends with '\\'
		int l = _tcslen(cpath);
		_tcscpy_s<sizeof(path)/sizeof(TCHAR)>(path,cpath);
		if (path[l-1]!=_T('\\'))
		{
			path[l]=_T('\\');       //strcat
			path[l+1]=0;
		}
	}
	if (GetFileAttributes(path)!=INVALID_FILE_ATTRIBUTES)
	{
		//SetCurrentDirectory(path);

		WIN32_FIND_DATA fd;
		BOOL b;
		_tcscpy_s<sizeof(pattern)/sizeof(TCHAR)>(pattern,path);
		_tcscat_s<sizeof(pattern)/sizeof(TCHAR)>(pattern,_T("*"));
		HANDLE h = FindFirstFile(pattern,&fd);
		if (h!=INVALID_HANDLE_VALUE)
		{
			do
			{
				TCHAR cFileName[MY_MAX_PATH];
				_tcscpy_s<sizeof(cFileName)/sizeof(TCHAR)>(cFileName, path);
				_tcscat_s<sizeof(cFileName)/sizeof(TCHAR)>(cFileName, fd.cFileName);

				DWORD attr = GetFileAttributes(cFileName);
				
				if ((attr&FILE_ATTRIBUTE_DIRECTORY)&&(!(attr&FILE_ATTRIBUTE_SYSTEM))&&(!(attr&FILE_ATTRIBUTE_HIDDEN)))
				{
					if ((strend(cFileName,_T(".."))&&strend(cFileName,_T("."))))		//not current and parent folder
					{

						TCHAR buf[MY_MAX_PATH];
						_tcscpy_s<sizeof(buf)/sizeof(TCHAR)>(buf,cFileName);
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
							if (!_tcscmp(extv[i],_T("*")))
							{
								b=true;
								break;
							}
							b = b || (!strend(cFileName,extv[i]));
						}
						if (b)	//if ok add to list
						{
							TCHAR *buf = new TCHAR[MY_MAX_PATH];
							_tcscpy_s(buf,MY_MAX_PATH,cFileName);
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