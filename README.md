# TaskJunction
This is Linux desktop application for task managment in kanban system

## Dependencies

1. Qt, QtQuick 6.7
2. PostgreSQL 16.2
3. Nlohmann-json 3.11.2

## Build

1. Fill in the config file or leave it with default values.
2. To create database use `build-database.sh` file:
```
./build-database.sh [-c | --client] [-s | --server] [-a | --all]
```

## Features
1. Create/Delete/Update boards, lists, cards, tags
2. Move cards from one list to another
3. Filter cards by tags
4. Share boards with other users on server
5. Make local copy of shared boards 

### Team
1. Vinogradov Ilya - server
2. Gladkikh Andrey - client
3. Grebenkin Ivan - database
4. Sirotkina Veronika - mentoring 
