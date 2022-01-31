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
    o.lStructSize = sizeof(o);		//	�\���̃T�C�Y
    o.hwndOwner = hwnd;				//	�e�E�B���h�E�̃n���h��
    o.lpstrInitialDir = initDir;	//	�����t�H���_�[
    o.lpstrFile = fname;			//	�擾�����t�@�C������ۑ�����o�b�t�@
    o.nMaxFile = sz;				//	�擾�����t�@�C������ۑ�����o�b�t�@�T�C�Y
    o.lpstrFilter = _TEXT("fbx�t�@�C��(*.fbx)\0*.fbx\0*.nk\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
    o.lpstrDefExt = _TEXT("fbx");
    o.lpstrTitle = _TEXT("���f���f�[�^���w��");
    o.nFilterIndex = 1;

    return GetOpenFileName(&o);

}

bool FileDialog::OpenFileA(CHAR* fname, int sz, CHAR* initDir)
{

    OPENFILENAMEA o;

    fname[0] = '\0';
    ZeroMemory(&o, sizeof(o));
    o.lStructSize = sizeof(o);		//	�\���̃T�C�Y
    o.hwndOwner = hwnd;				//	�e�E�B���h�E�̃n���h��
    o.lpstrInitialDir = initDir;	//	�����t�H���_�[
    o.lpstrFile = fname;			//	�擾�����t�@�C������ۑ�����o�b�t�@
    o.nMaxFile = sz;				//	�擾�����t�@�C������ۑ�����o�b�t�@�T�C�Y
    o.lpstrFilter = "fbx�t�@�C��(*.fbx)\0*.fbx\0" "���f���f�[�^(*.nk)\0*.nk\0" "�S�Ẵt�@�C��(*.*)\0*.*\0";
    o.lpstrDefExt = "fbx\0nk";
    o.lpstrTitle = "���f���f�[�^���w��";
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
   OFN.lpstrTitle = "���O��t���ĕۑ�";
   return GetSaveFileNameA(&OFN);
    
}
 std::string FileDialog::CreateDirectry(std::string dir)
{
    std::string f = Directory;
    std::string fulldir = f + "\\" + dir + "\\";
    MakeSureDirectoryPathExists(fulldir.c_str());
    return  fulldir;
}
