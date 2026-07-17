#include "qfindfiles.h"
 #include <utility> 
#ifdef Q_OS_WIN
#include "direntwin32.h"
   char * separador = "\\";
#else
#include "direntlinux.h"
   char * separador = "/";
#endif

QFindFiles::QFindFiles()
= default;

QStringList QFindFiles::getFileList(const QString& path)
{
    return getData(path,1);
}

QStringList QFindFiles::getDirectoryFileList(const QString& path)
{
    return getData(path,2);
}

QStringList QFindFiles::getDirectoryList(const QString& path)
{
    return getData(path,0);
}

listData QFindFiles::getDirectoryFileListSeparate(const QString& path,const QString& sys)
{
    string_list filelist; string_list pathlist;
    QStringList qfilelist; QStringList qpathlist;
    findAll(path.toStdString().c_str(),pathlist,filelist);
    for (auto v : filelist)
    {
        qfilelist.append(v.c_str());
    }
    for (auto v : pathlist)
    {
        qpathlist.append(v.c_str());
    }

    listData aux;
    EIndex data;
    for (int i = 0; i < qpathlist.length(); ++i)
    {
        data.action = 1;
        data.path = qpathlist.at(i);
        data.fname = qfilelist.at(i);
        data.system = sys;
        aux.append(data);
    }

    return aux;
}

bool QFindFiles::copyDirectory(QString from, QString to, bool replace)
{
    return copy_dir_recursive(std::move(from),std::move(to),replace);
}

QStringList QFindFiles::getData(const QString& path, int opc)
{
    string_list filelist;
    QStringList qfilelist;
    findAll(path.toStdString().c_str(),filelist,opc);
    for (auto v : filelist)
    {
        qfilelist.append(v.c_str());
    }
    return qfilelist;
}

int QFindFiles::fileList(const char *path, string_list &filelist)
{
    DIR * dir;
    struct dirent *entrada;

    if ((dir = opendir(path)) == nullptr)
        return 1;

    while ((entrada = readdir(dir)) != nullptr)
    {
        if ( strcmp(entrada->d_name,".") != 0 && strcmp(entrada->d_name,"..") )
        {
            string nombre;
            nombre += path;
            nombre += separador;
            nombre += entrada->d_name;
            if (entrada->d_type == DT_DIR)
                fileList(const_cast<char*>(nombre.c_str()),filelist);
            else
                filelist.push_back(nombre);
        }
    }
    return 0;
}

int QFindFiles::findAll(const char * path, string_list &listdata, int opc)
{
    DIR * dir;
    struct dirent *entrada;

    if ((dir = opendir(path)) == nullptr)
        return 1;

    if(listdata.empty() && opc==0) listdata.push_back(path);

    while ((entrada = readdir(dir)) != nullptr)
    {
        if ( strcmp(entrada->d_name,".") != 0 && strcmp(entrada->d_name,"..") )
        {
            string nombre;
            nombre += path;
            nombre += separador;
            nombre += entrada->d_name;
            if (entrada->d_type == DT_DIR)
            {
                if (opc == 0) listdata.push_back(nombre); //el nombre del directorio
                findAll(const_cast<char*>(nombre.c_str()),listdata,opc);
            }
            else
            {
                if (opc == 1) listdata.push_back(entrada->d_name); //solo el nombre de los ficheros
                if (opc == 2) listdata.push_back(nombre); //el nombre del directorio mas el del fichero
            }
        }
    }
    return 0;
}

int QFindFiles::findAll(const char * path, string_list &listPath, string_list &listFile)
{
    DIR * dir;
    struct dirent *entrada;

    if ((dir = opendir(path)) == nullptr)
        return 1;

    while ((entrada = readdir(dir)) != nullptr)
    {
        if ( strcmp(entrada->d_name,".") != 0 && strcmp(entrada->d_name,"..") )
        {
            string nombre;
            nombre += path;
            nombre += separador;
            nombre += entrada->d_name;
            if (entrada->d_type == DT_DIR)
            {
                findAll(const_cast<char*>(nombre.c_str()),listPath,listFile);
            }
            else
            {
                listFile.push_back(entrada->d_name); //solo el nombre de los ficheros
                listPath.push_back(path); //el nombre del directorio
            }
        }
    }
    return 0;
}


bool QFindFiles::copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit)
{
    QDir dir;
    dir.setPath(from_dir);

    from_dir += QDir::separator();
    to_dir += QDir::separator();

    foreach (QString copy_file, dir.entryList(QDir::Files))
    {
        QString from = from_dir + copy_file;
        QString to = to_dir + copy_file;

        if (QFile::exists(to))
        {
            if (replace_on_conflit)
            {
                if (!QFile::remove(to))
                {
                    return false;
                }
            }
            else
            {
                continue;
            }
        }

        if (!QFile::copy(from, to))
        {
            return false;
        }
    }

    foreach (QString copy_dir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString from = from_dir + copy_dir;
        QString to = to_dir + copy_dir;

        if (!dir.mkpath(to))
        {
            return false;
        }

        if (!copy_dir_recursive(from, to, replace_on_conflit))
        {
            return false;
        }
    }
    return true;
}



