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
    "parent-id": uint32,
    "board-id": uint32,
    "list-id": uint32,
    "card-id": uint32,
    "name": string,
    "description": string,
    "object-type": json
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
    "id": uint32,
    "object-type": string
}
```

5. Запрос обновления:
```
{
    "type": "update",
    "id": uint32,
    "new-value": string,
    "field": string,
    "object-type": string
}
```

6. Запрос перемещения карточки:
```
{
    "type": "move",
    "id": uint32,
    "list-id": uint32,
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
    "type": "create-response",
    "board-id": 
    "list-id":
    "card-id":
    "object-json": один из предыдущих нужного типа.
}
```

8. Ответ на подключение
```
{
    "type": "connect"
    "response": "ok" / "wrong"
    "board": json 
}