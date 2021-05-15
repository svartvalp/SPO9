#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

HANDLE hSlot;
LPCTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");

BOOL ReadSlot()
{
    DWORD cbMessage, cMessage, cbRead;
    BOOL fResult;
    LPTSTR lpszBuffer;
    TCHAR achID[80];
    DWORD cAllMessages;
    HANDLE hEvent;
    OVERLAPPED ov;

    cbMessage = cMessage = cbRead = 0;

    hEvent = CreateEvent(nullptr, FALSE, FALSE, TEXT("ExampleSlot"));
    if (hEvent == nullptr)
        return FALSE;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    ov.hEvent = hEvent;

    fResult = GetMailslotInfo(hSlot,
                              (LPDWORD)nullptr,
                              &cbMessage,
                              &cMessage,
                              (LPDWORD)nullptr);

    if (!fResult)
    {
        printf("GetMailslotInfo failed with %d.\n", GetLastError());
        return FALSE;
    }

    if (cbMessage == MAILSLOT_NO_MESSAGE)
    {
        printf("Waiting for a message...\n");
        return TRUE;
    }

    cAllMessages = cMessage;

    while (cMessage != 0)
    {

        StringCchPrintf((LPTSTR)achID,
                        80,
                        TEXT("\nMessage #%d of %d\n"),
                        cAllMessages - cMessage + 1,
                        cAllMessages);


        lpszBuffer = (LPTSTR)GlobalAlloc(GPTR,
                                         lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage);
        if (lpszBuffer == nullptr)
            return FALSE;
        lpszBuffer[0] = '\0';

        fResult = ReadFile(hSlot,
                           lpszBuffer,
                           cbMessage,
                           &cbRead,
                           &ov);

        if (!fResult)
        {
            printf("ReadFile failed with %d.\n", GetLastError());
            GlobalFree((HGLOBAL)lpszBuffer);
            return FALSE;
        }


        StringCbCat(lpszBuffer,
                    lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage,
                    (LPTSTR)achID);


        _tprintf(TEXT("Contents of the mailslot: %s\n"), lpszBuffer);

        GlobalFree((HGLOBAL)lpszBuffer);

        fResult = GetMailslotInfo(hSlot,
                                  (LPDWORD)nullptr,
                                  &cbMessage,
                                  &cMessage,
                                  (LPDWORD)nullptr);

        if (!fResult)
        {
            printf("GetMailslotInfo failed (%d)\n", GetLastError());
            return FALSE;
        }
    }
    CloseHandle(hEvent);
    return TRUE;
}

BOOL WINAPI MakeSlot(LPCTSTR lpszSlotName)
{
    hSlot = CreateMailslot(lpszSlotName,
                           0,
                           MAILSLOT_WAIT_FOREVER,
                           (LPSECURITY_ATTRIBUTES)nullptr);

    if (hSlot == INVALID_HANDLE_VALUE)
    {
        printf("CreateMailslot failed with %d\n", GetLastError());
        return FALSE;
    }
    return TRUE;
}

int main()
{
    MakeSlot(SlotName);

    HANDLE hFile;

    hFile = CreateFile(SlotName,
                       GENERIC_WRITE,
                       FILE_SHARE_READ,
                       (LPSECURITY_ATTRIBUTES)nullptr,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       (HANDLE)nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile failed with %d.\n", GetLastError());
    }

    while (1) {
        ReadSlot();
        Sleep(3000);
    }
}