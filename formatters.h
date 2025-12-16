#ifndef FORMATTERS_H
#define FORMATTERS_H

#include "interfaces.h"
#include <iomanip>
#include <sstream>

// HTML форматировщик
class HtmlFormatter : public Formatter {
public:
    std::string format(const DataTable& data) override {
        if (data.empty()) return "<p>No data</p>";
        
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
    
    std::string getFormatName() const override {
        return "HTML";
    }
};

// Текстовый форматировщик
class PlainTextFormatter : public Formatter {
public:
    std::string format(const DataTable& data) override {
        if (data.empty()) return "No data\n";
        
        std::ostringstream text;
        text << "Report\n";
        text << std::string(40, '=') << "\n\n";
        
        // Определяем ширину колонок
        std::map<std::string, size_t> col_widths;
        for (const auto& row : data) {
            for (const auto& [key, value] : row) {
                col_widths[key] = std::max(col_widths[key], key.length());
                
                // Получаем строковое представление значения
                std::string str_val = std::visit([](auto&& v) -> std::string {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        return v;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        return v ? "true" : "false";
                    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
                        return std::to_string(v);
                    }
                }, value);
                
                col_widths[key] = std::max(col_widths[key], str_val.length());
            }
        }
        
        // Выводим заголовки
        for (const auto& [key, width] : col_widths) {
            text << std::left << std::setw(width + 2) << key;
        }
        text << "\n" << std::string(std::accumulate(
            col_widths.begin(), col_widths.end(), 0,
            [](int sum, const auto& p) { return sum + p.second + 2; }
        ), '-') << "\n";
        
        // Выводим данные
        for (const auto& row : data) {
            for (const auto& [key, width] : col_widths) {
                auto it = row.find(key);
                if (it != row.end()) {
                    std::visit([&](auto&& v) { 
                        text << std::left << std::setw(width + 2) << v; 
                    }, it->second);
                } else {
                    text << std::left << std::setw(width + 2) << "";
                }
            }
            text << "\n";
        }
        
        return text.str();
    }
    
    std::string getFormatName() const override {
        return "Plain Text";
    }
};

// Markdown форматировщик
class MarkdownFormatter : public Formatter {
public:
    std::string format(const DataTable& data) override {
        if (data.empty()) return "*No data*";
        
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
    
    std::string getFormatName() const override {
        return "Markdown";
    }
};

#endif
