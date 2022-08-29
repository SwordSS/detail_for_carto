#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include "LogComponent.h"
#include "ThreadPool.h"
#include "TicToc.h"

/*
  pow((1+2+3...10),2)+pow((1+2+3...+10,2))
*/

namespace
{
    int ref_task()
    {
        int result = 0;
        for(int i=1;i<=10;i++)
        {
            result+=i;
        }
        return pow(result,2)+pow(result,2);
    }

    int sum_task(int start_num,int end_num)
    {
        int result = 0;
        for(int i=start_num;i<=end_num;i++)
        {
            result +=i;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        return result;
    }

    int pow_task(int menber,int pow_times)
    {
        int result = pow(menber,pow_times);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return result;
    }

    int add_task(int menber_1,int menber_2)
    {
        int result = menber_1 + menber_2;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        return result;
    }

    int operation_number_1()
    {
        int result_0 = sum_task(1,10);
        int result_1 = pow_task(result_0,2);
        int result_2 = sum_task(1,10);
        int result_3 = pow_task(result_2,2);
        int result_4 = add_task(result_1,result_3);
        return result_4;
    }
}

int multi_thread_task()
{
    std::vector<int> result_vec(7,0);
    std::mutex mutex;
    std::vector<std::thread> thread_vec;

    int& sum_result_0 = result_vec[0];
    int& sum_result_1 = result_vec[1];
    int& sum_result_2 = result_vec[2];
    int& sum_result_3 = result_vec[3];
    int& pow_result_0 = result_vec[4];
    int& pow_result_1 = result_vec[5];
    int& add_result_0 = result_vec[6];
    
    thread_vec.emplace_back([&](){sum_result_0 = sum_task(1,5);});
    thread_vec.emplace_back([&](){sum_result_1 = sum_task(6,10);});
    thread_vec.emplace_back([&](){pow_result_0 = pow_task(add_task(sum_result_0,sum_result_1),2);});
    thread_vec.emplace_back([&](){sum_result_2 = sum_task(1,5);});
    thread_vec.emplace_back([&](){sum_result_3 = sum_task(6,10);});
    thread_vec.emplace_back([&](){pow_result_1 = pow_task(add_task(sum_result_2,sum_result_3),2);});
    thread_vec.emplace_back([&](){add_result_0 = add_task(pow_result_0,pow_result_1);});
    for(int i=0;i<thread_vec.size();i++)
    {
        thread_vec[i].join();
    }
    return add_result_0;
}

int main(int argc,char** argv)
{
    // int thread_num = 4;
    // ThreadPool thread_pool(thread_num);
    TicToc clock_0;
    clock_0.Tic();

    int result_ref = ref_task();
    std::cout << "result_ref = "<< result_ref << std::endl;

    int result_0 = operation_number_1();
    std::cout << "result_0 = " << result_0 << " "
              << ",clock_0_time = " << clock_0.Toc() << " ms" << std::endl;

    
    TicToc clock_1;
    clock_1.Tic();

    int result_1 = multi_thread_task();
    std::cout << "result_1 = " << result_1 << " "
              << ",clock_1_time = " << clock_1.Toc() << " ms" << std::endl;

    
    return 0;
}