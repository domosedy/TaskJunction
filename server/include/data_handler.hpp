#ifndef DATA_HANDLER_HPP_
#define DATA_HANDLER_HPP_

#include <QString>
#include <memory>
#include <utility>
#include "query.hpp"

std::shared_ptr<query> parseData(const QString &data);

#endif