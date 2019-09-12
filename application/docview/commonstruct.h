#ifndef COMMONSTRUCT_H
#define COMMONSTRUCT_H

#include <QList>
#include <QString>

#include <QMetaType>

typedef struct SEARCH_RESULT {
    unsigned int ipages;
    QList<QString> listtext;
    SEARCH_RESULT()
    {
        ipages = 0;
        listtext.clear();
    }

} stSearchRes;

Q_DECLARE_METATYPE(stSearchRes)

#endif // COMMONSTRUCT_H
