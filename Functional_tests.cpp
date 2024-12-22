#include <iostream>
#include "shared_ptr.h"
#include "weak_ptr.h"
#include "unique_ptr.h"



struct Test {
    int value;
    Test(int v) : value(v) { std::cout << "Test(" << value << ") ������.\n"; }
    ~Test() { std::cout << "Test(" << value << ") ���������.\n"; }
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
    std::cout << "/// ���� unique_ptr |||\n";

    unique_ptr<Test> up1(new Test(1));
    ASSERT_TRUE(up1, "up1 ������ ��������� �� ������");
    ASSERT_EQ(up1->value, 1, "�������� ������� ������ ���� 1");

    unique_ptr<Test> up2 = std::move(up1);
    ASSERT_FALSE(up1, "up1 ������ ���� ������ ����� �����������");
    ASSERT_TRUE(up2, "up2 ������ ��������� �� ������ ����� �����������");
    ASSERT_EQ(up2->value, 1, "�������� ������� ������ ���� 1 � up2");

    up2.reset();
    ASSERT_FALSE(up2, "up2 ������ ���� ������ ����� ������");

    unique_ptr<Test> up3(new Test(3));
    Test* rawPtr = up3.release();
    ASSERT_FALSE(up3, "up3 ������ ���� ������ ����� release");
    ASSERT_EQ(rawPtr->value, 3, "�������� ������� ����� release ������ ���� 3");
    delete rawPtr;

    std::cout << "/// ���� unique_ptr �������� |||\n\n";
}


void test_shared_ptr() {
    setlocale(0, "ru");
    std::cout << "/// ���� shared_ptr |||\n";

    shared_ptr<Test> sp1(new Test(10));
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count ������ ���� 1");


    {
        shared_ptr<Test> sp2 = sp1;
        ASSERT_EQ(sp1.use_count(), 2, "sp1 use_count ������ ���� 2 ����� ����������� sp2");
        ASSERT_EQ(sp2.use_count(), 2, "sp2 use_count ������ ���� 2 ����� �����������");
        ASSERT_EQ(sp2->value, 10, "�������� ������� ������ ���� 10 � sp2");
    }

    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count ������ ���� 1 ����� ����������� sp2");

    sp1.reset();
    ASSERT_EQ(sp1.use_count(), 0, "sp1 use_count ������ ���� 0 ����� ������");

    std::cout << "/// ���� shared_ptr �������� |||\n\n";
}

void test_weak_ptr() {
    std::cout << "/// ���� weak_ptr ===\n";
    setlocale(0, "ru");
    shared_ptr<Test> sp1(new Test(20));
    weak_ptr<Test> wp1 = sp1;
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count ������ ���� 1");
    ASSERT_EQ(wp1.use_count(), 1, "wp1 use_count ������ ���� 1");

    {
        shared_ptr<Test> sp2 = wp1.lock();
        ASSERT_TRUE(sp2, "sp2 ������ ��������� �� ������");
        ASSERT_EQ(sp2->value, 20, "�������� ������� ������ ���� 20 � sp2");
        ASSERT_EQ(sp1.use_count(), 2, "sp1 use_count ������ ���� 2 �� ����� ������������� sp2");
        ASSERT_EQ(sp2.use_count(), 2, "sp2 use_count ������ ���� 2");
    }

    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count ������ ���� 1 ����� ����������� sp2");

    sp1.reset();
    ASSERT_EQ(sp1.use_count(), 0, "sp1 use_count ������ ���� 0 ����� ������");
    ASSERT_TRUE(wp1.expired(), "wp1 ������ ��������� �� �������� ������");


    shared_ptr<Test> sp3 = wp1.lock();
    ASSERT_FALSE(sp3, "sp3 ������ ���� ������, ��� ��� ������ ���������");

    std::cout << "/// ���� weak_ptr �������� |||\n\n";
}


void test_shared_ptr_swap() {
    std::cout << "/// ���� shared_ptr::swap |||\n";
    setlocale(0, "ru");
    shared_ptr<Test> sp1(new Test(30));
    shared_ptr<Test> sp2(new Test(40));
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count ������ ���� 1");
    ASSERT_EQ(sp2.use_count(), 1, "sp2 use_count ������ ���� 1");

    std::swap(sp1, sp2);
    ASSERT_EQ(sp1->value, 40, "����� swap sp1 ������ ��������� �� Test � value 40");
    ASSERT_EQ(sp2->value, 30, "����� swap sp2 ������ ��������� �� Test � value 30");

    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count ������ ���� 1 ����� swap");
    ASSERT_EQ(sp2.use_count(), 1, "sp2 use_count ������ ���� 1 ����� swap");

    std::cout << "/// ���� shared_ptr::swap �������� |||\n\n";
}

void test_shared_ptr_self_assignment() {
    setlocale(0, "ru");
    std::cout << "/// ���� shared_ptr ���������������� |||\n";

    shared_ptr<Test> sp1(new Test(50));
    ASSERT_EQ(sp1.use_count(), 1, "sp1 use_count ������ ���� 1");

    sp1 = sp1;
    ASSERT_EQ(sp1.use_count(), 1, "����� ���������������� sp1 use_count ������ ���� 1");
    ASSERT_EQ(sp1->value, 50, "sp1 ������ ���������� ��������� �� Test � value 50");

    std::cout << "/// ���� shared_ptr ���������������� �������� |||\n\n";
}



void test_cyclic_shared_ptr() {
    setlocale(0, "ru");
    std::cout << "/// ���� ����������� ������ |||\n";

    struct Node {
        int value;
        shared_ptr<Node> next;
        weak_ptr<Node> prev;
        Node(int v) : value(v) { std::cout << "Node(" << value << ") ������.\n"; }

    };

    shared_ptr<Node> node1(new Node(1));
    shared_ptr<Node> node2(new Node(2));

    node1->next = node2;
    node2->prev = node1;

    ASSERT_EQ(node1.use_count(), 1, "node1 use_count ������ ���� 1");
    ASSERT_EQ(node2.use_count(), 2, "node2 use_count ������ ���� 2");

    std::cout << "/// ���� ����������� ������ �������� |||\n\n";
}


// int main() {
//     test_unique_ptr();
//     test_shared_ptr();
//     test_shared_ptr_swap();
//     test_shared_ptr_self_assignment();
//     test_weak_ptr();
//     test_cyclic_shared_ptr();

//     return 0;
// }
