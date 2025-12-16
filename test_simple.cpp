#include <iostream>
#include "report_builder.h"
#include "report_factories.h"

int main() {
    std::cout << "=== Simple test of the reporting system ===\n\n";

    // 1. Тестируем фабрику финансовых отчетов
    std::cout << "1. Test FinanceReportFactory:\n";
    try {
        auto finance_factory = std::make_unique<FinanceReportFactory>();
        auto finance_report = finance_factory->createReport();

        finance_report->printPipeline();

        auto result = finance_report->generate();
        if (result.success) {
            std::cout << "The financial report has been successfully generated!\n";
        } else {
            std::cout << "Error: " << result.error_message.value_or("unknown error") << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "Exclusion: " << e.what() << "\n";
    }

    // 2. Тестируем строитель
    std::cout << "\n2. Test ReportBuilder:\n";
    try {
        // Создаем тестовый CSV файл
        std::ofstream test_file("test_data.csv");
        test_file << "name,age,salary,department\n";
        test_file << "Ivan,25,50000,IT\n";
        test_file << "Maria,30,60000,HR\n";
        test_file << "Aleksey,22,45000,IT\n";
        test_file << "Olga,35,70000,Finance\n";
        test_file.close();

        auto custom_report = ReportBuilder()
            .setDataSource(std::make_unique<CsvDataProvider>("test_data.csv"))
            .addProcessor(std::make_unique<FilterProcessor>(
                [](const DataRow& row) {
                    auto it = row.find("department");
                    if (it != row.end() && std::holds_alternative<std::string>(it->second)) {
                        return std::get<std::string>(it->second) == "IT";
                    }
                    return false;
                },
                "department == IT"
            ))
            .addProcessor(std::make_unique<SortProcessor>("salary", false)) // по убыванию
            .setFormatter(std::make_unique<MarkdownFormatter>())
            .setExportStrategy(std::make_unique<ConsoleExportStrategy>())
            .build();

        custom_report->printPipeline();

        auto result = custom_report->generate();
        if (result.success) {
            std::cout << "The custom report has been successfully generated!\n";
        } else {
            std::cout << "Error: " << result.error_message.value_or("unknown error") << "\n";
        }

        // Удаляем тестовый файл
        std::remove("test_data.csv");

    } catch (const std::exception& e) {
        std::cout << "Exclusion: " << e.what() << "\n";
    }

    return 0;
}
