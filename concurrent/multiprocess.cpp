#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
using namespace std;

vector<pid_t> vec;

void sig_handler(int sig)
{
    for (const auto &i : vec)
        kill(i, SIGINT);
}

int main()
{
    sigset_t wait_set;
    for (int i = 0; i < 4; ++i)
    {
        pid_t pid = fork();
        if (pid == 0)
            while (true);
        vec.push_back(pid);
    }
    signal(SIGINT, sig_handler);
    sigemptyset(&wait_set);
    sigsuspend(&wait_set);
    return 0;
}