#ifndef LIST_HPP_
#define LIST_HPP_

#include <QString>
#include "listmodel.hpp"

class List {

public:
    List(QString name = "list", QString description = "", QObject *parent = nullptr);

    QString get_name() const;
    QString get_description() const;
    ListModel* get_model() const;

private:
    QString name;
    QString description;
    ListModel* list_model;    
};

#endif  // LIST_HPP_
