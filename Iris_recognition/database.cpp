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
            "biometricsid integer primary key autoincrement,"
            "iris_pattern text not null,"
            "face_pattern text,"
            "fingerprint_pattern text"
            ");");
    db.exec("CREATE TABLE if not exists Faculties ("
            "faculty_id integer primary key autoincrement,"
            "faculty text not null"
            ");");
    db.exec("CREATE TABLE if not exists Specialisations ("
            "specialisation_id integer primary key autoincrement,"
             "faculty_id integer references Faculties,"
             "specialisation text not null"
            ");");
    db.exec("CREATE TABLE if not exists Subjects ("
            "subject_id integer primary key autoincrement,"
            "specialisation_id integer references Specialisations,"
            "year_of_studies integer,"
            "subject text not null"
            ");");
    db.exec("CREATE TABLE if not exists Students ("
            "student_id integer PRIMARY KEY autoINCREMENT,"
            "name text not null,"
            "surname text not null,"
            "specialisation_id integer references Specialisations,"
            "biometrics_id integer references Biometrics"
            ");");
    db.exec("CREATE TABLE if not exists Groups ("
            "group_id integer primary key,"
            "group_name text not null,"
            "day_of_week text not null,"
            "time_of_classes Time,"
            "specialisation_id integer references Specialisations"
            ");");
    db.exec("CREATE TABLE if not exists Students_Groups ("
            "group_id integer references Groups,"
            "student_id integer references Students,"
            "primary key (group_id, student_id)"
            ");");
    db.exec("CREATE TABLE if not exists Classes ("
            "class_id integer primary key,"
            "subject_id integer references Subjects,"
            "class_date Date,"
            "topic_id integer references Topics,"
            "group_id integer references Groups"
            ");");
    db.exec("CREATE TABLE if not exists Topics ("
            "topic_id integer primary key autoincrement,"
            "subject_id integer references Subjects,"
            "topic text not null"
            ");");
    db.exec("CREATE TABLE if not exists Attendance("
            "student_id integer  references Students,"
            "class_id integer references Classes,"
            "primary key (student_id, class_id)"
            ");");
    db.exec("CREATE TABLE if not exists Complements ("
            "student_id integer references Students,"
            "class_id integer references Classes,"
            "complement_date DateTime,"
            "primary key (student_id, class_id)"
            ");");
    db.exec("create table Photos ("
            "biometrics_id integer references Biometrics,"
            "iris_1 blob, "
            "iris_2 blob, "
            "iris_3 blob, "
            "iris_4 blob, "
            "iris_5 blob,"
            "finger_1 blob, "
            "finger_2 blob, "
            "finger_3 blob, "
            "finger_4 blob, "
            "finger_5 blob,"
            "face_1 blob, "
            "face_2 blob, "
            "face_3 blob, "
            "face_4 blob, "
            "face_5 blob"
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
    QSqlQuery *query = new QSqlQuery("SELECT `faculty_id`, `faculty` FROM Faculties");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}

QSqlQuery * Database::getSpecialisations()
{
    QSqlQuery *query = new QSqlQuery("SELECT `specialisation_id`, `faculty_id`, `specialisation` FROM Specialisations");
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}

QSqlQuery * Database::getSpecialisationsByFacultyId(int id)
{
    QSqlQuery *query = new QSqlQuery(db);
    query->prepare("SELECT `specialisation_id`, `faculty_id`, `specialisation` FROM Specialisations where `faculty_id` = :id");
    query->bindValue(":id", id);
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
QSqlQuery * Database::getGroupsBySpecialisationId(int id)
{
    QSqlQuery *query = new QSqlQuery(db);
    query->prepare("SELECT `group_id`, `group_name`, `day_of_week`, `time_of_classes`, `specialisation_id` FROM Groups where specialisation_id = :id");
    query->bindValue(":id", id);
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

QSqlQuery * Database::getSubjectsBySpecialisationId(int id)
{
    QSqlQuery *query = new QSqlQuery(db);
    query->prepare("SELECT `subject_id`, `specialisation_id`, `year_of_studies`, `subject` FROM Subjects where specialisation_id = :id");
    query->bindValue(":id", id);
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

QSqlQuery * Database::getTopicsBySubjectId(int id)
{
    QSqlQuery *query = new QSqlQuery(db);
    query->prepare("SELECT `topic_id`, `subject_id`, `topic` FROM Topics where subject_id = :id");
    query->bindValue(":id", id);
    query->exec();

    if( query->lastError().isValid()) {
        qDebug() << query->lastError().text();
        qDebug() << query->lastQuery();
    }

    return query;
}

bool Database::changeSpecialisationName(int id, QString name)
{
    QSqlQuery query(db);
    query.prepare("Update specialisations set specialisation = :name where specialisation_id = :id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);
    return query.exec();
}

bool Database::addSpecialisation(QString name, int faculty_id)
{
    QSqlQuery query(db);
    query.prepare("insert into specialisations (specialisation, faculty_id) values (:name, :id)");
    query.bindValue(":name", name);
    query.bindValue(":id", faculty_id);
    return query.exec();
}

bool Database::changeFacultyName(int id, QString name)
{
    QSqlQuery query(db);
    query.prepare("Update faculties set faculty = :name where faculty_id = :id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);
    return query.exec();
}

bool Database::addFaculty(QString name)
{
    QSqlQuery query(db);
    query.prepare("insert into faculties (faculty) values (:name)");
    query.bindValue(":name", name);
    return query.exec();
}

bool Database::changeSubject(int id, int year, QString name)
{
    QSqlQuery query1(db);
    query1.prepare("Update subjects set year_of_studies = :year where subject_id = :id");
    query1.bindValue(":year", year);
    query1.bindValue(":id", id);
    QSqlQuery query2(db);
    query2.prepare("Update subjects set subject = :name where subject_id = :id");
    query2.bindValue(":name", name);
    query2.bindValue(":id", id);
    return query1.exec() & query2.exec();
}

bool Database::addSubject(int specialisation_id, int year, QString name)
{
    QSqlQuery query(db);
    query.prepare("insert into subjects (specialisation_id, year_of_studies, subject) values (:id, :year, :name)");
    query.bindValue(":id", specialisation_id);
    query.bindValue(":year", year);
    query.bindValue(":name", name);
    return query.exec();
}

QString Database::getSubjectYearAsString(int id)
{
    QSqlQuery query(db);
    query.prepare("select year_of_studies from subjects where subject_id = :id");
    query.bindValue(":id", id);
    query.exec();
    if( query.lastError().isValid()) {
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();
    }
    query.next();
    return query.value(0).toString();
}

bool Database::changeTopic(int id, QString topic)
{
    QSqlQuery query(db);
    query.prepare("Update topics set topic = :topic where topic_id = :id");
    query.bindValue(":topic", topic);
    query.bindValue(":id", id);
    return query.exec();
}

bool Database::addTopic(int subject_id, QString topic)
{
    QSqlQuery query(db);
    query.prepare("insert into topics (subject_id, topic) values (:id, :topic)");
    query.bindValue(":id", subject_id);
    query.bindValue(":topic", topic);
    return query.exec();
}

bool Database::addGroup(int spec_id, QString name, QString day, QTime time)
{
    QSqlQuery query(db);
    query.prepare("insert into groups(specialisation_id, `group_name`, day_of_week, time_of_classes) values (:id, :name, :day, :time)");
    query.bindValue(":name", name);
    query.bindValue(":id", spec_id);
    query.bindValue(":day", day);
    query.bindValue(":time", time);
    return query.exec();

}

bool Database::changeGroup(int id, QString name, QString day, QTime time)
{
    QSqlQuery query1(db);
    query1.prepare("Update groups set day_of_week = :day where group_id = :id");
    query1.bindValue(":day", day);
    query1.bindValue(":id", id);
    QSqlQuery query2(db);
    query2.prepare("Update groups set group_name = :name where group_id = :id");
    query2.bindValue(":name", name);
    query2.bindValue(":id", id);
    QSqlQuery query3(db);
    query3.prepare("Update groups set time_of_classes = :name where group_id = :id");
    query3.bindValue(":time", time);
    query3.bindValue(":id", id);
    return query1.exec() & query2.exec() & query3.exec();
}

int Database::getGroupDayAsInt(int id)
{
    QSqlQuery query(db);
    query.prepare("select day_of_week from groups where group_id = :id");
    query.bindValue(":id", id);
    query.exec();
    if( query.lastError().isValid()) {
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();
    }
    query.next();
    QString tmp = query.value(0).toString();
    if (tmp == "poniedzia³ek")
    {
        return 0;
    }
    else if (tmp == "wtorek")
    {
        return 1;
    }
    else if (tmp == "œroda")
    {
        return 2;
    }
    else if (tmp == "czwartek")
    {
        return 3;
    }
    else if (tmp == "pi¹tek")
    {
        return 4;
    }
    else if (tmp == "sobota")
    {
        return 5;
    }
    else if (tmp == "niedziela")
    {
        return 6;
    }
}

QTime Database::getGroupTime(int id)
{
    QSqlQuery query(db);
    query.prepare("select time_of_classes from groups where group_id = :id");
    query.bindValue(":id", id);
    query.exec();
    if( query.lastError().isValid()) {
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();
    }
    query.next();
    return query.value(0).toTime();
}
