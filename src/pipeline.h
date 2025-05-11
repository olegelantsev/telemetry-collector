#ifndef PIPELINE_H
#define PIPELINE_H

#include "receiver.h"
#include "processor.h"
#include "exporter.h"
#include <vector>
#include <memory>

// Pipeline class to connect receivers, processors, and exporters
class Pipeline {
public:
    Pipeline(const std::string& name) : name_(name) {}
    
    void addReceiver(std::shared_ptr<Receiver> receiver) {
        receivers_.push_back(receiver);
    }
    
    void addProcessor(std::shared_ptr<Processor> processor) {
        processors_.push_back(processor);
    }
    
    void addExporter(std::shared_ptr<Exporter> exporter) {
        exporters_.push_back(exporter);
    }
    
    void initialize() {
        // Connect the components in the pipeline
        for (auto& receiver : receivers_) {
            for (auto& processor : processors_) {
                // In real implementation, would wire up callbacks
                // between receivers and processors
            }
        }
        
        // In real implementation, would wire up processors to exporters
    }
    
    void start() {
        for (auto& receiver : receivers_) {
            receiver->start();
        }
    }
    
    void shutdown() {
        for (auto& receiver : receivers_) {
            receiver->stop();
        }
    }
    
private:
    std::string name_;
    std::vector<std::shared_ptr<Receiver>> receivers_;
    std::vector<std::shared_ptr<Processor>> processors_;
    std::vector<std::shared_ptr<Exporter>> exporters_;
};

#endif // PIPELINE_H
