#include <iostream>
#include <windows.h>
#include <list>
using namespace std;
void GetFileList(const char * cpath, int extc, TCHAR ** extv, list<char*> * r)
{
	if (GetFileAttributes(cpath)==-1)
		return;
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
		{
			do
			{
				DWORD attr = GetFileAttributes(fd.cFileName);

				if ((attr&FILE_ATTRIBUTE_DIRECTORY)&&(!(attr&FILE_ATTRIBUTE_SYSTEM))&&(!(attr&FILE_ATTRIBUTE_HIDDEN)))
				{
					if ((strcmp(fd.cFileName,"..")&&strcmp(fd.cFileName,".")))		//not current and parent folder
					{
						char buf[MAX_PATH];
						strcpy(buf,path);
						strcat(buf,fd.cFileName);
						GetFileList(buf,extc, extv, r);		//recursion
					}
				}
				else
				{
					if ((attr&FILE_ATTRIBUTE_ARCHIVE)&&(!(attr&FILE_ATTRIBUTE_SYSTEM))&&(!(attr&FILE_ATTRIBUTE_HIDDEN)))
					{
						int l = _tcslen(fd.cFileName);
						int el;
						bool b = true;		//extension match ? 1 : 0
						for (int i=0;i< extc;++i)	
						{
							el = _tcslen(extv[i]);
							if (l<el)	//if filename shorter than ext.
							{
								b=false;
								continue;
							}
							for (int j=el-1;j>=0;--j)	//compare extension
								if (fd.cFileName[l+j-el]!=extv[i][j])
								{
									b = false;
									break;
								}

						}
						if (b)	//if ok add to list
						{
							char *buf = new char[MAX_PATH];
							strcpy(buf,path);
							strcat(buf,fd.cFileName);
							r->push_front(buf);
						}
					}
				}
				SetCurrentDirectory(path);
				b = FindNextFile(h,&fd);
			}
			while(b);
			FindClose(h);
		}
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
