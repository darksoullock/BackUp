#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <list>
#include "filelist.h"
using namespace std;

#ifdef _UNICODE
#define _tcout wcout
#else
#define _tcout cout
#endif
TCHAR dir[MAX_PATH];

struct fl
{
	TCHAR dest[MAX_PATH];
	TCHAR filename[MAX_PATH];
	TCHAR * path;
	int extc;		//	count of extensions
	TCHAR ** extv;	//	values of extensions
};

DWORD WINAPI StartBackup(PVOID arg2)
{
	auto arg = (fl*)arg2;
	list<TCHAR*> *a = new list<TCHAR*>();
	GetFileList(arg->path,arg->extc,arg->extv,a);
	list<TCHAR*>::iterator it;
	TCHAR outname[MY_MAX_PATH];
	_tcscpy_s<sizeof(outname)/sizeof(TCHAR)>(outname, dir);
	_tcscat_s<sizeof(outname)/sizeof(TCHAR)>(outname, _T("\\"));
	_tcscat_s<sizeof(outname)/sizeof(TCHAR)>(outname, arg->filename);
	_tcscat_s<sizeof(outname)/sizeof(TCHAR)>(outname, _T(".txt"));
	int l = _tcslen(outname);
	wofstream out(outname);
	for (it = a->begin(); it!=a->end(); ++it)
		out << *it << endl;
	out.flush();
	out.close();
	SetFileAttributes(outname, GetFileAttributes(outname)&(!FILE_ATTRIBUTE_ARCHIVE));
	//TCHAR cmdLine2[MY_MAX_PATH];
	//{
	TCHAR cmdLine[MY_MAX_PATH];
	_tcscpy_s(cmdLine, MY_MAX_PATH, _T("\"Rar.exe\" a "));
	_tcscat_s(cmdLine,MY_MAX_PATH,arg->dest);										//
	_tcscat_s(cmdLine,MY_MAX_PATH,arg->filename);
	_tcscat_s(cmdLine,MY_MAX_PATH,_T(" @\""));
	_tcscat_s(cmdLine,MY_MAX_PATH,dir);
	_tcscat_s(cmdLine,MY_MAX_PATH,_T("\\"));
	_tcscat_s(cmdLine,MY_MAX_PATH,arg->filename);
	_tcscat_s(cmdLine,MY_MAX_PATH,_T(".txt\""));
	//ExpandEnvironmentStrings(cmdLine,cmdLine2,MY_MAX_PATH);
	//}
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	BOOL b = CreateProcess(NULL, cmdLine,NULL,NULL,FALSE,NULL,NULL,arg->path,&si, &pi);
	WaitForSingleObject(pi.hThread,INFINITE);
	for (it = a->begin(); it!=a->end(); ++it)
		SetFileAttributes(*it,GetFileAttributes(*it)&(!FILE_ATTRIBUTE_ARCHIVE));
	DeleteFile(outname);
	delete a;
	return 0;
}



int _tmain(int argc, TCHAR ** argv)
{

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,_T("BackUpOSRunAllowedOnce_Skidanenko"));
	if (hMutex==NULL)
		hMutex = CreateMutex(0,0,_T("BackUpOSRunAllowedOnce_Skidanenko"));
	else
	{
		_tcout << "another istance of program running";
		return 1;
	}

	GetCurrentDirectory(MAX_PATH, dir);
	if (argc ==1)
	{

		_tcout << _T("usage: backup.exe backup_folder [folder [types]]\n");
		return 0;
	}

	int extc;		//	count of extensions
	TCHAR ** extv;	//	values of extensions
	int srcc;		//	count of sources
	TCHAR ** srcv;	//	values of sources
	TCHAR * dest;	//	destination
	{
		auto l = _tcslen(argv[1])+2;
		dest = new TCHAR[l];
		_tcscpy_s(dest,l,argv[1]);
		if (argv[1][l-3]!=_T('\\'))
			_tcscat_s(dest,l,_T("\\"));
	}
	//	search extension
	{
		if (4>argc||!_tcscmp(argv[3],_T("*")))
		{
			extc = 1;
			extv = new TCHAR*[1];
			extv[0] = new TCHAR[2];
			extv[0][0] = _T('*');
			extv[0][1]=0;
		}
		else 
		{
			extc = argc-3;
			extv = new TCHAR*[extc];
			for (int i=0;i<extc;++i)
			{
				int l =_tcslen(argv[i+3])+1;
				extv[i]= new TCHAR[l];
				_tcscpy_s(extv[i],l,argv[i+3]);
			}
		}
	}

	//	search sources
	{
		if (3<=argc&&_tcscmp(argv[2],_T("*"))&&argv[2][0]!=_T('*'))
		{
			srcc = 1;
			srcv = new TCHAR*[srcc];
			int l = _tcslen(argv[2])+1+1;
			srcv[0]= new TCHAR[l];
			_tcscpy_s(srcv[0],l,argv[2]);

		}
		else 
		{
			const int len = MAX_PATH;
			TCHAR buf[len];
			int count = GetLogicalDriveStrings(len,buf);
			srcc =0 ;
			for (int i=0;i<count;i+=4)
				if(GetDriveType(buf+i)==DRIVE_FIXED||GetDriveType(buf+i)==DRIVE_REMOVABLE)
					++srcc;
			srcv = new TCHAR*[srcc];
			int j=0;
			if (3>argc||!_tcscmp(argv[2],_T("*")))
			{
				for (int i=0;i<count;i+=4)
					if(GetDriveType(buf+i)==DRIVE_FIXED||GetDriveType(buf+i)==DRIVE_REMOVABLE)
					{
						srcv[j] = new TCHAR[4];
						_tcscpy_s(srcv[j],4,(buf+i));
						++j;
					}
			}
			else
			{
				for (int i=0;i<count;i+=4)
					if(GetDriveType(buf+i)==DRIVE_FIXED||GetDriveType(buf+i)==DRIVE_REMOVABLE)
					{
						int l =_tcslen(argv[2])+2;
						srcv[j] = new TCHAR[l];
						_tcscpy_s(srcv[j],l,argv[2]);
						srcv[j][0]= *(buf+i);
						++j;
					}
			}
		}
	}

	TCHAR datetime[16];
	{
		char datetime2[sizeof(datetime)/sizeof(TCHAR)];
		time_t ut = time(0);		//seconds from 1970
		tm *st = localtime(&ut);	//time struct
		strftime(datetime2,sizeof(datetime2)," _%y%m%d%H%M%S",st);	//	http://cplusplus.com/reference/ctime/strftime/
#ifdef _UNICODE
		MultiByteToWideChar(CP_ACP,0,datetime2,sizeof(datetime2),datetime,sizeof(datetime)/sizeof(TCHAR));
#else
		strcpy(datetime,datetime2);
#endif
	}

	HANDLE *h = new HANDLE[srcc];
	for (int i=0;i< srcc;++i)
	{
		fl *a = new fl();
		_tcscpy(a->filename, datetime);
		_tcscpy_s(a->dest,MAX_PATH,dest);
		a->filename[0] = srcv[i][0];
		a->path = srcv[i];
		a->extv = extv;
		a->extc = extc;
		h[i] = CreateThread(0,0,StartBackup,a,0,0);
	}
	WaitForMultipleObjects(srcc,h,TRUE,INFINITE);
	_tcout << _T("Disabling archive bit to added files...\n");
	for (int i=0;i< srcc;++i)
		CloseHandle(h[i]);
	_tcout << _T("Deleting temporary data...\n");
	delete [] h;
	//memfree
	for (int i=0;i< extc;++i)
		delete [] extv[i];
	delete [] extv;
	for (int i=0;i< srcc;++i)
		delete [] srcv[i];
	delete [] srcv;
	delete [] dest;

	return 0;
}