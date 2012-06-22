#include "movieimage.h"

MovieImage::MovieImage(QWidget *parent) :
    QMainWindow(parent)
{
    this->resize(1024,768);
}

void MovieImage::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QImage *img2 = new QImage("image2.jpg");
    QImage *img4 = new QImage("etoile.png");
    QImage *img5 = new QImage("etoileJaune.png");
    QFont font("arial",43, QFont::Bold);
    QFont font8("arial",16, QFont::Bold);
    QFont font1("arial",10, QFont::Normal);
    QFont font2("arial",10, QFont::Black);
    QFont font3("arial",20, QFont::Black);
    QFont font4("arial",18, QFont::Black);
    QFont font5("arial",13, QFont::Black);
    QFont font6("arial",13, QFont::Bold);
    QFont font7("arial",10, QFont::Bold);
    //img1->scaled(1400,1000);
    QImage *img = new QImage("design.png");

    p.drawImage(0,0,img2->scaledToWidth(1224));
    p.drawImage(0,0,*img);
    p.drawImage(80,357,imgPict.scaled(268,375));

    int i, j;
    j = this->pressRating.toDouble();
    for(i=0;i<5;i++)
    {
        if(i < j)
            p.drawImage(397+i*30,462,*img5);
        else
            p.drawImage(397+i*30,462,*img4);
    }



    // le titre
    if(this->title.size() < 10)
        p.setFont(font);
    else
        p.setFont(font8);

    p.drawText(403,453,title);

    p.setFont(font1);
    p.drawText(817,479,runtime);
    p.drawText(917,479,releaseYear);
    p.drawText(810,532,200,200,Qt::AlignLeft,directors);
    p.drawText(810,595,200,200,Qt::AlignLeft,actors);

    p.setFont(font3);
    p.drawText(408,711,"16:9");
    p.drawText(504,711,"24.");
    p.drawText(772,711,"AVC");
    p.drawText(860,711,"MKV");

    p.setFont(font7);
    p.drawText(542,698,tr("Hz"));
    p.setFont(font6);
    p.drawText(542,712,tr("00"));

    p.setFont(font4);
    p.drawText(584,709,"1080p");

    p.setFont(font2);
    p.drawText(403,500,364,136,Qt::AlignLeft | Qt::TextWordWrap, synopsis);

    p.setFont(font5);
    p.drawText(427,657,"FR");
}

// fait une capture d'ecran des informations récupérées du film
void MovieImage::grab()
{
    QPixmap pix(500,500);
    pix = QPixmap::grabWidget(this,0,0,1024,768);
    pix.save(this->fileName);
}

void MovieImage::setVars(QString fileName, QString title, QString synopsis, QString runtime, QString releaseDate, QString pressRating, QString directors, QString actors, QString urlPoster)
{
    this->fileName = fileName;
    this->title = title;
    this->synopsis = synopsis;
    this->runtime = runtime;
    this->releaseDate = releaseDate;
    this->pressRating = pressRating;
    this->directors = directors;
    this->actors = actors;

    // l'annee de production
    this->releaseYear = this->releaseDate;

    // la duree du film
    int hours, minutes;
    minutes = (this->runtime.toInt() % 3600) / 60;
    hours = this->runtime.toInt() / 3600;
    this->runtime = QString::number(hours) + "h" + QString::number(minutes);

    // les acteurs et realisateurs

    QStringList directorsList = this->directors.split(", ");
    if(directorsList.count() > 2)
        this->directors = directorsList.at(0) + "\n" + directorsList.at(1);
    else
        this->directors = this->directors.replace(", ","\n");

    QStringList actorsList = this->actors.split(", ");
    if(actorsList.count() > 4)
        this->actors = actorsList.at(0) + "\n" + actorsList.at(1) + "\n" + actorsList.at(2) + "\n" + actorsList.at(3);
    else
        this->actors = this->actors.replace(", ","\n");

    // image du poster
    QUrl url(urlPoster);
    http = new QHttp(this);
    connect(http, SIGNAL(requestFinished(int, bool)),this, SLOT(Finished(int,bool)));
    buffer = new QBuffer(&bytes);
    buffer->open(QIODevice::WriteOnly);
    http->setHost(url.host());
    Request=http->get (url.path(),buffer);
}
