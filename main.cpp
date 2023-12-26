#include "App.h"

#include <csignal>

static ZooScan::App *app;

void SignalHandler(int signal)
{
    delete app;
}

int main()
{
    signal(SIGINT, SignalHandler);
    signal(SIGILL, SignalHandler);
    signal(SIGABRT, SignalHandler);
    signal(SIGSEGV, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGKILL, SignalHandler);

    app = new ZooScan::App();
    app->Initialize();
    return app->Run();
}
