#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <pthread.h>

#include <cstdlib>
#include <iostream>
#include <iterator>

using namespace std;

int main(int argc, char *argv[]) {
    cout << "EDEADLK: " << EDEADLK << endl;
    cout << "EINVAL: " << EINVAL << endl;
    cout << "ESRCH: " << ESRCH << endl;
    return EXIT_SUCCESS;
}
