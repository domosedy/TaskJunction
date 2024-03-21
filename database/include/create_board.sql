CREATE TABLE user_authorization_data(
login varchar(50) PRIMARY KEY,
password text
);

CREATE TABLE user_signature(
id serial PRIMARY KEY,
name    varchar(50) REFERENCES user_authorization_data ON DELETE CASCADE
);

CREATE TABLE board_signature(
id serial PRIMARY KEY,
user_id integer REFERENCES user_signature ON DELETE CASCADE,
name varchar(50),
description text
);

CREATE TABLE list_signature(
id serial PRIMARY KEY,
board_id integer REFERENCES board_signature ON DELETE CASCADE,
name varchar(50),
description text
);

CREATE TABLE card_signature(
id serial PRIMARY KEY,
list_id integer REFERENCES list_signature ON DELETE CASCADE,
name varchar(50),
description text
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
