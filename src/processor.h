#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>

class Processor {
public:
    virtual ~Processor() = default;
    
    virtual void processTraces(std::vector<opentelemetry::sdk::trace::SpanData>& spans) = 0;
    virtual void processMetrics(std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) = 0;
};

class BatchProcessor : public Processor {
public:
    BatchProcessor(size_t maxBatchSize = 512, uint64_t maxBatchDelayMillis = 5000);
    
    void processTraces(std::vector<opentelemetry::sdk::trace::SpanData>& spans) override;
    void processMetrics(std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) override;

private:
    size_t maxBatchSize_;
    uint64_t maxBatchDelayMillis_;
};

#endif // PROCESSOR_H
