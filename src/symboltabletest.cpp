#include <symboltable.h>
#include <symboltableentry.h>
#include <iostream>
#include <string>

using namespace std;
using namespace Environment;

#define prefix __FILE__ << ":" << __LINE__ << ":"

/**
 * @mainpage
 *
 * The bulk of reference for this compiler comes from the language specification
 * given at: http://web.mit.edu/sunsoft_v5.1/www/pascal/lang_ref/ref_lex.doc.html#210
 * This allows us to see what options might be nice as well as what others have done.
 */

void AddLexeme(string lexeme, SymbolTable &symboltable)
{
    int level;
    SymbolTableEntry *symboltableentry = new SymbolTableEntry(lexeme, 0);
    cout << "Adding " << *symboltableentry;
    symboltable.Insert(*symboltableentry);
    if (symboltable.Find(lexeme, level)) cout << " [ok]" << endl;
    else cout << " [!!]" << endl;
    delete symboltableentry;
}

void FindLexeme(string lexeme, SymbolTable &symboltable)
{
    int level;
    if (symboltable.Find(lexeme, level)) cout << *(symboltable.Find(lexeme, level)) << " found at level " << level << " [ok]" << endl;
    else cout << lexeme << " not found [!!]" << endl;
}

int main(int argc, char *argv[])
{
    SymbolTable *symboltable;
    SymbolTableEntry *symboltableentry;
    int level = 0;

    symboltable = new SymbolTable();

    /**
     * @todo Convert this to use unit tests and place it as an executable to
     *   be created in the test directory to check for regressions.
     */

    AddLexeme("foo", *symboltable);
    AddLexeme("bar", *symboltable);
    AddLexeme("rar", *symboltable);
    AddLexeme("oof", *symboltable);

    cout << endl << "Symbol Table: " << endl;
    cout << *symboltable << endl;

    /**
     * @note Add a scope.
     */

    cout << "Adding a lexical scope!" << endl;

    symboltable->Push();

    AddLexeme("farsi", *symboltable);
    AddLexeme("francais", *symboltable);
    AddLexeme("deutsch", *symboltable);

    cout << endl << "Symbol Table: " << endl;
    cout << *symboltable << endl;

    cout << "Overwriting previous lexical scope values!" << endl;
    AddLexeme("foo", *symboltable);
    AddLexeme("bar", *symboltable);

    cout << "Checking lexical scope values of overwritten and non-overwritten values!" << endl;

    cout << "Checking local scope: " << endl;
    FindLexeme("farsi", *symboltable);

    cout << "Checking previous scope: " << endl;
    FindLexeme("rar", *symboltable);

    cout << "Checking overwritten scope: " << endl;
    FindLexeme("foo", *symboltable);

    cout << "Removing a lexical scope!" << endl;

    symboltable->Pop();

    cout << endl << "Symbol Table: " << endl;
    cout << *symboltable << endl;

    delete symboltable;

    return 0;
}
