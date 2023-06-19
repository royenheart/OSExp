#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <ctime>

#define SPLIT '/'

struct Inode {
    std::string name;
    std::string type;
    std::string permission;
    int size;
    bool ondelete;
    time_t mtime;
    std::set<int> pointer;
};

/// @brief 文件系统
class FileSystem {
private:
    std::string cur_dir;
    std::vector<Inode> inode_table;

    int getInodeIndex(const std::string& name, const std::string& type) {
        for (int i = 0; i < inode_table.size(); i++) {
            if (inode_table[i].name == name && inode_table[i].type == type) {
                return i;
            }
        }
        return -1;
    }

    int getInodeTableNextIndex() {
        int i = 0;
        for (; i < inode_table.size(); i++) {
            if (inode_table[i].ondelete == true) {
                return i;
            }
        }
        return i;
    }

    void push_back_inode_table(const Inode& n) {
        int i = getInodeTableNextIndex();
        if (i >= inode_table.size()) {
            inode_table.push_back(n);
        } else {
            inode_table[i] = n;
        }
    }

    int getCurDirIndex() {
        return getInodeIndex(cur_dir, "dir");
    }

    Inode& getCurDirInode() {
        int ii = getCurDirIndex();
        if (ii == -1) {
            std::cout << "当前所处目录引用错误，请检查程序" << std::endl;
        }
        if (inode_table[ii].ondelete == true) {
            std::cout << "当前所在目录已被删除，对其操作将无效" << std::endl;
        }
        return inode_table[ii];
    }

public:
    FileSystem() {
        cur_dir = "/";
        Inode root;
        root.name = "/";
        root.type = "dir";
        root.permission = "rwxrwxrwx";
        root.size = 100;
        root.ondelete = false;
        root.mtime = std::time(nullptr);
        root.pointer = std::set<int>();
        push_back_inode_table(root);
    }

    const std::string& currentFolder() {
        return cur_dir;
    }

    void touch(const std::string& name) {
        time_t mtime = std::time(nullptr);
        // 创建文件 inode
        Inode file;
        file.name = name;
        file.type = "f";
        file.permission = "rwxrwxrwx";
        file.size = 0;
        file.ondelete = false;
        file.mtime = mtime;
        // 获取当前目录 inode
        Inode& dir_inode = getCurDirInode();
        // 查看当前目录是否已经存在该文件
        for (int inode_index : dir_inode.pointer) {
            Inode& target_inode = inode_table[inode_index];
            if (target_inode.name == name && target_inode.type == "f") {
                target_inode.mtime = mtime;
                std::cout << "文件存在, 更新其创建日期为： " << std::asctime(std::localtime(&mtime));
                return;
            }
        }
        dir_inode.pointer.insert(getInodeTableNextIndex());
        push_back_inode_table(file);
        std::cout << "创建文件 " << name << std::endl;
    }

    void rm(const std::string& name) {
        for (int i = 0; i < inode_table.size(); i++) {
            Inode& file = inode_table[i];
            if (file.name == name && file.type == "f") {
                Inode& dir_inode = getCurDirInode();
                // 取消目录的指向
                dir_inode.pointer.erase(getInodeIndex(name, "f"));
                // inode 表中标识为删除
                file.ondelete = true;
                std::cout << "删除文件 " << name << std::endl;
                return;
            }
        }
        std::cout << "文件 " << name << " 不存在" << std::endl;
    }

    /// @brief 打印当前目录的文件，默认为当前目录
    /// @param target 
    void ls(const std::string& target = "") {
        std::string target_dir = (target.empty()) ? cur_dir : target;
        int ii = getInodeIndex(target_dir, "dir");
        if (ii == -1) {
            std::cout << "未找到目标目录 inode 信息：" << target_dir << std::endl;
            return;
        } else if (inode_table[ii].type != "dir") {
            std::cout << "目标非目录" << target_dir << std::endl;
            return;
        } else {
            Inode& dir_inode = inode_table[ii];
            std::cout << "打印目录内容： " << dir_inode.name << std::endl;
            std::cout << "名称\t类型\t权限\t大小\t修改时间" << std::endl;
            for (int inode_index : dir_inode.pointer) {
                Inode& now = inode_table[inode_index];
                if (now.ondelete == false) {
                    std::cout << now.name << "\t" << now.type << "\t" << now.permission 
                    << "\t" << now.size << "\t" << std::asctime(std::localtime(&now.mtime));
                }
            }
        }
    }

    void mkdir(const std::string& target) {
        Inode new_dir;
        new_dir.name = target;
        new_dir.type = "dir";
        new_dir.permission = "rwxrwxrwx";
        new_dir.size = 100;
        new_dir.mtime = std::time(nullptr);
        new_dir.pointer = std::set<int>();
        new_dir.ondelete = false;
        Inode& dir_inode = getCurDirInode();
        // 查看当前目录是否已经存在该目录
        for (int inode_index : dir_inode.pointer) {
            Inode& target_inode = inode_table[inode_index];
            if (target_inode.name == target && target_inode.type == "dir") {
                std::cout << "目录已存在，无法创建：" << target << std::endl;
                return;
            }
        }
        dir_inode.pointer.insert(getInodeTableNextIndex());
        push_back_inode_table(new_dir);
    }

    void rmdir(const std::string& target) {
        int target_index = getInodeIndex(target, "dir");
        if (target_index != -1) {
            Inode& target_inode = inode_table[target_index];
            if (!target_inode.pointer.empty()) {
                std::cout << "目录非空" << std::endl;
                return;
            }
            Inode& dir_inode = getCurDirInode();
            dir_inode.pointer.erase(target_index);
            target_inode.ondelete = true;
            std::cout << "删除目录 " << target << std::endl;
        }
    }

    void cd(const std::string& target = "/") {
        if (getInodeIndex(target, "dir") == -1) {
            std::cout << "未找到目录：" << target << std::endl;
        } else {
            cur_dir = target;
            std::cout << "更新当前目录为 " << target << std::endl;
        }
    }

    void saveToFile(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return;
        }

        size_t curDirSize = cur_dir.size();
        file.write(reinterpret_cast<const char*>(&curDirSize), sizeof(size_t));
        file.write(cur_dir.c_str(), curDirSize);

        for (const auto& inode : inode_table) {
            // 保存字符串长度和内容
            size_t nameSize = inode.name.size();
            size_t typeSize = inode.type.size();
            size_t permissionSize = inode.permission.size();

            file.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
            file.write(inode.name.c_str(), nameSize);

            file.write(reinterpret_cast<const char*>(&typeSize), sizeof(size_t));
            file.write(inode.type.c_str(), typeSize);

            file.write(reinterpret_cast<const char*>(&permissionSize), sizeof(size_t));
            file.write(inode.permission.c_str(), permissionSize);

            // 保存其他成员
            file.write(reinterpret_cast<const char*>(&inode.size), sizeof(int));
            file.write(reinterpret_cast<const char*>(&inode.ondelete), sizeof(bool));
            file.write(reinterpret_cast<const char*>(&inode.mtime), sizeof(time_t));

            // 保存指针集合
            size_t pointerSize = inode.pointer.size();
            file.write(reinterpret_cast<const char*>(&pointerSize), sizeof(size_t));
            for (const auto& p : inode.pointer) {
                file.write(reinterpret_cast<const char*>(&p), sizeof(int));
            }
        }

        file.close();
        std::cout << "Data saved to file: " << filename << std::endl;
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file for reading: " << filename << std::endl;
            return;
        }

        inode_table.clear();

        size_t dirSize;
        if (file.read(reinterpret_cast<char*>(&dirSize), sizeof(size_t))) {
            std::string lastDir(dirSize, '\0');
            file.read(&lastDir[0], dirSize);
            cur_dir = lastDir;
        } else {
            std::cout << "无法获取上一次的目录" << std::endl;
        }

        while (true) {
            // 加载字符串长度和内容
            size_t nameSize, typeSize, permissionSize;
            if (!file.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t))) {
                break;
            }
            std::string name(nameSize, '\0');
            file.read(&name[0], nameSize);

            if (!file.read(reinterpret_cast<char*>(&typeSize), sizeof(size_t))) {
                break;
            }
            std::string type(typeSize, '\0');
            file.read(&type[0], typeSize);

            if (!file.read(reinterpret_cast<char*>(&permissionSize), sizeof(size_t))) {
                break;
            }
            std::string permission(permissionSize, '\0');
            file.read(&permission[0], permissionSize);

            // 加载其他成员
            int size;
            bool ondelete;
            time_t mtime;
            file.read(reinterpret_cast<char*>(&size), sizeof(int));
            file.read(reinterpret_cast<char*>(&ondelete), sizeof(bool));
            file.read(reinterpret_cast<char*>(&mtime), sizeof(time_t));

            // 加载指针集合
            size_t pointerSize;
            file.read(reinterpret_cast<char*>(&pointerSize), sizeof(size_t));
            std::set<int> pointer;
            for (size_t i = 0; i < pointerSize; ++i) {
                int p;
                file.read(reinterpret_cast<char*>(&p), sizeof(int));
                pointer.insert(p);
            }

            // 构造Inode对象并添加到数据集合
            inode_table.push_back({name, type, permission, size, ondelete, mtime, pointer});
        }

        file.close();
        std::cout << "Data loaded from file: " << filename << std::endl;
    }
};

/// @brief 判断名称是否合法
/// @param name 文件、目录名称
bool checkIllegalName(const std::string& name) {
    size_t findIndex = name.find(SPLIT);
    if (findIndex != std::string::npos) {
        std::cout << "名称中包含非法字符：" << name << std::endl;
        return false;
    }
    return true;
}

int main(int main, char* argv[]) {
    FileSystem fs;
    auto user = std::string("root");
    while (true) {
        std::string command;
        std::cout << "<" << user << " " << fs.currentFolder() << ">:";
        std::getline(std::cin, command);
        std::vector<std::string> command_list;
        std::stringstream ss(command);
        std::string token;
        // 获取命令以空格分割的每一个字符
        while (ss >> token) {
            command_list.push_back(token);
        }
        if (command_list.empty()) {
            continue;
        }
        if (command_list[0] == "exit") {
            fs.saveToFile("virtual_file_system.bin");
            break;
        } else if (command_list[0] == "q") {
            break;
        } else if (command_list[0] == "enter") {
            fs.loadFromFile("virtual_file_system.bin");
        } else if (command_list[0] == "touch") {
            if (command_list.size() > 1) {
                std::string& name = command_list[1];
                if (checkIllegalName(name)) {
                    fs.touch(name);
                }
            } else {
                std::cout << "创建文件时缺失文件名" << std::endl;
            }
        } else if (command_list[0] == "rm") {
            if (command_list.size() > 1) {
                std::string& name = command_list[1];
                if (checkIllegalName(name)) {
                    fs.rm(name);
                }
            } else {
                std::cout << "未输入文件名" << std::endl;
            }
        } else if (command_list[0] == "ls") {
            if (command_list.size() > 1) {
                std::string& name = command_list[1];
                if (checkIllegalName(name)) {
                    fs.ls(name);
                }
            } else {
                fs.ls();
            }
        } else if (command_list[0] == "mkdir") {
            if (command_list.size() > 1) {
                std::string& name = command_list[1];
                if (checkIllegalName(name)) {
                    fs.mkdir(name);
                }
            } else {
                std::cout << "缺失需要创建的目录名称" << std::endl;
            }
        } else if (command_list[0] == "rmdir") {
            if (command_list.size() > 1) {
                std::string& name = command_list[1];
                if (checkIllegalName(name)) {
                    fs.rmdir(name);
                }
            } else {
                std::cout << "缺失需要删除的目录名称" << std::endl;
            }
        } else if (command_list[0] == "cd") {
            if (command_list.size() > 1) {
                std::string& name = command_list[1];
                if (checkIllegalName(name)) {
                    fs.cd(name);
                }
            } else {
                fs.cd();
            }
        } else {
            std::cout << "错误指令" << std::endl;
        }
    }
    return 0;
}
