#include <compiler.h>
#include <cstdlib>

#define prefix __FILE__ << ":" << __LINE__ << ":"

int main(int argc, char *argv[])
{
    Compiler application(argc, argv);
    try
    {
        application.Run();
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
