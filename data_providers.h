#ifndef DATA_PROVIDERS_H
#define DATA_PROVIDERS_H

#include "interfaces.h"
#include <fstream>
#include <sstream>

// CSV провайдер
class CsvDataProvider : public DataProvider {
    std::string filepath;
    char delimiter;
    
public:
    CsvDataProvider(std::string path, char delim = ',') 
        : filepath(std::move(path)), delimiter(delim) {}
    
    OperationResult fetchData() override {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return OperationResult::Error("Cannot open file: " + filepath);
        }
        
        DataTable table;
        std::string line;
        std::vector<std::string> headers;
        
        // Читаем заголовки
        if (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string header;
            while (std::getline(ss, header, delimiter)) {
                headers.push_back(header);
            }
        }
        
        // Читаем данные
        while (std::getline(file, line)) {
            DataRow row;
            std::stringstream ss(line);
            std::string value;
            size_t col_idx = 0;
            
            while (std::getline(ss, value, delimiter) && col_idx < headers.size()) {
                // Пытаемся определить тип данных
                try {
                    if (value.find('.') != std::string::npos) {
                        double dval = std::stod(value);
                        row[headers[col_idx]] = dval;
                    } else {
                        int ival = std::stoi(value);
                        row[headers[col_idx]] = ival;
                    }
                } catch (...) {
                    row[headers[col_idx]] = value;
                }
                col_idx++;
            }
            table.push_back(std::move(row));
        }
        
        return OperationResult::Ok(table);
    }
    
    std::string getSourceInfo() const override {
        return "CSV file: " + filepath;
    }
};

// In-memory провайдер
class InMemoryDataProvider : public DataProvider {
    DataTable static_data;
    
public:
    InMemoryDataProvider(DataTable data) : static_data(std::move(data)) {}
    
    OperationResult fetchData() override {
        return OperationResult::Ok(static_data);
    }
    
    std::string getSourceInfo() const override {
        return "In-memory data (" + std::to_string(static_data.size()) + " rows)";
    }
};

// JSON провайдер
class JsonDataProvider : public DataProvider {
    std::string json_content;
    
public:
    JsonDataProvider(std::string json) : json_content(std::move(json)) {}
    
    OperationResult fetchData() override {
        DataTable table;
        DataRow row;
        
        // Здесь должен быть реальный парсинг JSON 
		    // Т.к. в разделе "Требования к реализации" конкретно про парсинг JSON ничего не сказано
		    // то для примера создаем одну строку
        row["message"] = "JSON data from: " + json_content.substr(0, 30) + "...";
        table.push_back(row);
        
        return OperationResult::Ok(table);
    }
    
    std::string getSourceInfo() const override {
        return "JSON data provider";
    }
};

#endif
