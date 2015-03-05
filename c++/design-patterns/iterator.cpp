/*
 * =====================================================================================
 *
 *       Filename:  iterator.cpp
 *
 *    Description:  Iterator Pattern
 *
 *        Version:  1.0
 *        Created:  2014年09月05日 15时41分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */

/**
 *  Interator Pattern
 *  Iterator 模式应该是最为熟悉的模式了,最简单的证明就是我在实现 Composite 模式、
 *  Flyweight 模式、Observer 模式中就直接用到了 STL 提供的 Iterator 来遍历 Vector 或者 List
 *  数据结构。
 *  Iterator 模式也正是用来解决对一个聚合对象的遍历问题,将对聚合的遍历封装到一个
 *  类中进行,这样就避免了暴露这个聚合对象的内部表示的可能。
 */

#include <iostream>
using namespace std;

class Iterator;
typedef int Object;

class Aggregate
{
    public:
        virtual ~Aggregate();
        virtual Iterator* CreateIterator() = 0;
        virtual Object GetItem(int idx) = 0;
        virtual int GetSize() = 0;
    protected:
        Aggregate();
    private:
};

class ConcreteAggregate:public Aggregate
{
    public:
        enum {SIZE = 3};
        ConcreteAggregate();
        ~ConcreteAggregate();
        Iterator* CreateIterator();
        Object GetItem(int idx);
        int GetSize();
    protected:
    private:
        Object _objs[SIZE];
};

class Iterator
{
    public:
        virtual ~Iterator();
        virtual void First() = 0;
        virtual void Next() = 0;
        virtual bool IsDone() = 0;
        virtual Object CurrentItem() = 0;
    protected:
        Iterator();
    private:
};

class ConcreteIterator:public Iterator
{
    public:
        ConcreteIterator(Aggregate* ag , int idx = 0);
        ~ConcreteIterator();
        void First();
        void Next();
        bool IsDone();
        Object CurrentItem();
    protected:
    private:
        Aggregate* _ag;
        int _idx;
};

/**
 *  Aggregate
 */
Aggregate::Aggregate()
{
}
Aggregate::~Aggregate()
{
}

/**
 *  ConcreteAggregate
 */
ConcreteAggregate::ConcreteAggregate()
{
    for (int i = 0; i < SIZE; i++)
        _objs[i] = i;
}
ConcreteAggregate::~ConcreteAggregate()
{
}
Iterator* ConcreteAggregate::CreateIterator()
{
    return new ConcreteIterator(this);
}
Object ConcreteAggregate::GetItem(int idx)
{
    if (idx < this->GetSize())
        return _objs[idx];
    else
        return -1;
}
int ConcreteAggregate::GetSize()
{
    return SIZE;
}

/**
 *  Iterator
 */
Iterator::Iterator()
{
}
Iterator::~Iterator()
{
}

/**
 *  ConcreteIterator
 */
ConcreteIterator::ConcreteIterator(Aggregate* ag , int idx)
{
    this->_ag = ag;
    this->_idx = idx;
}
ConcreteIterator::~ConcreteIterator()
{
}
Object ConcreteIterator::CurrentItem()
{
    return _ag->GetItem(_idx);
}
void ConcreteIterator::First()
{
    _idx = 0;
}
void ConcreteIterator::Next()
{
    if (_idx < _ag->GetSize())
        _idx++;
}
bool ConcreteIterator::IsDone()
{
    return (_idx == _ag->GetSize());
}

/**
 *  Client
 */
int main(int argc,char* argv[])
{
    Aggregate* ag = new ConcreteAggregate();
    Iterator* it = new ConcreteIterator(ag);
    for (; !(it->IsDone()) ; it->Next())
    {
        cout<<it->CurrentItem()<<endl;
    }
    return 0;
}
