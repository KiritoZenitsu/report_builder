#ifndef REPORT_BUILDER_DATA_PROVIDERS_H
#define REPORT_BUILDER_DATA_PROVIDERS_H

#include <fstream>
#include <sstream>

#include "report_builder/interfaces.h"

namespace report_builder {
    // CSV провайдер
    class TCsvDataProvider: public IDataProvider {
    private:
        std::string Filepath;
        char Delimiter;

    public:
        TCsvDataProvider(std::string path, char delim = ',')
            : Filepath(std::move(path))
            , Delimiter(delim) {
        }

        TOperationResult FetchData() override {
            std::ifstream file(Filepath);
            if (!file.is_open()) {
                return TOperationResult::Error("Cannot open file: " + Filepath);
            }

            DataTable table;
            std::string line;
            std::vector<std::string> headers;

            // Читаем заголовки
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string header;
                while (std::getline(ss, header, Delimiter)) {
                    headers.push_back(header);
                }
            }

            // Читаем данные
            while (std::getline(file, line)) {
                DataRow row;
                std::stringstream ss(line);
                std::string value;
                size_t colIdx = 0;

                while (std::getline(ss, value, Delimiter) && colIdx < headers.size()) {
                    // Пытаемся определить тип данных
                    try {
                        if (value.find('.') != std::string::npos) {
                            double dval = std::stod(value);
                            row[headers[colIdx]] = dval;
                        } else {
                            int ival = std::stoi(value);
                            row[headers[colIdx]] = ival;
                        }
                    } catch (...) {
                        row[headers[colIdx]] = value;
                    }
                    colIdx++;
                }
                table.push_back(std::move(row));
            }

            return TOperationResult::Ok(table);
        }

        std::string GetSourceInfo() const override {
            return "CSV file: " + Filepath;
        }
    };

    // In-memory провайдер
    class TInMemoryDataProvider: public IDataProvider {
    private:
        DataTable StaticData;

    public:
        TInMemoryDataProvider(DataTable data)
            : StaticData(std::move(data)) {
        }

        TOperationResult FetchData() override {
            return TOperationResult::Ok(StaticData);
        }

        std::string GetSourceInfo() const override {
            return "In-memory data (" + std::to_string(StaticData.size()) + " rows)";
        }
    };

    // JSON провайдер
    class TJsonDataProvider: public IDataProvider {
    private:
        std::string JsonContent;

    public:
        TJsonDataProvider(std::string json)
            : JsonContent(std::move(json)) {
        }

        TOperationResult FetchData() override {
            DataTable table;
            DataRow row;

            // Здесь должен быть реальный парсинг JSON
            // Т.к. в разделе "Требования к реализации" конкретно про парсинг JSON ничего не сказано
            // то для примера создаем одну строку
            row["message"] = "JSON data from: " + JsonContent.substr(0, 30) + "...";
            table.push_back(row);

            return TOperationResult::Ok(table);
        }

        std::string GetSourceInfo() const override {
            return "JSON data provider";
        }
    };
} // namespace report_builder

#endif
