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
    db.exec("DROP TABLE if exists Specializations;");
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
            "id bigint not null primary key auto_increment,"
            "iris_pattern text not null,"
            "face_pattern text,"
            "fingerprint_pattern text"
            ");");
    db.exec("CREATE TABLE if not exists Specialisations ("
            "specialisation_id int not null auto_increment,"
             "   faculty varchar(30) not null,"
              "  specialisation varchar(40) not null,"
               " primary key (specialisation_id, specialisation)"
            ");");
    db.exec("CREATE TABLE if not exists Students ("
            "student_id bigint NOT NULL AUTO_INCREMENT PRIMARY KEY,"
        "name varchar(20) not null,"
            "surname varchar(30) not null,"
            "faculty varchar(30) references Specialisations,"
            "specialisation varchar(40) references Specialisations,"
            "biometrics bigint not null references Biometrics,"
            "year_of_studies int references Subjects,"
            "subject_id int references Subjects"
            ");");
    db.exec("CREATE TABLE if not exists Students_Groups ("
            "group_id varchar(20) not null references Groups,"
            "student_id bigint references Students,"
            "primary key (group_id, student_id)"
            ");");
    db.exec("CREATE TABLE if not exists Groups ("
            "group_id serial not null primary key,"
             "   group_name varchar(30) not null unique,"
              "  day_of_week varchar(15) not null,"
               " time_of_classes Time,"
                "specialisation_id int references Specialisations,"
            ");");
    db.exec("CREATE TABLE if not exists Classes ("
            "class_id varchar(30) not null primary key,"
                "subject varchar(40) not null references Subjects,"
            "class_date Date,"
            "topic_id varchar(50) not null references Topics,"
                "group_id varchar(20) references Groups"
            ");");
    db.exec("CREATE TABLE if not exists Subjects ("
            "subject_id int not null auto_increment primary key,"
            "   specialisation varchar(40) references Specialisations,"
            "   year_of_studies int,"
            "   subject varchar(40) not null"
            ");");
    db.exec("CREATE TABLE if not exists Topics ("
            "topic_id bigint primary key not null auto_increment,"
         "subject_id int references Subjects"
            ");");
    db.exec("CREATE TABLE if not exists Attendance("
            "student_id bigint not null references Students,"
            "class_id varchar(30) not null references Classes,"
            "primary key (student_id, class_id)"
            ");");
    db.exec("CREATE TABLE if not exists Complements ("
            "student_id bigint not null references Students,"
            "class_id varchar(30) not null references Classes,"
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
