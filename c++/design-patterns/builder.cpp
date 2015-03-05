/*
 * =====================================================================================
 *
 *       Filename:  builder.cpp
 *
 *    Description:  GOF builder -- 建造模式
 *
 *        Version:  1.0
 *        Created:  2014年08月28日 16时31分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yanwzh (GX), yanwzh@nationalchip.com
 *   Organization:  
 *
 * =====================================================================================
 */


/**
 *  Builder 模式的示例代码中,BuildPart 的参数是通过客户程序员传入的,这里为了简单
 *  说明问题,使用“user-defined”代替,实际的可能是在 Construct 方法中传入这 3 个参数,
 *  这样就可以得到不同的细微差别的复杂对象了
 */

/**
 *  Builder 模式的关键是其中的 Director 对象并不直接返回对象,而是通过一步步
 *  (BuildPartA,BuildPartB,BuildPartC)来一步步进行对象的创建。当然这里 Director 可以
 *  提供一个默认的返回对象的接口(即返回通用的复杂对象的创建,即不指定或者特定唯一指
 *  定 BuildPart 中的参数)
 */
using namespace std;

class Product
{
    public:
        Product();
        ~Product();
        void ProducePart();
    protected:
    private:
}

class ProductPart
{
    public:
        ProductPart();
        ~ProductPart();
        ProductPart* BuildPart();
    protected:
    private:
};

class Builder
{
    public:
        virtual ~Builder();
        virtual void BuildPartA(const string& buildPara) = 0;
        virtual void BuildPartB(const string& buildPara) = 0;
        virtual void BuildPartC(const string& buildPara) = 0;
        virtual Product* GetProduct() = 0;
    protected:
        Builder();
    private:
};

class ConcreteBuilder:public Builder
{
    public:
        ConcreteBuilder();
        ~ConcreteBuilder();
        void BuildPartA(const string& buildPara);
        void BuildPartB(const string& buildPara);
        void BuildPartC(const string& buildPara);
        Product* GetProduct();
    protected:
    private:
};

class Director
{
    public:
        Director(Builder* bld);
        ~Director();
        void Construct();
    protected:
    private:
        Builder* _bld;
};


/**
 *  Functions For Class
 */

Product::Product()
{
    ProducePart();
    cout<<"return a product"<<endl;
}

Product::~Product()
{

}

void Product::ProducePart()
{
    cout<<"build part of product.."<<endl;
}

ProductPart::ProductPart()
{
    //cout<<"build productpart.."<<endl;
}

ProductPart::~ProductPart()
{

}

ProductPart* ProductPart::BuildPart()
{
    return new ProductPart;
}

Builder::Builder()
{

}

Builder::~Builder()
{

}

ConcreteBuilder::ConcreteBuilder()
{

}

ConcreteBuilder::~ConcreteBuilder()
{

}

void ConcreteBuilder::BuildPartA(const string& buildPara)
{
    cout<<"Step1:Build PartA..."<<buildPara<<endl;
}

void ConcreteBuilder::BuildPartB(const string& buildPara)
{
    cout<<"Step1:Build PartB..."<<buildPara<<endl;
}

void ConcreteBuilder::BuildPartC(const string& buildPara)
{
    cout<<"Step1:Build PartC..."<<buildPara<<endl;
}

Product* ConcreteBuilder::GetProduct()
{
    BuildPartA("pre-defined");
    BuildPartB("pre-defined");
    BuildPartC("pre-defined");
    return new Product();
}

Director::Director(Builder* bld)
{
    _bld = bld;
}

Director::~Director()
{

}

void Director::Construct()
{
    _bld->BuildPartA("user-defined");
    _bld->BuildPartB("user-defined");
    _bld->BuildPartC("user-defined");
}

int main(int argc,char* argv[])
{
    Director* d = new Director(new ConcreteBuilder());
    d->Construct();
    return 0;
}

