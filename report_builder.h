#ifndef REPORT_BUILDER_H
#define REPORT_BUILDER_H

#include "interfaces.h"
#include "data_providers.h"
#include "data_processors.h"
#include "formatters.h"
#include "export_strategies.h"
#include <memory>

class ReportBuilder {
private:
    std::unique_ptr<DataProvider> data_source;
    std::vector<std::unique_ptr<DataProcessor>> processors;
    std::unique_ptr<Formatter> formatter;
    std::unique_ptr<ExportStrategy> exporter;
    
public:
    ReportBuilder() = default;
    
    ReportBuilder& setDataSource(std::unique_ptr<DataProvider> provider) {
        data_source = std::move(provider);
        return *this;
    }
    
    ReportBuilder& addProcessor(std::unique_ptr<DataProcessor> processor) {
        processors.push_back(std::move(processor));
        return *this;
    }
    
    ReportBuilder& setFormatter(std::unique_ptr<Formatter> fmt) {
        formatter = std::move(fmt);
        return *this;
    }
    
    ReportBuilder& setExportStrategy(std::unique_ptr<ExportStrategy> exp) {
        exporter = std::move(exp);
        return *this;
    }
    
    std::unique_ptr<Report> build() {
        if (!data_source || !formatter || !exporter) {
            throw std::runtime_error("Incomplete report configuration");
        }
        
        return std::make_unique<Report>(
            std::move(data_source),
            std::move(processors),
            std::move(formatter),
            std::move(exporter)
        );
    }
};

#endif
