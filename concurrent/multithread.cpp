#include <cstdio>
#include <thread>
#include <vector>
using namespace std;

void loop()
{
    while (true);
}

int main()
{
    vector<thread> vec;
    for (int i = 0; i < 4; ++i)
        vec.push_back(thread(loop));
    for (auto &i : vec)
        i.join();
    return 0;
}