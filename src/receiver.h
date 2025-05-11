#ifndef RECEIVER_H
#define RECEIVER_H

#include <string>
#include <memory>
#include "processor.h"

class Receiver {
public:
    virtual ~Receiver() = default;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    
    void setNextProcessor(std::shared_ptr<Processor> processor) {
        nextProcessor_ = processor;
    }

protected:
    std::shared_ptr<Processor> nextProcessor_;
};

class OtlpGrpcReceiver : public Receiver {
public:
    OtlpGrpcReceiver(const std::string& endpoint);
    
    void start() override;
    void stop() override;

private:
    std::string endpoint_;
    bool running_;
    // In a real implementation, we'd have a gRPC server here
};

class OtlpHttpReceiver : public Receiver {
public:
    OtlpHttpReceiver(const std::string& endpoint);
    
    void start() override;
    void stop() override;

private:
    std::string endpoint_;
    bool running_;
    // In a real implementation, we'd have an HTTP server here
};

#endif // RECEIVER_H
