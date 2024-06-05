#!/bin/bash
# In honor of Aleksey Zubakov

create_database () {
    if psql -U postgres -lqt | cut -d \| -f 1 | grep -qw $2; then
        echo "$2 already exists!";
    else
        sudo -u postgres psql -c "CREATE DATABASE $2 OWNER $1;"
        sudo -u postgres psql --username=$1 $2< $create_path
    fi
}

if [[ -f "config" ]]; then

    create_path="./database/include/create_board.sql"
    args=()
    while IFS= read -r line; do
        line=$(echo "$line" | xargs)
        var_name=$(echo "$line" | cut -d '=' -f 1)
        var_value=$(echo "$line" | cut -d '=' -f 2)
        args+=("$var_value")
        echo "Loaded $var_name with value: $var_value"
    done < "config"

    if [[ $# -eq 0 ]]; then
        echo 'Provide build options please!'
        exit 0
    fi

    if psql -U postgres -tAc "SELECT 1 FROM pg_roles WHERE rolname='${args[0]}'" | grep -q 1; then
        echo The user ${args[0]} already exists!
    else
        sudo -u postgres psql -c "CREATE USER ${args[0]} WITH PASSWORD '${args[1]}' CREATEDB;"
    fi

    while [[ $# -gt 0 ]]; do
        case $1 in
            -c|--client)
                create_database "${args[0]}" "${args[2]}"
                shift
                ;;
            -s|--server)
                create_database "${args[0]}" "${args[3]}"
                shift
                ;;
            -a|--all)
                create_database "${args[0]}" "${args[2]}"
                create_database "${args[0]}" "${args[3]}"
                shift
                ;;
            -*|--*)
                echo "Unknown option $1"
                exit 1
        esac
    done
else
    echo "File 'config' not found."
fi
