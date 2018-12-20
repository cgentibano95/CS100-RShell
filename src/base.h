#ifndef BASE_H
#define BASE_H

#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace std;
/*
* Base class
*/
class Base
{
  public:
	/* Constructors */
	Base(){};

	/* Pure Virtual Functions */
	virtual string evaluate() = 0;
};

/*
* Command class - leaf
* Used for holding the command / connector string
*/
class Command : public Base
{
  private:
	string command_str;
	string type;

  public:
	Command() {}
	Command(string cmd, string command_type)
	{
		command_str = cmd;
		type = command_type;
	}
	string evaluate()
	{
		return command_str;
	}
	string get_type()
	{
		return type;
	}
};

/*
* UOperator class - Composite
* Used in running the solo class, holds Base*
*/
class Operator : public Base
{
  public:
	double leftChild;
	Base *rightChild;
	Operator(){};
	Operator(double left, Base *right)
	{
		leftChild = left;
		rightChild = right;
	}
	virtual string evaluate() = 0;
};

/*
* UOperator class - Composite
* Used in running the solo class, holds Base*
*/
class UOperator : public Base
{
  public:
	Base *command;
	UOperator(){};
	UOperator(Base *cmd)
	{
		command = cmd;
	}

	virtual string evaluate() = 0;
};

/*
* Exit class - leaf
* Used for exiting the program
*/
class Exit : public Base
{
  public:
	Exit() : Base(){};

	string evaluate()
	{
		cout << "Exiting RSHELL" << endl;
		exit(0);
		return "Exited Program";
	}
};

//Composite-And
class AndCommand : public Operator
{
  public:
	AndCommand(){};
	AndCommand(double xVal, Base *yVal) : Operator(xVal, yVal) {}

	string evaluate() {}
	double execute();
};

//Composite-Or
class OrCommand : public Operator
{
  public:
	OrCommand(){};
	OrCommand(double xVal, Base *yVal) : Operator(xVal, yVal) {}
	string evaluate() {}
	double execute();
};

/*
* Solo class - composite
* Executes the command and returns the result
*/
class Solo : public UOperator
{
  public:
	Solo(){};
	Solo(Base *first_cmd) : UOperator(first_cmd) {}
	string evaluate() {}
	double execute();
};

double testCommand(Base *command)
{
	string current_flag = "";
	string test_cmd_str = command->evaluate();
	struct stat stat_buffer;

	//Check if the first 6 letters (which is the beginning of a command) is
	//"test" in order to trim the correct whitepspace around the command
	if (test_cmd_str.substr(0, 4) == "test")
	{
		//Remove "test" from command
		test_cmd_str = test_cmd_str.substr(4, test_cmd_str.size() - 1);
		//Trim white space in beginning
		for (int i = 0; i < test_cmd_str.size(); i++)
		{
			if (i < 3 && test_cmd_str.at(0) == ' ')
			{
				test_cmd_str = test_cmd_str.substr(1, test_cmd_str.size() - 1);
			}
		}
		//Trim white space at end of command
		while (test_cmd_str.at(test_cmd_str.size() - 1) == ' ')
		{
			test_cmd_str = test_cmd_str.substr(0, test_cmd_str.size() - 1);
		}
	}
	//Adjust command for []
	else
	{
		//Remove beginning and ending brackets and trim white spaces
		test_cmd_str = test_cmd_str.substr(1, test_cmd_str.size() - 1);
		test_cmd_str = test_cmd_str.substr(0, test_cmd_str.size() - 2);

		while (test_cmd_str.at(0) == ' ')
		{
			test_cmd_str = test_cmd_str.substr(1, test_cmd_str.size() - 1);
		}
		while (test_cmd_str.at(test_cmd_str.size() - 1) == ' ')
		{
			test_cmd_str = test_cmd_str.substr(0, test_cmd_str.size() - 1);
		}
	}

	//Check for current flag and account for spaces for no errors in the command
	if (test_cmd_str.substr(0, 3) == "-f " || test_cmd_str.substr(0, 3) == "-d " || test_cmd_str.substr(0, 3) == "-e ")
	{
		current_flag = test_cmd_str.substr(0, 2);
		test_cmd_str = test_cmd_str.substr(3, test_cmd_str.size() - 1);
	}
	//Check if flag is invalid
	else if (test_cmd_str.at(0) == '-')
	{
		cout << "INVALID FLAG" << endl;
		cout << "False" << endl;
		return 0;
	}
	//Default flag -e
	else
	{
		current_flag = "-e";
	}

	//Depending on current flag, execute the command and return result for
	//following commands
	//Checks if the file/directory exists and is a regular file '-f'
	if (current_flag == "-f")
	{
		if (stat(test_cmd_str.c_str(), &stat_buffer) == 0)
		{
			if (S_ISREG(stat_buffer.st_mode & S_IFMT))
			{
				cout << "True" << endl;
				return 1;
			}
		}
	}
	//Checks if the file/directory exists '-e'
	else if (current_flag == "-e")
	{
		if (stat(test_cmd_str.c_str(), &stat_buffer) == 0)
		{
			cout << "True" << endl;
			return 1;
		}
	}
	//Checks if the file/directory exists and is a directory '-d'
	else
	{
		if (stat(test_cmd_str.c_str(), &stat_buffer) == 0)
		{
			if (S_ISDIR(stat_buffer.st_mode & S_IFMT))
			{
				cout << "True" << endl;
				return 1;
			}
		}
	}
	//If all else fails, print fail and return 0
	cout << "False" << endl;
	return 0;
}

/* Check if command is a test command, return true or false, if true then run */
bool isTest(Base *command)
{
	//Variables
	vector<string> commands;
	istringstream iss(command->evaluate());
	string test_cmd_str = "";

	for (string s; iss >> s;)
	{
		commands.push_back(s);
	}
	//Check if the current command is a test and return true or false
	for (int i = 0; i < commands.size(); i++)
	{
		if (i == 0 && commands.at(i) == "[")
		{
			return true;
		}
		//Add character to string and check if it is = "test"
		else
		{
			test_cmd_str += commands.at(i);
		}
		if (test_cmd_str == "test")
		{
			return true;
		}
	}
	return false;
}

double runCommand(Base *command)
{
	/* Declare variables to use in function */
	string cmd_string = command->evaluate();
	string exit_string = "exit";
	if (cmd_string.find(exit_string) != string::npos)
	{
		Exit *newExit = new Exit();
		newExit->evaluate();
	}
	else
	{
		if (isTest(command))
		{
			return testCommand(command);
		}
		else
		{
			vector<string> commands;
			istringstream iss(command->evaluate());

			for (string s; iss >> s;)
			{
				commands.push_back(s);
			}

			char *args[commands.size() + 1];
			/* Inserts arguments into args */
			for (int i = 0; i < commands.size(); i++)
			{
				args[i] = (char *)commands.at(i).c_str();
			}

			args[commands.size()] = 0;
			/* Executes command below */
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
				if (execvp(args[0], args) < 0)
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
					return 1;
				}
				else if (WEXITSTATUS(status) != 0)
				{
					return 0;
				}
			}
		}
	}
}
double Solo::execute()
{
	return runCommand(command);
}
/* Logic for executing an AND command */
double AndCommand::execute()
{
	if (leftChild == 1)
	{
		return runCommand(rightChild);
	}
	else
	{
		return leftChild;
	}
}
/* Logic for executing an OR command */
double OrCommand::execute()
{
	if (leftChild == 0)
	{
		return runCommand(rightChild);
	}
	else
	{
		return leftChild;
	}
}
#endif
