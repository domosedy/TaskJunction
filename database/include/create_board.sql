CREATE TABLE user_authorization_data(
login varchar(50) PRIMARY KEY,
password text
);

CREATE TABLE user_signature(
id serial PRIMARY KEY,
name    varchar(50) REFERENCES user_authorization_data(login) ON DELETE CASCADE
);

CREATE TABLE board_signature(
id serial PRIMARY KEY,
user_id integer REFERENCES user_signature ON DELETE CASCADE,
name varchar(50),
description text,
number serial
);

CREATE TABLE list_signature(
id serial PRIMARY KEY,
board_id integer REFERENCES board_signature ON DELETE CASCADE,
name varchar(50),
description text,
number serial
);

CREATE TABLE card_signature(
id serial PRIMARY KEY,
list_id integer REFERENCES list_signature ON DELETE CASCADE,
name varchar(50),
description text,
number serial
);

CREATE TABLE tag_signature(
id serial PRIMARY KEY,
name varchar(50)
);

CREATE TABLE card_to_tags(
card_id integer REFERENCES card_signature ON DELETE CASCADE,
tag_id integer REFERENCES tag_signature ON DELETE CASCADE,
PRIMARY KEY(card_id, tag_id)
);

CREATE TABLE user_to_board(
user_id integer REFERENCES user_signature ON DELETE CASCADE,
board_id integer REFERENCES board_signature ON DELETE CASCADE,
PRIMARY KEY(user_id, board_id)
);

INSERT INTO user_authorization_data VALUES ('default', 'default');

INSERT INTO user_signature VALUES (DEFAULT, 'default');

CREATE FUNCTION insert_user(login_ text, password_ text) RETURNS integer AS $$
begin
    INSERT INTO user_authorization_data VALUES (login_, password_);
    INSERT INTO user_signature VALUES (DEFAULT, login_);
    return last_value FROM user_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;

CREATE FUNCTION authorize_user(login_ text, password_ text) RETURNS integer AS $$
begin
    IF exists (SELECT * FROM user_authorization_data WHERE login = login_ LIMIT 1)
    THEN
         IF exists (SELECT * FROM user_authorization_data WHERE login = login_ AND password = password_ LIMIT 1)
         THEN return id FROM user_signature WHERE name = login_;
         ELSE return 0;
         END IF;
    ELSE return insert_user(login_, password_);
    END IF;
end;
$$ LANGUAGE plpgsql;

CREATE FUNCTION insert_board(user_id_ integer, name_ text, description_ text, OUT board_id_ integer) RETURNS integer AS $$
begin
    INSERT INTO board_signature VALUES (DEFAULT, user_id_, name_, description_, DEFAULT);
    board_id_ = last_value FROM board_signature_id_seq;
    INSERT INTO user_to_board VALUES (user_id_, board_id_);
end;
$$ LANGUAGE plpgSQL;

CREATE FUNCTION insert_list(board_id_ integer, name_ text, description_ text, OUT list_id_ integer) RETURNS integer AS $$
begin
    INSERT INTO list_signature VALUES (DEFAULT, board_id_, name_, description_, DEFAULT);
    list_id_ = last_value FROM list_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;

CREATE FUNCTION insert_card(list_id_ integer, name_ text, description_ text, OUT card_id_ integer) RETURNS integer AS $$
begin
    INSERT INTO card_signature VALUES (DEFAULT, list_id_, name_, description_, DEFAULT);
    card_id_ = last_value FROM card_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;

CREATE FUNCTION insert_tag(name_ text, OUT card_id_ integer) RETURNS integer AS $$
begin
    INSERT INTO tag_signature VALUES (DEFAULT, name_);
    card_id_ = last_value FROM card_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;
