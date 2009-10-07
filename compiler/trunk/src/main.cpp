#include <compiler.h>

#define prefix __FILE__ << ":" << __LINE__ << ":"

/**
 * @mainpage
 *
 * The bulk of reference for this compiler comes from the language specification
 * given at: http://web.mit.edu/sunsoft_v5.1/www/pascal/lang_ref/ref_lex.doc.html#210
 * This allows us to see what options might be nice as well as what others have done.
 */

int main(int argc, char *argv[])
{
    Compiler application(argc, argv);
    try
    {
        application.Run();
    }
    catch (...)
    {
        return 1;
    }

    return 0;
}
// kate: indent-mode cstyle; space-indent on; indent-width 4; 
