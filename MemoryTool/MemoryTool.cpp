#include "Scan.h"

int main()
{
    const wchar_t* nombreVentana = L"Rise of Nations: Extended Edition";

    HWND myWindow = FindWindow(NULL, nombreVentana);
    DWORD pID;

    GetWindowThreadProcessId(myWindow, &pID);

    HANDLE proceso = MTool::HandleReadWrite(pID);

    BYTE* baseAddress = MTool::GetBaseAdr(pID);
    DWORD mainOffset = 0x9C9258;
    DWORD instructionOffset = 0x205783;

    int offsetJugadores = 0x6EEC;
    int claveEncripcion = 0x8221;

    wprintf(TEXT("El Base Address de %s es [%p]\n"), nombreVentana, baseAddress);
    MTool::PrintSeparador();
    
    DWORD* pointer1 = (DWORD*)baseAddress + mainOffset / 4;
    DWORD* deRef;
    UINT i;
    int eleccion;

    int numJugadores;

    printf("Ingrese el numero de jugadores: ");
    std::cin >> numJugadores;

    while (true)
    {
        system("CLS");

        //Menu
        {
            printf("[1] Para mostrar los recursos de todos los jugadores\n");
            printf("[2] Para cambiar los recursos de un jugador\n");
            printf("[3] Para agregarte 100 de comida (Thread Injection)\n");
            printf("[4] Para agregarte 100 de comida (Main Thread Manipulation)\n");
            printf("[5] Function Caller\n");
        }
        std::cin >> eleccion;

        switch (eleccion)
        {
        
        case 1:
        {
            for (i = 0; i < numJugadores; i++)
            {
                pointer1 = (DWORD*)baseAddress + mainOffset / 4 + i * (offsetJugadores / 4);
                MTool::LeerMemoria(proceso, pointer1, deRef);
                MTool::PrintRecursos(proceso, deRef, i + 1);

            }
            printf("Enter para volver al menu.\n");

            std::cin.get();
            std::cin.get();

            break;
        }

        case 2:
        {
            printf("Ingrese el numero de jugador:\n");
            std::cin >> eleccion;

            pointer1 = (DWORD*)baseAddress + mainOffset / 4 + (eleccion - 1) * (offsetJugadores / 4);

            MTool::LeerMemoria(proceso, pointer1, deRef);

            printf("[1] Comida\n");
            printf("[2] Madera\n");
            printf("[3] Oro\n");
            printf("[4] Conocimiento\n");
            printf("[5] Metal\n");
            printf("[6] Petroleo\n");
            printf("[7] Todos\n");


            std::cin >> eleccion;

            if (eleccion = 7)
            {
                printf("Ingrese un nuevo valor: ");
                MTool::EncriptarNuevoValor(eleccion, claveEncripcion);

                for (i = 0; i < 6; i++)
                {
                    MTool::EscribirMemoria(proceso, deRef + i, eleccion);
                }
            }

            else
            {

                deRef += (eleccion - 1);

                printf("Ingrese un nuevo valor: ");
                MTool::EncriptarNuevoValor(eleccion, claveEncripcion);

                MTool::EscribirMemoria(proceso, deRef, eleccion);

            }

            break;
        }
            
        case 3:
        {
            DWORD address;
            MTool::LeerMemoria(proceso, pointer1, address);
            printf("%x\n", address);

            BYTE shellcode[8] = {
                0x83, 0x05, 0x00, 0x00, 0x00, 0x00, 0x64,    //mov address,64              
                0xC3
            };

            memcpy(&shellcode[2], &address, 4);

            int caveLen = sizeof(shellcode);

            auto remoteCave = VirtualAllocEx(proceso, 0, caveLen, MEM_COMMIT, PAGE_EXECUTE);
            WriteProcessMemory(proceso, remoteCave, shellcode, caveLen, NULL);

            HANDLE thread = CreateRemoteThread(proceso, NULL, NULL, (LPTHREAD_START_ROUTINE)remoteCave, NULL, NULL, NULL);
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
            VirtualFreeEx(proceso, remoteCave, caveLen, MEM_RELEASE);

            break;
        }
        
        case 4:
        {
           BYTE shellcode2[17] = {
                0x60,                                       //PUSHFD
                0x9C,                                       //PUSHAD
                0x83, 0x05, 0x00, 0x00, 0x00, 0x00, 0x64,   //MOVE ADD [ADDRESS2], ...
                0x9D,                                       //POPAD
                0x61,                                       //POPFD
                0x68, 0x00, 0x00, 0x00, 0x00,               //PUSH...
                0xC3 };                                     //RETURN
            
            DWORD address2;
            ReadProcessMemory(proceso, pointer1, &address2, 4, NULL);
            memcpy(&shellcode2[4], &address2, 4);
            
            HANDLE thread;
            CONTEXT threadContext;
            MTool::GetThreadContext(proceso, thread, threadContext);      
            memcpy(&shellcode2[12], &threadContext.Eip, 4);

            //Shellcode listo

            //MTool::MainThreadDetour(proceso, shellcode2, thread, threadContext);  CRASHEA NO SE PORQUE
            auto remoteCave = VirtualAllocEx(proceso, 0, sizeof(shellcode2), MEM_COMMIT, PAGE_EXECUTE);

            WriteProcessMemory(proceso, remoteCave, shellcode2, sizeof(shellcode2), NULL);

            threadContext.Eip = (DWORD)remoteCave;
            threadContext.ContextFlags = CONTEXT_CONTROL;
            SetThreadContext(thread, &threadContext);
            ResumeThread(thread);
            break;
        };

        case 5:
            BYTE shellcode3[] =
            {   0x60,                                       //PUSHFD
                0x9C,                                       //PUSHAD

                0xB8, 0x00, 0x00, 0x00, 0x00,               // MOV EAX, 0x0
                0xFF, 0xD0,                                 //CALL EAX

                0x9D,                                       //POPAD
                0x61,                                       //POPFD
                0x68, 0x00, 0x00, 0x00, 0x00,               //PUSH 0x0
                0xC3 };                                     //return

            HANDLE thread;
            CONTEXT threadContext;
            MTool::GetThreadContext(proceso, thread, threadContext);
            memcpy(&shellcode3[12], &threadContext.Eip, 4);

            DWORD functionAddress = 0x00795952;
            memcpy(&shellcode3[3], &functionAddress, 4);

            //Shellcode listo

            auto remoteCave = VirtualAllocEx(proceso, 0, sizeof(shellcode3), MEM_COMMIT, PAGE_EXECUTE);

            WriteProcessMemory(proceso, remoteCave, shellcode3, sizeof(shellcode3), NULL);

            threadContext.Eip = (DWORD)remoteCave;
            threadContext.ContextFlags = CONTEXT_CONTROL;
            SetThreadContext(thread, &threadContext);
            ResumeThread(thread);

            break;
        }
    }
   
    std::cout << "Cerrando Handle..." << std::endl;
    CloseHandle(proceso);
}
