#ifndef REPORT_BUILDER_DATA_PROCESSORS_H
#define REPORT_BUILDER_DATA_PROCESSORS_H

#include <algorithm>
#include <functional>
#include <numeric>
#include <sstream>
#include <type_traits>

#include "report_builder/interfaces.h"

namespace report_builder {
    // Фильтр данных
    class TFilterProcessor: public IDataProcessor {
    private:
        std::function<bool(const DataRow&)> FilterFunc;
        std::string ConditionDesc;

    public:
        TFilterProcessor(std::function<bool(const DataRow&)> func, std::string desc = "")
            : FilterFunc(std::move(func))
            , ConditionDesc(std::move(desc)) {
        }

        TOperationResult Process(DataTable data) override {
            DataTable filtered;
            std::copy_if(data.begin(), data.end(), std::back_inserter(filtered), FilterFunc);
            return TOperationResult::Ok(filtered);
        }

        std::string GetDescription() const override {
            return "Filter" + (ConditionDesc.empty() ? "" : " (" + ConditionDesc + ")");
        }
    };

    // Сортировщик данных
    class TSortProcessor: public IDataProcessor {
    private:
        std::string SortField;
        bool Ascending;

    public:
        TSortProcessor(std::string field, bool asc = true)
            : SortField(std::move(field))
            , Ascending(asc) {
        }

        TOperationResult Process(DataTable data) override {
            std::sort(data.begin(), data.end(),
                      [this](const DataRow& a, const DataRow& b) {
                          auto itA = a.find(SortField);
                          auto itB = b.find(SortField);

                          if (itA == a.end() || itB == b.end()) {
                              return false;
                          }

                          // Используем лямбду для преобразования значения в строку
                          auto getStringValue = [](const DataValue& val) -> std::string {
                              return std::visit(
                                  [](auto&& v) -> std::string {
                                      using T = std::decay_t<decltype(v)>;
                                      if constexpr (std::is_same_v<T, std::string>) {
                                          return v;
                                      } else if constexpr (std::is_same_v<T, bool>) {
                                          return v ? "true" : "false";
                                      } else if constexpr (std::is_same_v<T, int> ||
                                                           std::is_same_v<T, double>) {
                                          return std::to_string(v);
                                      }
                                  },
                                  val);
                          };

                          std::string strA = getStringValue(itA->second);
                          std::string strB = getStringValue(itB->second);

                          return Ascending ? strA < strB : strA > strB;
                      });

            return TOperationResult::Ok(data);
        }

        std::string GetDescription() const override {
            return "Sort by " + SortField + " (" + (Ascending ? "asc" : "desc") + ")";
        }
    };

    // Агрегатор данных
    class TAggregationProcessor: public IDataProcessor {
    private:
        std::string Field;
        std::string Operation; // sum, avg, count

    public:
        TAggregationProcessor(std::string field, std::string op)
            : Field(std::move(field))
            , Operation(std::move(op)) {
        }

        TOperationResult Process(DataTable data) override {
            if (data.empty()) {
                return TOperationResult::Ok(data);
            }

            DataTable resultTable;
            DataRow summaryRow;

            if (Operation == "sum" || Operation == "avg") {
                double total = 0.0;
                int count = 0;

                for (const auto& row : data) {
                    auto it = row.find(Field);
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

                // Доп. проверка: были ли найдены данные
                if (count == 0) {
                    // Если поле не найдено ни в одной строке, возвращаем ошибку
                    return TOperationResult::Error("Field '" + Field + "' not found in data for aggregation");
                }

                if (Operation == "sum") {
                    summaryRow["field"] = Field;
                    summaryRow["operation"] = std::string("sum");
                    summaryRow["value"] = total;
                    summaryRow["count"] = count;
                } else { // avg
                    summaryRow["field"] = Field;
                    summaryRow["operation"] = std::string("average");
                    summaryRow["value"] = total / count; // count > 0 гарантировано
                    summaryRow["count"] = count;
                }
            } else if (Operation == "count") {
                summaryRow["field"] = Field;
                summaryRow["operation"] = std::string("count");
                summaryRow["value"] = static_cast<int>(data.size());
            }

            resultTable.push_back(summaryRow);
            return TOperationResult::Ok(resultTable);
        }

        std::string GetDescription() const override {
            return Operation + " of " + Field;
        }
    };

    // Новое: Процессор для множественной агрегации
    class TMultiAggregationProcessor: public IDataProcessor {
    private:
        std::vector<std::pair<std::string, std::string>> Aggregations; // поле -> операция

    public:
        TMultiAggregationProcessor(std::vector<std::pair<std::string, std::string>> aggregations)
            : Aggregations(std::move(aggregations)) {
        }

        TOperationResult Process(DataTable data) override {
            if (data.empty()) {
                return TOperationResult::Ok(data);
            }

            DataTable resultTable;

            for (const auto& [field, operation] : Aggregations) {
                DataRow summaryRow;

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

                    if (count == 0) {
                        // Пропускаем поля, которых нет в данных
                        continue;
                    }

                    if (operation == "sum") {
                        summaryRow["field"] = field;
                        summaryRow["operation"] = std::string("sum");
                        summaryRow["value"] = total;
                        summaryRow["count"] = count;
                    } else { // avg
                        summaryRow["field"] = field;
                        summaryRow["operation"] = std::string("average");
                        summaryRow["value"] = total / count;
                        summaryRow["count"] = count;
                    }
                } else if (operation == "count") {
                    summaryRow["field"] = field;
                    summaryRow["operation"] = std::string("count");
                    summaryRow["value"] = static_cast<int>(data.size());
                }

                resultTable.push_back(summaryRow);
            }

            return TOperationResult::Ok(resultTable);
        }

        std::string GetDescription() const override {
            std::string desc = "Multi Aggregation: ";
            for (const auto& [field, op] : Aggregations) {
                desc += field + "(" + op + ") ";
            }
            return desc;
        }
    };
} // namespace report_builder

#endif
