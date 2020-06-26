// MemoryTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>

void CambiarBase(HANDLE process, DWORD address, DWORD nuevaBase)
{
    //Obtener el address de kernel32.dll
    HMODULE k32 = GetModuleHandle(L"kernel32.dll");

    //Obtener address de GetModuleHandle()
    LPVOID funcAdr = GetProcAddress(k32, "GetoduleHandleA");
    if (!funcAdr)
        funcAdr = GetProcAddress(k32, "GetModuleHandleW");

    //Creamos el thread    
    HANDLE thread = CreateRemoteThread(process, NULL, NULL,
        (LPTHREAD_START_ROUTINE)funcAdr,
        NULL, NULL, NULL);
    
    

    //Dejamos que la thread termine
    WaitForSingleObject(thread, INFINITE);

    //Obtenemos el exit code
    GetExitCodeThread(thread, &nuevaBase);

    //Limpiamos
    CloseHandle(thread);

    /*DWORD delta = address - 0x400000;
    return delta + nuevaBase;*/
}



template <typename T>
BOOL LeerMemoria(HANDLE process, DWORD address, T& valor)
{
    return ReadProcessMemory(process, (LPVOID)address, &valor, sizeof(valor), 0);
}

template <typename T>
BOOL EscribirMemoria(HANDLE process, DWORD address, T& valor)
{
    return WriteProcessMemory(process, (LPVOID)address, &valor, sizeof(valor), 0);
}

template <typename T>
DWORD ProtegerMemoria(HANDLE process, DWORD address, DWORD nuevaProteccion, T tipo)
{
    DWORD protOriginal;
    VirtualProtectEx(process,(LPVOID)address, sizeof(T), nuevaProteccion,  &protOriginal);
    return protOriginal;
}

int main()
{
    HWND myWindow = FindWindow(NULL, L"Rise of Nations: Extended Edition");
    DWORD address = 0x01A484B0; 
    DWORD pID;
 
    int val = 0;
    char eleccion = 0;

    //Obtencion del ID del Proceso

    GetWindowThreadProcessId(myWindow, &pID);

    HANDLE process = OpenProcess(
        PROCESS_VM_OPERATION |
        PROCESS_VM_WRITE |
        PROCESS_VM_READ,
        FALSE,
        pID);

    if (process == NULL || process == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error (" << GetLastError() << ") obteniendo el Handle del Processo con ID: " << pID << std::endl;
    }
    else
    {
        std::cout << "Handle obtenido con exito!" << std::endl;
    }

    //Cambiamos la Proteccion de la memoria
    DWORD protOrig = ProtegerMemoria(process, address, PAGE_READWRITE, val);

    //Elegir si Escribir o Leer la memoria del Address indicado.

    while (toupper(eleccion) != 'E' && toupper(eleccion) != 'L')
    {
        system("CLS");
        std::cout << "Leer [L] o Escribir [E] Memoria?" << std::endl;
        std::cin >> eleccion;
    } 
    
    //Ejecuta la accion seleccionada
    
    switch (toupper(eleccion))
    {
    case 'E':
    {
        std::cout << "Valor Deseado: " << std::endl;
        std::cin >> val;

        if (EscribirMemoria(process, address, val))
        {
            std::cout << "Valor ubicado en [" << address << "] fue cambiado a:" << val << std::endl;
        }
        else
        {
            std::cout << "No se pudo sobreescribir la memoria pedida." << std::endl;
        }

        break;
    }
        

    case 'L':
    {
        if (LeerMemoria(process, address, val))
        {
            std::cout << "Valor ubicado en [" << address << "]: " << val << std::endl;
        }
        else
        {
            std::cout << "No se pudo leer memoria pedida. " << std::endl;
        }

        break;
    }

    }

    Sleep(2000);
    system("CLS");

    std::cout << "Cerrando Handle..." << std::endl;
    CloseHandle(process);
}
