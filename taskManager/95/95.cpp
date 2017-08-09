#include <string>
#include <windows.h>
#include "resource.h"
#include <tlhelp32.h>
#include <wchar.h>
using namespace std;
#pragma comment(lib,"Kernel32")

HWND hButton1;
HWND hButton2;
HWND hButton3;
HWND hEdit;
HWND hl;
HWND hStatic2;
HINSTANCE hInst;
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);


HANDLE hProcessSnap;
HANDLE hProcess;
PROCESSENTRY32 pe32;

void CreateProcessList(HWND hl)//функция, создающая список текущих процессов
{
	SendMessage(hl, LB_RESETCONTENT, 0, 0);//очищаем листбокс

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		if( hProcessSnap == INVALID_HANDLE_VALUE )
		{
			MessageBox(0,TEXT("1"),TEXT("INVALID_HANDLE_VALUE"),0);
			return ;
		}

		
		pe32.dwSize = sizeof( PROCESSENTRY32 );

		
		if( !Process32First( hProcessSnap, &pe32 ) )
		{
			MessageBox(0,TEXT("2"),TEXT("Process32First"),0); 

			CloseHandle( hProcessSnap );          
			return ;
		}

		
		do
		{
			wchar_t*str=new wchar_t[200];
			wsprintf(str,L"%d",pe32.th32ProcessID);
			
			 SendMessage(hl, LB_ADDSTRING, 0, LPARAM(pe32.szExeFile));
			

		} while( Process32Next( hProcessSnap, &pe32 ) );

}

    void StopProcessByName(LPTSTR szProcessName) //функция завершает процесс, получая имя процесса
	{
        HANDLE hSnapshot; 
        PROCESSENTRY32 pe = {sizeof(pe)}; 
        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
        if (hSnapshot == INVALID_HANDLE_VALUE) 
            return ;                         
        if (!Process32First(hSnapshot, &pe))   
            return ;                          
        do {
            if (!lstrcmpi(pe.szExeFile, szProcessName)) //по имени процесса находим его идентификатор
				 
             {HANDLE hProcess; 
			 hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID); //получаем дескриптор процесса по его идентификатору
        TerminateProcess(hProcess, 0); //завершаем процесс по его дескриптору
		CloseHandle(hProcess); 
			return;
			}

		}                
        while (Process32Next(hSnapshot, &pe));  
        return ;                               
    }
	

	

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;

	case WM_INITDIALOG:
		{
			

			
			 hl = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", 0, WS_CHILD | WS_VISIBLE | LBS_SORT | LBS_NOTIFY| WS_VSCROLL, 10, 90, 300, 300, hWnd, 
				 (HMENU)500, GetModuleHandle(0), 0);
			
			SetWindowText(hStatic2, L"result");
			SetWindowText(hWnd, L"Диспетчер задач");
			hButton1 = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, TEXT("BUTTON"), TEXT("Обновить"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 10, 110, 40, hWnd, 0, hInst, 0);
			hButton2 = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, TEXT("BUTTON"), TEXT("Завершить"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 45, 110, 40, hWnd, 0, hInst, 0);
			hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL, 10, 390, 300, 40, hWnd, 0, hInst, 0);
			hButton3 = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, TEXT("BUTTON"), TEXT("Создать"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 440, 110, 40, hWnd, 0, hInst, 0);
			MoveWindow(hWnd, 400, 200, 550, 550, TRUE);

			CreateProcessList(hl);


		}
		return TRUE;

	case WM_COMMAND:
		HWND h = (HWND)lParam;
		if (h==hButton1)//обновить
		{
			CreateProcessList(hl);
		}

		if (h==hButton2)//завершить процесс
		{
			
			int index = SendMessage(hl, LB_GETCURSEL, 0, 0);
			if(index != LB_ERR) // выбран ли элемент списка
			{
				
				int length = SendMessage(hl, LB_GETTEXTLEN, index, 0);
				
				TCHAR *pBuffer = new TCHAR[length + 1];
				
				SendMessage(hl, LB_GETTEXT, index, LPARAM(pBuffer));
				MessageBox(hWnd, pBuffer, TEXT("Завершить процесс?"), MB_OK | MB_ICONINFORMATION);
			

				StopProcessByName(pBuffer);
				CreateProcessList(hl);
				delete [] pBuffer;
			}
			else
				MessageBox(hWnd, TEXT("процесс не выбран!"), TEXT("Завершение процесса"), MB_OK | MB_ICONSTOP);
		
		    CreateProcessList(hl);
		}

		if (h==hButton3)//создать процесс
		{	
			int length = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
			
			TCHAR *pBuffer = new TCHAR[length + 1];
			
			GetWindowText(hEdit, pBuffer, length + 1);

			STARTUPINFO si = {sizeof(si)};
         PROCESS_INFORMATION pi;
		
    CreateProcess(NULL, pBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	CreateProcessList(hl);
			
			delete [] pBuffer;

			CreateProcessList(hl);
		}
	    return true;
	}
	return FALSE;
}









