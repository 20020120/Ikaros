#include "FileDialog.h"
#include <tchar.h>
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")


FileDialog::FileDialog(HWND h)
{
    f_SetDirectory();
    hwnd = h;
}

FileDialog::~FileDialog()
{
}

	
bool FileDialog::OpenFile(TCHAR* fname, int sz, TCHAR* initDir)
{
    OPENFILENAME o;
    
    fname[0] = _T('\0');
    ZeroMemory(&o, sizeof(o));
    o.lStructSize = sizeof(o);		//	構造体サイズ
    o.hwndOwner = hwnd;				//	親ウィンドウのハンドル
    o.lpstrInitialDir = initDir;	//	初期フォルダー
    o.lpstrFile = fname;			//	取得したファイル名を保存するバッファ
    o.nMaxFile = sz;				//	取得したファイル名を保存するバッファサイズ
    o.lpstrFilter = _TEXT("fbxファイル(*.fbx)\0*.fbx\0*.nk\0") _TEXT("全てのファイル(*.*)\0*.*\0");
    o.lpstrDefExt = _TEXT("fbx");
    o.lpstrTitle = _TEXT("モデルデータを指定");
    o.nFilterIndex = 1;

    return GetOpenFileName(&o);

}

bool FileDialog::OpenFileA(CHAR* fname, int sz, CHAR* initDir)
{

    OPENFILENAMEA o;

    fname[0] = '\0';
    ZeroMemory(&o, sizeof(o));
    o.lStructSize = sizeof(o);		//	構造体サイズ
    o.hwndOwner = hwnd;				//	親ウィンドウのハンドル
    o.lpstrInitialDir = initDir;	//	初期フォルダー
    o.lpstrFile = fname;			//	取得したファイル名を保存するバッファ
    o.nMaxFile = sz;				//	取得したファイル名を保存するバッファサイズ
    o.lpstrFilter = "fbxファイル(*.fbx)\0*.fbx\0" "モデルデータ(*.nk)\0*.nk\0" "全てのファイル(*.*)\0*.*\0";
    o.lpstrDefExt = "fbx\0nk";
    o.lpstrTitle = "モデルデータを指定";
    o.nFilterIndex = 1;

    return GetOpenFileNameA(&o);
}

void FileDialog::f_SetDirectory()
{
    DWORD size{};
    GetCurrentDirectoryA(MAX_PATH, Directory);
}

bool FileDialog::SaveFileA( LPCSTR Filter, char* FileName, DWORD Flags)
{
   OPENFILENAMEA OFN;

   FileName[0] = '\0';
   ZeroMemory(&OFN, sizeof(OFN));
   OFN.lStructSize = sizeof(OFN);
   OFN.hwndOwner = hwnd;

   OFN.lpstrFilter = Filter;
   OFN.lpstrFile = FileName;
   OFN.nMaxFile = MAX_PATH;
   OFN.Flags = Flags;
   OFN.lpstrTitle = "名前を付けて保存";
   return GetSaveFileNameA(&OFN);
    
}
 std::string FileDialog::CreateDirectry(std::string dir)
{
    std::string f = Directory;
    std::string fulldir = f + "\\" + dir + "\\";
    MakeSureDirectoryPathExists(fulldir.c_str());
    return  fulldir;
}
