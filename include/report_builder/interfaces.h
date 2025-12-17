#ifndef REPORT_BUILDER_INTERFACES_H
#define REPORT_BUILDER_INTERFACES_H

#include <functional>
#include <memory>
#include <iostream>

#include "report_builder/data_types.h"

namespace report_builder {
    // Базовый класс для поставщика данных
    class IDataProvider {
    public:
        virtual ~IDataProvider() = default;
        virtual TOperationResult FetchData() = 0;
        virtual std::string GetSourceInfo() const = 0;
    };

    // Базовый класс для обработчика данных
    class IDataProcessor {
    public:
        virtual ~IDataProcessor() = default;
        virtual TOperationResult Process(DataTable data) = 0;
        virtual std::string GetDescription() const = 0;
    };

    // Базовый класс для форматировщика
    class IFormatter {
    public:
        virtual ~IFormatter() = default;
        virtual std::string Format(const DataTable& data) = 0;
        virtual std::string GetFormatName() const = 0;
    };

    // Базовый класс для стратегии экспорта
    class IExportStrategy {
    public:
        virtual ~IExportStrategy() = default;
        virtual bool ExportData(const std::string& formattedData) = 0;
        virtual std::string GetMethodName() const = 0;
    };

    // Класс отчета
    class TReport {
    private:
        std::unique_ptr<IDataProvider> DataSource;
        std::vector<std::unique_ptr<IDataProcessor>> Processors;
        std::unique_ptr<IFormatter> Formatter;
        std::unique_ptr<IExportStrategy> Exporter;

    public:
        TReport(std::unique_ptr<IDataProvider> source,
                std::vector<std::unique_ptr<IDataProcessor>> processors,
                std::unique_ptr<IFormatter> formatter,
                std::unique_ptr<IExportStrategy> exporter)
            : DataSource(std::move(source))
            , Processors(std::move(processors))
            , Formatter(std::move(formatter))
            , Exporter(std::move(exporter)) {
        }

        TOperationResult Generate() {
            auto rawData = DataSource->FetchData();
            if (!rawData.Success) {
                return rawData;
            }

            DataTable processed = std::move(rawData.Data);
            for (auto& processor : Processors) {
                auto result = processor->Process(processed);
                if (!result.Success) {
                    return result;
                }
                processed = std::move(result.Data);
            }

            auto formatted = Formatter->Format(processed);
            bool exported = Exporter->ExportData(formatted);

            if (!exported) {
                return TOperationResult::Error("Export failed");
            }

            return TOperationResult::Ok(processed);
        }

        void PrintPipeline() const {
            std::cout << "Report Pipeline:\n";
            std::cout << "  Source: " << DataSource->GetSourceInfo() << "\n";
            for (const auto& processor : Processors) {
                std::cout << "  Processor: " << processor->GetDescription() << "\n";
            }
            std::cout << "  Formatter: " << Formatter->GetFormatName() << "\n";
            std::cout << "  Exporter: " << Exporter->GetMethodName() << "\n";
        }
    };
} // namespace report_builder

#endif
