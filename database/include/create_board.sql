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
user_id int REFERENCES user_signature ON DELETE CASCADE,
name varchar(50),
description text
);

CREATE TABLE list_signature(
id serial PRIMARY KEY,
board_id int REFERENCES board_signature ON DELETE CASCADE,
name varchar(50),
description text,
cards_number int DEFAULT 0
);

CREATE TABLE card_signature(
id serial PRIMARY KEY,
list_id int REFERENCES list_signature ON DELETE CASCADE,
name varchar(50),
description text,
number int
);

CREATE TABLE tag_signature(
id serial PRIMARY KEY,
name varchar(50)
);

CREATE TABLE card_to_tags(
card_id int REFERENCES card_signature ON DELETE CASCADE,
tag_id int REFERENCES tag_signature ON DELETE CASCADE,
PRIMARY KEY(card_id, tag_id)
);

CREATE TABLE user_to_board(
user_id int REFERENCES user_signature ON DELETE CASCADE,
board_id int REFERENCES board_signature ON DELETE CASCADE,
PRIMARY KEY(user_id, board_id)
);

INSERT INTO user_authorization_data VALUES ('default', 'default');

INSERT INTO user_signature VALUES (DEFAULT, 'default');

CREATE OR REPLACE FUNCTION insert_user(login_ text, password_ text) RETURNS int AS $$
begin
    INSERT INTO user_authorization_data VALUES (login_, password_);
    INSERT INTO user_signature VALUES (DEFAULT, login_);
    return last_value FROM user_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION authorize_user(login_ text, password_ text) RETURNS int AS $$
begin
    IF exists (SELECT * FROM user_authorization_data WHERE login = login_ LIMIT 1)
    THEN
         IF exists (SELECT * FROM user_authorization_data WHERE login = login_ AND password = password_ LIMIT 1) THEN
            return id FROM user_signature WHERE name = login_;
         ELSE return 0;
         END IF;
    ELSE return insert_user(login_, password_);
    END IF;
end;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_board(user_id_ int, name_ text, description_ text, OUT board_id_ int) AS $$
begin
    INSERT INTO board_signature VALUES (DEFAULT, user_id_, name_, description_);
    board_id_ = last_value FROM board_signature_id_seq;
    INSERT INTO user_to_board VALUES (user_id_, board_id_);
end;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION insert_list(board_id_ int, name_ text, description_ text, OUT list_id_ int) AS $$
begin
    INSERT INTO list_signature VALUES (DEFAULT, board_id_, name_, description_, DEFAULT);
    list_id_ = last_value FROM list_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION insert_card(list_id_ int, name_ text, description_ text, OUT card_id_ int) AS $$
begin
    INSERT INTO card_signature VALUES (DEFAULT, list_id_, name_, description_, DEFAULT);
    card_id_ = last_value FROM card_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION insert_tag(name_ text, OUT card_id_ int) AS $$
begin
    INSERT INTO tag_signature VALUES (DEFAULT, name_);
    card_id_ = last_value FROM card_signature_id_seq;
end;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION move_card(id_ int,  new_list_id_ int, new_number_ int) RETURNS VOID AS $$
DECLARE
    current_number int;
    difference int;
    left_border int;
    right_border int;
BEGIN
    UPDATE card_signature SET list_id = new_list_id_
        WHERE id = id_;

    current_number = number FROM card_signature WHERE id = id_;
    IF new_number_ < current_number THEN
        difference = 1;
        left_border = new_number_ - 1;
        right_border = current_number;
    ELSE
        difference = -1;
        left_border = current_number;
        right_border = new_number_ + 1;
    END IF;

    UPDATE card_signature SET number = number + difference
        WHERE number > left_border AND number < right_border;
    UPDATE card_signature SET number = new_number_
        WHERE id = id_;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION card_invariant() RETURNS TRIGGER AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        UPDATE list_signature SET cards_number = cards_number + 1 WHERE id = NEW.list_id;
        NEW.number = cards_number FROM list_signature WHERE id = NEW.list_id;
        RETURN NEW;
    ELSEIF TG_OP = 'DELETE' THEN
        UPDATE list_signature SET cards_number = cards_number - 1 WHERE id = OLD.list_id;
        UPDATE card_signature SET number = number - 1 WHERE list_id = OLD.list_id AND number > OLD.number;
        RETURN OLD;
    ELSEIF TG_OP = 'UPDATE' THEN
        IF OLD.list_id <> NEW.list_id THEN
            UPDATE list_signature SET cards_number = cards_number + 1 WHERE id = NEW.list_id;
            UPDATE list_signature SET cards_number = cards_number - 1 WHERE id = OLD.list_id;
            UPDATE card_signature SET number = number - 1 WHERE list_id = OLD.list_id AND number > OLD.number;
            UPDATE card_signature SET number = number + 1 WHERE list_id = NEW.list_id AND number >= NEW.number;
        END IF;
        RETURN NEW;
    END if;
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER card_invariant BEFORE INSERT OR DELETE OR UPDATE ON card_signature
     FOR EACH ROW EXECUTE PROCEDURE card_invariant();
