#ifndef REPORT_BUILDER_FORMATTERS_H
#define REPORT_BUILDER_FORMATTERS_H

#include <iomanip>
#include <sstream>

#include "report_builder/interfaces.h"

namespace report_builder {
    // HTML форматировщик
    class THtmlFormatter: public IFormatter {
    public:
        std::string Format(const DataTable& data) override {
            if (data.empty()) {
                return "<p>No data</p>";
            }

            std::ostringstream html;
            html << "<!DOCTYPE html>\n<html>\n<head>\n";
            html << "  <style>\n";
            html << "    table { border-collapse: collapse; width: 100%; }\n";
            html << "    th, td { border: 1px solid #ddd; padding: 8px; }\n";
            html << "    th { background-color: #f2f2f2; }\n";
            html << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
            html << "  </style>\n</head>\n<body>\n";
            html << "  <h2>Report</h2>\n";
            html << "  <table>\n    <tr>\n";

            // Заголовки
            for (const auto& [key, _] : data[0]) {
                html << "      <th>" << key << "</th>\n";
            }
            html << "    </tr>\n";

            // Данные
            for (const auto& row : data) {
                html << "    <tr>\n";
                for (const auto& [_, value] : row) {
                    html << "      <td>";
                    std::visit([&](auto&& v) { html << v; }, value);
                    html << "</td>\n";
                }
                html << "    </tr>\n";
            }

            html << "  </table>\n</body>\n</html>";
            return html.str();
        }

        std::string GetFormatName() const override {
            return "HTML";
        }
    };

    // Текстовый форматировщик
    class TPlainTextFormatter: public IFormatter {
    public:
        std::string Format(const DataTable& data) override {
            if (data.empty()) {
                return "No data\n";
            }

            std::ostringstream text;
            text << "Report\n";
            text << std::string(40, '=') << "\n\n";

            // Определяем ширину колонок
            std::map<std::string, size_t> colWidths;
            for (const auto& row : data) {
                for (const auto& [key, value] : row) {
                    colWidths[key] = std::max(colWidths[key], key.length());

                    // Получаем строковое представление значения
                    std::string strVal = std::visit(
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
                        value);

                    colWidths[key] = std::max(colWidths[key], strVal.length());
                }
            }

            // Выводим заголовки
            for (const auto& [key, width] : colWidths) {
                text << std::left << std::setw(width + 2) << key;
            }
            text << "\n"
                 << std::string(std::accumulate(colWidths.begin(), colWidths.end(), 0,
                                                [](int sum, const auto& p) { return sum + p.second + 2; }),
                                '-')
                 << "\n";

            // Выводим данные
            for (const auto& row : data) {
                for (const auto& [key, width] : colWidths) {
                    auto it = row.find(key);
                    if (it != row.end()) {
                        std::visit([&](auto&& v) { text << std::left << std::setw(width + 2) << v; }, it->second);
                    } else {
                        text << std::left << std::setw(width + 2) << "";
                    }
                }
                text << "\n";
            }

            return text.str();
        }

        std::string GetFormatName() const override {
            return "Plain Text";
        }
    };

    // Markdown форматировщик
    class TMarkdownFormatter: public IFormatter {
    public:
        std::string Format(const DataTable& data) override {
            if (data.empty()) {
                return "*No data*";
            }

            std::ostringstream md;
            md << "# Report\n\n";

            // Заголовки
            for (const auto& [key, _] : data[0]) {
                md << "| " << key << " ";
            }
            md << "|\n";

            // Разделитель
            for (size_t i = 0; i < data[0].size(); i++) {
                md << "| --- ";
            }
            md << "|\n";

            // Данные
            for (const auto& row : data) {
                md << "| ";
                for (const auto& [_, value] : row) {
                    std::visit([&](auto&& v) { md << v << " | "; }, value);
                }
                md << "\n";
            }

            return md.str();
        }

        std::string GetFormatName() const override {
            return "Markdown";
        }
    };
} // namespace report_builder

#endif
