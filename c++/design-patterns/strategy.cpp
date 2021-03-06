/*
 * =====================================================================================
 *
 *       Filename:  strategy.cpp
 *
 *    Description:  Strategy Mode
 *
 *        Version:  1.0
 *        Created:  2014年09月03日 14时20分10秒
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
 *  Strategy 模式和 Template 模式要解决的问题是相同(类似)的,都是为了给业务逻辑(算
 *  法)具体实现和抽象接口之间的解耦。Strategy 模式将逻辑(算法)封装到一个类(Context)
 *  里面,通过组合的方式将具体算法的实现在组合对象中实现,再通过委托的方式将抽象接口
 *  的实现委托给组合对象实现。State 模式也有类似的功能,他们之间的区别将在讨论中给出。
 *
 *  这里的关键就是将算法的逻辑抽象接口(DoAction)封装到一个类中(Context),再
 *  通过委托的方式将具体的算法实现委托给具体的 Strategy 类来实现(ConcreteStrategeA
 *  类)
 *
 *  Strategy 模式和 Template 模式解决了类似的问题,也正如在 Template 模式中
 *  分析的,Strategy 模式和 Template 模式实际是实现一个抽象接口的两种方式:继承和组合之
 *  间的区别。要实现一个抽象接口,继承是一种方式:我们将抽象接口声明在基类中,将具体
 *  的实现放在具体子类中。组合(委托)是另外一种方式:我们将接口的实现放在被组合对象
 *  中,将抽象接口放在组合类中。这两种方式各有优缺点,先列出来:
 *
 *  1) 继承:
 *  优点
 *  1)易于修改和扩展那些被复用的实现。
 *
 *  缺点
 *  1)破坏了封装性,继承中父类的实现细节暴露给子类了;
 *  2)“白盒”复用,原因在 1)中;
 *  3)当父类的实现更改时,其所有子类将不得不随之改变
 *  4)从父类继承而来的实现在运行期间不能改变(编译期间就已经确定了)。
 *
 *  2) 组合
 *  优点
 *  1)“黑盒”复用,因为被包含对象的内部细节对外是不可见的;
 *  2)封装性好,原因为 1)
 *  3)实现和抽象的依赖性很小(组合对象和被组合对象之间的依赖性小)
 *  4)可以在运行期间动态定义实现(通过一个指向相同类型的指针,典型的是抽象
 *  基类的指针).
 *   
 *  缺点
 *  1)系统中对象过多。
 *  从上面对比中我们可以看出,组合相比继承可以取得更好的效果,因此在面向对象
 *  的设计中的有一条很重要的原则就是:优先使用(对象)组合,而非(类)继承(Favor
 *  Composition Over Inheritance)。
 *  实际上,继承是一种强制性很强的方式,因此也使得基类和具体子类之间的耦合
 *  性很强。例如在 Template 模式中在 ConcreteClass1 中定义的原语操作别的类是不能够直
 *  接复用(除非你继承自 AbstractClass,具体分析请参看 Template 模式文档)
 *  。而组合(委
 *  托)的方式则有很小的耦合性,实现(具体实现)和接口(抽象接口)之间的依赖性很
 *  小,例如在本实现中,ConcreteStrategyA 的具体实现操作很容易被别的类复用,例如我
 *  们要定义另一个 Context 类 AnotherContext,只要组合一个指向 Strategy 的指针就可以
 *  很容易地复用 ConcreteStrategyA 的实现了。
 *  我们在 Bridge 模式的问题和 Bridge 模式的分析中,正是说明了继承和组合之间的
 *  区别。请参看相应模式解析。
 *  另外 Strategy 模式很 State 模式也有相似之处,但是 State 模式注重的对象在不同的
 *  状态下不同的操作。两者之间的区别就是 State 模式中具体实现类中有一个指向 Context
 *  的引用,而 Strategy 模式则没有。具体分析请参看相应的 State 模式分析中。
 *
 *
 */

class Strategy
{
    public:
        Strategy();
        virtual ~Strategy();
        virtual void AlgrithmInterface() = 0;
    protected:
    private:
};

class ConcreteStrategyA:public Strategy
{
    public:
        ConcreteStrategyA();
        virtual ~ConcreteStrategyA();
        void AlgrithmInterface();
    protected:
    private:
};

class ConcreteStrategyB:public Strategy
{
    public:
        ConcreteStrategyB();
        virtual ~ConcreteStrategyB();
        void AlgrithmInterface();
    protected:
    private:
};

/**
 * 这个类是Strategy 模式的关键,也是 Strategy 模式和Template模式的根本区别所在。
 * Strategy通过“组合”(委托)方式实现算法(实现)的异构,而Template模式则采取的是继承的方式
 * 这两个模式的区别也是继承和组合两种实现接口重用的方式的区别
 */
class Context
{
    public:
        Context(Strategy* stg);
        ~Context();
        void DoAction();
    protected:
    private:
        Strategy* _stg;
};

/**
 *  Strategy Class
 */
Strategy::Strategy()
{
}
Strategy::~Strategy()
{
    cout<<"~Strategy....."<<endl;
}
void Strategy::AlgrithmInterface()
{
}

/**
 *  ConcreteStrategyA
 */
ConcreteStrategyA::ConcreteStrategyA()
{
}
ConcreteStrategyA::~ConcreteStrategyA()
{
    cout<<"~ConcreteStrategyA....."<<endl;
}
void ConcreteStrategyA::AlgrithmInterface()
{
    cout<<"test ConcreteStrategyA....."<<endl;
}

/**
 *  ConcreteStrategyB
 */
ConcreteStrategyB::ConcreteStrategyB()
{
}
ConcreteStrategyB::~ConcreteStrategyB()
{
    cout<<"~ConcreteStrategyB....."<<endl;
}
void ConcreteStrategyB::AlgrithmInterface()
{
    cout<<"test ConcreteStrategyB....."<<endl;
}

/**
 *  Context
 */
Context::Context(Strategy* stg)
{
    _stg = stg;
}
Context::~Context()
{
    if (!_stg)
        delete _stg;
}
void Context::DoAction()
{
    _stg->AlgrithmInterface();
}

/**
 *  Client
 */
int main(int argc,char* argv[])
{
    Strategy* ps = new ConcreteStrategyA();
    Context* pc = new Context(ps);
    pc->DoAction();
    if (NULL != pc)
        delete pc;
    return 0;
}
