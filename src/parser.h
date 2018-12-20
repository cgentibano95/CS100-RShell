#ifndef PARSER_H
#define PARSER_H

#include "base.h"
#include <iostream>
#include <queue>
#include <vector>
#include <limits.h>
#include <unistd.h>
#include <stack>

using namespace std;

class Parser
{
  public:
    void buildPrecedence(vector<Command *> container);
    double parseCommandString(string s);
    double runCommands(vector<Command *> completed_container, double prev_rslt);
    string removeComments(string s);
};
/* Forward Declaration */
double runCommands(vector<Command *>, double prev_rslt);

/*
* This function is used to remove the comments from the command for it to run 
* the part of the string without comments
* Parameters:
* 1. String str from parseCommandString
* Return:
* 1. str (fixed string)
*/
string Parser::removeComments(string str)
{
    //Find comment tag
    int commentStart = str.find("#");

    //If comment tag found, then set str to the start of string till the
    //position of comment start
    if (commentStart != string::npos)
    {
        str = str.substr(0, commentStart);
        return str;
    }
    return str;
}

/*
* This function is used to parse the command line string sent in from main. It 
* handles precendence using parenthesis and keeps the brackets for test commands
* 
* Parameters:
* 1. String s from main
* Return:
* 1. int (result from commands being run, not really necessary)
*/
double Parser::parseCommandString(string s)
{
    //Remove comments
    s = Parser::removeComments(s);

    //Instantiate variables for precedence handling
    int chars_right = 0;
    int chars_left = 0;
    string current_command;
    vector<string> expanded_vector;
    istringstream iss(s);

    //Handle precedence, continue loop until istringstream is finished writing
    //to current_command
    while (iss >> current_command)
    {
        //If parenthesis are found, then iterate over all inner commands and
        //connectors
        if (current_command.find("(") != string::npos)
        {
            for (int i = 0; i < current_command.size(); i++)
            {
                if (current_command.at(i) == ')')
                {
                    chars_right++;
                }
                else if (current_command.at(i) == '(')
                {
                    chars_left++;
                }
            }

            //New inside_string for temporary holding of inside commands and
            //connectors
            current_command += " ";
            string inside_string;

            //Add inner commands and connectors to inside_string
            while (iss >> inside_string)
            {
                for (int i = 0; i < inside_string.size(); i++)
                {
                    if (inside_string.at(i) == ')')
                    {
                        chars_right++;
                    }
                    else if (inside_string.at(i) == '(')
                    {
                        chars_left++;
                    }
                }

                //Add inside string to current command string
                current_command += inside_string;

                //Check if characters are equal, if not, then add space for
                //command parsing
                if (chars_left != chars_right)
                {
                    current_command = current_command + " ";
                }
                else
                {
                    break;
                }
            }
            //Reset variables for next loop/iteration
            chars_right = 0;
            chars_left = 0;
        }
        //Add command to the first converted vector
        expanded_vector.push_back(current_command);
    }
    string current_command_check = "";
    int current_command_size = 0;
    //Adds semi colon as a connector in expanded_vector
    for (int i = 0; i < expanded_vector.size(); i++)
    {
        current_command_check = expanded_vector.at(i);
        current_command_size = current_command_check.size();
        if (current_command_check.at(current_command_size - 1) == ';')
        {
            //Using iterator to insert into vector, void insert was not working
            //Record current point in vector + 1 to insert after
            vector<string>::iterator it = expanded_vector.begin() + (i + 1);
            //Remove semi-colon from string
            expanded_vector.at(i) =
                expanded_vector.at(i).substr(0,
                                             expanded_vector.at(i).size() - 1);
            //Insert semi-colon into vector at iterator position
            expanded_vector.insert(it, ";");
            //This caused too many problems. Not sure why needed, but it works
            i++;
        }
    }
    //Vector hold the completed commands, separated by connectors. Options are
    //Chunk (parenthesis), command, or, and, semi
    vector<Command *> completed_container;
    string command_hold;

    //Joins commands and connectors and sets appropriate type name to base
    //pointers
    for (int i = 0; i < expanded_vector.size(); i++)
    {
        if (expanded_vector.at(i) == ";")
        {
            completed_container.push_back(new Command(command_hold, "command"));
            completed_container.push_back(new Command(";", "semi"));
            command_hold.clear();
        }
        else if (expanded_vector.at(i) == "&&")
        {
            completed_container.push_back(new Command(command_hold, "command"));
            completed_container.push_back(new Command("&&", "and"));
            command_hold.clear();
        }
        else if (expanded_vector.at(i) == "||")
        {
            completed_container.push_back(new Command(command_hold, "command"));
            completed_container.push_back(new Command("||", "or"));
            command_hold.clear();
        }
        else if (expanded_vector.at(i) == "|")
        {
            completed_container.push_back(new Command(command_hold, "direction_command"));
            completed_container.push_back(new Command("|", "pipe"));
            command_hold.clear();
        }
        else if (expanded_vector.at(i) == "<")
        {
            completed_container.push_back(new Command(command_hold, "direction_command"));
            completed_container.push_back(new Command("<", "less_than"));
            command_hold.clear();
        }
        else if (expanded_vector.at(i) == ">")
        {
            completed_container.push_back(new Command(command_hold, "direction_command"));
            completed_container.push_back(new Command(">", "greater_than"));
            command_hold.clear();
        }
        else if (expanded_vector.at(i) == ">>")
        {
            completed_container.push_back(new Command(command_hold, "direction_command"));
            completed_container.push_back(new Command(">>", "greater_than_two"));
            command_hold.clear();
        }
        else if (expanded_vector.at(i).find("(") != string::npos)
        {
            completed_container.push_back(new Command(command_hold, "command"));
            expanded_vector.at(i).erase(expanded_vector.at(i).begin() + expanded_vector.at(i).find('('));
            expanded_vector.at(i).erase(expanded_vector.at(i).begin() + expanded_vector.at(i).find_last_of(')'));
            completed_container.push_back(new Command(expanded_vector.at(i),
                                                      "chunk"));
            command_hold.clear();
        }
        //Default add space for command
        else
        {
            command_hold += expanded_vector.at(i);
            if (command_hold.at(command_hold.size() - 1) != ' ')
            {
                command_hold += " ";
            }
        }
        //If command_hold is not empty after loop then add final command
        if (i + 1 == expanded_vector.size())
        {
            if (command_hold != "")
            {
                completed_container.push_back(new Command(command_hold,
                                                          "command"));
            }
        }
    }

    return Parser::runCommands(completed_container, -1);
}
double Parser::runCommands(vector<Command *> completed_container, double prev_rslt = -1)
{
    //Clean completed_container of empty strings, should probably be in above
    //function but doesnt matter
    for (int i = 0; i < completed_container.size(); i++)
    {
        if (completed_container.at(i)->evaluate() == "")
        {
            completed_container.erase(completed_container.begin() + i);
        }
    }
    //Variables for completed_container iteration
    string command_case = "";
    int result = 0;
    Solo *runCommand = new Solo();
    AndCommand *prev_and_cmd = new AndCommand();
    string inputFile;
    string outputFile;
    int usePreviousDirection = 0;
    int pid;
    int pipe1[2];
    int pipe2[2];

    //Loop through completed_container
    for (int i = 0; i < completed_container.size(); i++)
    {   
        command_case = completed_container.at(i)->get_type();
        //Prev_rslt = always run command, but check for chunk and call recursion
        //if it is chunk, else normal command
        if (prev_rslt == -1)
        {
            if (command_case == "chunk")
            {
                prev_rslt =
                    Parser::parseCommandString(completed_container.at(i)->evaluate());
            }
            else if (command_case == "command")
            {
                runCommand = new Solo(completed_container.at(i));
                prev_rslt = runCommand->execute();
            }
            else if (command_case == "direction_command")
            {
                if (completed_container.at(i + 1)->get_type() == "greater_than",
                    "less_than", "greater_than_two", "pipe")
                {
                    usePreviousDirection = 1;
                    prev_rslt = -2;
                }
            }
            else
            {
                perror("Command starting with connector not allowed!");
            }
        }
        //AND connector, check for chunk and if chunk and 1 prev_rslt then
        //recursion else normal command
        else if (command_case == "and")
        {
            if (completed_container.at(i + 1)->get_type() == "chunk" &&
                prev_rslt == 1)
            {
                prev_rslt = Parser::parseCommandString(completed_container.at(i + 1)->evaluate());
            }
            else if (completed_container.at(i + 1)->get_type() == "command")
            {
                AndCommand *and_result = new AndCommand(prev_rslt,
                                                        completed_container.at(i + 1));
                prev_rslt = and_result->execute();
            }
        }
        //AND connector, check for chunk and if chunk and 0 prev_rslt then
        //recursion else normal command
        else if (command_case == "or")
        {
            if (completed_container.at(i + 1)->get_type() == "chunk" &&
                prev_rslt == 0)
            {
                prev_rslt = parseCommandString(completed_container.at(i + 1)->evaluate());
            }
            else if (completed_container.at(i + 1)->get_type() == "command")
            {
                OrCommand *and_result = new OrCommand(prev_rslt,
                                                      completed_container.at(i + 1));
                prev_rslt = and_result->execute();
            }
        }
        else if (command_case == "pipe")
        {
            vector<string> commands;
			istringstream iss(completed_container.at(i - 1)->evaluate());

			for (string s; iss >> s;)
			{
				commands.push_back(s);
			}

			char *args_left[commands.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands.size(); i++)
			{
				args_left[i] = (char *)commands.at(i).c_str();
			}

			args_left[commands.size()] = 0;
            
            vector<string> commands2;
			istringstream iss2(completed_container.at(i + 1)->evaluate());

			for (string s; iss2 >> s;)
			{
				commands2.push_back(s);
			}

			char *args_right[commands2.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands2.size(); i++)
			{
				args_right[i] = (char *)commands2.at(i).c_str();
			}
			args_right[commands2.size()] = 0;
            if (pipe(pipe1) == -1)
            {
                perror("bad pipe1");
                exit(1);
            }
            
            if ((pid = fork()) == -1)
            {
                perror("bad fork1");
                exit(1);
            }
            else if (pid == 0)
            { 
                dup2(pipe1[1], 1);
                
                close(pipe1[0]);
                close(pipe1[1]);
                
                execvp(args_left[0], args_left);
                
                perror("bad exec ps");
                _exit(1);
            }

            if ((pid = fork()) == -1)
            {
                perror("bad fork2");
                exit(1);
            }
            else if (pid == 0)
            {
                dup2(pipe1[0], 0);

                close(pipe1[0]);
                close(pipe1[1]);

                execvp(args_right[0], args_right);
                
                perror("bad exec grep root");
                _exit(1);
            }
            close(pipe1[0]);
            close(pipe1[1]);
            int status;
            if (pid > 0)
			{
				int waitPid;
				do
				{
					waitPid = waitpid(pid, &status, WUNTRACED);
				} while (waitPid == -1 && errno == EINTR);
				if (waitPid == -1)
				{
					perror("Error waiting for child process");
				}
				else if (WEXITSTATUS(status) == 0)
				{
					
				}
				else if (WEXITSTATUS(status) != 0)
				{
					
				}
			}
            prev_rslt = 1;
        }
        else if (command_case == "less_than")
        {
            vector<string> commands;
			istringstream iss(completed_container.at(i - 1)->evaluate());

			for (string s; iss >> s;)
			{
				commands.push_back(s);
			}

			char *args_left[commands.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands.size(); i++)
			{
				args_left[i] = (char *)commands.at(i).c_str();
			}

			args_left[commands.size()] = 0;
            
            vector<string> commands2;
			istringstream iss2(completed_container.at(i + 1)->evaluate());

			for (string s; iss2 >> s;)
			{
				commands2.push_back(s);
			}

			char *args_right[commands2.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands2.size(); i++)
			{
				args_right[i] = (char *)commands2.at(i).c_str();
			}
			args_right[commands2.size()] = 0;
            int in;
            in = open(args_right[0], O_RDONLY);

            pid_t pid = fork();
			int status;

			if (pid < 0)
			{
				perror("Error");
				exit(1);
			}
			else if (pid == 0)
			{
				/* Execute command  */
                dup2(in, 0);
                close(in);
				if (execvp(args_left[0], args_left) < 0)
				{
					perror("*** ERROR: exec failed\n");
					exit(1);
				}
			}
			else if (pid > 0)
			{
				int waitPid;
				do
				{
					waitPid = waitpid(pid, &status, WUNTRACED);
				} while (waitPid == -1 && errno == EINTR);
				if (waitPid == -1)
				{
					perror("Error waiting for child process");
				}
				else if (WEXITSTATUS(status) == 0)
				{
					prev_rslt = 1;  
				}
				else if (WEXITSTATUS(status) != 0)
				{
					prev_rslt = 0;
				}
			}                
        }
        else if (command_case == "greater_than")
        {
            vector<string> commands;
			istringstream iss(completed_container.at(i - 1)->evaluate());

			for (string s; iss >> s;)
			{
				commands.push_back(s);
			}

			char *args_left[commands.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands.size(); i++)
			{
				args_left[i] = (char *)commands.at(i).c_str();
			}

			args_left[commands.size()] = 0;
            
            vector<string> commands2;
			istringstream iss2(completed_container.at(i + 1)->evaluate());

			for (string s; iss2 >> s;)
			{
				commands2.push_back(s);
			}

			char *args_right[commands2.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands2.size(); i++)
			{
				args_right[i] = (char *)commands2.at(i).c_str();
			}
			args_right[commands2.size()] = 0;
            int out;
            out = open(args_right[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

            pid_t pid = fork();
			int status;

			if (pid < 0)
			{
				perror("Error");
				exit(1);
			}
			else if (pid == 0)
			{
				/* Execute command  */
                dup2(out, 1);
                close(out);
				if (execvp(args_left[0], args_left) < 0)
				{
					perror("*** ERROR: exec failed\n");
					exit(1);
				}
			}
			else if (pid > 0)
			{
				int waitPid;
				do
				{
					waitPid = waitpid(pid, &status, WUNTRACED);
				} while (waitPid == -1 && errno == EINTR);
				if (waitPid == -1)
				{
					perror("Error waiting for child process");
				}
				else if (WEXITSTATUS(status) == 0)
				{
					prev_rslt = 1;
				}
				else if (WEXITSTATUS(status) != 0)
				{
					prev_rslt = 0;
				}
			} 
        }
        else if (command_case == "greater_than_two")
        {
            vector<string> commands;
			istringstream iss(completed_container.at(i - 1)->evaluate());

			for (string s; iss >> s;)
			{
				commands.push_back(s);
			}

			char *args_left[commands.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands.size(); i++)
			{
				args_left[i] = (char *)commands.at(i).c_str();
			}

			args_left[commands.size()] = 0;
            
            vector<string> commands2;
			istringstream iss2(completed_container.at(i + 1)->evaluate());

			for (string s; iss2 >> s;)
			{
				commands2.push_back(s);
			}

			char *args_right[commands2.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands2.size(); i++)
			{
				args_right[i] = (char *)commands2.at(i).c_str();
			}
			args_right[commands2.size()] = 0;
            
            pid_t pid = fork();
			int status;

			if (pid < 0)
			{
				perror("Error");
				exit(1);
			}
			else if (pid == 0)
			{
				/* Execute command  */
                int file_desc = open(args_right[0], O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); 
                if(file_desc < 0) {
                    printf("Error opening the file\n");
                } 
                dup2(file_desc, 1);
                close(file_desc);
				if (execvp(args_left[0], args_left) < 0)
				{
					perror("*** ERROR: exec failed\n");
					exit(1);
				}
			}
			else if (pid > 0)
			{
				int waitPid;
				do
				{
					waitPid = waitpid(pid, &status, WUNTRACED);
				} while (waitPid == -1 && errno == EINTR);
				if (waitPid == -1)
				{
					perror("Error waiting for child process");
				}
				else if (WEXITSTATUS(status) == 0)
				{
					prev_rslt = 1;  
				}
				else if (WEXITSTATUS(status) != 0)
				{
					prev_rslt = 0;  
				}
			}         
        }
        //if semi_colon is present, then set prev_rslt to -1 to always run next
        //command
        else if (command_case == "semi")
        {
            prev_rslt = -1;
        }
    }
    return prev_rslt;
}

#endif