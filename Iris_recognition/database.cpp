#include "database.h"

Database::Database() {
    /*db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("iris");
    db.setUserName("iris");
    db.setPassword("iris");*/
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("iris.sqlite3");
    if (!db.open()) qDebug() << "Failed to connect to mysql" ; //TODO: dodaæ informacjê u¿ytkownikowi, ¿e jest b³¹d z po³¹czeniem
    //Przy pierwszym uruchomieniu, aby utworzyæ bazê nale¿y odkomentowaæ poprzedni¹ linijkê
    //db.exec("CREATE TABLE users (id integer primary key autoincrement, name TEXT, surname TEXT, groupa TEXT, faculty TEXT, iris_code TEXT);");
}

bool Database::insertUser(QString name, QString surname, QString group, QString faculty, QString iris_code) {
    QSqlQuery query(db);

    query.prepare("INSERT INTO users (name, surname, groupa, faculty, iris_code) VALUES (:name, :surname, :groupa, :faculty, :iris_code)");
    query.bindValue(":name", name);
    query.bindValue(":surname", surname);
    query.bindValue(":groupa", group);
    query.bindValue(":faculty", faculty);
    query.bindValue(":iris_code", iris_code);
    query.exec();
    if( query.lastError().isValid()) {
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();
        return false;
    } else {
        return true;
    }
}

QSqlQuery * Database::searchUsers() {
    QSqlQuery *query = new QSqlQuery("SELECT `id`, `iris_code`, `name`, `surname` FROM users");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}

Database::~Database()
{
    db.close();
}
