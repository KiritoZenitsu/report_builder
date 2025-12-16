#include <iostream>
#include "report_builder.h"
#include "report_factories.h"

void createSampleCSV() {
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
    createSampleCSV();

    // Пример 1: Использование фабрики
    std::cout << "\n1. Using Finance Report Factory:\n";
    auto finance_factory = std::make_unique<FinanceReportFactory>();
    auto finance_report = finance_factory->createReport();
    finance_report->printPipeline();

    auto result = finance_report->generate();
    if (result.success) {
        std::cout << "Finance report generated successfully!\n";
    } else {
        std::cout << "Error: " << result.error_message.value() << "\n";
    }

    // Пример 2: Использование строителя
    std::cout << "\n\n2. Using Report Builder (custom report):\n";

    auto custom_report = ReportBuilder()
        .setDataSource(std::make_unique<CsvDataProvider>("sales.csv"))
        .addProcessor(std::make_unique<FilterProcessor>(
            [](const DataRow& row) {
                auto it = row.find("price");
                if (it != row.end() && std::holds_alternative<double>(it->second)) {
                    return std::get<double>(it->second) < 500.0;
                }
                return false;
            },
            "affordable products"
        ))
        .addProcessor(std::make_unique<SortProcessor>("units", false))
        .setFormatter(std::make_unique<MarkdownFormatter>())
        .setExportStrategy(std::make_unique<ConsoleExportStrategy>())
        .build();

    custom_report->printPipeline();
    custom_report->generate();

    // Пример 3: Еще один отчет
    std::cout << "\n\n3. Sales Report from Factory:\n";
    auto sales_factory = std::make_unique<SalesReportFactory>();
    auto sales_report = sales_factory->createReport();
    sales_report->printPipeline();
    sales_report->generate();

    return 0;
}
