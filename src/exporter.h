#ifndef EXPORTER_H
#define EXPORTER_H

#include <string>
#include <vector>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>
#include "chain.h"

class Exporter: public Chain {
public:
    virtual ~Exporter() = default;
    
};

#endif // EXPORTER_H
