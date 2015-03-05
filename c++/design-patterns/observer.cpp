/*
 * =====================================================================================
 *
 *       Filename:  observer.cpp
 *
 *    Description:  Observer Mode 观察者模式
 *
 *        Version:  1.0
 *        Created:  2014年09月04日 16时34分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */

/**
 *  Observer 模式要解决的问题为:建立一个
 *  一(Subject)对多(Observer)的依赖关系,并且做到当“一”变化的时候,依赖这个“一”
 *  的多也能够同步改变。最常见的一个例子就是:对同一组数据进行统计分析时候,我们希望
 *  能够提供多种形式的表示(例如以表格进行统计显示、柱状图统计显示、百分比统计显示等)。
 *  这些表示都依赖于同一组数据,我们当然需要当数据改变的时候,所有的统计的显示都能够
 *  同时改变。Observer 模式就是解决了这一个问题。
 */

#include <iostream>
#include <list>
#include <string>

using namespace std;

typedef string State;
class Observer;

/**
 *  目标 Subject 提供依赖于它的观察者 Observer 的注册(Attach)和注销(Detach)
 *  操作,并且提供了使得依赖于它的所有观察者同步的操作(Notify)。观察者 Observer 则提
 *  供一个 Update 操作,注意这里的 Observer 的 Update 操作并不在 Observer 改变了 Subject 目
 *  标状态的时候就对自己进行更新,这个更新操作要延迟到 Subject 对象发出 Notify 通知所有
 *  Observer 进行修改(调用 Update)。
 *
 *  在 Observer 模式的实现中,Subject 维护一个 list 作为存储其所有观察者的容器。每当
 *  调用 Notify 操作就遍历 list 中的 Observer 对象,并广播通知改变状态(调用 Observer 的 Update
 *  操作)。目标的状态 state 可以由 Subject 自己改变(示例),也可以由 Observer 的某个操作引
 *  起 state 的改变(可调用 Subject 的 SetState 操作)。Notify 操作可以由 Subject 目标主动广播
 *  (示例),也可以由 Observer 观察者来调用(因为 Observer 维护一个指向 Subject 的指针)。
 *  运行示例程序,可以看到当 Subject 处于状态“old”时候,依赖于它的两个观察者都显
 *  示“old”,当目标状态改变为“new”的时候,依赖于它的两个观察者也都改变为“new”。
 */
class Subject
{
    public:
        virtual ~Subject();
        virtual void Attach(Observer* obv);
        virtual void Detach(Observer* obv);
        virtual void Notify();
        virtual void SetState(const State& st) = 0;
        virtual State GetState() = 0;
    protected:
        Subject();  //New操作只能是派生类调用
    private:
        list<Observer* >* _obvs;
};

class ConcreteSubject:public Subject
{
    public:
        ConcreteSubject();
        ~ConcreteSubject();
        State GetState();
        void SetState(const State& st);
    protected:
    private:
        State _st;
};

class Observer
{
    public:
        virtual ~Observer();
        virtual void Update(Subject* sub) = 0;
        virtual void PrintInfo() = 0;
    protected:
        Observer();
        State _st;
    private:
};

class ConcreteObserverA:public Observer
{
    public:
        virtual Subject* GetSubject();
        ConcreteObserverA(Subject* sub);
        virtual ~ConcreteObserverA();
        //传入 Subject 作为参数,这样可以让一个 View 属于多个的 Subject。
        void Update(Subject* sub);
        void PrintInfo();
    protected:
    private:
        Subject* _sub;
};

class ConcreteObserverB:public Observer
{
    public:
        virtual Subject* GetSubject();
        ConcreteObserverB(Subject* sub);
        virtual ~ConcreteObserverB();
        //传入 Subject 作为参数,这样可以让一个 View 属于多个的 Subject。
        void Update(Subject* sub);
        void PrintInfo();
    protected:
    private:
        Subject* _sub;
};

/**
 *  Subject
 */
Subject::Subject()
{
    //****在模板的使用之前一定要 new,创建
    _obvs = new list<Observer*>;
}

Subject::~Subject()
{
}

void Subject::Attach(Observer* obv)
{
    _obvs->push_front(obv);
}

void Subject::Detach(Observer* obv)
{
    if (obv != NULL)
        _obvs->remove(obv);
}

void Subject::Notify()
{
    list<Observer*>::iterator it;
    it = _obvs->begin();
    for (;it != _obvs->end();it++)
    {
        //关于模板和 iterator 的用法
        (*it)->Update(this);
    }
}

/**
 *  ConcreteSubject
 */
ConcreteSubject::ConcreteSubject()
{
    _st = '\0';
}

ConcreteSubject::~ConcreteSubject()
{
}

State ConcreteSubject::GetState()
{
    return _st;
}

void ConcreteSubject::SetState(const State& st)
{
    _st = st;
}

Observer::Observer()
{
    _st = '\0';
}

Observer::~Observer()
{
}

/**
 *  ConcreteObserverA
 */
ConcreteObserverA::ConcreteObserverA(Subject* sub)
{
    _sub = sub;
    _sub->Attach(this);
}

ConcreteObserverA::~ConcreteObserverA()
{
    _sub->Detach(this);
    if (_sub != 0)
    {
        delete _sub;
    }
}

Subject* ConcreteObserverA::GetSubject()
{
    return _sub;
}

void ConcreteObserverA::PrintInfo()
{
    cout<<"ConcreteObserverA observer.... "<<_sub->GetState()<<endl;
}

void ConcreteObserverA::Update(Subject* sub)
{
    _st = sub->GetState();
    PrintInfo();
}

/**
 *  ConcreteObserverB
 */
ConcreteObserverB::ConcreteObserverB(Subject* sub)
{
    _sub = sub;
    _sub->Attach(this);
}

ConcreteObserverB::~ConcreteObserverB()
{
    _sub->Detach(this);
    if (_sub != 0)
    {
        delete _sub;
    }
}

Subject* ConcreteObserverB::GetSubject()
{
    return _sub;
}

void ConcreteObserverB::PrintInfo()
{
    cout<<"ConcreteObserverB observer.... "<<_sub->GetState()<<endl;
}

void ConcreteObserverB::Update(Subject* sub)
{
    _st = sub->GetState();
    PrintInfo();
}

/**
 *  ConcreteObserverB
 */
int main(int argc,char* argv[])
{
    ConcreteSubject* sub = new ConcreteSubject();
    Observer* o1 = new ConcreteObserverA(sub);
    Observer* o2 = new ConcreteObserverB(sub);
    sub->SetState("old");
    sub->Notify();
    sub->SetState("new"); //也可以由 Observer 调用
    sub->Notify();
    return 0;
}
