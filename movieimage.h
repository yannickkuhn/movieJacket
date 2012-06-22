#ifndef MOVIEIMAGE_H
#define MOVIEIMAGE_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QImage>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QPixmap>
#include <QDate>
#include <QBuffer>
#include <QHttp>
#include <QUrl>

class MovieImage : public QMainWindow
{
    Q_OBJECT
public:
    explicit MovieImage(QWidget *parent = 0);
private:
    QString fileName, title, synopsis, releaseDate, releaseYear, country, actors, directors, runtime, pressRating;
    QBuffer *buffer;
    QByteArray bytes;
    int Request;
    QImage imgPict;

    void paintEvent(QPaintEvent *e);
    void grab();
public:
    QHttp *http;
    void setVars(QString fileName, QString title, QString synopsis, QString runtime, QString releaseDate, QString pressRating, QString directors, QString actors, QString urlPoster);
signals:
    
public slots:
    void Finished(int requestId, bool error){
        if (Request == requestId){
            imgPict.loadFromData(bytes);
            this->update();
            this->grab();
        }
    }
};

#endif // MOVIEIMAGE_H
