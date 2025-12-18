#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "report_builder/data_types.h"
#include "report_builder/interfaces.h"
#include "report_builder/data_providers.h"
#include "report_builder/data_processors.h"
#include "report_builder/formatters.h"
#include "report_builder/export_strategies.h"

namespace fs = std::filesystem;
using namespace report_builder;

class DataProvidersTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем тестовый CSV файл
        std::ofstream csvFile("test_data.csv");
        csvFile << "id,name,value\n";
        csvFile << "1,Item1,10.5\n";
        csvFile << "2,Item2,20.3\n";
        csvFile.close();
    }

    void TearDown() override {
        // Удаляем тестовые файлы
        std::remove("test_data.csv");
        std::remove("test.csv");
    }
};

TEST_F(DataProvidersTest, InMemoryDataProviderWorks) {
    DataTable testData = {
        {{"id", 1}, {"name", std::string("Test1")}, {"value", 100.5}},
        {{"id", 2}, {"name", std::string("Test2")}, {"value", 200.3}},
    };

    auto provider = std::make_unique<TInMemoryDataProvider>(testData);
    auto result = provider->FetchData();

    EXPECT_TRUE(result.Success);
    EXPECT_EQ(result.Data.size(), 2);
    EXPECT_EQ(std::get<std::string>(result.Data[0]["name"]), "Test1");
    EXPECT_EQ(std::get<int>(result.Data[0]["id"]), 1);
}

TEST_F(DataProvidersTest, CsvDataProviderWorks) {
    auto provider = std::make_unique<TCsvDataProvider>("test_data.csv");
    auto result = provider->FetchData();

    EXPECT_TRUE(result.Success);
    EXPECT_EQ(result.Data.size(), 2);
    
    if (result.Data.size() > 0) {
        EXPECT_TRUE(std::holds_alternative<std::string>(result.Data[0]["name"]));
        EXPECT_TRUE(std::holds_alternative<int>(result.Data[0]["id"]));
        EXPECT_TRUE(std::holds_alternative<double>(result.Data[0]["value"]));
    }
}

TEST(DataProcessorsTest, FilterProcessorWorks) {
    DataTable testData = {
        {{"id", 1}, {"age", 25}, {"active", true}},
        {{"id", 2}, {"age", 17}, {"active", true}},
        {{"id", 3}, {"age", 30}, {"active", false}},
    };

    auto filter = std::make_unique<TFilterProcessor>(
        [](const DataRow& row) {
            auto it = row.find("age");
            if (it != row.end() && std::holds_alternative<int>(it->second)) {
                return std::get<int>(it->second) >= 18;
            }
            return false;
        },
        "age >= 18");

    auto result = filter->Process(testData);
    
    EXPECT_TRUE(result.Success);
    EXPECT_EQ(result.Data.size(), 2);  // Только 25 и 30
}

TEST(DataProcessorsTest, SortProcessorWorks) {
    DataTable testData = {
        {{"id", 1}, {"age", 30}},
        {{"id", 2}, {"age", 20}},
        {{"id", 3}, {"age", 40}},
    };

    auto sorter = std::make_unique<TSortProcessor>("age", false); // по убыванию
    auto result = sorter->Process(testData);
    
    EXPECT_TRUE(result.Success);
    EXPECT_EQ(result.Data.size(), 3);
    EXPECT_EQ(std::get<int>(result.Data[0]["age"]), 40); // Самый большой первый
    EXPECT_EQ(std::get<int>(result.Data[2]["age"]), 20); // Самый маленький последний
}

TEST(DataProcessorsTest, AggregationProcessorWorks) {
    DataTable testData = {
        {{"id", 1}, {"salary", 50000}},
        {{"id", 2}, {"salary", 60000}},
        {{"id", 3}, {"salary", 70000}},
    };

    auto aggregator = std::make_unique<TAggregationProcessor>("salary", "avg");
    auto result = aggregator->Process(testData);
    
    EXPECT_TRUE(result.Success);
    EXPECT_EQ(result.Data.size(), 1); // Только агрегированная строка
    
    auto& aggRow = result.Data[0];
    EXPECT_EQ(std::get<std::string>(aggRow["field"]), "salary");
    EXPECT_EQ(std::get<std::string>(aggRow["operation"]), "average");
    EXPECT_DOUBLE_EQ(std::get<double>(aggRow["value"]), 60000.0); // (50000+60000+70000)/3
}

TEST(FormattersTest, HtmlFormatterWorks) {
    DataTable testData = {
        {{"id", 1}, {"name", std::string("Item1")}, {"price", 100.50}},
        {{"id", 2}, {"name", std::string("Item2")}, {"price", 200.00}},
    };

    auto formatter = std::make_unique<THtmlFormatter>();
    std::string html = formatter->Format(testData);
    
    EXPECT_FALSE(html.empty());
    EXPECT_NE(html.find("<table>"), std::string::npos);
    EXPECT_NE(html.find("Item1"), std::string::npos);
    EXPECT_NE(html.find("Item2"), std::string::npos);
}

TEST(FormattersTest, MarkdownFormatterWorks) {
    DataTable testData = {
        {{"id", 1}, {"name", std::string("Item1")}, {"price", 100.50}},
        {{"id", 2}, {"name", std::string("Item2")}, {"price", 200.00}},
    };

    auto formatter = std::make_unique<TMarkdownFormatter>();
    std::string md = formatter->Format(testData);
    
    EXPECT_FALSE(md.empty());
    EXPECT_NE(md.find("| id |"), std::string::npos);
    EXPECT_NE(md.find("| --- |"), std::string::npos);
}

TEST(ExportStrategiesTest, ConsoleExportStrategyWorks) {
    testing::internal::CaptureStdout();
    
    auto exporter = std::make_unique<TConsoleExportStrategy>();
    bool result = exporter->ExportData("Test output");
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(result);
    EXPECT_NE(output.find("=== REPORT OUTPUT ==="), std::string::npos);
    EXPECT_NE(output.find("Test output"), std::string::npos);
}

TEST(ExportStrategiesTest, FileExportStrategyWorks) {
    // Создаем временную директорию для тестов
    fs::create_directories("test_output");
    
    auto exporter = std::make_unique<TFileExportStrategy>("test_output/");
    bool result = exporter->ExportData("<h1>Test Report</h1>");
    
    EXPECT_TRUE(result);
    
    // Проверяем, что файл создан
    bool fileExists = false;
    for (const auto& entry : fs::directory_iterator("test_output")) {
        if (entry.path().extension() == ".html") {
            fileExists = true;
            break;
        }
    }
    
    EXPECT_TRUE(fileExists);
    
    // Очистка
    fs::remove_all("test_output");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
