#ifndef INTERFACES_H
#define INTERFACES_H

#include "data_types.h"
#include <memory>
#include <functional>

// Базовый класс для поставщика данных
class DataProvider {
public:
    virtual ~DataProvider() = default;
    virtual OperationResult fetchData() = 0;
    virtual std::string getSourceInfo() const = 0;
};

// Базовый класс для обработчика данных
class DataProcessor {
public:
    virtual ~DataProcessor() = default;
    virtual OperationResult process(DataTable data) = 0;
    virtual std::string getDescription() const = 0;
};

// Базовый класс для форматировщика
class Formatter {
public:
    virtual ~Formatter() = default;
    virtual std::string format(const DataTable& data) = 0;
    virtual std::string getFormatName() const = 0;
};

// Базовый класс для стратегии экспорта
class ExportStrategy {
public:
    virtual ~ExportStrategy() = default;
    virtual bool exportData(const std::string& formattedData) = 0;
    virtual std::string getMethodName() const = 0;
};

// Класс отчета
class Report {
private:
    std::unique_ptr<DataProvider> data_source;
    std::vector<std::unique_ptr<DataProcessor>> processors;
    std::unique_ptr<Formatter> formatter;
    std::unique_ptr<ExportStrategy> exporter;
    
public:
    Report(std::unique_ptr<DataProvider> source,
           std::vector<std::unique_ptr<DataProcessor>> procs,
           std::unique_ptr<Formatter> fmt,
           std::unique_ptr<ExportStrategy> exp)
        : data_source(std::move(source))
        , processors(std::move(procs))
        , formatter(std::move(fmt))
        , exporter(std::move(exp)) {}
    
    OperationResult generate() {
        auto raw_data = data_source->fetchData();
        if (!raw_data.success) {
            return raw_data;
        }
        
        DataTable processed = std::move(raw_data.data);
        for (auto& proc : processors) {
            auto result = proc->process(processed);
            if (!result.success) {
                return result;
            }
            processed = std::move(result.data);
        }
        
        auto formatted = formatter->format(processed);
        bool exported = exporter->exportData(formatted);
        
        if (!exported) {
            return OperationResult::Error("Export failed");
        }
        
        return OperationResult::Ok(processed);
    }
    
    void printPipeline() const {
        std::cout << "Report Pipeline:\n";
        std::cout << "  Source: " << data_source->getSourceInfo() << "\n";
        for (const auto& proc : processors) {
            std::cout << "  Processor: " << proc->getDescription() << "\n";
        }
        std::cout << "  Formatter: " << formatter->getFormatName() << "\n";
        std::cout << "  Exporter: " << exporter->getMethodName() << "\n";
    }
};

#endif
