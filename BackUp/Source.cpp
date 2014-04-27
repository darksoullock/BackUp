#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <list>
#include "filelist.h"
using namespace std;

TCHAR dir[MAX_PATH];

struct fl
{
	TCHAR filename[MAX_PATH];
	TCHAR * path;
	int extc;		//	count of extensions
	TCHAR ** extv;	//	values of extensions
};

DWORD WINAPI saveFileList(PVOID arg2)
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
	delete a;
	return 0;
}



int _tmain(int argc, TCHAR ** argv)
{
	/*
	list<char*> *a = new list<char*>();
	char ** rrr = new char*[1];
	rrr[0] = "txt";
	GetFileList("E:\\Program Files (x86)\\Driver Sweeper\\Backup\\13-12-15-20-37-07\\AMD - Display\\Directories\\AMD\\Support\\13-9_win7_win8_32_dd_ccc_whql\\Config\\",1,rrr,a);
	list<char*>::iterator it;
	for (it = a->begin(); it!=a->end(); ++it)
	cout << *it << endl;
	return 0;*/
	GetCurrentDirectory(MAX_PATH, dir);
	//cout << dir << endl;
	if (argc ==1)
	{
		cout << _T("usage: backup.exe backup_folder [folder [types]]\n");
		return 0;
	}

	int extc;		//	count of extensions
	TCHAR ** extv;	//	values of extensions
	int srcc;		//	count of sources
	TCHAR ** srcv;	//	values of sources
	TCHAR * dest;	//	destination
	{
		auto l = _tcslen(argv[1])+1;
		dest = new TCHAR[l];
		_tcscpy_s(dest,l,argv[1]);
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
				extv[i]= new TCHAR[_tcslen(argv[i+3])+1];
				extv[i]= _tcscpy(extv[i],argv[i+3]);
			}
		}
	}

	//	search sources
	// TODO: add slashes at the ends of paths
	{
		if (3<=argc&&_tcscmp(argv[2],_T("*"))&&argv[2][0]!=_T('*'))
		{
			srcc = 1;
			srcv = new TCHAR*[srcc];
			srcv[0]= new TCHAR[_tcslen(argv[2])+1+1];
			_tcscpy(srcv[0],argv[2]);
		}
		else 
		{
			const int len = 128;
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
						_tcscpy(srcv[j],(buf+i));
						++j;
					}
			}
			else
			{
				for (int i=0;i<count;i+=4)
					if(GetDriveType(buf+i)==DRIVE_FIXED||GetDriveType(buf+i)==DRIVE_REMOVABLE)
					{
						srcv[j] = new TCHAR[_tcslen(argv[2])+1];
						_tcscpy(srcv[j],argv[2]);
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
	/*
	cout << _T("names of archives:") << endl;
	for(int i=0;i<srcc;++i)
	{
	datetime[0] = srcv[i][0];
	cout << datetime << endl;
	}
	cout <<"\n";

	cout <<"list of used devices:\n";
	for (int i=0;i<srcc;++i)
	{

	srcv[i][3]=0;	//!!!!!
	cout << srcv[i] << endl;
	}
	cout << "\n";

	cout <<"list of file extensions:\n";
	for (int i=0;i<extc;++i)
	cout << extv[i] << endl;*/

	HANDLE *h = new HANDLE[srcc];
	for (int i=0;i< srcc;++i)
	{
		fl *a = new fl();
		_tcscpy(a->filename, datetime);
		a->filename[0] = srcv[i][0];
		a->path = srcv[i];
		a->extv = extv;
		a->extc = extc;
		h[i] = CreateThread(0,0,saveFileList,a,0,0);
	}
	WaitForMultipleObjects(srcc,h,TRUE,INFINITE);
	for (int i=0;i< srcc;++i)
		CloseHandle(h[i]);
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