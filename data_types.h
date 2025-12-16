#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <memory>
#include <optional>

// Тип для ячейки данных
using DataValue = std::variant<std::string, int, double, bool>;

// Строка данных - пары "поле-значение"
using DataRow = std::map<std::string, DataValue>;

// Таблица данных - вектор строк
using DataTable = std::vector<DataRow>;

// Результат операции
struct OperationResult {
    bool success;
    std::optional<std::string> error_message;
    DataTable data;
    
    OperationResult(bool s, const std::string& msg = "", DataTable d = {})
        : success(s), error_message(msg), data(std::move(d)) {}
    
    static OperationResult Ok(DataTable d) { 
        return {true, "", std::move(d)}; 
    }
    
    static OperationResult Error(const std::string& msg) { 
        return {false, msg, {}}; 
    }
};

#endif
