CREATE TABLE user_authorization_data(
login varchar(50) PRIMARY KEY,
password text
);

CREATE TABLE group_signature(
id serial PRIMARY KEY,
name text
);

CREATE TABLE user_signature(
id serial PRIMARY KEY,
name    varchar(50) REFERENCES user_authorization_data(login) ON DELETE CASCADE
);

CREATE TABLE board_signature(
id serial PRIMARY KEY,
group_id integer REFERENCES group_signature ON DELETE CASCADE,
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

CREATE TABLE user_to_group(
user_id integer REFERENCES user_signature ON DELETE CASCADE,
group_id integer REFERENCES group_signature ON DELETE CASCADE,
PRIMARY KEY(user_id, group_id)
);

INSERT INTO user_authorization_data VALUES ('default', 'default');

INSERT INTO user_signature VALUES (DEFAULT, 'default');
