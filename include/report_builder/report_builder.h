#ifndef REPORT_BUILDER_REPORT_BUILDER_H
#define REPORT_BUILDER_REPORT_BUILDER_H

#include <memory>

#include "report_builder/data_processors.h"
#include "report_builder/data_providers.h"
#include "report_builder/export_strategies.h"
#include "report_builder/formatters.h"
#include "report_builder/interfaces.h"

namespace report_builder {

    class TReportBuilder {
    private:
        std::unique_ptr<IDataProvider> DataSource;
        std::vector<std::unique_ptr<IDataProcessor>> Processors;
        std::unique_ptr<IFormatter> Formatter;
        std::unique_ptr<IExportStrategy> Exporter;

    public:
        TReportBuilder() = default;

        TReportBuilder& SetDataSource(std::unique_ptr<IDataProvider> provider) {
            DataSource = std::move(provider);
            return *this;
        }

        TReportBuilder& AddProcessor(std::unique_ptr<IDataProcessor> processor) {
            Processors.push_back(std::move(processor));
            return *this;
        }

        TReportBuilder& SetFormatter(std::unique_ptr<IFormatter> fmt) {
            Formatter = std::move(fmt);
            return *this;
        }

        TReportBuilder& SetExportStrategy(std::unique_ptr<IExportStrategy> exp) {
            Exporter = std::move(exp);
            return *this;
        }

        std::unique_ptr<TReport> Build() {
            if (!DataSource || !Formatter || !Exporter) {
                throw std::runtime_error("Incomplete report configuration");
            }

            return std::make_unique<TReport>(std::move(DataSource), std::move(Processors),
                                             std::move(Formatter), std::move(Exporter));
        }
    };

} // namespace report_builder

#endif
