/*
 * =====================================================================================
 *
 *       Filename:  command.cpp
 *
 *    Description:  Command Pattern 命令模式
 *
 *        Version:  1.0
 *        Created:  2014年09月05日 10时44分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */

/**
 *  Command 模式通过将请求封装到一个对象(Command)中,并将请求的接受者存放到
 *  具体的 ConcreteCommand 类中(Receiver)中,从而实现调用操作的对象和操作的具体实现
 *  者之间的解耦。
 *
 *  Command 模式结构图中,将请求的接收者(处理者)放到 Command 的具体子类
 *  ConcreteCommand 中,当请求到来时(Invoker 发出 Invoke 消息激活 Command 对象),
 *  ConcreteCommand 将处理请求交给 Receiver 对象进行处理。
 */

#include <iostream>
using namespace std;

class Reciever
{
    public:
        Reciever();
        ~Reciever();
        void Action();
    protected:
    private:
};

class Command
{
    public:
        virtual ~Command();
        virtual void Excute() = 0;
    protected:
        Command();
    private:
};

class ConcreteCommand:public Command
{
    public:
        ConcreteCommand(Reciever* rev);
        ~ConcreteCommand();
        void Excute();
    protected:
    private:
        Reciever* _rev;
};

class Invoker
{
    public:
        Invoker(Command* cmd);
        ~Invoker();
        void Invoke();
    protected:
    private:
        Command* _cmd;
};

/**
 *  Reciever
 */
Reciever::Reciever()
{
}
Reciever::~Reciever()
{
}
void Reciever::Action()
{
    std::cout<<"Reciever action......."<<std::endl;
}

/**
 *  Command
 */
Command::Command()
{
}
Command::~Command()
{
}
void Command::Excute()
{
}

/**
 *  ConcreteCommand
 */
ConcreteCommand::ConcreteCommand(Reciever* rev)
{
    this->_rev = rev;
}
ConcreteCommand::~ConcreteCommand()
{
    delete this->_rev;
}
void ConcreteCommand::Excute()
{
    _rev->Action();
    std::cout<<"ConcreteCommand..."<<std::endl;
}

/**
 *  Invoke
 */
Invoker::Invoker(Command* cmd)
{
    _cmd = cmd;
}
Invoker::~Invoker()
{
    delete _cmd;
}
void Invoker::Invoke()
{
    _cmd->Excute();
}

/**
 *  Client
 *
 *  Command 模式在实现的实现和思想都很简单,其关键就是将一个请求封装到一个类中
 *  (Command),再提供处理对象(Receiver),最后 Command 命令由 Invoker 激活。
 */
int main(int argc,char* argv[])
{
    Reciever* rev = new Reciever();
    Command* cmd = new ConcreteCommand(rev);
    Invoker* inv = new Invoker(cmd);
    inv->Invoke();
    return 0;
}
