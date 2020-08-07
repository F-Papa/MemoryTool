#pragma once

#include <Windows.h>

#include <Psapi.h>

#include <iostream>

#include <tchar.h>

#include <string.h>

#include <TlHelp32.h>

namespace MTool
{
    void GetThreadContext(HANDLE proceso, HANDLE& thread, CONTEXT& threadContext);

    void MainThreadDetour(HANDLE proceso, BYTE shellcode[], HANDLE thread, CONTEXT& threadContext);

    DWORD GetProcessThreadID(HANDLE process);

    void PrintSeparador();

    void EncriptarNuevoValor(int& eleccion, int claveEncripcion);

    DWORD   ProtegerMemoria(HANDLE process, DWORD* address, DWORD nuevaProteccion);

    template <typename T>
    BOOL    LeerMemoria(HANDLE process, DWORD* address, T& valor)
    {
        return ReadProcessMemory(process, address, &valor, sizeof(valor), 0);
    }

    template <typename T>
    BOOL    EscribirMemoria(HANDLE process, DWORD* address, T& valor)
    {
        return WriteProcessMemory(process, address, &valor, sizeof(valor), 0);
    }

    HANDLE  HandleReadWrite(int pID);

    BYTE* GetBaseAdr(DWORD pID);

    void    PrintRecursos(HANDLE proceso, DWORD* playerAddress, unsigned int nJugador);
}