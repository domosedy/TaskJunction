#ifndef DATA_HANDLER_HPP_
#define DATA_HANDLER_HPP_

#include "query.hpp"
#include <QString>
#include <memory>
#include <optional>
#include <utility>

std::optional<query_type> parseData(const QString &data);

#endif