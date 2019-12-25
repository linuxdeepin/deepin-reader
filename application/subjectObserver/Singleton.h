#ifndef SINGLETON_H
#define SINGLETON_H

#include <QReadWriteLock>
#include <QMutex>
#include <QDebug>

template<class T>
class CSingleton
{
private:
    CSingleton();//防止构造函数
    CSingleton(const CSingleton<T> &);//防止拷贝构造函数
    CSingleton<T> &operator=(const CSingleton<T> &); //防止赋值拷贝构造函数

    QReadWriteLock m_internalMutex;//读写锁
    static QMutex m_mutex;//互斥锁
    static QAtomicPointer<T> m_instance;//实例

public:
    static T *getInstance(void); //获取唯一实例
};

template<class T>
QMutex CSingleton<T>::m_mutex(QMutex::Recursive);//一个线程可以多次锁同一个互斥量

template<class T>
QAtomicPointer<T>CSingleton<T>::m_instance;//原子指针，默认初始化是0

template<typename T>
T *CSingleton<T>::getInstance(void)
{
#ifndef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
    if (!QAtomicPointer::isTestAndSetNative()) //运行时检测
        qDebug() << "Error: TestAndSetNative not supported!";
#endif

    QMutexLocker locker(&m_mutex);//互斥锁
    if (m_instance.testAndSetOrdered(0, 0)) { //第一次检测
        m_instance.testAndSetOrdered(0, new T);//第二次检测
    }
    return m_instance.load();
}

#endif // SINGLETON_H
