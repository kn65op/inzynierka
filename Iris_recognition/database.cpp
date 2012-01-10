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
}

void Database::createDB()
{
    //Przy pierwszym uruchomieniu, aby utworzyæ bazê nale¿y odkomentowaæ poprzedni¹ linijkê
    //
    db.exec("DROP TABLE if exists Biometrics;");
    db.exec("DROP TABLE if exists Specialisations;");
    db.exec("DROP TABLE if exists Students;");
    db.exec("DROP TABLE if exists Students_Groups;");
    db.exec("DROP TABLE if exists Groups;");
    db.exec("DROP TABLE if exists Classes;");
    db.exec("DROP TABLE if exists Subjects;");
    db.exec("DROP TABLE if exists Topics;");
    db.exec("DROP TABLE if exists Attendance;");
    db.exec("DROP TABLE if exists Complements;");
    //db.exec("CREATE TABLE users (id integer primary key autoincrement, name TEXT, surname TEXT, groupa TEXT, faculty TEXT, iris_code TEXT);");
    db.exec("CREATE TABLE if not exists Biometrics ("
            "id integer primary key autoincrement,"
            "iris_pattern text not null,"
            "face_pattern text,"
            "fingerprint_pattern text"
            ");");
    db.exec("CREATE TABLE if not exists Specialisations ("
            "specialisation_id integer primary key autoincrement,"
             "faculty text not null,"
              "specialisation text not null"
            ");");
    db.exec("CREATE TABLE if not exists Subjects ("
            "subject_id integer primary key autoincrement,"
            "   specialisation_id integer references Specialisations,"
            "   year_of_studies integer,"
            "   subject text not null"
            ");");
    db.exec("CREATE TABLE if not exists Students ("
            "student_id integer PRIMARY KEY autoINCREMENT,"
        "name text not null,"
            "surname text not null,"
            "faculty integer references Specialisations,"
            "biometrics integer references Biometrics"
            ");");
    db.exec("CREATE TABLE if not exists Groups ("
            "group_id integer primary key,"
             "   group_name text not null,"
              "  day_of_week text not null,"
               " time_of_classes integer,"
                "specialisation_id integer references Specialisations"
            ");");
    db.exec("CREATE TABLE if not exists Students_Groups ("
            "group_id text references Groups,"
            "student_id integer references Students,"
            "primary key (group_id, student_id)"
            ");");
    db.exec("CREATE TABLE if not exists Classes ("
            "class_id text primary key,"
                "subject text references Subjects,"
            "class_date Date,"
            "topic_id text references Topics,"
                "group_id text references Groups"
            ");");
    db.exec("CREATE TABLE if not exists Topics ("
            "topic_id integer primary key autoincrement,"
         "subject_id integer references Subjects,"
            "topic text"
            ");");
    db.exec("CREATE TABLE if not exists Attendance("
            "student_id integer  references Students,"
            "class_id text references Classes,"
            "primary key (student_id, class_id)"
            ");");
    db.exec("CREATE TABLE if not exists Complements ("
            "student_id integer references Students,"
            "class_id text references Classes,"
            "complement_date DateTime,"
            "primary key (student_id, class_id)"
            ");");
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

QSqlQuery * Database::getFaculties()
{
    QSqlQuery *query = new QSqlQuery("SELECT `specialisation_id`, `faculty`, `specialisation` FROM Specialisations");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}

QSqlQuery * Database::getClasses()
{
    QSqlQuery *query = new QSqlQuery("SELECT `class_id`, `subject`, `class_date`, `topic_id`, `group_id` FROM Classes");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}
QSqlQuery * Database::getGroups()
{
    QSqlQuery *query = new QSqlQuery("SELECT `group_id`, `group_name`, `day_of_week`, `time_of_classes`, `specialisation_id` FROM Groups");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}
QSqlQuery * Database::getSubjects()
{
    QSqlQuery *query = new QSqlQuery("SELECT `subject_id`, `specialisation_id`, `year_of_studies`, `subject` FROM Subjects");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}
QSqlQuery * Database::getTopics()
{
    QSqlQuery *query = new QSqlQuery("SELECT `topic_id`, `subject_id`, `topic` FROM Topics");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}
