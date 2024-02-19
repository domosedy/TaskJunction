#include "list.hpp"

List::List(QString name, QString description, QObject *parent)
    : name(std::move(name)),
      description(std::move(description)),
      list_model(new ListModel(parent)) {
        qDebug() << "List created!";
}

QString List::get_name() const {
    return name;
}

QString List::get_description() const {
    return description;
}

ListModel *List::get_model() const {
    return list_model;
}
