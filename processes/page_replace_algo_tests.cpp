#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <tuple>
#include <functional>
#include <vector>
#include <random>

// 页面尺寸大小
#define PAGESIZE 8 
// 各个算法实验次数
#define TEST_TIMES 100
// 序列长度
#define SEQ_LEN 40

// 声明一种新类型--物理块类型
typedef struct BLOCK {
    // 页号
    int pagenum;  
    // 访问量,其值表示多久未被访问
    int accessed; 
} BLOCK;
// 程序计数器，用来记录对应的页号
int pc;                     
// 缺页计数器，用来记录缺页的次数
int n;                      
// 用来存储 SEQ_LEN 条随机数
int *num = NULL;        
// 定义一大小为 8 的物理块数组
BLOCK block[PAGESIZE];      
// 程序初始化函数
void init();                
// 查找物理块中是否有该页面
int findExist(int curpage); 
// 查找是否有空闲物理块
int findSpace();            
// 查找应予置换的页面
int findReplace();          
// 显示
void display();             
// 产生 SEQ_LEN 条随机数,显示并存储到 num[SEQ_LEN]
void randam();          
// 显示调用的页面队列    
void pagestring();      

int OPT();
int LRU();
int FIFO();

int main(int argc, char* argv[]) {
    // 记录页面缺失率
    std::vector<std::tuple<int, int, int>> page_losts(TEST_TIMES);

    num = (int*)malloc(sizeof(int) * SEQ_LEN);

    for (int i = 0; i < TEST_TIMES; i++) {
        // 根据时间生成调用序列
        randam();
        // 打印调用界面队列
        printf("界面调用队列\n");
        pagestring();
        init();
        int l1 = OPT();
        init();
        int l2 = FIFO();
        init();
        int l3 = LRU();
        page_losts.push_back(std::make_tuple(l1, l2, l3));
    }
    
    int time = -1;
    double avg_l1, avg_l2, avg_l3;
    for (auto t : page_losts) {
        ++time;
        int l1 = std::get<0>(t);
        int l2 = std::get<1>(t);
        int l3 = std::get<2>(t);
        printf("OPT 缺页次数第%d次为：%d\n", time, l1);
        printf("FIFO 缺页次数第%d次为：%d\n", time, l2);
        printf("LRU 缺页次数第%d次为：%d\n", time, l3);
        printf("OPT 缺页率第%d次为：%.2lf%\n", time, ((double)l1 / SEQ_LEN) * 100.0);
        printf("FIFO 缺页率第%d次为：%.2lf%\n", time, ((double)l2 / SEQ_LEN) * 100.0);
        printf("LRU 缺页率第%d次为：%.2lf%\n", time, ((double)l3 / SEQ_LEN) * 100.0);
        avg_l1 += l1;
        avg_l2 += l2;
        avg_l3 += l3;
    }

    printf("OPT 平均缺页率为：%.2lf%\n", (avg_l1 / (SEQ_LEN * TEST_TIMES)) * 100.0);
    printf("FIFO 平均缺页率为：%.2lf%\n", (avg_l2 / (SEQ_LEN * TEST_TIMES)) * 100.0);
    printf("LRU 平均缺页率为：%.2lf%\n", (avg_l3 / (SEQ_LEN * TEST_TIMES)) * 100.0);

    free(num);
}

void init() {
    for (int i = 0; i < PAGESIZE; i++) {
        block[i].pagenum = -1;
        block[i].accessed = 0;
    }
    pc = n = 0;
}

int findExist(int curpage) {
    // 检测到内存中有该页面,返回 block 中的位置
    for (int i = 0; i < PAGESIZE; i++) {
        if (block[i].pagenum == curpage) {
            return i; 
        }
    }
    return -1;
}

int findSpace() {
    // 找到空闲的 block，返回 block 中的位置
    for (int i = 0; i < PAGESIZE; i++) {
        if (block[i].pagenum == -1) {
            return i;
        }
    } 
    return -1;
}

int findReplace() {
    int pos = 0;
    for (int i = 0; i < PAGESIZE; i++) {
        // 当前查询到的物理块的未访问次数大于比较的物理块
        if (block[i].accessed > block[pos].accessed) {
            // 找到应该置换页面，返回 BLOCK 中位置
            pos = i;                                 
        }
    }
    return pos;
}

void display() {
    for (int i = 0; i < PAGESIZE; i++) {
        if (block[i].pagenum != -1) {
            printf(" %02d", block[i].pagenum);
        }
    }
    printf("\n");
}

void randam() {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(0, PAGESIZE * 20 - 1);
    for (int i = 0; i < SEQ_LEN; i++) {
        pc = distribution(generator); 
        num[i] = pc;
    }
}

// 显示调用的页面队列,页面号取法为随机数除 10 取整
void pagestring() {
    for (int i = 0; i < SEQ_LEN; i++) {
        printf(" %02d", num[i] / 10);
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }
}

// 最佳替换算法
int OPT() {
    int exist, space, position;
    int curpage;
    for (int i = 0; i < SEQ_LEN; i++) {
        pc = num[i];
        curpage = pc / 10;
        exist = findExist(curpage);
        if (exist == -1) {
            space = findSpace();
            if (space != -1) {
                block[space].pagenum = curpage;
                display();
                n = n + 1;
            } else {
                for (int k = 0; k < PAGESIZE; k++) {
                    for (int j = i; j < SEQ_LEN; j++) {
                        if (block[k].pagenum != num[i] / 10) {
                            block[k].accessed = 1000;
                        } else {
                            // 将来不会用，设置为一个很大数
                            block[k].accessed = j;
                            break;
                        }
                    }
                }
                position = findReplace();
                block[position].pagenum = curpage;
                display();
                n++;
            }
        }
    }
    return n;
}

// 最近最久未使用算法 
int LRU() {
    int exist, space, position;
    int curpage;
    for (int i = 0; i < SEQ_LEN; i++) {
        pc = num[i];
        // 转换为页面号
        curpage = pc / 10;          
        // 查找物理块中是否有该页面,没有的话，置为 - 1 if (exist == -1)
        exist = findExist(curpage); 
        if (exist == -1) {
            // 查找是否有空的物理块，没有的话，置为 - 1;有的话，把位置返回
            space = findSpace(); 
            // 有空闲物理块，进行存储
            if (space != -1) {
                block[space].pagenum = curpage;
                display();
                n = n + 1;
            } else { 
                // 没有空闲物理块，进行置换
                position = findReplace();
                block[position].pagenum = curpage;
                display();
                n++;
            }
        } else {
            // 恢复存在的并刚访问过的BLOCK 中页面 accessed 为 - 1 for (int j = 0; j < pages ze; j++)
            block[exist].accessed = -1; 
        }
        // 把所有在页面里的页面号的访问次数加 1
        block[i].accessed++; 
    }
    return n;
}

// FIFO 先进先出算法
int FIFO() {
    int exist, space, position;
    int curpage;
    for (int i = 0; i < SEQ_LEN; i++) {
        pc = num[i];
        // 转换为页面号
        curpage = pc / 10;          
        // 查找物理块中是否有该页面,没有的话，置为-1 if (exist == -1)
        exist = findExist(curpage); 
        if (exist == -1) {
            // 查找是否有空的物理块，没有的话，置为-1;有的话，把位置返回
            space = findSpace(); 
            if (space != -1) {
                block[space].pagenum = curpage;
                display();
                n = n + 1;
            } else {
                position = findReplace(); // 没有空闲物理块，进行置换
                block[position].pagenum = curpage;
                display();
                n++;
                block[position].accessed--;
            }
        }
        // 把所有在页面里的页面号的访问次数加 1
        for (int j = 0; j < PAGESIZE; j++) {
            block[j].accessed++;
        }
    }
    return n;
}