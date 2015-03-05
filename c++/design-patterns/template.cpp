/*
 * =====================================================================================
 *
 *       Filename:  template.cpp
 *
 *    Description:  Template Mode 
 *
 *        Version:  1.0
 *        Created:  2014年09月03日 13时59分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */

/**
 * Template 模式是采用继承的方式实现这一点:将逻辑(算法)框架放在抽象基类中,并
 * 定义好细节的接口,子类中实现细节。
 *
 * Template 模式实际上就是利用面向对象中多态的概念实现算法实现细节和高层接口的
 * 松耦合。可以看到 Template 模式采取的是继承方式实现这一点的,由于继承是一种强约束
 * 性的条件,因此也给 Template 模式带来一些许多不方便的地方
 *
 * 其关键是将通用算法(逻
 * 辑)封装起来,而将算法细节让子类实现(多态)。
 *
 * Template 模式获得一种反向控制结构效果,这也是面向对象系统的分析和设计中一个原
 * 则 DIP(依赖倒置:Dependency Inversion Principles)
 * 。其含义就是父类调用子类的操作(高
 * 层模块调用低层模块的操作),低层模块实现高层模块声明的接口。这样控制权在父类(高
 * 层模块),低层模块反而要依赖高层模块。
 *
 * 继 承 的 强 制 性 约 束 关 系 也 让 Template 模 式 有 不 足 的 地 方 , 我 们 可 以 看 到 对 于
 * ConcreteClass 类中的实现的原语方法 Primitive1(),是不能被别的类复用。假设我们要创建
 * 一个 AbstractClass 的变体 AnotherAbstractClass,并且两者只是通用算法不一样,其原语操
 * 作想复用 AbstractClass 的子类的实现。但是这是不可能实现的,因为 ConcreteClass 继承自
 * AbstractClass,也就继承了 AbstractClass 的通用算法,AnotherAbstractClass 是复用不了
 * ConcreteClass 的实现,因为后者不是继承自前者。
 */

#include <iostream>
using namespace std;

class AbstractClass
{
    public:
        virtual ~AbstractClass();
        void TemplateMethod();
    protected:
        virtual void PrimitiveOperation1() = 0;
        virtual void PrimitiveOperation2() = 0;
        AbstractClass();
    private:
};

class ConcreteClass1:public AbstractClass
{
    public:
        ConcreteClass1();
        ~ConcreteClass1();
    protected:
        void PrimitiveOperation1();
        void PrimitiveOperation2();
    private:
};

class ConcreteClass2:public AbstractClass
{
    public:
        ConcreteClass2();
        ~ConcreteClass2();
    protected:
        void PrimitiveOperation1();
        void PrimitiveOperation2();
    private:
};

/**
 *  AbstractClass
 */
AbstractClass::AbstractClass()
{
}
AbstractClass::~AbstractClass()
{
}
void AbstractClass::TemplateMethod()
{
    this->PrimitiveOperation1();
    this->PrimitiveOperation2();
}

/**
 *  ConcteteClass1
 */
ConcreteClass1::ConcreteClass1()
{
}
ConcreteClass1::~ConcreteClass1()
{
}
void ConcreteClass1::PrimitiveOperation1()
{
    cout<<"ConcreteClass1...PrimitiveOperation1"<<endl;
}
void ConcreteClass1::PrimitiveOperation2()
{
    cout<<"ConcreteClass1...PrimitiveOperation2"<<endl;
}

/**
 *  ConcteteClass2
 */
ConcreteClass2::ConcreteClass2()
{
}
ConcreteClass2::~ConcreteClass2()
{
}
void ConcreteClass2::PrimitiveOperation1()
{
    cout<<"ConcreteClass2...PrimitiveOperation1"<<endl;
}
void ConcreteClass2::PrimitiveOperation2()
{
    cout<<"ConcreteClass2...PrimitiveOperation2"<<endl;
}

int main(int argc,char* argv[])
{
    AbstractClass* p1 = new ConcreteClass1();
    AbstractClass* p2 = new ConcreteClass2();
    p1->TemplateMethod();
    p2->TemplateMethod();
    return 0;
}

