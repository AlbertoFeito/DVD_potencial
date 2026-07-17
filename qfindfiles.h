#ifndef QFINDFILES_H
#define QFINDFILES_H

#include <QStringList>
#include <QFile>
#include <QString>
#include <QDir>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

struct EIndex
{
   QString system ,path, fname;
   int action;
};

typedef QList<EIndex> listData;

typedef vector<string> string_list;

typedef string_list::iterator string_list_iterator;

class QFindFiles
{
public:
    QFindFiles();
    QStringList getFileList(const QString& path);
    QStringList getDirectoryFileList(const QString& path);
    QStringList getDirectoryList(const QString& path);
    listData getDirectoryFileListSeparate(const QString& path, const QString& sys);
    bool copyDirectory(QString from, QString to, bool replace);

private:
    int fileList(const char *, string_list &);
    int findAll(const char *path, string_list &listdata, int opc);
    int findAll(const char * path, string_list &listPath, string_list &listFile);
    bool copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit);
    QStringList getData(const QString& path, int opc);
};

#endif // QFINDFILES_H
