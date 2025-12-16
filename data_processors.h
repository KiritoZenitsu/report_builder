#ifndef DATA_PROCESSORS_H
#define DATA_PROCESSORS_H

#include <type_traits>
#include "interfaces.h"
#include <algorithm>
#include <numeric>
#include <functional>
#include <sstream>

// Функция для преобразования любого значения в строку
template<typename T>
std::string valueToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Специализация для bool
template<>
std::string valueToString<bool>(const bool& value) {
    return value ? "true" : "false";
}

// Фильтр данных
class FilterProcessor : public DataProcessor {
    std::function<bool(const DataRow&)> filter_func;
    std::string condition_desc;
    
public:
    FilterProcessor(std::function<bool(const DataRow&)> func, std::string desc = "")
        : filter_func(std::move(func)), condition_desc(std::move(desc)) {}
    
    OperationResult process(DataTable data) override {
        DataTable filtered;
        std::copy_if(data.begin(), data.end(), 
                     std::back_inserter(filtered),
                     filter_func);
        
        return OperationResult::Ok(filtered);
    }
    
    std::string getDescription() const override {
        return "Filter" + (condition_desc.empty() ? "" : " (" + condition_desc + ")");
    }
};

// Сортировщик данных
class SortProcessor : public DataProcessor {
    std::string sort_field;
    bool ascending;
    
public:
    SortProcessor(std::string field, bool asc = true)
        : sort_field(std::move(field)), ascending(asc) {}
    
    OperationResult process(DataTable data) override {
        std::sort(data.begin(), data.end(),
            [this](const DataRow& a, const DataRow& b) {
                auto it_a = a.find(sort_field);
                auto it_b = b.find(sort_field);
                
                if (it_a == a.end() || it_b == b.end()) return false;
                
                // Используем лямбду для преобразования значения в строку
                auto getStringValue = [](const DataValue& val) -> std::string {
                    return std::visit([](auto&& v) -> std::string {
                        using T = std::decay_t<decltype(v)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            return v;
                        } else if constexpr (std::is_same_v<T, bool>) {
                            return v ? "true" : "false";
                        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
                            return std::to_string(v);
                        }
                    }, val);
                };
                
                std::string str_a = getStringValue(it_a->second);
                std::string str_b = getStringValue(it_b->second);
                
                return ascending ? str_a < str_b : str_a > str_b;
            });
        
        return OperationResult::Ok(data);
    }
    
    std::string getDescription() const override {
        return "Sort by " + sort_field + " (" + (ascending ? "asc" : "desc") + ")";
    }
};

// Агрегатор данных
class AggregationProcessor : public DataProcessor {
    std::string field;
    std::string operation; // "sum", "avg", "count"
    
public:
    AggregationProcessor(std::string fld, std::string op)
        : field(std::move(fld)), operation(std::move(op)) {}
    
    OperationResult process(DataTable data) override {
        if (data.empty()) {
            return OperationResult::Ok(data);
        }
        
        DataRow summary_row;
        
        if (operation == "sum" || operation == "avg") {
            double total = 0.0;
            int count = 0;
            
            for (const auto& row : data) {
                auto it = row.find(field);
                if (it != row.end()) {
                    if (std::holds_alternative<int>(it->second)) {
                        total += std::get<int>(it->second);
                        count++;
                    } else if (std::holds_alternative<double>(it->second)) {
                        total += std::get<double>(it->second);
                        count++;
                    }
                }
            }
            
            if (operation == "sum") {
                summary_row[field + "_sum"] = total;
                summary_row["count"] = count;
            } else { // avg
                summary_row[field + "_avg"] = (count > 0) ? total / count : 0.0;
                summary_row["count"] = count;
            }
        } else if (operation == "count") {
            summary_row["total_count"] = static_cast<int>(data.size());
        }
        
        data.push_back(summary_row);
        return OperationResult::Ok(data);
    }
    
    std::string getDescription() const override {
        return operation + " of " + field;
    }
};

#endif
