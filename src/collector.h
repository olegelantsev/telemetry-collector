#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <memory>
#include <string>
#include <vector>

#include "processor.h"
#include "receiver.h"
#include "exporter.h"

class Collector {
public:
    Collector(const std::string& configPath);
    ~Collector();

    void start();
    void shutdown();

private:
    bool parseConfig(const std::string& configPath);
    
    std::vector<std::unique_ptr<Receiver>> receivers_;
    std::vector<std::unique_ptr<Processor>> processors_;
    std::vector<std::unique_ptr<Exporter>> exporters_;
    
    bool running_;
};

#endif // COLLECTOR_H