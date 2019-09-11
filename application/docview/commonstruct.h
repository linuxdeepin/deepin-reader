#ifndef COMMONSTRUCT_H
#define COMMONSTRUCT_H
#include <QList>
#include <QString>

typedef struct SEARCH_RESULT{
    unsigned int ipages;
    QList<QString> listtext;
    SEARCH_RESULT(){
        ipages=0;
        listtext.clear();
    }

}stSearchRes;

#endif // COMMONSTRUCT_H
