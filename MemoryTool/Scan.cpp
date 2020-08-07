#include "Scan.h"
#include <stdio.h>

namespace MTool
{
    void GetThreadContext(HANDLE proceso, HANDLE& thread, CONTEXT& threadContext)
    {
        DWORD threadID = GetProcessThreadID(proceso);

        thread = OpenThread(
            (THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT),
            false, threadID);

        SuspendThread(thread);

        threadContext.ContextFlags = CONTEXT_CONTROL;
        GetThreadContext(thread, &threadContext);

    }

    void MainThreadDetour(HANDLE proceso, BYTE shellcode[], HANDLE thread, CONTEXT& threadContext)
    {
        auto remoteCave = VirtualAllocEx(proceso, 0, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE);

        WriteProcessMemory(proceso, remoteCave, shellcode, sizeof(shellcode), NULL);

        threadContext.Eip = (DWORD)remoteCave;
        threadContext.ContextFlags = CONTEXT_CONTROL;
        SetThreadContext(thread, &threadContext);
       }


    void PrintSeparador()
    {
        printf("-------------------------------------------------------------------\n");
    }

    DWORD   ProtegerMemoria(HANDLE process, DWORD* address, DWORD nuevaProteccion)
    {
        DWORD protOriginal;
        VirtualProtectEx(process, address, NULL, nuevaProteccion, &protOriginal);
        return protOriginal;
    }

    HANDLE  HandleReadWrite(int pID)
    {
        return OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_VM_READ, FALSE, pID);
    }

    BYTE* GetBaseAdr(DWORD pID)
    {
        HANDLE h = CreateToolhelp32Snapshot(8, pID);

        MODULEENTRY32 me;
        me.dwSize = sizeof(MODULEENTRY32);

        int ret = Module32First(h, &me);

        CloseHandle(h);

        return me.modBaseAddr;
    }

    void    PrintRecursos(HANDLE proceso, DWORD* playerAddress, unsigned int nJugador)
    {

        const char* recursos[] = { "Comida", "Piedra", "Oro", "Conocim.", "Metal", "Petroleo" };
        int valor;
        UINT i;

        for (i = 0; i < 6; i++)
        {
            LeerMemoria(proceso, playerAddress, valor);
            printf("[J%d] %s \t(%p): %d\n", nJugador, recursos[i], playerAddress, valor ^ 0x8221);
            playerAddress++;
        }

        PrintSeparador();

    }

    void EncriptarNuevoValor(int& eleccion, int claveEncripcion)
    {
        std::cin >> eleccion;
        eleccion = eleccion ^ claveEncripcion;
    }

    DWORD GetProcessThreadID(HANDLE process)
    {
        THREADENTRY32 entry;
        entry.dwSize = sizeof(THREADENTRY32);
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

        if (Thread32First(snapshot, &entry) == TRUE)
        {
            DWORD pID = GetProcessId(process);
            while (Thread32Next(snapshot, &entry) == TRUE)
            {
                if (entry.th32OwnerProcessID == pID)
                {
                    CloseHandle(snapshot);
                    return entry.th32ThreadID;
                }
            }
        }
        CloseHandle(snapshot);
        return NULL;
    }
}