/*
 * =====================================================================================
 *
 *       Filename:  state.cpp
 *
 *    Description:  State Mode
 *
 *        Version:  1.0
 *        Created:  2014年09月03日 15时00分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
using namespace std;

/**
 *  State 模式在实现中,有两个关键点:
 *  1)将 State 声明为 Context 的友元类(friend class),其作用是让 State 模式访问 Context
 *  的 protected 接口 ChangeSate()。
 *  2)State 及其子类中的操作都将 Context*传入作为参数,其主要目的是 State 类可以通
 *  过这个指针调用 Context 中的方法(在本示例代码中没有体现)
 *  。这也是 State 模式和 Strategy
 *  模式的最大区别所在。
 *  运行了示例代码后可以获得以下的结果:连续 3 次调用了 Context 的 OprationInterface()
 *  因为每次调用后状态都会改变(A-B-A),因此该动作随着 Context 的状态的转变而获得了不同的结果。
 */

class State
{
    public:
        State();
        virtual ~State();
        virtual void OperationInterface(Context* ) = 0;
        virtual void OperationChangeState(Context*) = 0;
    protected:
        bool ChangeState(Context* con,State* st);
    private:
        //bool ChangeState(Context* con,State* st);
};

class ConcreteStateA:public State
{
    public:
        ConcreteStateA();
        virtual ~ConcreteStateA();
        virtual void OperationInterface(Context* );
        virtual void OperationChangeState(Context*);
    protected:
    private:
};

class ConcreteStateB:public State
{
    public:
        ConcreteStateB();
        virtual ~ConcreteStateB();
        virtual void OperationInterface(Context* );
        virtual void OperationChangeState(Context*);
    protected:
    private:
};

class Context
{
    public:
        Context();
        Context(State* state);
        ~Context();
        void OprationInterface();
        void OperationChangState();
    protected:
    private:
        friend class State; //表明在 State 类中可以访问 Context 类的 private 字段
        bool ChangeState(State* state);
    private:
        State* _state;
};

/**
 *  State
 */
State::State()
{
}

State::~State()
{
}

void State::OperationInterface(Context* con)
{
    cout<<"State::.."<<endl;
}

bool State::ChangeState(Context* con,State* st)
{
    con->ChangeState(st);
    return true;
}
void State::OperationChangeState(Context* con)
{
}

/**
 *  ConcreteStateA
 */
ConcreteStateA::ConcreteStateA()
{
}

ConcreteStateA::~ConcreteStateA()
{
}

void ConcreteStateA::OperationInterface(Context* con)
{
    cout<<"ConcreteStateA::OperationInterface......"<<endl;
}

void ConcreteStateA::OperationChangeState(Context* con)
{
    OperationInterface(con);
    this->ChangeState(con,new ConcreteStateB());
}


/**
 *  ConcreteStateB
 */
ConcreteStateB::ConcreteStateB()
{
}

ConcreteStateB::~ConcreteStateB()
{
}

void ConcreteStateB::OperationInterface(Context* con)
{
    cout<<"ConcreteStateB::OperationInterface......"<<endl;
}

void ConcreteStateB::OperationChangeState(Context* con)
{
    OperationInterface(con);
    this->ChangeState(con,new ConcreteStateA());
}

/**
 *  Context
 */
Context::Context()
{
}

Context::Context(State* state)
{
    this->_state = state;
}

Context::~Context()
{
    delete _state;
}

void Context::OprationInterface()
{
    _state->OperationInterface(this);
}

bool Context::ChangeState(State* state)
{
    ///_state->ChangeState(this,state);
    this->_state = state;
    return true;
}

void Context::OperationChangState()
{
    _state->OperationChangeState(this);
}

/**
 *  Client
 */
int main(int argc,char* argv[])
{
    State* st = new ConcreteStateA();
    Context* con = new Context(st);
    con->OperationChangState();
    con->OperationChangState();
    con->OperationChangState();
    if (con != NULL)
        delete con;
    if (st != NULL)
        st = NULL;
    return 0;
}
