#ifndef EXPORT_STRATEGIES_H
#define EXPORT_STRATEGIES_H

#include "interfaces.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <filesystem>  // Кроссплатформенно

namespace fs = std::filesystem;

// Экспорт в файл
class FileExportStrategy : public ExportStrategy {
    std::string directory;
    
public:
    FileExportStrategy(std::string dir = "./reports/") 
        : directory(std::move(dir)) {
        // Создаем директорию рекурсивно (кроссплатформенно)
        fs::create_directories(directory);
    }
    
    bool exportData(const std::string& formattedData) override {
        // Используем std::filesystem для кроссплатформенных путей
        fs::path filepath = fs::path(directory) / ("report_" + std::to_string(time(nullptr)) + ".html");
        
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
    
    std::string getMethodName() const override {
        return "File export to " + directory;
    }
};

// Экспорт в консоль (для тестирования)
class ConsoleExportStrategy : public ExportStrategy {
public:
    bool exportData(const std::string& formattedData) override {
        std::cout << "\n=== REPORT OUTPUT ===\n";
        std::cout << formattedData << "\n";
        std::cout << "=== END REPORT ===\n\n";
        return true;
    }
    
    std::string getMethodName() const override {
        return "Console output";
    }
};

// Мок для email экспорта (для тестов)
class EmailExportStrategy : public ExportStrategy {
    std::string recipient;
    
public:
    EmailExportStrategy(std::string to) : recipient(std::move(to)) {}
    
    bool exportData(const std::string& formattedData) override {
        std::cout << "[MOCK] Email sent to: " << recipient << "\n";
        std::cout << "[MOCK] Subject: Report generated at " << time(nullptr) << "\n";
        std::cout << "[MOCK] Body length: " << formattedData.length() << " chars\n";
        return true;
    }
    
    std::string getMethodName() const override {
        return "Email to " + recipient;
    }
};

#endif
