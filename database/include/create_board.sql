CREATE TABLE user_signature(
login varchar(50) PRIMARY KEY,
password text,
id serial UNIQUE,
salt varchar(10)
);

CREATE TABLE board_signature(
id serial PRIMARY KEY,
user_id int REFERENCES user_signature (id) ON DELETE CASCADE,
name varchar(50),
description text,
link varchar(20)
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
number int DEFAULT 1
);

CREATE TABLE tag_signature(
name varchar(50) PRIMARY KEY,
id serial UNIQUE
);

CREATE TABLE card_to_tags(
card_id int REFERENCES card_signature ON DELETE CASCADE,
tag_id int REFERENCES tag_signature (id) ON DELETE CASCADE,
PRIMARY KEY(card_id, tag_id)
);

CREATE TABLE user_to_board(
user_id int REFERENCES user_signature (id) ON DELETE CASCADE,
board_id int REFERENCES board_signature ON DELETE CASCADE,
PRIMARY KEY(user_id, board_id)
);

CREATE OR REPLACE FUNCTION insert_user(login_ text, password_ text) RETURNS int AS $$
BEGIN
    INSERT INTO user_signature VALUES (login_, password_, DEFAULT, '');
    return last_value FROM user_signature_id_seq;
END;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION authorize_user(login_ text, password_ text) RETURNS int AS $$
BEGIN
    IF exists (SELECT * FROM user_signature WHERE login = login_ LIMIT 1) THEN
         IF exists (SELECT * FROM user_signature WHERE login = login_ AND password = password_ LIMIT 1) THEN
            return id FROM user_signature WHERE login = login_;
         ELSE return 0;
         END IF;
    ELSE return insert_user(login_, password_);
    END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_board(user_id_ int, name_ text, description_ text, link_ varchar(20), OUT board_id_ int) AS $$
BEGIN
    INSERT INTO board_signature VALUES (DEFAULT, user_id_, name_, description_, link_);
    board_id_ = last_value FROM board_signature_id_seq;
    INSERT INTO user_to_board VALUES (user_id_, board_id_);
END;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION insert_list(board_id_ int, name_ text, description_ text, OUT list_id_ int) AS $$
BEGIN
    INSERT INTO list_signature VALUES (DEFAULT, board_id_, name_, description_, DEFAULT);
    list_id_ = last_value FROM list_signature_id_seq;
END;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION insert_card(list_id_ int, name_ text, description_ text, OUT card_id_ int) AS $$
BEGIN
    INSERT INTO card_signature VALUES (DEFAULT, list_id_, name_, description_, DEFAULT);
    card_id_ = last_value FROM card_signature_id_seq;
END;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION insert_tag(name_ text, OUT tag_id_ int) AS $$
BEGIN
    INSERT INTO tag_signature VALUES (name_, DEFAULT);
    tag_id_ = last_value FROM tag_signature_id_seq;
END;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION delete_card(id_ int) RETURNS VOID AS $$
DECLARE
    list_id_ int;
    number_ int;
BEGIN
    list_id_ = list_id FROM card_signature WHERE id = id_;
    number_ = number FROM card_signature WHERE id = id_;
    UPDATE list_signature SET cards_number = cards_number - 1 WHERE id = list_id_;
    UPDATE card_signature SET number = number - 1 WHERE list_id = list_id_ AND number_ < number;
    DELETE FROM card_signature WHERE id = id_;
END;
$$ LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION move_card(id_ int, new_list_id_ int, new_number_ int) RETURNS boolean AS $$
DECLARE
    list_id_ int;
    number_ int;
BEGIN
    list_id_ = list_id FROM card_signature WHERE id = id_;
    number_ = number FROM card_signature WHERE id = id_;

    IF new_number_ < 0 THEN
        RETURN FALSE;
    END IF;
    IF list_id_ = new_list_id_ THEN
       IF new_number_ > cards_number FROM list_signature WHERE id = new_list_id_ THEN
            RETURN FALSE;
         END IF;
    ELSEIF new_number_ > 1 + cards_number FROM list_signature WHERE id = new_list_id_ THEN
       RETURN FALSE;
    END IF;
    IF new_number_ = 0 THEN
        new_number_ = cards_number FROM list_signature WHERE id = new_list_id_;
        IF list_id_ <> new_list_id_ THEN
           new_number_ = new_number_ + 1;
        END IF;
    END IF;
    IF list_id_ <> new_list_id_ THEN
        UPDATE list_signature SET cards_number = cards_number + 1 WHERE id = new_list_id_;
        UPDATE list_signature SET cards_number = cards_number - 1 WHERE id = list_id_;
        UPDATE card_signature SET number = number - 1 WHERE list_id = list_id_ AND number_ < number;
        UPDATE card_signature SET number = number + 1 WHERE list_id = new_list_id_ AND new_number_ <= number;
    ELSEIF new_number_ < number_ THEN
        UPDATE card_signature SET number = number + 1
            WHERE list_id = list_id_ AND new_number_ <= number AND number < number_;
    ELSEIF number_ < new_number_ THEN
        UPDATE card_signature SET number = number - 1
            WHERE list_id = list_id_ AND number <= new_number_ AND number_ < number;
    END IF;
    UPDATE card_signature SET list_id = new_list_id_, number = new_number_ WHERE id = id_;
    RETURN TRUE;
END;
$$LANGUAGE plpgSQL;

CREATE OR REPLACE FUNCTION card_invariant() RETURNS TRIGGER AS $$
BEGIN
    IF NEW.number < 0 OR 1 + cards_number < NEW.number FROM list_signature WHERE id = NEW.list_id THEN
        RETURN NULL;
    END IF;
    IF TG_OP = 'INSERT' THEN
        UPDATE list_signature SET cards_number = cards_number + 1 WHERE id = NEW.list_id;
        NEW.number = cards_number FROM list_signature WHERE id = NEW.list_id;
        RETURN NEW;
    ELSE
        IF OLD.id <> NEW.id THEN
            RETURN NULL;
        END IF;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER card_invariant BEFORE INSERT OR UPDATE ON card_signature
     FOR EACH ROW EXECUTE PROCEDURE card_invariant();

INSERT INTO user_signature VALUES ('default', 'default', DEFAULT, '');
