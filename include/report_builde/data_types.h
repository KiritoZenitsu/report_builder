#ifndef REPORT_BUILDER_DATA_TYPES_H
#define REPORT_BUILDER_DATA_TYPES_H

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace report_builder {
    // Тип для ячейки данных
    using DataValue = std::variant<std::string, int, double, bool>;

    // Строка данных - пары "поле-значение"
    using DataRow = std::map<std::string, DataValue>;

    // Таблица данных - вектор строк
    using DataTable = std::vector<DataRow>;

    // Результат операции
    struct TOperationResult {
        bool Success;
        std::optional<std::string> ErrorMessage;
        DataTable Data;

        TOperationResult(bool success, const std::string& message = "", DataTable data = {})
            : Success(success)
            , ErrorMessage(message)
            , Data(std::move(data)) {
        }

        static TOperationResult Ok(DataTable data) {
            return {true, "", std::move(data)};
        }

        static TOperationResult Error(const std::string& message) {
            return {false, message, {}};
        }
    };
} // namespace report_builder

#endif
