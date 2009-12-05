#include <iostream>
#include <cstdlib>

#include "../include/compiler.h"
#include "../include/output.h"

using namespace std;

int main(int argc, char *argv[])
{
    Compiler *application;
    try
    {
        application = new Compiler(argc, argv);
        application->Run();
    }
    catch (CompilerArgumentError e)
    {
        if (e.GetMessage().length() > 0) ERROR(e.GetMessage());
        cout << e.GetDescription() << endl;
        return EXIT_FAILURE;
    }
    catch (CompilerError e)
    {
        if (e.GetMessage().length() > 0) ERROR(e.GetMessage());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
