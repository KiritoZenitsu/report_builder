#ifndef REPORT_BUILDER_REPORT_FACTORIES_H
#define REPORT_BUILDER_REPORT_FACTORIES_H

#include "report_builder/data_processors.h"
#include "report_builder/data_providers.h"
#include "report_builder/export_strategies.h"
#include "report_builder/formatters.h"
#include "report_builder/interfaces.h"

namespace report_builder {

    class IReportFactory {
    public:
        virtual ~IReportFactory() = default;
        virtual std::unique_ptr<IDataProvider> CreateDataProvider() = 0;
        virtual std::vector<std::unique_ptr<IDataProcessor>> CreateProcessors() = 0;
        virtual std::unique_ptr<IFormatter> CreateFormatter() = 0;
        virtual std::unique_ptr<IExportStrategy> CreateExportStrategy() = 0;

        std::unique_ptr<TReport> CreateReport() {
            return std::make_unique<TReport>(CreateDataProvider(), CreateProcessors(),
                                             CreateFormatter(), CreateExportStrategy());
        }
    };

    // Фабрика финансовых отчетов - ИСПРАВЛЕННАЯ
    class TFinanceReportFactory: public IReportFactory {
    public:
        std::unique_ptr<IDataProvider> CreateDataProvider() override {
            DataTable sampleData = {
                {{"date", "2024-01-01"}, {"revenue", 15000}, {"expenses", 8000}},
                {{"date", "2024-01-02"}, {"revenue", 18000}, {"expenses", 8500}},
                {{"date", "2024-01-03"}, {"revenue", 12000}, {"expenses", 7000}},
                {{"date", "2024-01-04"}, {"revenue", 22000}, {"expenses", 9500}},
            };
            return std::make_unique<TInMemoryDataProvider>(sampleData);
        }

        std::vector<std::unique_ptr<IDataProcessor>> CreateProcessors() override {
            std::vector<std::unique_ptr<IDataProcessor>> processors;
            processors.push_back(std::make_unique<TSortProcessor>("date", true));

            // Используем TMultiAggregationProcessor вместо двух отдельных
            std::vector<std::pair<std::string, std::string>> aggregations = {
                {"revenue", "sum"},
                {"expenses", "sum"}};
            processors.push_back(std::make_unique<TMultiAggregationProcessor>(aggregations));

            return processors;
        }

        std::unique_ptr<IFormatter> CreateFormatter() override {
            return std::make_unique<THtmlFormatter>();
        }

        std::unique_ptr<IExportStrategy> CreateExportStrategy() override {
            return std::make_unique<TFileExportStrategy>("reports/finance/");
        }
    };

    // Фабрика отчетов по продажам (оставляем без изменений)
    class TSalesReportFactory: public IReportFactory {
    public:
        std::unique_ptr<IDataProvider> CreateDataProvider() override {
            DataTable sampleData = {
                {{"product", "Laptop"}, {"units", 15}, {"price", 999.99}, {"region", "North"}},
                {{"product", "Phone"}, {"units", 32}, {"price", 699.99}, {"region", "South"}},
                {{"product", "Tablet"}, {"units", 21}, {"price", 449.99}, {"region", "North"}},
                {{"product", "Monitor"}, {"units", 8}, {"price", 299.99}, {"region", "East"}},
            };
            return std::make_unique<TInMemoryDataProvider>(sampleData);
        }

        std::vector<std::unique_ptr<IDataProcessor>> CreateProcessors() override {
            std::vector<std::unique_ptr<IDataProcessor>> processors;
            processors.push_back(std::make_unique<TFilterProcessor>(
                [](const DataRow& row) {
                    auto it = row.find("price");
                    if (it != row.end() && std::holds_alternative<double>(it->second)) {
                        return std::get<double>(it->second) > 500.0;
                    }
                    return false;
                },
                "price > 500"));
            processors.push_back(std::make_unique<TAggregationProcessor>("units", "sum"));
            return processors;
        }

        std::unique_ptr<IFormatter> CreateFormatter() override {
            return std::make_unique<TPlainTextFormatter>();
        }

        std::unique_ptr<IExportStrategy> CreateExportStrategy() override {
            return std::make_unique<TEmailExportStrategy>("sales@company.com");
        }
    };
} // namespace report_builder

#endif
