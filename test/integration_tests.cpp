#include <gtest/gtest.h>
#include <fstream>

#include "report_builder/report_builder.h"
#include "report_builder/report_factories.h"

using namespace report_builder;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем тестовый CSV файл
        std::ofstream csvFile("test_integration.csv");
        csvFile << "product,units,price,region\n";
        csvFile << "Laptop,15,999.99,North\n";
        csvFile << "Phone,32,699.99,South\n";
        csvFile << "Tablet,21,449.99,North\n";
        csvFile << "Monitor,8,299.99,East\n";
        csvFile.close();
    }

    void TearDown() override {
        std::remove("test_integration.csv");
    }
};

TEST_F(IntegrationTest, ReportBuilderCreatesValidReport) {
    auto report = TReportBuilder()
        .SetDataSource(std::make_unique<TCsvDataProvider>("test_integration.csv"))
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

    EXPECT_NE(report, nullptr);
    
    // Захватываем вывод консоли
    testing::internal::CaptureStdout();
    auto result = report->Generate();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(result.Success);
    EXPECT_FALSE(output.empty());
}

TEST_F(IntegrationTest, FinanceReportFactoryCreatesValidReport) {
    auto factory = std::make_unique<TFinanceReportFactory>();
    auto report = factory->CreateReport();
    
    EXPECT_NE(report, nullptr);
    
    testing::internal::CaptureStdout();
    auto result = report->Generate();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(result.Success);
}

TEST_F(IntegrationTest, SalesReportFactoryCreatesValidReport) {
    auto factory = std::make_unique<TSalesReportFactory>();
    auto report = factory->CreateReport();
    
    EXPECT_NE(report, nullptr);
    
    testing::internal::CaptureStdout();
    auto result = report->Generate();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(result.Success);
}

TEST(IntegrationTest, ReportHandlesMissingFileGracefully) {
    auto report = TReportBuilder()
        .SetDataSource(std::make_unique<TCsvDataProvider>("non_existent_file.csv"))
        .SetFormatter(std::make_unique<TPlainTextFormatter>())
        .SetExportStrategy(std::make_unique<TConsoleExportStrategy>())
        .Build();

    testing::internal::CaptureStdout();
    auto result = report->Generate();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_FALSE(result.Success);
    EXPECT_TRUE(result.ErrorMessage.has_value());
    EXPECT_NE(result.ErrorMessage.value().find("Cannot open file"), std::string::npos);
}
