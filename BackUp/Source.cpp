#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <ctime>
using namespace std;

int main(int argc, TCHAR ** argv)
{
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

	dest = new TCHAR[_tcslen(argv[1])+1];
	_tcscpy(dest,argv[1]);

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
			for (int i=0;i<argc-3;++i)
			{
				extv[i]= new TCHAR[_tcslen(argv[i+3])+1];
				extv[i]= _tcscpy(extv[i],argv[i+3]);
			}
		}
	}

	//	search sources
	{
		if (3<=argc&&_tcscmp(argv[2],"*")&&argv[2][0]!='*')
		{
			srcc = 1;
			srcv = new TCHAR*[1];
			srcv[0]= new TCHAR[_tcslen(argv[2])+1];
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
#ifdef __UNICODE
		MultiByteToWideChar(CP_ACP,0,datetime2,sizeof(datetime2),datetime,sizeof(datetime)/sizeof(TCHAR));
#else
		strcpy(datetime,datetime2);
#endif
	}

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
		srcv[i][3]=0;
		cout << srcv[i] << endl;
	}
	cout << "\n";

	cout <<"list of file extensions:\n";
	for (int i=0;i<extc;++i)
		cout << extv[i] << endl;

	/*DWORD dwAttrs;
	dwAttrs = GetFileAttributes(argv[1]);
	bool b = false;
	if (dwAttrs==-1)
	b=CreateDirectory(argv[1],0);*/

	//memfree
	for (int i=0;i< extc;++i)
		delete [] extv[i];
	delete[] extv;
	for (int i=0;i< srcc;++i)
		delete [] srcv[i];
	delete [] srcv;
	delete [] dest;

	return 0;
}