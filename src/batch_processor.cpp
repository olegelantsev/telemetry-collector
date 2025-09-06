#include "batch_processor.h"
#include <iostream>

BatchProcessor::BatchProcessor(size_t maxBatchSize, uint64_t maxBatchDelayMillis)
    : maxBatchSize_(maxBatchSize), maxBatchDelayMillis_(maxBatchDelayMillis) {
}

void BatchProcessor::processTraces(std::vector<opentelemetry::sdk::trace::SpanData>& spans) {
    std::cout << "BatchProcessor: Processing " << spans.size() << " spans" << std::endl;
    // In a real implementation, we would buffer spans here
    // until we reach maxBatchSize_ or maxBatchDelayMillis_
}

void BatchProcessor::processMetrics(std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) {
    std::cout << "BatchProcessor: Processing " << metrics.size() << " metrics" << std::endl;
    // In a real implementation, we would buffer metrics here
    // until we reach maxBatchSize_ or maxBatchDelayMillis_
}
