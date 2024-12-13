#include <iostream>
#include "shared_ptr.h"
#include "weak_ptr.h"
#include "unique_ptr.h"



struct Test {
    int value;
    Test(int v) : value(v) { std::cout << "Test(" << value << ") создан.\n"; }
    ~Test() { std::cout << "Test(" << value << ") уничтожен.\n"; }
};


#define ASSERT_TRUE(condition, message) \
    if (!(condition)) { \
        std::cerr << "Assertion failed: " << message << "\n"; \
        return; \
    }

#define ASSERT_FALSE(condition, message) \
    if (condition) { \
        std::cerr << "Assertion failed: " << message << "\n"; \
        return; \
    }

#define ASSERT_EQ(val1, val2, message) \
    if ((val1) != (val2)) { \
        std::cerr << "Assertion failed: " << message << " (" << val1 << " != " << val2 << ")\n"; \
        return; \
    }


void test_unique_ptr() {
    setlocale(0, "ru");
    std::cout << "/// Тест unique_ptr |||\n";

    unique_ptr<Test> up1(new Test(1));
    ASSERT_TRUE(up1, "up1 должно указывать на объект");
    ASSERT_EQ(up1->value, 1, "Значение объекта должно быть 1");

    unique_ptr<Test> up2 = std::move(up1);
    ASSERT_FALSE(up1, "up1 должно быть пустым после перемещения");
    ASSERT_TRUE(up2, "up2 должно указывать на объект после перемещения");
    ASSERT_EQ(up2->value, 1, "Значение объекта должно быть 1 в up2");

    up2.reset();
    ASSERT_FALSE(up2, "up2 должно быть пустым после сброса");

    unique_ptr<Test> up3(new Test(3));
    Test* rawPtr = up3.release();
    ASSERT_FALSE(up3, "up3 должно быть пустым после release");
    ASSERT_EQ(rawPtr->value, 3, "Значение объекта после release должно быть 3");
    delete rawPtr;

    std::cout << "/// Тест unique_ptr завершен |||\n\n";
}


void test_shared_ptr() {
    setlocale(0, "ru");
    std::cout << "/// Тест shared_ptr |||\n";

    shared_ptr<Test> sp1(new Test(10));
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count должно быть 1");


    {
        shared_ptr<Test> sp2 = sp1;
        ASSERT_EQ(sp1.use_count(), 2, "sp1 use_count должно быть 2 после копирования sp2");
        ASSERT_EQ(sp2.use_count(), 2, "sp2 use_count должно быть 2 после копирования");
        ASSERT_EQ(sp2->value, 10, "Значение объекта должно быть 10 в sp2");
    }

    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count должно быть 1 после уничтожения sp2");

    sp1.reset();
    ASSERT_EQ(sp1.use_count(), 0, "sp1 use_count должно быть 0 после сброса");

    std::cout << "/// Тест shared_ptr завершен |||\n\n";
}

void test_weak_ptr() {
    std::cout << "/// Тест weak_ptr ===\n";
    setlocale(0, "ru");
    shared_ptr<Test> sp1(new Test(20));
    weak_ptr<Test> wp1 = sp1;
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count должно быть 1");
    ASSERT_EQ(wp1.use_count(), 1, "wp1 use_count должно быть 1");

    {
        shared_ptr<Test> sp2 = wp1.lock();
        ASSERT_TRUE(sp2, "sp2 должно указывать на объект");
        ASSERT_EQ(sp2->value, 20, "Значение объекта должно быть 20 в sp2");
        ASSERT_EQ(sp1.use_count(), 2, "sp1 use_count должно быть 2 во время существования sp2");
        ASSERT_EQ(sp2.use_count(), 2, "sp2 use_count должно быть 2");
    }

    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count должно быть 1 после уничтожения sp2");

    sp1.reset();
    ASSERT_EQ(sp1.use_count(), 0, "sp1 use_count должно быть 0 после сброса");
    ASSERT_TRUE(wp1.expired(), "wp1 должно указывать на истекший объект");


    shared_ptr<Test> sp3 = wp1.lock();
    ASSERT_FALSE(sp3, "sp3 должно быть пустым, так как объект уничтожен");

    std::cout << "/// Тест weak_ptr завершен |||\n\n";
}


void test_shared_ptr_swap() {
    std::cout << "/// Тест shared_ptr::swap |||\n";
    setlocale(0, "ru");
    shared_ptr<Test> sp1(new Test(30));
    shared_ptr<Test> sp2(new Test(40));
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count должно быть 1");
    ASSERT_EQ(sp2.use_count(), 1, "sp2 use_count должно быть 1");

    std::swap(sp1, sp2);
    ASSERT_EQ(sp1->value, 40, "После swap sp1 должно указывать на Test с value 40");
    ASSERT_EQ(sp2->value, 30, "После swap sp2 должно указывать на Test с value 30");

    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count должно быть 1 после swap");
    ASSERT_EQ(sp2.use_count(), 1, "sp2 use_count должно быть 1 после swap");

    std::cout << "/// Тест shared_ptr::swap завершен |||\n\n";
}

void test_shared_ptr_self_assignment() {
    setlocale(0, "ru");
    std::cout << "/// Тест shared_ptr самоприсваивания |||\n";

    shared_ptr<Test> sp1(new Test(50));
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count должно быть 1");

    sp1 = sp1;
    ASSERT_EQ(sp1.use_count(), 1, "После самоприсваивания sp1 use_count должно быть 1");
    ASSERT_EQ(sp1->value, 50, "sp1 должно продолжать указывать на Test с value 50");

    std::cout << "/// Тест shared_ptr самоприсваивания завершен |||\n\n";
}



void test_cyclic_shared_ptr() {
    setlocale(0, "ru");
    std::cout << "/// Тест циклических ссылок |||\n";

    struct Node {
        int value;
        shared_ptr<Node> next;
        weak_ptr<Node> prev;
        Node(int v) : value(v) { std::cout << "Node(" << value << ") создан.\n"; }

    };

    shared_ptr<Node> node1(new Node(1));
    shared_ptr<Node> node2(new Node(2));

    node1->next = node2;
    node2->prev = node1;

    ASSERT_EQ(node1.use_count(), 1, "node1 use_count должно быть 1");
    ASSERT_EQ(node2.use_count(), 2, "node2 use_count должно быть 2");

    std::cout << "/// Тест циклических ссылок завершен |||\n\n";
}

//
//int main() {
//    test_unique_ptr();
//    test_shared_ptr();
//    test_shared_ptr_swap();
//    test_shared_ptr_self_assignment();
//    test_weak_ptr();
//    test_cyclic_shared_ptr();
//
//    return 0;
//}