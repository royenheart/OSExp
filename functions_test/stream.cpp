/**
 * @file stream.cpp
 * @author royenheart (royenheart@outlook.com)
 * @brief stream test
 * @version 0.1
 * @date 2022-10-16
 *
 * @copyright Copyright (c) 2022
 *
 * https://blog.csdn.net/kingstar158/article/details/6859379
 */

#include <fstream>
#include <ios>
#include <iostream>
#include <string>

/**
 * 1. stream: 流
 * 2. i: 输入
 * 3. o: 输出
 * 4. f: file，文件
 */

/**
 * istream + ostream 的继承构成 iostream库
 * iostream类型继承自istream和ostream
 */

#define BUFFER_SIZE 256

using namespace std;

int main(int argc, char *argv[]) {
    cout << "Test iostream: cout, cerr, clog" << endl << "from ostream" << endl;
    cout << "It's cout" << endl;
    cerr << "It's error" << endl;
    clog << "It's clog" << endl;
    cout << "Test iostream: cin. Insert one number" << endl
         << "from isteem" << endl;
    float a;
    cin >> a;
    cout << "Enter: " << a << endl;
    cout << "Test fstream" << endl;
    fstream ioFile;
    ioFile.open(__FILE__, ios::in | ios::out);
    if (!ioFile.is_open()) {
        cerr << "File " << __FILE__ << " is not open!" << endl;
        exit(EXIT_FAILURE);
    }
    istreambuf_iterator<char> begin(ioFile);
    istreambuf_iterator<char> end;
    string fileContent(begin, end);
    cout << fileContent << endl;
    return 0;
}