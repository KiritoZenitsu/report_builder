#ifndef REPORT_BUILDER_EXPORT_STRATEGIES_H
#define REPORT_BUILDER_EXPORT_STRATEGIES_H

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "report_builder/interfaces.h"

namespace report_builder {

    namespace fs = std::filesystem;

    // Экспорт в файл
    class TFileExportStrategy: public IExportStrategy {
    private:
        std::string Directory;

    public:
        TFileExportStrategy(std::string dir = "./reports/")
            : Directory(std::move(dir)) {
            // Создаем директорию рекурсивно (кроссплатформенно)
            fs::create_directories(Directory);
        }

        bool ExportData(const std::string& formattedData) override {
            // Используем std::filesystem для кроссплатформенных путей
            fs::path filepath = fs::path(Directory) / ("report_" + std::to_string(time(nullptr)) + ".html");

            std::ofstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Cannot write to file: " << filepath.string() << "\n";
                return false;
            }

            file << formattedData;
            file.close();

            std::cout << "Report saved to: " << filepath.string() << "\n";
            return true;
        }

        std::string GetMethodName() const override {
            return "File export to " + Directory;
        }
    };

    // Экспорт в консоль (для тестирования)
    class TConsoleExportStrategy: public IExportStrategy {
    public:
        bool ExportData(const std::string& formattedData) override {
            std::cout << "\n=== REPORT OUTPUT ===\n";
            std::cout << formattedData << "\n";
            std::cout << "=== END REPORT ===\n\n";
            return true;
        }

        std::string GetMethodName() const override {
            return "Console output";
        }
    };

    // Мок для email экспорта (для тестов)
    class TEmailExportStrategy: public IExportStrategy {
    private:
        std::string Recipient;

    public:
        TEmailExportStrategy(std::string to)
            : Recipient(std::move(to)) {
        }

        bool ExportData(const std::string& formattedData) override {
            std::cout << "[MOCK] Email sent to: " << Recipient << "\n";
            std::cout << "[MOCK] Subject: Report generated at " << time(nullptr) << "\n";
            std::cout << "[MOCK] Body length: " << formattedData.length() << " chars\n";
            return true;
        }

        std::string GetMethodName() const override {
            return "Email to " + Recipient;
        }
    };
} // namespace report_builder

#endif
