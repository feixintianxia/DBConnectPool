#!/bin/sh
user="root"
passwd="lihai"
dbName="DBConnectPool"
host="localhost"
port=3306
mysql -u${user} -p${passwd} -h${host} -P${port} ${dbName} < sql.sql
