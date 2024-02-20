#ifndef QUERY_HPP_
#define QUERY_HPP_

#include <string>

struct query {
    virtual ~query() = default;
    virtual void execute() = 0;
};

struct update_query : query {
    std::size_t value_id;
    std::string new_value;
    std::string value_name;
    std::string updated_type;

    update_query(
        std::size_t value_id,
        std::string &new_value,
        std::string &value_name,
        std::string &updated_type
    )
        : value_id(value_id),
          new_value(std::move(new_value)),
          value_name(std::move(value_name)),
          updated_type(std::move(updated_type)) {
    }

    void execute() override;
};

struct delete_query : query {
    std::size_t value_id;
    std::string value_type;

    delete_query(std::size_t value_id, std::string &value_type)
        : value_id(value_id), value_type(std::move(value_type)) {
    }

    void execute() override;
};

struct create_query : query {
    std::size_t parent_id;
    std::string value_type;
    std::string value_name;

    create_query(
        std::size_t parent_id,
        std::string &value_type,
        std::string &value_name
    )
        : parent_id(parent_id),
          value_type(std::move(value_type)),
          value_name(std::move(value_name)) {
    }

    void execute() override;
};

#endif