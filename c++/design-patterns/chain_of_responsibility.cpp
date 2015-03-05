/*
 * =====================================================================================
 *
 *       Filename:  chain_of_responsibility.cpp
 *
 *    Description:  Chain of Responsibility Pattern
 *
 *        Version:  1.0
 *        Created:  2014年09月05日 15时24分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */

/**
 *  Chain of Responsibility 模式中 ConcreteHandler 将自己的后继对象(向下传递消息的对
 *  象)记录在自己的后继表中,当一个请求到来时,ConcreteHandler 会先检查看自己有没有
 *  匹配的处理程序,如果有就自己处理,否则传递给它的后继。当然这里示例程序中为了简化,
 *  ConcreteHandler 只是简单的检查看自己有没有后继,有的话将请求传递给后继进行处理,
 *  没有的话就自己处理。
 *
 *  Chain of Responsibility 模式描述其实就是这样一类问题将可能处理一个请求的对象链
 *  接成一个链,并将请求在这个链上传递,直到有对象处理该请求
 */

#include <iostream>
using namespace std;

class Handle
{
    public:
        virtual ~Handle();
        virtual void HandleRequest() = 0;
        void SetSuccessor(Handle* succ);
        Handle* GetSuccessor();
    protected:
        Handle();
        Handle(Handle* succ);
    private:
        Handle* _succ;
};

class ConcreteHandleA:public Handle
{
    public:
        ConcreteHandleA();
        ~ConcreteHandleA();
        ConcreteHandleA(Handle* succ);
        void HandleRequest();
    protected:
    private:
};

class ConcreteHandleB:public Handle
{
    public:
        ConcreteHandleB();
        ~ConcreteHandleB();
        ConcreteHandleB(Handle* succ);
        void HandleRequest();
    protected:
    private:
};

/**
 *  Handle
 */
Handle::Handle()
{
    _succ = 0;  // 0:NULL
}
Handle::~Handle()
{
    delete _succ;
}
Handle::Handle(Handle* succ)
{
    this->_succ = succ;
}
void Handle::SetSuccessor(Handle* succ)
{
    _succ = succ;
}
Handle* Handle::GetSuccessor()
{
    return _succ;
}
void Handle::HandleRequest()
{
}

/**
 *  A Concrete For Handle
 */
ConcreteHandleA::ConcreteHandleA()
{
}
ConcreteHandleA::ConcreteHandleA(Handle* succ):Handle(succ)
{
}
ConcreteHandleA::~ConcreteHandleA()
{
}
void ConcreteHandleA::HandleRequest()
{
    if (this->GetSuccessor() != 0)
    {
        cout<<"ConcreteHandleA 我把处理权给后继节点....."<<endl;
        this->GetSuccessor()->HandleRequest();
    }
    else
    {
        cout<<"ConcreteHandleA 没有后继了,我必须自己处理...."<<endl;
    }
}

/**
 *  B Concrete For Handle
 */
ConcreteHandleB::ConcreteHandleB()
{
}
ConcreteHandleB::ConcreteHandleB(Handle* succ):Handle(succ)
{
}
ConcreteHandleB::~ConcreteHandleB()
{
}
void ConcreteHandleB::HandleRequest()
{
    if (this->GetSuccessor() != 0)
    {
        cout<<"ConcreteHandleB 我把处理权给后继节点....."<<endl;
        this->GetSuccessor()->HandleRequest();
    }
    else
    {
        cout<<"ConcreteHandleB 没有后继了,我必须自己处理...."<<endl;
    }
}

/**
 *  Client
 */

/**
 *  ConcreteHandleA 的对象和 h1 拥有一个后继 ConcreteHandleB 的对象 h2,当一个请求到来时
 *  候,h1 检查看自己有后继,于是 h1 直接将请求传递给其后继 h2 进行处理,h2 因为没有后
 *  继,当请求到来时候,就只有自己提供响应了。于是程序的输出为:
 *  1) ConcreteHandleA 我把处理权给后继节点.....;
 *  2) ConcreteHandleB 没有后继了,我必须自己处理....。
 */
int main(int argc,char* argv[])
{
    Handle* h1 = new ConcreteHandleA();
    Handle* h2 = new ConcreteHandleB();
    h1->SetSuccessor(h2);   //Requset H2
    h1->HandleRequest();
    return 0;
}
