#ifndef BATCH_PROCESSOR_H
#define BATCH_PROCESSOR_H

#include <string>
#include <vector>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>
#include "processor.h"

class BatchProcessor : public Processor {
public:
    BatchProcessor(size_t maxBatchSize = 512, uint64_t maxBatchDelayMillis = 5000);
    
    void handleTraces(std::vector<opentelemetry::sdk::trace::SpanData>& spans) override;
    void handleMetrics(std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) override;

private:
    size_t maxBatchSize_;
    uint64_t maxBatchDelayMillis_;
};

#endif // BATCH_PROCESSOR_H
