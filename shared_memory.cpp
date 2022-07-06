#include <iostream>
#include <thread>
#include <memory>
#include <windows.h>
#include <vector>
#include <string>

#define SIZE_P

struct philosopher_tag
{
    std::string name;
    bool left_fork;
    bool right_fork;
};

void eat(std::vector<philosopher_tag> &philosopher_t, std::vector<std::mutex> &table_t)
{
    philosopher_args_t *arg = (philosopher_args_t *)args;
    const philosopher_t *philosopher = arg->philosopher;
    const table_t *table = arg->table;
    unsigned rand;

    do
    {
        printf("%s started dinner\n", philosopher->name);

        pthread_mutex_lock(&entry_point);
        pthread_mutex_lock(&table->forks[philosopher->left_fork]);
        rand_s(&rand);
        rand %= 1000;
        Sleep(rand);
        pthread_mutex_lock(&table->forks[philosopher->right_fork]);
        pthread_mutex_unlock(&entry_point);

        printf("%s is eating after %d ms sleep\n", philosopher->name, rand);

        pthread_mutex_unlock(&table->forks[philosopher->right_fork]);
        pthread_mutex_unlock(&table->forks[philosopher->left_fork]);

        printf("%s finished dinner\n", philosopher->name);
    } while (1);
}

int main(int argc, char *argv[])
{
    std::vector<philosopher_tag> philosophers_t = {
        {"1", 0, 1},
        {"2", 1, 2},
        {"3", 2, 3},
        {"4", 3, 4},
        {"5", 4, 0}};

    std::vector<std::mutex> table_t(SIZE_P);

    return 0;
}