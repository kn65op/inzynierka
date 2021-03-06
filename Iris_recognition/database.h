#ifndef DATABASE_H
#define DATABASE_H

#include <QtSQL/QSqlDatabase>
#include <QtSQL/QSqlQueryModel>
#include <QtSQL/QSqlQuery>
#include <QtSQL/QSqlError>
#include <QtCore/QDebug>
#include "QTime"
#include "QDate"

class Database
{
    public:
        Database();
        bool insertUser(QString, QString, QString, QString, QString);
        int insertEyeCode(QString code);
        bool insertStudent(QString, QString, int, int);
        bool addLastStudentToGroup(int gid);
        QSqlQuery *searchUsers();
        QSqlQuery *searchStudents();
        void createDB();
        QSqlDatabase db;
        QSqlQueryModel model;
        ~Database();
        QSqlQuery *getFaculties();
        QString getFacultyById(int);
        QSqlQuery *getSpecialisations();
        QSqlQuery *getSpecialisationsByFacultyId(int);
        QSqlQuery *getSubjects();
        QSqlQuery *getSubjectsBySpecialisationId(int);
        QSqlQuery *getTopics();
        QSqlQuery *getTopicsBySubjectId(int);
        QSqlQuery *getGroups();
        QSqlQuery *getGroupsBySpecialisationId(int);
        QSqlQuery *getClasses();
        bool changeSpecialisationName(int id, QString name);
        bool addSpecialisation(QString name, int faculty_id);
        bool changeFacultyName(int id, QString name);
        bool addFaculty(QString name);
        bool changeSubject(int id, int year, QString name);
        bool addSubject(int specialisation_id, int year, QString name);
        bool changeTopic(int id, QString topic);
        bool addTopic(int subject_id, QString topic);
        bool changeGroup(int id, QString name, QString day, QTime time);
        bool addGroup(int spec_id, QString name, QString day, QTime time);
        bool addClass(int subj_id, int topic_id, int group_id, QDate date);
        QString getSubjectYearAsString(int id);
        int getGroupDayAsInt(int id);
        QTime getGroupTime(int id);

};

#endif // DATABASE_H
