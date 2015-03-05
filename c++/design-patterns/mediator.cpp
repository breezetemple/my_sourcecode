/*
 * =====================================================================================
 *
 *       Filename:  mediator.cpp
 *
 *    Description:  Mediator Mode 中介者模式
 *
 *        Version:  1.0
 *        Created:  2014年09月05日 10时16分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */

/**
 *  在面向对象系统的设计和开发过程中,对象之间的交互和通信是最为常见的情况,因为
 *  对象间的交互本身就是一种通信。在系统比较小的时候,可能对象间的通信不是很多、对象
 *  也比较少,我们可以直接硬编码到各个对象的方法中。但是当系统规模变大,对象的量变引
 *  起系统复杂度的急剧增加,对象间的通信也变得越来越复杂,这时候我们就要提供一个专门
 *  处理对象间交互和通信的类,这个中介者就是 Mediator 模式。Mediator 模式提供将对象间
 *  的交互和通讯封装在一个类中,各个对象间的通信不必显势去声明和引用,大大降低了系统
 *  的复杂性能(了解一个对象总比深入熟悉 n 个对象要好)。另外 Mediator 模式还带来了系统
 *  对象间的松耦合
 *
 *  Mediator 模式是一种很有用并且很常用的模式,它通过将对象间的通信封装到一个类
 *  中,将多对多的通信转化为一对多的通信,降低了系统的复杂性。Mediator 还获得系统解耦
 *  的特性,通过 Mediator,各个 Colleague 就不必维护各自通信的对象和通信协议,降低了系
 *  统的耦合性,Mediator 和各个 Colleague 就可以相互独立地修改了。
 *  Mediator 模式还有一个很显著额特点就是将控制集中,集中的优点就是便于管理,也正
 *  式符合了 OO 设计中的每个类的职责要单一和集中的原则。
 */

#include <stdlib.h>
#include <string>
using namespace std;
class Mediator;

/**
 *  Mediator 模式中,每个 Colleague 维护一个 Mediator,当要进行交互,例如图中
 *  ConcreteColleagueA 和 ConcreteColleagueB 之间的交互就可以通过 ConcreteMediator 提供的
 *  DoActionFromAtoB 来处理, ConcreteColleagueA 和 ConcreteColleagueB 不必维护对各自的引
 *  用,甚至它们也不知道各个的存在。Mediator 通过这种方式将多对多的通信简化为了一
 *  (Mediator)对多(Colleague)的通信。
 */

class Colleage
{
    public:
        virtual ~Colleage();
        virtual void Aciton() = 0;
        virtual void SetState(const string& sdt) = 0;
        virtual string GetState() = 0;
    protected:
        Colleage();
        Colleage(Mediator* mdt);
        Mediator* _mdt;
    private:
};

class ConcreteColleageA:public Colleage
{
    public:
        ConcreteColleageA();
        ConcreteColleageA(Mediator* mdt);
        ~ConcreteColleageA();
        void Aciton();
        void SetState(const string& sdt);
        string GetState();
    protected:
    private:
        string _sdt;
};

class ConcreteColleageB:public Colleage
{
    public:
        ConcreteColleageB();
        ConcreteColleageB(Mediator* mdt);
        ~ConcreteColleageB();
        void Aciton();
        void SetState(const string& sdt);
        string GetState();
    protected:
    private:
        string _sdt;
};

class Mediator
{
    public:
        virtual ~Mediator();
        virtual void DoActionFromAtoB() = 0;
        virtual void DoActionFromBtoA() = 0;
    protected:
        Mediator();
    private:
};

class ConcreteMediator:public Mediator
{
    public:
        ConcreteMediator();
        ConcreteMediator(Colleage* clgA,Colleage* clgB);
        ~ConcreteMediator();
        void SetConcreteColleageA(Colleage* clgA);
        void SetConcreteColleageB(Colleage* clgB);
        Colleage* GetConcreteColleageA();
        Colleage* GetConcreteColleageB();
        void IntroColleage(Colleage* clgA,Colleage* clgB);
        void DoActionFromAtoB();
        void DoActionFromBtoA();
    protected:
    private:
        Colleage* _clgA;
        Colleage* _clgB;
};

/**
 *  Colleage
 */
Colleage::Colleage()
{
    //_sdt = " ";
}
Colleage::Colleage(Mediator* mdt)
{
    this->_mdt = mdt;
    //_sdt = " ";
}
Colleage::~Colleage()
{
}

/**
 *  ConcreteColleageA
 */
ConcreteColleageA::ConcreteColleageA()
{
}
ConcreteColleageA::~ConcreteColleageA()
{
}
ConcreteColleageA::ConcreteColleageA(Mediator* mdt):Colleage(mdt)
{
}
string ConcreteColleageA::GetState()
{
    return _sdt;
}
void ConcreteColleageA::SetState(const string& sdt)
{
    _sdt = sdt;
}
void ConcreteColleageA::Aciton()
{
    _mdt->DoActionFromAtoB();
    cout<<"State of ConcreteColleageB:"<<" "<<this->GetState()<<endl;
}

/**
 *  ConcreteColleageB
 */
ConcreteColleageB::ConcreteColleageB()
{
}
ConcreteColleageB::~ConcreteColleageB()
{
}
ConcreteColleageB::ConcreteColleageB(Mediator* mdt):Colleage(mdt)
{
}
string ConcreteColleageB::GetState()
{
    return _sdt;
}
void ConcreteColleageB::SetState(const string& sdt)
{
    _sdt = sdt;
}
void ConcreteColleageB::Aciton()
{
    _mdt->DoActionFromBtoA();
    cout<<"State of ConcreteColleageB:"<<" "<<this->GetState()<<endl;
}

/**
 *  Mediator
 */
Mediator::Mediator()
{
}
Mediator::~Mediator()
{
}

/**
 *  ConcreteMediator
 */
ConcreteMediator::ConcreteMediator()
{
}
ConcreteMediator::~ConcreteMediator()
{
}
ConcreteMediator::ConcreteMediator(Colleage* clgA,Colleage* clgB)
{
    this->_clgA = clgA;
    this->_clgB = clgB;
}
void ConcreteMediator::DoActionFromAtoB()
{
    _clgB->SetState(_clgA->GetState());
}
void ConcreteMediator::SetConcreteColleageA(Colleage* clgA)
{
    this->_clgA = clgA;
}
void ConcreteMediator::SetConcreteColleageB(Colleage* clgB)
{
    this->_clgB = clgB;
}
Colleage* ConcreteMediator::GetConcreteColleageA()
{
    return _clgA;
}
Colleage* ConcreteMediator::GetConcreteColleageB()
{
    return _clgB;
}
void ConcreteMediator::IntroColleage(Colleage* clgA,Colleage* clgB)
{
    this->_clgA = clgA;
    this->_clgB = clgB;
}
void ConcreteMediator::DoActionFromBtoA()
{
    _clgA->SetState(_clgB->GetState());
}

/**
 *  Client
 */

/**
 *  Mediator 模式的实现关键就是将对象 Colleague 之间的通信封装到一个类种单独处理,
 *  为了模拟 Mediator 模式的功能,这里给每个 Colleague 对象一个 string 型别以记录其状态,
 *  并通过状态改变来演示对象之间的交互和通信。这里主要就 Mediator 的示例运行结果给出
 *  分析:
 *  1)将 ConcreteColleageA 对象设置状态“old”,ConcreteColleageB 也设置状态“old”;
 *  2)ConcreteColleageA 对象改变状态,并在 Action 中和 ConcreteColleageB 对象进行通信,并改变
 *  ConcreteColleageB 对象的状态为“new”;
 *  3)ConcreteColleageB 对象改变状态,并在 Action 中和 ConcreteColleageA 对象进行通信,并改变
 *  ConcreteColleageA 对象的状态为“new”;
 *  注意到,两个 Colleague 对象并不知道它交互的对象,并且也不是显示地处理交互过程,这一切都是
 *  通过 Mediator 对象完成的,示例程序运行的结果也正是证明了这一点。
 */
int main(int argc,char* argv[])
{
#if 1
    ConcreteMediator* m = new ConcreteMediator();
    ConcreteColleageA* c1 = new ConcreteColleageA(m);
    ConcreteColleageB* c2 = new ConcreteColleageB(m);
    m->IntroColleage(c1,c2);
#else
    ConcreteMediator* m = new ConcreteMediator();
    ConcreteColleageA* c1 = new ConcreteColleageA(m);
    ConcreteColleageB* c2 = new ConcreteColleageB(m);
    m->IntroColleage(c1,c2);
#endif
    c1->SetState("old");
    c2->SetState("old");
    c1->Aciton();
    c2->Aciton();
    cout<<endl;
    c1->SetState("new");
    c1->Aciton();
    c2->Aciton();
    cout<<endl;
    c2->SetState("old");
    c2->Aciton();
    c1->Aciton();
    return 0;
}
