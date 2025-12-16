#ifndef REPORT_FACTORIES_H
#define REPORT_FACTORIES_H

#include "interfaces.h"
#include "data_providers.h"
#include "data_processors.h"
#include "formatters.h"
#include "export_strategies.h"

// Абстрактная фабрика
class ReportFactory {
public:
    virtual ~ReportFactory() = default;
    virtual std::unique_ptr<DataProvider> createDataProvider() = 0;
    virtual std::vector<std::unique_ptr<DataProcessor>> createProcessors() = 0;
    virtual std::unique_ptr<Formatter> createFormatter() = 0;
    virtual std::unique_ptr<ExportStrategy> createExportStrategy() = 0;
    
    std::unique_ptr<Report> createReport() {
        return std::make_unique<Report>(
            createDataProvider(),
            createProcessors(),
            createFormatter(),
            createExportStrategy()
        );
    }
};

// Фабрика финансовых отчетов
class FinanceReportFactory : public ReportFactory {
public:
    std::unique_ptr<DataProvider> createDataProvider() override {
		// Создаем тестовые данные
        DataTable sample_data = {
            {{"date", "2024-01-01"}, {"revenue", 15000}, {"expenses", 8000}},
            {{"date", "2024-01-02"}, {"revenue", 18000}, {"expenses", 8500}},
            {{"date", "2024-01-03"}, {"revenue", 12000}, {"expenses", 7000}},
            {{"date", "2024-01-04"}, {"revenue", 22000}, {"expenses", 9500}},
        };
        return std::make_unique<InMemoryDataProvider>(sample_data);
    }
    
    std::vector<std::unique_ptr<DataProcessor>> createProcessors() override {
        std::vector<std::unique_ptr<DataProcessor>> processors;
        processors.push_back(std::make_unique<SortProcessor>("date", true));
        processors.push_back(std::make_unique<AggregationProcessor>("revenue", "sum"));
        processors.push_back(std::make_unique<AggregationProcessor>("expenses", "sum"));
        return processors;
    }
    
    std::unique_ptr<Formatter> createFormatter() override {
        return std::make_unique<HtmlFormatter>();
    }
    
    std::unique_ptr<ExportStrategy> createExportStrategy() override {
        return std::make_unique<FileExportStrategy>("reports/finance/");
    }
};

// Фабрика отчетов по продажам
class SalesReportFactory : public ReportFactory {
public:
    std::unique_ptr<DataProvider> createDataProvider() override {
        DataTable sample_data = {
            {{"product", "Laptop"}, {"units", 15}, {"price", 999.99}, {"region", "North"}},
            {{"product", "Phone"}, {"units", 32}, {"price", 699.99}, {"region", "South"}},
            {{"product", "Tablet"}, {"units", 21}, {"price", 449.99}, {"region", "North"}},
            {{"product", "Monitor"}, {"units", 8}, {"price", 299.99}, {"region", "East"}},
        };
        return std::make_unique<InMemoryDataProvider>(sample_data);
    }
    
    std::vector<std::unique_ptr<DataProcessor>> createProcessors() override {
        std::vector<std::unique_ptr<DataProcessor>> processors;
		
		// Пример: фильтр для дорогих товаров
        processors.push_back(std::make_unique<FilterProcessor>(
            [](const DataRow& row) {
                auto it = row.find("price");
                if (it != row.end() && std::holds_alternative<double>(it->second)) {
                    return std::get<double>(it->second) > 500.0;
                }
                return false;
            },
            "price > 500"
        ));
        processors.push_back(std::make_unique<AggregationProcessor>("units", "sum"));
        return processors;
    }
    
    std::unique_ptr<Formatter> createFormatter() override {
        return std::make_unique<PlainTextFormatter>();
    }
    
    std::unique_ptr<ExportStrategy> createExportStrategy() override {
        return std::make_unique<EmailExportStrategy>("sales@company.com");
    }
};

#endif
