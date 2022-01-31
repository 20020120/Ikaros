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
    
    //--------------------<ファイルを開く>--------------------//
    bool OpenFile(TCHAR* fname, int sz, TCHAR* initDir);

    //--------------------<ファイルを開く２>--------------------//
    bool OpenFileA(CHAR* fname, int sz, CHAR* initDir);

    //--------------------<プログラム起動時に絶対パスを取得する>--------------------//
    void f_SetDirectory();

   
    //--------------------<絶対パスのゲッター>--------------------//
    auto GetDirectory() { return Directory; }

    //--------------------<名前を付けて保存>--------------------//
    bool SaveFileA(LPCSTR Filter, char* FileName, DWORD Flags);

    //--------------------<任意のパスまでのファイルを作成する>--------------------//
    std::string CreateDirectry(std::string dir); //戻り値は作成されたフォルダまでの絶対パス

private:
    char Directory[MAX_PATH]{};
    HWND hwnd;
};