#ifndef ABSTRACTFACTORY_H
#define ABSTRACTFACTORY_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

class CustomWidget;
class CustomMenu;

class AbstractFactory
{
protected:
    AbstractFactory();

public:
    virtual ~AbstractFactory();

    virtual CustomWidget *CreateWidget(const int &, DWidget *parent) = 0;
    virtual CustomMenu *CreateMenu(const int &, DWidget *parent)  = 0;
};

class ConcreteFactory: public AbstractFactory
{
public:
    static ConcreteFactory *Instance()
    {
        static ConcreteFactory factroy;
        return &factroy;
    }

    ~ConcreteFactory() override;

private:
    ConcreteFactory();

    // AbstractFactory interface
public:
    CustomWidget *CreateWidget(const int &, DWidget *parent = nullptr) override;
    CustomMenu *CreateMenu(const int &, DWidget *parent = nullptr) override;
};

#endif // ABSTRACTFACTORY_H
