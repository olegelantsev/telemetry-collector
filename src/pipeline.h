#ifndef PIPELINE_H
#define PIPELINE_H

#include "receiver.h"
#include "processor.h"
#include "exporter.h"
#include <vector>
#include <memory>
#include <functional>

// Pipeline class to connect receivers, processors, and exporters
class Pipeline {
public:
    Pipeline(const std::string& name);

    void addReceiver(std::shared_ptr<Receiver> receiver);

    void addProcessor(std::shared_ptr<Processor> processor);

    void addExporter(std::shared_ptr<Exporter> exporter) ;

    void initialize();

    void start();

    void shutdown();

private:
    std::string name_;
    std::vector<std::shared_ptr<Receiver>> receivers_;
    std::vector<std::shared_ptr<Processor>> processors_;
    std::vector<std::shared_ptr<Exporter>> exporters_;
};

#endif // PIPELINE_H
