#include <iostream>
#include <memory>

struct Node {
    std::string name;
    std::shared_ptr<Node> child;
    std::weak_ptr<Node> parent;  // 用 weak_ptr 避免循环引用

    ~Node() { std::cout << name << " destroyed" << std::endl; }
};

/**
(.venv) royenheart@RoyenHeartTB:~/gits/OSExp/build/cpptests$ ./smartptr
Parent ref count before lock: 1
Child ref count before lock: 2
Parent ref count during lock:2
Child's parent: Parent
Parent ref count after lock: 1
Parent destroyed
Parent reset, child ref count: 1
Parent is gone
Child destroyed
*/
int main() {
    // 创建父子节点
    auto parent = std::make_shared<Node>();
    parent->name = "Parent";
    auto child = std::make_shared<Node>();
    child->name = "Child";

    // 建立关系
    parent->child = child;   // parent 持有 child 的 shared_ptr
    child->parent = parent;  // child 通过 weak_ptr 引用 parent

    std::cout << "Parent ref count before lock: " << parent.use_count()
              << std::endl;
    std::cout << "Child ref count before lock: " << child.use_count()
              << std::endl;

    // 检查 parent 是否存在
    // weak_ptr 不增加引用计数，仅仅作为观察，不实际持有资源。需要调用
    // lock 观察是否存在再返回实际资源，引用计数 +1
    if (auto parent_ptr = child->parent.lock()) {  // 提升为 shared_ptr
        std::cout << "Parent ref count during lock:" << parent.use_count()
                  << std::endl;
        std::cout << "Child's parent: " << parent_ptr->name << std::endl;
    } else {
        std::cout << "Parent already released" << std::endl;
    }

    std::cout << "Parent ref count after lock: " << parent.use_count()
              << std::endl;
    // 手动释放 parent（仅示例，实际会在作用域结束时自动释放）
    parent.reset();
    std::cout << "Parent reset, child ref count: " << child.use_count()
              << std::endl;

    // 再次检查 parent
    if (auto parent_ptr = child->parent.lock()) {  // 返回空
        std::cout << "Parent still exists" << std::endl;
    } else {
        std::cout << "Parent is gone" << std::endl;
    }

    return 0;
}