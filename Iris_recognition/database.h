#ifndef DATABASE_H
#define DATABASE_H

#include <QtSQL/QSqlDatabase>
#include <QtSQL/QSqlQueryModel>
#include <QtSQL/QSqlQuery>
#include <QtSQL/QSqlError>
#include <QtCore/QDebug>

class Database
{
    public:
        Database();
        bool insertUser(QString, QString, QString, QString, QString);
        QSqlQuery *searchUsers();
        void createDB();
        QSqlDatabase db;
        QSqlQueryModel model;
        ~Database();

};

#endif // DATABASE_H
