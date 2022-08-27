#ifndef _LOGCOMPONENT_H_
#define _LOGCOMPONENT_H_

//c++
#include <iostream>
#include <string>
#include <map>
#include <mutex>

//thirdparty
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#define INFO_LOG(...) LogComponent::Instance()->InfoPrint("ThreadPool_log",__VA_ARGS__)

class LogComponent
{
private:
    LogComponent(){}
    ~LogComponent(){}
    LogComponent(const LogComponent& LogComponent){}
public:
    static LogComponent* Instance();

    template<typename... Args>
    void InfoPrint(const std::string& log_name,Args... args)
    {
        auto& map_logger = m_instance->m_map_logger;
        auto iter = map_logger.find(log_name);
        if(iter != map_logger.end())
        {
            spdlog::info(std::forward<Args>(args)...);
            iter->second->info(std::forward<Args>(args)...);
        }
    }

private:
    void RegisterLog(const std::string& log_name,
                     const std::string& path_prefix,
                     unsigned int capacity,
                     unsigned int amount
                     )
    {
        try
        {
            std::string path_suffix = ".txt";
            auto temp_logger = spdlog::rotating_logger_mt(log_name, path_prefix+log_name+path_suffix, capacity, amount);
            temp_logger -> set_pattern("[%Y-%m-%d %T.%e] [%l] %v");
            temp_logger -> flush_on(spdlog::level::info);
            m_instance->m_map_logger[log_name] = temp_logger;

            //初始化打印
            int space =2;
            for(int i=0;i<space;i++)
            {
                temp_logger->info("");
            }
            temp_logger->info("================");
            temp_logger->info("Running!!!");
            temp_logger->info("================");
            for(int i=0;i<space;i++)
            {
                temp_logger->info("");
            }
        }
        catch(const spdlog::spdlog_ex& ex)
        {
            std::cout << log_name << " : "<< ex.what() << std::endl;
        }
    }

private:
    std::map<std::string,std::shared_ptr<spdlog::logger> > m_map_logger;
    static LogComponent* m_instance;

    std::mutex m_init_mutex;
    bool m_init_flag = false;
};

#endif