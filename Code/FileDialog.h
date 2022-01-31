#pragma once
#pragma comment(lib, "Comdlg32.lib")
#include<windows.h>
#include <stdlib.h>
#include<string>

class FileDialog
{
public:
    FileDialog(HWND h);
    ~FileDialog();


public:
    
    //--------------------<�t�@�C�����J��>--------------------//
    bool OpenFile(TCHAR* fname, int sz, TCHAR* initDir);

    //--------------------<�t�@�C�����J���Q>--------------------//
    bool OpenFileA(CHAR* fname, int sz, CHAR* initDir);

    //--------------------<�v���O�����N�����ɐ�΃p�X���擾����>--------------------//
    void f_SetDirectory();

   
    //--------------------<��΃p�X�̃Q�b�^�[>--------------------//
    auto GetDirectory() { return Directory; }

    //--------------------<���O��t���ĕۑ�>--------------------//
    bool SaveFileA(LPCSTR Filter, char* FileName, DWORD Flags);

    //--------------------<�C�ӂ̃p�X�܂ł̃t�@�C�����쐬����>--------------------//
    std::string CreateDirectry(std::string dir); //�߂�l�͍쐬���ꂽ�t�H���_�܂ł̐�΃p�X

private:
    char Directory[MAX_PATH]{};
    HWND hwnd;
};