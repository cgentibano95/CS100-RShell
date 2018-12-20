# Squiggly Squid RShell

![alt text](https://images-wixmp-ed30a86b8c4ca887773594c2.wixmp.com/intermediary/f/55f7dd68-3733-4ad9-b55b-05d003dd9181/d3ie252-bf1ab771-35bc-4f35-8c96-e79ee758061f.jpg/v1/fill/w_900,h_720,q_70,strp/squiggly_squid_id_by_gordoluis_d3ie252-fullview.jpg)

## UML
![alt text](https://raw.githubusercontent.com/cs100/assignment-1-operation-squiggly-squid/master/src/UML_ASSN3.png?token=ApoRHuTHUc2cwvGko-2fJDJnjpFTE42kks5cHE9GwA%3D%3D)

## Installation
```bash
$ git clone https://github.com/cs100/assignment-1-operation-squiggly-squid.git

$ cmake . (if on hammer, use cmake3 .)

$ make

$ ./rshell
```

## Introduction

This is the RShell created by Jason Jewell & Christopher Genitbano for CS100 Fall 2018 at UCR. Our program can take in various commands that are also used in bash & Ubuntu. In order to do so we used a composite design pattern which consists of a Base, Command, Operator, UOperator, AndCommand, OrCommand, Solo, and an Exit class.

The Composite for this is the Solo, Operator, UOperator, AndCommand, OrCommand, and exit classes. The Solo class runs a command directly and returns the result (1 for success, 0 for failure). The Operator class is used to set the left and right "children" for the arguements for the AndCommand and OrCommand classes. The UOperator class is used to set the child for the solo class. The AndCommand and OrCommand classes is used to handle the logic to determine whether a command should be run or not. The Exit class is used to exit the rshell program. All functionality would be identical to the connectors and commands of bash. For example using the &&, ||,; and # symbols each have their feature when using the RShell.

In our main.cpp file, we determine if there is a username and host and we print that to the console to imitate the regular linux command line. We then take in the user input and send the command line string to parser.h in order to parse the string for further execution.

Upon completion the parse_command function passes in the joined vector to the run_commands function which takes in a vector and a prev_result variable. The vector is the joined commands and the prev_result is the result of the last command run (deffaulted to -1 to signify that the first command should always be run). 

Currently our Rshell can only run commands that can be executed through a path directory.

## Functionality

Takes in commands with the following format:
>cmd = executable [argumentList] [connector] [cmd]

Some Examples listed below:

> $ ls -a;
> $ echo hello && echo world
> $ ls -a || echo hello
> $ mkdir test
> $ ls -a; echo hello; mkdir test

## Features

Can run all bash commands, and can also take in any commands that consists of the following connectors:

	- ";" Semi-colon: Will execute the line provided before semi-colon
	- "&&" And: Runs second command if the first command can run
	- "#" Hash Sign: Comments out any string provided after the sign
	- "||" Or: If the first command fails, it will try to run the second.
	- "|" Pipe: General purpose process and command chaining tool
	- ">" Greater Than: Redirector for files
	- ">>" Redirects stdout to a file.
	- "<" Less Than: Companion command to ">", and often used in combination with it
	
## Parsing Process for each command

In order to have to run each command correctly based on the connector, we needed to parse the provided line in the correct form. During the parsing process we had to implement vectors[] to move certain commands in the correct order. After we have parsed the string correctly, we then stored the string into an vector of strings, then lastly converted into a char* array so that execvp can runthe correct arguments for the command to run.

## Bugs

A few of the bugs that we have ran into consists of:

        - Cannot run cd and a few other built-in bash commands
        - Commands with various pipes "|", not chaining correctly
        - Cannot handle single quotes
