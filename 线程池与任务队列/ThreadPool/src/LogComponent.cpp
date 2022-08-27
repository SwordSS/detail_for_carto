#include "LogComponent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

LogComponent* LogComponent::m_instance = new LogComponent();

LogComponent* LogComponent::Instance()
{
    if(!m_instance->m_init_flag)
    {
        std::lock_guard<std::mutex> lock(m_instance->m_init_mutex);
        if(!m_instance->m_init_flag)
        {
            std::string user_path = getenv("HOME");
            std::string prefix = user_path+"/spdlog/";

            float capacity = 1048576*10;
            float amount = 3;
            m_instance->RegisterLog("ThreadPool_log",
                                    prefix,
                                    capacity,
                                    amount);
            m_instance->m_init_flag = true;
        }
    }
    return m_instance;
}