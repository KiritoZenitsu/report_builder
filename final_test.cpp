#include <iostream>
#include "report_builder.h"
#include "report_factories.h"

int main() {
    std::cout << "=== Final test ===\n\n";
    
    // Тест 1: Полный конвейер через фабрику
    std::cout << "Test 1: Full conveyor (factory)\n";
    auto factory = std::make_unique<SalesReportFactory>();
    auto report1 = factory->createReport();
    report1->printPipeline();
    auto result1 = report1->generate();
    std::cout << (result1.success ? "Success" : "Error") << "\n\n";
    
    // Тест 2: Кастомный конвейер через строитель
    std::cout << "Test 2: Custom conveyor (builder)\n";
    
    // Создаем тестовые данные
    std::ofstream data_file("employees.csv");
    data_file << "name,department,salary,years\n";
    data_file << "Alice,Engineering,80000,3\n";
    data_file << "Bob,Marketing,60000,5\n";
    data_file << "Charlie,Engineering,90000,7\n";
    data_file << "Diana,HR,55000,2\n";
    data_file.close();
    
    auto report2 = ReportBuilder()
        .setDataSource(std::make_unique<CsvDataProvider>("employees.csv"))
        .addProcessor(std::make_unique<FilterProcessor>(
            [](const DataRow& row) {
                auto it = row.find("department");
                return it != row.end() && 
                       std::holds_alternative<std::string>(it->second) &&
                       std::get<std::string>(it->second) == "Engineering";
            },
            "department == Engineering"
        ))
        .addProcessor(std::make_unique<SortProcessor>("salary", false))
        .addProcessor(std::make_unique<AggregationProcessor>("salary", "avg"))
        .setFormatter(std::make_unique<HtmlFormatter>())
        .setExportStrategy(std::make_unique<FileExportStrategy>())
        .build();
    
    report2->printPipeline();
    auto result2 = report2->generate();
    std::cout << (result2.success ? "Success" : "Error") << "\n\n";
    
    // Тест 3: Обработка ошибок
    std::cout << "Test 3: Error handling (non-existent file)\n";
    auto report3 = ReportBuilder()
        .setDataSource(std::make_unique<CsvDataProvider>("non_existent.csv"))
        .setFormatter(std::make_unique<PlainTextFormatter>())
        .setExportStrategy(std::make_unique<ConsoleExportStrategy>())
        .build();
    
    auto result3 = report3->generate();
    if (!result3.success) {
        std::cout << "The error was handled correctly: " 
                  << result3.error_message.value_or("unknown") << "\n";
    } else {
        std::cout << "An error was expected, but the report was generated.\n";
    }
    
    // Очистка
    std::remove("employees.csv");
    
    std::cout << "\n=== All tests completed===\n";
    return 0;
}
