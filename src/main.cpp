#include <iostream>
#include <fstream>

#include "report_builder/report_builder.h"
#include "report_builder/report_factories.h"

using namespace report_builder;

void CreateSampleCSV() {
    std::ofstream file("sales.csv");
    file << "product,units,price,region\n";
    file << "Laptop,15,999.99,North\n";
    file << "Phone,32,699.99,South\n";
    file << "Tablet,21,449.99,North\n";
    file << "Monitor,8,299.99,East\n";
    file.close();
    std::cout << "Created sample CSV: sales.csv\n";
}

int main() {
    std::cout << "=== Report Builder System ===\n\n";

    // Создаем тестовый CSV файл
    CreateSampleCSV();

    // Пример 1: Использование фабрики
    std::cout << "\n1. Using Finance Report Factory:\n";
    auto financeFactory = std::make_unique<TFinanceReportFactory>();
    auto financeReport = financeFactory->CreateReport();
    financeReport->PrintPipeline();

    auto result = financeReport->Generate();
    if (result.Success) {
        std::cout << "Finance report generated successfully!\n";
    } else {
        std::cout << "Error: " << result.ErrorMessage.value() << "\n";
    }

    // Пример 2: Использование строителя
    std::cout << "\n\n2. Using Report Builder (custom report):\n";

    auto customReport = TReportBuilder()
                            .SetDataSource(std::make_unique<TCsvDataProvider>("sales.csv"))
                            .AddProcessor(std::make_unique<TFilterProcessor>(
                                [](const DataRow& row) {
                                    auto it = row.find("price");
                                    if (it != row.end() && std::holds_alternative<double>(it->second)) {
                                        return std::get<double>(it->second) < 500.0;
                                    }
                                    return false;
                                },
                                "affordable products"))
                            .AddProcessor(std::make_unique<TSortProcessor>("units", false))
                            .SetFormatter(std::make_unique<TMarkdownFormatter>())
                            .SetExportStrategy(std::make_unique<TConsoleExportStrategy>())
                            .Build();

    customReport->PrintPipeline();
    customReport->Generate();

    // Пример 3: Еще один отчет
    std::cout << "\n\n3. Sales Report from Factory:\n";
    auto salesFactory = std::make_unique<TSalesReportFactory>();
    auto salesReport = salesFactory->CreateReport();
    salesReport->PrintPipeline();
    salesReport->Generate();

    return 0;
}
