/*
 * =====================================================================================
 *
 *       Filename:  prototype.cpp
 *
 *    Description:  prototype.cpp
 *
 *        Version:  1.0
 *        Created:  2014年08月28日 17时06分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

/**
 *  Prototype 模式的结构和实现都很简单,其关键就是(C++中)拷贝构造函数的实现方
 *  式,这也是 C++实现技术层面上的事情。由于在示例代码中不涉及到深层拷贝(主要指有指
 *  针、复合对象的情况),因此我们通过编译器提供的默认的拷贝构造函数(按位拷贝)的方
 *  式进行实现。说明的是这一切只是为了实现简单起见,也因为本文档的重点不在拷贝构造函
 *  数的实现技术,而在 Prototype 模式本身的思想.
 */


/**
 *  Prototype 模式通过复制原型(Prototype)而获得新对象创建的功能,这里 Prototype 本
 *  身就是“对象工厂”(因为能够生产对象),实际上 Prototype 模式和 Builder 模式、
 *  AbstractFactory 模式都是通过一个类(对象实例)来专门负责对象的创建工作(工厂对象),
 *  它们之间的区别是:Builder 模式重在复杂对象的一步步创建(并不直接返回对象),
 *  AbstractFactory 模式重在产生多个相互依赖类的对象,而 Prototype 模式重在从自身复制自
 *  己创建新类。
 */
class Prototype
{
    public:
        virtual ~Prototype();
        virtual Prototype* Clone() const = 0;
    protected:
        Prototype();
    private:
};

class ConcretePrototype:public Prototype
{
    public:
        ConcretePrototype();
        ConcretePrototype(const ConcretePrototype& cp);
        ~ConcretePrototype();
        Prototype* Clone() const;
    protected:
    private:
};

Prototype::Prototype()
{
}

Prototype::~Prototype()
{
}

Prototype* Prototype::Clone() const
{
    return 0;
}

ConcretePrototype::ConcretePrototype()
{
}

ConcretePrototype::~ConcretePrototype()
{
}

ConcretePrototype::ConcretePrototype(const ConcretePrototype& cp)
{
    cout<<"ConcretePrototype copy ..."<<endl;
}

Prototype* ConcretePrototype::Clone() const
{
    return new ConcretePrototype(*this);
}

int main(int argc,char* argv[])
{
    Prototype* p = new ConcretePrototype();
    Prototype* p1 = p->Clone();
    return 0;
}

