#ifndef MOVIEJACKET_H
#define MOVIEJACKET_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QtDebug>
#include <QStringListModel>
#include <QStandardItem>
#include <QUrl>
#include <QtNetwork>
#include <stdexcept>

#include <QXmlStreamReader>
#include <QMutex>

#include <QFile>
#include <QTextStream>

#include "movieimage.h"

using namespace std;

namespace Ui {
class MovieJacket;
}

class MovieJacket : public QMainWindow
{
    Q_OBJECT
public slots:
    void button1Click();
    void readDataSearch(QNetworkReply *r);
    void readDataMovie(QNetworkReply *r);
    void downloadBar(int, int);
private:
    QStringList *listTxtBox1, *listTxtBox2, *listTxtBox3, *listTxtBox4, *listTxtBox5, *listTxtBox6;
    void DirSearch(QString sDir, QString pattern);
    void makeMovieName();
    void initForms();
    void allocineSearch(QString movie);
    void allocineMovie(int codeOfMovie);
public:
    void writeLog(QString log, QString severity = "info");
public:
    explicit MovieJacket(QWidget *parent = 0);
    ~MovieJacket();
    
private:
    Ui::MovieJacket *ui;
    QHttp *http1;
    int indice;
    QFile logger;
};

#endif // MOVIEJACKET_H
