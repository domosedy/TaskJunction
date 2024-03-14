CREATE SCHEMA project_template;
SET search_path TO project_template;

CREATE TABLE user_signature(
                               user_id serial PRIMARY KEY,
                               name    varchar(50)
);

CREATE TABLE board_signature(
                                board_id serial PRIMARY KEY,
                                user_id integer REFERENCES user_signature ON DELETE CASCADE,
                                name varchar(50)
);

CREATE TABLE list_signature(
                               list_id serial PRIMARY KEY,
                               board_id integer REFERENCES board_signature ON DELETE CASCADE,
                               name varchar(50),
                               description text
);

CREATE TABLE card_signature(
                               card_id serial PRIMARY KEY,
                               list_id integer REFERENCES list_signature ON DELETE CASCADE,
                               name varchar(50),
                               description text
);

CREATE TABLE tag_signature(
                              tag_id serial PRIMARY KEY,
                              name varchar(50)
);

CREATE TABLE card_to_tags(
                             card_id integer REFERENCES card_signature ON DELETE CASCADE,
                             tag_id integer REFERENCES tag_signature ON DELETE CASCADE,
                             PRIMARY KEY(card_id, tag_id)
);

CREATE FUNCTION insert_user(name_ varchar(50)) RETURNS
    void AS $$
INSERT INTO user_signature VALUES (DEFAULT, name_);
$$ LANGUAGE SQL;

CREATE FUNCTION insert_board(user_id_ integer, name_ varchar(50)) RETURNS
    void AS $$
INSERT INTO board_signature VALUES (DEFAULT, user_id_, name_);
$$ LANGUAGE SQL;

CREATE FUNCTION insert_list(board_id_ integer, name_ varchar(50),
                            description_ text) RETURNS void AS $$
INSERT INTO list_signature VALUES (DEFAULT, board_id_, name_,
description_);
$$ LANGUAGE SQL;

CREATE FUNCTION insert_card(list_id_ integer, name_ varchar(50),
                            description_ text) RETURNS void AS $$
INSERT INTO card_signature VALUES (DEFAULT, list_id_, name_,
description_);
$$ LANGUAGE SQL;

CREATE FUNCTION insert_tag(name_ varchar(50)) RETURNS void AS $$
INSERT INTO tag_signature VALUES (DEFAULT, name_);
$$ LANGUAGE SQL;

CREATE FUNCTION insert_into_card_to_tags(card_id_ integer, tag_id_
    integer) RETURNS void AS $$
INSERT INTO card_to_tags VALUES (card_id_, tag_id_);
$$ LANGUAGE SQL;


