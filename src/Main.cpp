#include "Game.h"

Game *gGameInstance = nullptr;

int main(int argc, char *argv[])
{
    gGameInstance = new Game();

    if (gGameInstance->Initialize())
    {
        gGameInstance->RunLoop();
    }

    gGameInstance->Shutdown();
    delete gGameInstance;
    return 0;
}