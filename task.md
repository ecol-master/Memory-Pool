
Задача - реализовать шаблонный класс для работы с памятью - memory pool и применить его в задании с океаном.

При работе с динамически выделенными объектами есть несколько проблем, которые могут стать существенными, когда важна производительность:
1. Каждый вызов стандартных new/delete это обращение к операционной системе (долго)
    Если объекты выделяются часто и много, стоимость системного вызова становится чувствительной
2. Объекты, выделяемные через new/delete могут находиться в памяти где угодно, что тоже может влиять на производительность.
    Процессор и подсистема памяти работает куда быстрее, когда данные лежат рядом и последовательно

Часто в программе может быть заранее известно максимальное количество выделяемых объектов, тогда обе проблемы могут быть решены например мемори пулом.
Суть проста - заранее выделить объем памяти достаточный для того, чтобы вместить заданное количество объектов одинакового размера.

Весь функционал такого решения сводится к:
    get - получить новый объект из пула
    put - вернуть объект в пул (он больше нам не принадлежит, пул имеет право переиспользовать эту память)
Ну а что бы работать с любыми объектами будем использовать шаблон.

```
template<typename ElemT>
class MemPool {

    // Default constructor, does not allocate any memory
    ObjectPool();

    // Allocates memory for given number of objects
    ObjectPool(size_t _capacity);

    // Allocates memory for given number of objects and initializes them with `_default_value`
    ObjectPool(size_t _capacity, const ElemT& _default_value);

    // Re-allocates memory to fit new capacity
    void resize(size_t new_cap);

    // Creates object inside memory pool and provides pointer to it
    // can return nullptr in case if memory cannot be allocated.
    template<class... Args>
    ElemT* get(Args&&... args);

    // Returns object to pool, calls destructor.
    // Returns bool to indicate error (maybe object is not from this pool?)
    bool put(ElemT* elem);

    // Creates object in memory pool and provides unique pointer to it
    template<class... Args>
    std::unique_ptr<ElemT, Deleter> unique(Args&&... args);

    // Creates object in memory pool and provides shared pointer to it
    template<class... Args>
    std::shared_ptr<ElemT> shared(Args&&... args);
};
```


Теперь возникает другая проблема.
Теперь придется во всем коде заменять new/delete на get/put.
Или постоянно в каждом классе перегружать операторы new/delete, что бы они использовали memory pool.
Что долго, некрасиво и ведет к ошибкам.
Хотелось бы не писать один и тот же код, но и при этом не трогать тот, что уже написан.

Но и тут на помощь приходят шаблоны.
В C++ существуют некоторые паттерны проектирования специфичные только для этого языка (ну или почти только для него).
Один из таких - CRTP - Curiously recurring template pattern. Переводится буквально как "Странный рекурсивный шаблонный паттерн".

Сам паттерн выглядит так:
```
template<Derived>
class Base {}

class MyClass : Base<MyClass> {}
```

Суть - мы наследуемся от какого-то базового *шаблонного* класса, и в качестве шаблонного параметра передаем себя же.
Таким образом базовый класс будет в курсе, кто от него унаследовался.

Зачем это надо?
Что бы "подмешивать" новый функционал в класс наследника, при этом никак его не меняя (за исключением добавления наследования).
Такой прием называется миксин (от англ. Mix-in)

На вики можно найти такой пример, он довольно простой, пусть и не очень показательный:
```
template <typename T>
struct counter
{
    static inline int objects_created = 0;
    static inline int objects_alive = 0;

    counter()
    {
        ++objects_created;
        ++objects_alive;
    }
    
    counter(const counter&)
    {
        ++objects_created;
        ++objects_alive;
    }
protected:
    ~counter() // objects should never be removed through pointers of this type
    {
        --objects_alive;
    }
};

class X : counter<X>
{
    // ...
};

class Y : counter<Y>
{
    // ...
};
```

Тут есть два класса X и Y, и помощью CRTP мы добавляем им функционал подсчета количества текущих живых объектов.
Не очень полезно на практике, зато можно увидеть как быстро у вас утекает память :)


По идее, нет никакой проблемы в том, что бы долго и монотонно в каждом классе руками переопределять new и delete:
```
class A {
    static void* operator new(size_t count) {
        auto obj = pool.get();
        ...
    }

    static void operator delete(void* ptr) {
        pool.put(ptr)
    }
protected:
    static MemPool<A> pool;

    ...
};

class B {
    static void* operator new(size_t count) {
        ...
    }

    static void operator delete(void* ptr) {
        ...
    }
protected:
    static MemPool<B> pool;

    ...
};
```
И так на каждый класс в вашей программе...


Для задания нам необходимо придумать миксин, позволяющий использовать new/delete как обычно, но на самом деле они будут вызывать get/put.
Выглядеть должно как-то так:
```
struct A : PoolMixin<A>
{
    // Затащим new/delete в пространство имен класса `A`
    using PoolMixin<A>::operator new;
    using PoolMixin<A>::operator delete;
}
```
Ну и заиспользовать этот миксин с объектами океана
