/**
C++ 引用类型对比演示
*/

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

class TestClass {
   private:
    std::string data;
    static int instance_count;
    int id;

   public:
    // 构造函数
    TestClass(const std::string& str = "default")
        : data(str), id(++instance_count) {
        std::cout << "构造 TestClass[" << id << "] with: " << data << "\n";
    }

    // 拷贝构造函数
    TestClass(const TestClass& other) : data(other.data), id(++instance_count) {
        std::cout << "拷贝构造 TestClass[" << id << "] from TestClass["
                  << other.id << "]\n";
    }

    // 移动构造函数
    TestClass(TestClass&& other) noexcept
        : data(std::move(other.data)), id(++instance_count) {
        std::cout << "移动构造 TestClass[" << id << "] from TestClass["
                  << other.id << "]\n";
        other.data = "moved";  // 标记为已移动
    }

    // 析构函数
    ~TestClass() {
        std::cout << "析构 TestClass[" << id << "] with data: " << data << "\n";
    }

    // 获取数据
    const std::string& getData() const { return data; }
    int getId() const { return id; }

    // 修改数据
    void setData(const std::string& newData) { data = newData; }
};

int TestClass::instance_count = 0;

// 函数重载演示：接受不同类型的引用
void func_lvalue_ref(TestClass& obj) {
    std::cout << "调用 func_lvalue_ref(TestClass&) - 左值引用版本\n";
    obj.setData("modified by lvalue ref");
}

void func_const_ref(const TestClass& obj) {
    std::cout << "调用 func_const_ref(const TestClass&) - const引用版本\n";
    std::cout << "  只能读取数据: " << obj.getData() << "\n";
    // obj.setData("error"); // 编译错误：不能通过const引用修改对象
}

void func_rvalue_ref(TestClass&& obj) {
    std::cout << "调用 func_rvalue_ref(TestClass&&) - 右值引用版本\n";
    obj.setData("modified by rvalue ref");
    std::cout << "  可以修改即将销毁的对象: " << obj.getData() << "\n";
}

// 完美转发演示
template <typename T>
void perfect_forward(T&& arg) {
    std::cout << "\n--- 完美转发分析 ---\n";
    std::cout << "参数类型: ";

    if constexpr (std::is_lvalue_reference_v<T>) {
        std::cout << "左值引用 (T&)\n";
    } else if constexpr (std::is_rvalue_reference_v<T>) {
        std::cout << "右值引用 (T&&)\n";
    } else {
        std::cout << "值类型 (T)\n";
    }

    // 根据原始类型转发
    if constexpr (std::is_lvalue_reference_v<T>) {
        func_lvalue_ref(arg);  // 转发给左值引用版本
    } else {
        func_rvalue_ref(std::forward<T>(arg));  // 转发给右值引用版本
    }
}

// 返回不同引用类型的函数
TestClass createObject() { return TestClass("temporary"); }

TestClass& getStaticObject() {
    static TestClass static_obj("static");
    return static_obj;
}

const TestClass& getConstStaticObject() {
    static TestClass const_static_obj("const_static");
    return const_static_obj;
}

int main() {
    std::cout << "=== 引用类型对比演示 ===\n\n";

    // 1. 基本引用类型声明和绑定
    std::cout << "1. 引用类型声明和绑定\n";
    std::cout << "--- 创建测试对象 ---\n";
    TestClass obj1("original");

    std::cout << "\n--- 左值引用 (TestClass&) ---\n";
    TestClass& lref = obj1;  // 左值引用，必须绑定到左值
    std::cout << "左值引用绑定到 obj1，可以修改原对象\n";
    lref.setData("modified via lref");
    std::cout << "原对象数据: " << obj1.getData() << "\n";

    std::cout << "\n--- const引用 (const TestClass&) ---\n";
    const TestClass& cref = obj1;  // const引用，可以绑定到左值或右值
    std::cout << "const引用绑定到 obj1，只能读取\n";
    std::cout << "通过const引用读取: " << cref.getData() << "\n";
    // cref.setData("error"); // 编译错误

    // const引用可以绑定到临时对象
    const TestClass& cref_temp = TestClass("temporary");
    std::cout << "const引用可以绑定到临时对象: " << cref_temp.getData() << "\n";

    std::cout << "\n--- 右值引用 (TestClass&&) ---\n";
    TestClass&& rref = TestClass("rvalue");  // 右值引用，绑定到右值
    std::cout << "右值引用绑定到临时对象\n";
    rref.setData("modified via rvalue ref");
    std::cout << "通过右值引用修改: " << rref.getData() << "\n";

    // 右值引用也可以绑定到通过std::move转换的左值
    TestClass obj2("for_move");
    TestClass&& rref2 = std::move(obj2);
    std::cout << "右值引用绑定到std::move(obj2): " << rref2.getData() << "\n";

    // 2. 函数参数中的引用类型
    std::cout << "\n2. 函数参数中的引用类型\n";

    TestClass obj3("function_test");

    std::cout << "\n--- 传递左值给不同函数 ---\n";
    func_lvalue_ref(obj3);  // 左值 -> 左值引用
    func_const_ref(obj3);   // 左值 -> const引用
    // func_rvalue_ref(obj3); // 编译错误：不能将左值绑定到右值引用
    func_rvalue_ref(std::move(obj3));  // 通过std::move转换为右值引用

    std::cout << "\n--- 传递右值给不同函数 ---\n";
    // func_lvalue_ref(TestClass("temp")); // 编译错误：不能将右值绑定到左值引用
    func_const_ref(TestClass("temp"));   // 右值 -> const引用（可以）
    func_rvalue_ref(TestClass("temp"));  // 右值 -> 右值引用

    // 3. 引用折叠和万能引用
    std::cout << "\n3. 万能引用 (Universal Reference) 演示\n";

    TestClass obj4("universal_test");

    std::cout << "传递左值：";
    perfect_forward(obj4);  // T推导为TestClass&，T&&变为TestClass&

    std::cout << "\n传递右值：";
    perfect_forward(
        TestClass("universal_rvalue"));  // T推导为TestClass，T&&变为TestClass&&

    // 4. 生命周期对比
    std::cout << "\n4. 生命周期对比\n";

    std::cout << "\n--- const引用延长临时对象生命周期 ---\n";
    {
        const TestClass& temp_ref = createObject();
        std::cout << "临时对象通过const引用延长生命周期: " << temp_ref.getData()
                  << "\n";
        // 临时对象在这里仍然有效
    }  // 临时对象在此处析构

    std::cout << "\n--- 右值引用延长临时对象生命周期 ---\n";
    {
        TestClass&& temp_rref = createObject();
        std::cout << "临时对象通过右值引用延长生命周期: " << temp_rref.getData()
                  << "\n";
        temp_rref.setData("modified temp");
        std::cout << "可以修改临时对象: " << temp_rref.getData() << "\n";
    }  // 临时对象在此处析构

    // 5. 引用类型的特性总结
    std::cout << "\n5. 引用类型特性对比\n";
    std::cout << "=================================\n";
    std::cout
        << "| 引用类型       | 绑定左值 | 绑定右值 | 可修改 | 延长生命周期 |\n";
    std::cout
        << "|---------------|---------|---------|-------|-------------|\n";
    std::cout
        << "| T&            | ✓       | ✗       | ✓     | ✗           |\n";
    std::cout
        << "| const T&      | ✓       | ✓       | ✗     | ✓           |\n";
    std::cout
        << "| T&&           | ✗*      | ✓       | ✓     | ✓           |\n";
    std::cout
        << "|               |         |         |       |             |\n";
    std::cout << "| *T&&可以通过std::move绑定到左值                        |\n";
    std::cout << "=================================\n";

    // 6. 重载决议演示
    std::cout << "\n6. 重载决议演示\n";

    // 定义重载函数
    auto overload_test = [](TestClass& t) {
        std::cout << "选择了 TestClass& 版本\n";
    };
    auto overload_test2 = [](const TestClass& t) {
        std::cout << "选择了 const TestClass& 版本\n";
    };
    auto overload_test3 = [](TestClass&& t) {
        std::cout << "选择了 TestClass&& 版本\n";
    };

    TestClass obj5("overload_test");
    const TestClass obj6("const_obj");

    std::cout << "非const左值：";
    func_const_ref(obj5);  // 可以绑定到const引用

    std::cout << "const左值：";
    func_const_ref(obj6);  // 绑定到const引用

    std::cout << "右值：";
    func_const_ref(TestClass("temp"));   // 右值绑定到const引用
    func_rvalue_ref(TestClass("temp"));  // 右值绑定到右值引用

    std::cout << "\n=== 演示结束 ===\n";
    return 0;
}