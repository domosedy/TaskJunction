### Протокол

## От клиента

1. Запрос подключения:
```
{
    "type": "login",
    "username": string,
    "password": string
}
```

2. Запрос создания:
```
{
    "type": "create",
    "board-id": uint32,
    "list-id": uint32,
    "card-id": uint32,
    "tag-id": uint32,
    "parent-id": uint32,
    "name": string,
    "description": string,
    "object-type": string
}
```

3. Запрос доски:

```
{
    "type": "get-boards-info",
    "id": uint32
}
```

4. Запрос удаления:
```
{
    "type": "delete",
    "board-id": uint32,
    "list-id": uint32,
    "card-id": uint32,
    "tag-id": uint32,
    "id": uint32,
    "object-type": string
}
```

5. Запрос обновления:
```
{
    "type": "update",
    "board-id": uint32,
    "list-id": uint32,
    "card-id": uint32,
    "tag-id": uint32,   
    "id": uint32,
    "object-type": string,
    "new-value": string,
    "field": string,
}
```

6. Запрос перемещения карточки:
```
{
    "type": "move",
    "board-id": uint32,
    "list-id": uint32,
    "card-id": uint32,
    "tag-id": uint32,
    "old-list-id": uint32,
    "new_list-id": uint32
    "new_index": int
}
```

7. Запрос подключения к доске:
```
{
    "type": "connect"
    "link": string
}
```

## От сервера

1. Ошибка:
```
{
    "type": "error",
    "msg": сообщение
}
```
2. Доска:
```
{
    "type": "board",
    "lists": [
        все листы
    ],
    "id": id доски,
    "name": имя доски,
    "description": описание доски
}
```
3. Лист:
```
{
    "type": "list",
    "cards": [
        все карточки внутри этого листа
    ],
    "id": id листа,
    "name": имя листа,
    "description": описание листа
}
```
4. Карточка:
```
{
    "type": "card",
    "tags": [
        все теги при карточке
    ],
    "id": id карточки,
    "name": имя карточки,
    "description": описание карточки
}
```
5. Тег:
```
{
    "type": "tag",
    "id": id тега,
    "name": имя тега,
    "description": описание тега
}
```
6. Ответ на авторизацию
```
{
    "type": "authorization"
    "response": "ok" / "wrong"
    "boards": [
        {
            "name": ...,
            "id": ...
        },
        ...
    ]
}
```
7. Ответ за создание
```
{
    "type": "create",
    "board-id": 
    "list-id":
    "card-id":
    "tag-id": 
    "object-json": один из предыдущих нужного типа.
}
```

8. Ответ на обновление
```
{
    "type": "update",
    "board-id": 
    "list-id":
    "card-id":   
    "new-value": string,
    "field": string,
}
```

9. Ответ на удаление
```
{
    "type": "delete",
    "board-id": 
    "list-id":
    "card-id":
    "tag-id":   
}
```

10. Ответ на перемещение
```

{
    "type": "move",
    "board-id": uint32
    "old-list-id": uint32,
    "card_id": uint32,
    "new_list-id": uint32
    "new_index": int
}

```

8. Ответ на подключение
```
{
    "type": "connect"
    "status": "ok" / "wrong"
    "board": json 
}

