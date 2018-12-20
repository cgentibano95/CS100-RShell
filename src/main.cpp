#include "base.h"
#include "parser.h"
#include <iostream>
#include <vector>
#include <limits.h>
#include <unistd.h>

using namespace std;

void print_vector(vector<Command *> container)
{
    for (int i = 0; i < container.size(); i++)
    {
        cout << container.at(i)->evaluate() << endl;
    }
}

bool validInput(string s)
{
    size_t open_paren = std::count(s.begin(), s.end(), '(');
    size_t closed_paren = std::count(s.begin(), s.end(), ')');
    size_t open_bracket = std::count(s.begin(), s.end(), '[');
    size_t closed_bracket = std::count(s.begin(), s.end(), ']');

    if (open_paren == closed_paren && open_bracket == closed_bracket)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// (echo hello || (echo test && ls -a)) || exit

int main()
{
    /* Declare hostname and username */
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];

    /* Logic for setting hostname and username, check if valid, if not, set to 
       default values */
    if (gethostname(hostname, HOST_NAME_MAX) != 0)
    {
        strcpy(hostname, "ERROR_HOST");
    }
    if (getlogin_r(username, LOGIN_NAME_MAX) != 0)
    {
        strcpy(username, "ERROR_USERNAME");
    }
    /* Run loop until exit command is entered */
    while (true)
    {
        cout << username << "@" << hostname << "$";
        string command_str;
        getline(cin, command_str);
        //Check for valid input, if valid, then run
        if (validInput(command_str))
        {
            Parser* parse = new Parser();
            parse->parseCommandString(command_str);
        }
        else
        {
            perror("*** ERROR: Invalid precedence\n");
        }
    }
    return 0;
};

// ((echo first || echo second && echo third ) || echo fourth ) || echo fifth && [ -e src ]
// echo first; echo second || echo third
// ((echo first || (echo second && echo third) ) || echo fourth ) || (echo fifth && echo sixth)
// (echo hello) || (echo hello)
// (echo first && (echo second && echo third) || echo fourth) && echo fifth
// echo first; echo second && echo third