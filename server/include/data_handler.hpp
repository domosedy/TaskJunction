#ifndef DATA_HANDLER_HPP_
#define DATA_HANDLER_HPP_

#include <QString>
#include <memory>
#include <utility>
#include <optional>
#include "query.hpp"

std::optional<query_type> parseData(const QString &data);

#endif