#include <windows.h>
#include <tchar.h>
#include <cstdio>
#include <strsafe.h>

HANDLE hSlot;
LPCTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");

BOOL WINAPI MakeSlot(LPCTSTR lpszSlotName)
{
    hSlot = CreateFile(lpszSlotName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

    if (hSlot == INVALID_HANDLE_VALUE)
    {
        printf("CreateMailslot failed with %d\n", GetLastError());
        return FALSE;
    }
    return TRUE;
}

BOOL WriteSlot(LPCTSTR lpszMessage)
{
    BOOL fResult;
    DWORD cbWritten;

    fResult = WriteFile(hSlot,
                        lpszMessage,
                        (DWORD)(lstrlen(lpszMessage) + 1) * sizeof(TCHAR),
                        &cbWritten,
                        (LPOVERLAPPED)nullptr);

    if (!fResult)
    {
        printf("WriteFile failed with %d.\n", GetLastError());
        return FALSE;
    }

    printf("Slot written to successfully.\n");

    return TRUE;
}

int main()
{
    MakeSlot(SlotName);

    while (1)
    {
        WriteSlot(TEXT("Hello, world"));
        Sleep(3000);
    }
}