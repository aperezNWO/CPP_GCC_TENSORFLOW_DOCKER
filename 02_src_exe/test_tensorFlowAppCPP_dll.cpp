/*

1) TOOLCHAIN : C:\msys64\ucrt64\bin (CHANGE PATH)

2) g++ -std=c++20 -o test_tensorFlowApp_dll.exe  test_tensorFlowAppCPP_dll.cpp 


3) UTILIZAR PROYECDTO CPP_GCC_TENSORFLOW.DEV (Embarcadero Dev C++) PROVISIONALMENTE PARA 
   
   A) VISUALIZAR Y EDITAR ARCHIVOS.
   B) COMPILAR CON LINEA DE COMANDOS.
   C) EJECUTAR COMANDOS GIT
   
*/

#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <ctime>
#include <fstream>   // For file reading/writing
#include <iomanip>   // For std::setprecision

struct TicTacToeResultOnline {
    int finalBoard[9];
    int moves[9];
    int winner;
    int moveCount;
    int history[10][9];
    int historyCount;
};


typedef const char* (*GetTensorFlowAPIVersionFunc)();  // Define function pointer type
typedef const char* (*GetTensorFlowAPPVersionFunc)();  // Define function pointer type
typedef const char* (*GetCPPSTDVersionFunc)();         // Define function pointer type

typedef const char* (*GetStringFunc)();
typedef bool (*PlayTTTFunc)(TicTacToeResultOnline*);



// print board history
void printBoard(const int* board) {
    for (int i = 0; i < 9; ++i) {
        char c = '.';
        if (board[i] == 1) c = 'X';
        else if (board[i] == -1) c = 'O';
        std::cout << c;
        if ((i+1) % 3 == 0) std::cout << '\n';
    }
    std::cout << "---\n";
}

#ifdef _WIN32
    #include <windows.h>
    void sleep_ms(int ms) { Sleep(ms); }
#else
    #include <unistd.h>
    void sleep_ms(int ms) { usleep(ms * 1000); }
#endif

// Function to pause until user presses Enter
void waitForEnter() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //std::cin.get(); // Wait for Enter (handles newline from previous input)
}

// Function to ask if user wants to continue
bool askToContinue() {
    std::string response;
    while (true) {
        std::cout << "\nDo you want to watch another game? (y/n): ";
        std::getline(std::cin, response);

        // Convert to lowercase for case-insensitive comparison
        std::string lowerResponse;
        std::transform(response.begin(), response.end(), std::back_inserter(lowerResponse),
                      [](unsigned char c){ return std::tolower(c); });

        if (lowerResponse == "y" || lowerResponse == "yes") {
            return true;
        } else if (lowerResponse == "n" || lowerResponse == "no") {
            return false;
        } else {
            std::cout << "Please enter 'y' or 'n'.\n";
        }
    }
}


int main() {
    // Load the DLL
    HMODULE hDLL = LoadLibrary("TensorFlowAppCPP.dll");
    if (hDLL == NULL) {
        printf("Failed to load DLL.\n");
        return 1;
    }
    
	//////////////////////////////////////////////////////
    //GetTensorFlowAPIVersion
    //////////////////////////////////////////////////////
    
    GetTensorFlowAPIVersionFunc GetTensorFlowAPIVersion = (GetTensorFlowAPIVersionFunc)GetProcAddress(hDLL, "GetTensorFlowAPIVersion");
    if (!GetTensorFlowAPIVersion) {
        printf("Could not locate the function 'GetTensorFlowAPIVersion'.\n");
        FreeLibrary(hDLL);
        return 1;
    }

    // Call the function
    const char* apiVersion = GetTensorFlowAPIVersion();
    printf("'GetTensorFlowAPIVersion' : %s\n", apiVersion);

	//////////////////////////////////////////////////////
    //GetTensorFlowAPPVersion
    //////////////////////////////////////////////////////
    
    
    GetTensorFlowAPPVersionFunc GetTensorFlowAPPVersion = (GetTensorFlowAPPVersionFunc)GetProcAddress(hDLL, "GetTensorFlowAppVersion");
    if (!GetTensorFlowAPPVersion) {
        printf("Could not locate the function 'GetTensorFlowAPPVersion'.\n");
        FreeLibrary(hDLL);
        return 1;
    }

    // Call the function
    const char* appVersion = GetTensorFlowAPPVersion();
    printf("'GetTensorFlowAPPVersion' : %s\n", appVersion);
    
    //////////////////////////////////////////////////////
    //GetCPPSTDVersion
    //////////////////////////////////////////////////////
    
    
    GetCPPSTDVersionFunc GetCPPSTDVersion = (GetCPPSTDVersionFunc)GetProcAddress(hDLL, "GetCPPSTDVersion");
    if (!GetCPPSTDVersion) {
        printf("Could not locate the function 'GetCPPSTDVersion'.\n");
        FreeLibrary(hDLL);
        return 1;
    }

    // Call the function
    const char* cppSTDVersion = GetCPPSTDVersion();
    printf("'GetCPPSTDVersion' : %s\n", cppSTDVersion);
    
    askToContinue();
    
    ///////////////////////////////////////////////////
    // === New Function: PlayTicTacToeGame ===
    ///////////////////////////////////////////////////
    
    PlayTTTFunc PlayTicTacToeGame = (PlayTTTFunc)GetProcAddress(hDLL, "PlayTicTacToeGameWithHistory");
    if (!PlayTicTacToeGame) {
        printf("Could not locate 'PlayTicTacToeGameWithHistory'\n");
        FreeLibrary(hDLL);
        return 1;
    }

    do {
        TicTacToeResultOnline result{};
        if (!PlayTicTacToeGame(&result)) {
            std::cerr << "Game execution failed.\n";
            continue;
        }

        // === ANIMATE THE GAME ===
        std::cout << "\n=== REPLAYING GAME: X vs O ===\n";

        for (int step = 0; step < result.historyCount; ++step) {
            system("cls");

            std::cout << "\n=== MOVE " << step << " ===\n";
            printBoard(result.history[step]);

            if (step > 0) {
                int move = result.moves[step - 1];
                char player = (step % 2 == 1) ? 'X' : 'O';
                std::cout << "Player " << player << " plays at position " << move << "\n";
            }

#ifdef _WIN32
            Sleep(600);
#else
            usleep(600000);
#endif
        }

        if (result.winner == 1)      std::cout << " X wins!\n";
        else if (result.winner == -1) std::cout << " O wins!\n";
        else                          std::cout << " Draw!\n";

    } while (askToContinue());

    std::cout << "Thanks for watching! Goodbye!\n";
    

    /////////////////////////////////////////////////////////////////////
    // Clean up
    /////////////////////////////////////////////////////////////////////

    FreeLibrary(hDLL);
    return 0;
}

