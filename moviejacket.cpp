#include "moviejacket.h"
#include "ui_moviejacket.h"

MovieJacket::MovieJacket(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MovieJacket)
{
    ui->setupUi(this);
    initForms();
    this->setWindowTitle("Programme générateur de jacquettes de films");
    QObject::connect(ui->button1,SIGNAL(clicked()),this,SLOT(button1Click()));

    logger.setFileName("log_movieJacket.txt");
    if (!logger.open(QIODevice::WriteOnly | QIODevice::Text))
        QMessageBox::information(this,"fichier","Le fichier de log n'a pas pu etre ouvert !");

    writeLog("Début de l'application ...");
}

// initialisation des listes 4,5 et 6
void MovieJacket::initForms()
{
    http1 = new QHttp(this);

    indice = 0;

    ui->progressBar1->setValue(0);

    listTxtBox1 = new QStringList();
    listTxtBox2 = new QStringList();
    listTxtBox3 = new QStringList();

    // les listes de mots
    listTxtBox4 = new QStringList();
    listTxtBox5 = new QStringList();
    listTxtBox6 = new QStringList();

    QFile *f = new QFile(QCoreApplication::applicationDirPath() + "\\list1.txt");
    if(!f->open(QIODevice::ReadOnly))
        QMessageBox::warning(this,"fichier list1.txt","ce fichier n'existe pas ou n'est pas disponible !");
    else
    {
        QTextStream in(f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            listTxtBox4->append(line);
        }
    }
    f->close();

    f = new QFile(QCoreApplication::applicationDirPath() + "\\list2.txt");
    if(!f->open(QIODevice::ReadOnly))
        QMessageBox::warning(this,"fichier list2.txt","ce fichier n'existe pas ou n'est pas disponible !");
    else
    {
        QTextStream in(f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            listTxtBox5->append(line);
        }
    }
    f->close();

    f = new QFile(QCoreApplication::applicationDirPath() + "\\list3.txt");
    if(!f->open(QIODevice::ReadOnly))
        QMessageBox::warning(this,"fichier list3.txt","ce fichier n'existe pas ou n'est pas disponible !");
    else
    {
        QTextStream in(f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            listTxtBox6->append(line);
        }
    }
    f->close();

    ui->listBox4->setModel(new QStringListModel(*listTxtBox4));
    ui->listBox5->setModel(new QStringListModel(*listTxtBox5));
    ui->listBox6->setModel(new QStringListModel(*listTxtBox6));
}

// ecrire un log
void MovieJacket::writeLog(QString log, QString severity)
{
    QTextStream flux(&logger);
    flux.setCodec("UTF-8");
    flux << "[le " << QDate::currentDate().toString() << " ] - " << severity << " - " << log << endl;
}

// clique sur le bouton "start"
void MovieJacket::button1Click()
{
    QString dossier = QFileDialog::getExistingDirectory(this);
    if(dossier != "")
    {
        ui->textBox1->setText(dossier);
        DirSearch(dossier,"*.avi");
        makeMovieName();

        QString nameOfMovie;
        nameOfMovie = listTxtBox3->at(0);
        allocineSearch(nameOfMovie);
    }
}

// rechercher les fichiers dans un dossier en fonction d'une expression reguliere
void MovieJacket::DirSearch(QString sDir, QString pattern)
{
    try
    {
        QDir dir(sDir);
        dir.setFilter(QDir::Files);

        QStringList dirFiles = dir.entryList(QStringList(pattern));
        for (int i = 0; i < dirFiles.size(); ++i)       // pour tous les fichiers récupérés (locaux)
        {
            QString f = dirFiles.at(i);
            listTxtBox2->append(ui->textBox1->toPlainText() + "\\" + f.left(f.lastIndexOf(".")) + ".png");
            listTxtBox1->append(f);
        }

        ui->listBox1->setModel(new QStringListModel(*listTxtBox1));
        ui->listBox2->setModel(new QStringListModel(*listTxtBox2));
    }
    catch(exception e)
    {
        // gestion des erreurs ...
    }
}

// modifie les noms de films
void MovieJacket::makeMovieName()
{
    QString newName = "";
    QString file = "";
    QString item = "";
    int myIndex = 0;
    for (int i = 0; i < listTxtBox1->size(); ++i)
    {
        file = listTxtBox1->at(i);
        newName = file.toUpper();
        newName = newName.replace(".AVI","");

        for (int j = 0; j < listTxtBox4->size(); ++j)
        {
            item = listTxtBox4->at(j);
            if(newName.indexOf(item) != -1)
            {
                int discriminator = newName.indexOf(item);
                newName = newName.left(discriminator);
            }

        }

        for (int j = 0; j < listTxtBox5->size(); ++j)
        {
            item = listTxtBox5->at(j);
            newName = newName.replace(item,"");
        }

        newName = newName.replace(".","");
        newName = newName.replace("-","");
        newName = newName.replace("_","");
        listTxtBox3->append(newName);
        if(QFile::exists(listTxtBox2->at(myIndex)))
        {
            QListWidgetItem *item = new QListWidgetItem(newName);
            item->setCheckState(Qt::Unchecked);
            ui->checkedListBox1->addItem(item);
        }
        else
        {
            QListWidgetItem *item = new QListWidgetItem(newName);
            item->setCheckState(Qt::Checked);
            ui->checkedListBox1->addItem(item);
        }
        myIndex++;
    }
    ui->listBox3->setModel(new QStringListModel(*listTxtBox3));
}

void MovieJacket::allocineSearch(QString movie)
{
    QNetworkAccessManager *m = new QNetworkAccessManager();
    m->get(QNetworkRequest(QString("http://api.allocine.fr/rest/v3/search?partner=YW5kcm9pZC12M3M&format=xml&q=") + movie));
    writeLog("(search) lancement d'une requete du nom de film : "+movie+" ...");

    QObject::connect(m,SIGNAL(finished(QNetworkReply*)),this,SLOT(readDataSearch(QNetworkReply*)));
}
void MovieJacket::allocineMovie(int codeOfMovie)
{
    QNetworkAccessManager *m = new QNetworkAccessManager();
    m->get(QNetworkRequest(QString("http://api.allocine.fr/rest/v3/movie?partner=YW5kcm9pZC12M3M&format=xml&code=") + QString::number(codeOfMovie)));
    writeLog("(movie) lancement d'une requete sur le code "+QString::number(codeOfMovie)+" ...");

    QObject::connect(m,SIGNAL(finished(QNetworkReply*)),this,SLOT(readDataMovie(QNetworkReply*)));
}

void MovieJacket::readDataSearch(QNetworkReply *r)
{
    QString text = QString::fromUtf8(r->readAll().constData());
    QXmlStreamReader xml(text);
    QXmlStreamAttributes atb;
    int flag = 0, nbMovies = 0, code = 0;

    int flagFirstMovie = 0;

    while(!xml.atEnd())
    {
        xml.readNext();
        atb = xml.attributes();
        if(xml.name() == "results")
        {
            if(atb.value("type").toString() == "movie")
            {
                nbMovies = xml.readElementText().toInt();
                writeLog("(search) on obtient "+QString::number(nbMovies)+" resultats ...");
                if(nbMovies == 0)
                {
                    writeLog("(search) aucun résultat pour ce film, on passe au suivant ...");
                    QString nameOfMovie;
                    indice = indice + 1;
                    nameOfMovie = listTxtBox3->at(indice);
                    allocineSearch(nameOfMovie);
                }
            }
        }

        if(xml.name() == "movie")
        {
            code = atb.value("code").toString().toInt();
            if(code != 0)   // on est sur de bien etre dans un film
                flag = 1;
            else
                flag = 0;   // si on repasse sur une fermeture d'un movie
        }
        if(flag == 1)
        {
            if(nbMovies == 1)   // si on trouve un seul film
            {
                // on récupère ce film
                writeLog("(search) Pour ce resultat, on a bien un code de film :"+QString::number(code)+", on lance allocineMovie ...");
                allocineMovie(code);
            }
            else
            {
                if(flagFirstMovie != 1)     // lorsqu'on a une liste de films, on ne prend que le premier de cette liste ...
                {
                    writeLog("(search) Pour ce resultat, on a une liste de films, le code actuel est le :"+QString::number(code)+", on lance allocineMovie ...");
                    allocineMovie(code);
                }
                flagFirstMovie = 1;
            }
            flag = 0;
        }
    }
}
void MovieJacket::readDataMovie(QNetworkReply *r)
{
    QString movieFile = QString::fromUtf8(r->readAll().constData());
    QXmlStreamReader xml(movieFile);
    QXmlStreamAttributes atb;

    QString title, synopsisShort, runtime, releaseDate, pressRating, directors, actors, urlPoster;

    while(!xml.atEnd())
    {
        xml.readNext();
        atb = xml.attributes();

        if(xml.name() == "originalTitle")
            title = xml.readElementText();

        if(xml.name() == "synopsisShort")
            synopsisShort = xml.readElementText();

        if(xml.name() == "runtime")
            runtime = xml.readElementText();

        if(xml.name() == "productionYear")
            releaseDate = xml.readElementText();

        if(xml.name() == "pressRating")
            pressRating = xml.readElementText();

        if(xml.name() == "directors")
            directors = xml.readElementText();

        if(xml.name() == "actors")
            actors = xml.readElementText();

        if(xml.name() == "poster")
        {
            foreach(QXmlStreamAttribute att, atb)
            {
                if(att.name().toString() == "href")
                    urlPoster = att.value().toString();
            }
        }
    }

    writeLog("(movie) resultats : 0) "+listTxtBox2->at(indice)+", 1)"+title+", 2)"+synopsisShort+", 3)"+runtime+", 4)"+releaseDate+", 5)"+pressRating+", 6)"+directors+", 7)"+actors+", 8)"+urlPoster+" ...");
    if(pressRating == "")
        writeLog("(movie) il n'y a pas de note pour ce film ...");
    if(urlPoster == "")
    {
        urlPoster = "http://www.eusa.ed.ac.uk/asset/Event/11192/Films.jpg";
        writeLog("(movie) il n'y a pas de pochette pour ce film ...");
    }

    if(title != "" && synopsisShort != "" && runtime != "" && releaseDate != "" && directors != "" && actors != "")
    {
        writeLog("(movie) on creer un movieImage ...");
        MovieImage *img = new MovieImage(this);
        img->setVars(listTxtBox2->at(indice), title, synopsisShort, runtime, releaseDate, pressRating, directors, actors, urlPoster);
        connect(img->http,SIGNAL(dataReadProgress(int,int)),this,SLOT(downloadBar(int,int)));

        if(indice < listTxtBox3->count() - 1) {
            QString nameOfMovie;
            indice = indice + 1;
            nameOfMovie = listTxtBox3->at(indice);
            allocineSearch(nameOfMovie);
        }

    }
}

void MovieJacket::downloadBar(int done, int total)
{
    ui->progressBar1->setValue(done);
    ui->progressBar1->setMaximum(total);
}

MovieJacket::~MovieJacket()
{
    delete ui;
}
