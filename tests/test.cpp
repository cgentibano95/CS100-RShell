#include "../src/base.h"
#include "../src/parser.h"
#include "gtest/gtest.h"

TEST(SoloTest, SoloTestOne)
{
    string commandStr = "ls -a";
    Command *cmd = new Command(commandStr, "command");
    stringstream ss;
    string temp;
    auto old_buf = cout.rdbuf(ss.rdbuf());
    cmd->evaluate();
    cout.rdbuf(old_buf);
    temp = ss.str();
    EXPECT_EQ("ls -a", cmd->evaluate());
}

TEST(SoloTest, SoloTestTwo)
{
    string commandStr = "git status";
    Command *cmd = new Command(commandStr, "command");
    stringstream ss;
    string temp;
    auto old_buf = cout.rdbuf(ss.rdbuf());
    cmd->evaluate();
    cout.rdbuf(old_buf);
    temp = ss.str();
    EXPECT_EQ("git status", cmd->evaluate());
}

TEST(SoloTest, SoloTestThree)
{
    string commandStr = "&&";
    Command *cmd = new Command(commandStr, "and");
    stringstream ss;
    string temp;
    auto old_buf = cout.rdbuf(ss.rdbuf());
    cmd->evaluate();
    cout.rdbuf(old_buf);
    temp = ss.str();
    EXPECT_EQ("&&", cmd->evaluate());
}

TEST(ANDTest, Test)
{
    Command *cmd = new Command("echo world", "command");
    AndCommand *runCmd = new AndCommand(1, cmd);

    EXPECT_EQ(1, runCmd->execute());
}

TEST(ANDTest, TestTwo)
{
    Command *cmd = new Command("echo world", "command");
    AndCommand *runCmd = new AndCommand(0, cmd);

    EXPECT_EQ(0, runCmd->execute());
}

TEST(EchoOrTest, Test)
{
    Command *cmd = new Command("echo world", "command");
    OrCommand *runCmd = new OrCommand(1, cmd);

    EXPECT_EQ(1, runCmd->execute());
}

TEST(EchoOrTest, TestTwo)
{
    Command *cmd = new Command("echo world", "command");
    OrCommand *runCmd = new OrCommand(0, cmd);

    EXPECT_EQ(1, runCmd->execute());
}

TEST(EchoTest, Test)
{
    Command *cmd = new Command("echo hello", "command");
    Solo *runCmd = new Solo(cmd);

    EXPECT_EQ(1, runCmd->execute());
}

TEST(TestCommandTest, Test)
{
    Command *cmd = new Command("[ -e src ]", "command");
    OrCommand *runCmd = new OrCommand(0, cmd);

    EXPECT_EQ(1, runCmd->execute());
}

TEST(TestCommandTest, TestTwo)
{
    Command *cmd = new Command("[-e src ]", "command");
    OrCommand *runCmd = new OrCommand(0, cmd);

    EXPECT_EQ(0, runCmd->execute());
}

TEST(RemoveCommentsTest, Test)
{
    Parser* parse = new Parser();
    string s = "echo hello world #test test test";
    s = parse->removeComments(s);

    EXPECT_EQ("echo hello world ", s);
}
TEST(RemoveComments2, Test)
{
	Parser* parse = new Parser();
	string s = "echo hello #ignore this";
	s = parse->removeComments(s);
	
	EXPECT_EQ("echo hello ", s);
}

TEST(ParseCommandTest, Test)
{
	string s = "((echo hello && echo world) || (echo pop))";
	Parser* p = new Parser();
	
	int result = p->parseCommandString(s);
	EXPECT_EQ(1,result);
}

TEST(ParseCommandFailTest, Test)
{
	string s = "(echo hello && cd fakedir) && cd false";
	Parser* p = new Parser();
	
	int result = p->parseCommandString(s);
	EXPECT_EQ(0, result);
}
TEST(ExitTest, Test)
{
    Exit *exit = new Exit();

    EXPECT_EXIT(exit->evaluate(), ::testing::ExitedWithCode(0), "");
}

TEST(GreaterandLessTest, test)
{
        string s = "cat < existingInputFile > newoutputfile";
        cout << " press control d to exit cat " << endl ;
        Parser* p = new Parser();
        int result = p->parseCommandString(s);
        EXPECT_EQ(0, result);
}

TEST(GreaterthanTest, Test)
{
        string s = "ls -lR > dir-tree.list";
        cout << " press control d to exit cat " << endl ;
        Parser* p = new Parser();
        int result = p->parseCommandString(s);
        EXPECT_EQ(1, result);
}
TEST(PipeTest, test)
{
        string s = "cat < existingInputfile | tr A-Z a-z > newOutputFile2";
        cout << " press control d to exit cat " << endl ;
        Parser* p = new Parser();
        int result = p->parseCommandString(s);
        EXPECT_EQ(1, result);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

