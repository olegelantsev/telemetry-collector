#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <memory>
#include <string>
#include <vector>

#include "pipeline.h"

class Collector {
public:
    Collector(const std::string& configPath);
    ~Collector();

    void start();
    void shutdown();

private:
    bool parseConfig(const std::string& configPath);
    
    std::vector<std::unique_ptr<Pipeline>> pipelines_;
    
    bool running_;
};

#endif // COLLECTOR_H