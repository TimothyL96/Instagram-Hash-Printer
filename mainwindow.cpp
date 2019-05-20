#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtPrintSupport>
#include <QPrintDialog>
#include <QPrinter>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->setWindowIcon(QIcon(windowIcon));
	this->setWindowTitle(windowTitle);

	ui->pushButtonSearch->setEnabled(false);

	// Setup SIGNAL and SLOT
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_netwManagerFinished(QNetworkReply*)));

	for (int i = 1; i < 13; i++)
	{
		currentFrame = ui->centralWidget->findChild<QGraphicsView *>("graphicsView" + QString::number(i));
		currentFrame->viewport()->installEventFilter(this);
	}

	ui->pushButtonSearch->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
	delete currentFrame;
	delete manager;
	delete m_netwManager;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
		if (watched == ui->graphicsView1->viewport() && loaded[0] == true)
        {
			vio.setImage(nextCount * maxFrame);
            vio.exec();
            return false;
		}else if (watched == ui->graphicsView2->viewport() && loaded[1] == true)
        {
			vio.setImage(nextCount * maxFrame + 1);
            vio.exec();
            return false;
		}else if (watched == ui->graphicsView3->viewport() && loaded[2] == true)
		{
			vio.setImage(nextCount * maxFrame + 2);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView4->viewport() && loaded[3] == true)
		{
			vio.setImage(nextCount * maxFrame + 3);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView5->viewport() && loaded[4] == true)
		{
			vio.setImage(nextCount * maxFrame + 4);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView6->viewport() && loaded[5] == true)
		{
			vio.setImage(nextCount * maxFrame + 5);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView7->viewport() && loaded[6] == true)
		{
			vio.setImage(nextCount * maxFrame + 6);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView8->viewport() && loaded[7] == true)
		{
			vio.setImage(nextCount * maxFrame + 7);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView9->viewport() && loaded[8] == true)
		{
			vio.setImage(nextCount * maxFrame + 8);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView10->viewport() && loaded[9] == true)
		{
			vio.setImage(nextCount * maxFrame + 9);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView11->viewport() && loaded[10] == true)
		{
			vio.setImage(nextCount * maxFrame + 10);
			vio.exec();
			return false;
		}else if (watched == ui->graphicsView12->viewport() && loaded[11] == true)
		{
			vio.setImage(nextCount * maxFrame + 11);
			vio.exec();
			return false;
		}
	}

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::on_pushButtonExit_clicked()
{
    this->close();
}

void MainWindow::on_pushButtonSearch_clicked()
{
	removeAllFiles();

	for (int i = 0; i < maxFrame; i++)
		loaded[i] = false;

	ui->pushButtonSearch->setEnabled(false);
	hashTag = ui->lineEdit->text();
	if (hashTag == "")
    {
        ui->pushButton_Back->setEnabled(false);
        ui->pushButton_Next->setEnabled(false);
		ui->pushButtonSearch->setEnabled(true);
		ui->pushButtonSearch->setText(pushButtonSearchDefault);
        return;
    }

	nextCount = 0;
	nextCode = "";
    jpegData.clear();
	nodesArray = QJsonArray{};

	QString url = urlFirstPart;
	url.append(hashTag);
	url.append(urlSecondPart);

    QNetworkRequest req(url);

	manager->get(req);
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    //CONNECT: manager SIGNAL finished
    QString html = reply->readAll();

    ui->tabWidget->setCurrentIndex(1);
    // So create an object Json Document, by reading into it all the data from the response
    //QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

    if (html != "")
    {
		// First get all HTML into QJsonDocument
        QJsonDocument data = QJsonDocument::fromJson(html.toUtf8());

		// Get to the tags we need
		QJsonObject root = data.object();
		QJsonObject edgehashtagtomedia = root.value("graphql").toObject()
											 .value("hashtag").toObject()
											 .value("edge_hashtag_to_media").toObject();

		// pageInfo as the info of whether there's next page
		QJsonObject pageInfo = edgehashtagtomedia.value("page_info").toObject();

		// edges has all the latest images link we need
		QJsonArray edges = edgehashtagtomedia.value("edges").toArray();

		// Append to the QJsonValue
		foreach (const QJsonValue &nodesput, edges)
        {
			nodesArray.append(nodesput);
        }

		// Check if next page exists
		QJsonValue nextpage = pageInfo.value("end_cursor");
		if (pageInfo.value("has_next_page") == true)
			nextCode = nextpage.toString();
        else
			nextCode = "";
	}

	if (nextCode == "" && nodesArray.size() == 0)
	{
		QMessageBox::warning(this, "Empty", "No images found");
		ui->pushButtonSearch->setEnabled(true);
		ui->pushButtonSearch->setText(pushButtonSearchDefault);
		ui->tabWidget->setCurrentIndex(0);
		return;
	}

	if (nodesArray.size() < maxFrame * nextCount + maxFrame && nextCode != "")
        loadNext();
    else
	{
		for (int i = maxFrame * nextCount; i < nodesArray.size() && i < maxFrame * nextCount + maxFrame; i++)
        {
			//This will run if back then next
            if (i < jpegData.size())
            {
				setImage(i);
				continue;
            }

			if (nextCode == "")
				if (i >= nodesArray.size())
                    break;

			loadingGif(i % maxFrame + 1);

			// Get the specific node display url
			QJsonValue nodeURL = nodesArray.at(i).toObject().value("node").toObject()
														  .value("display_url");

			QUrl url(nodeURL.toString());
            request.setUrl(url);
            request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
            m_netwManager->get(request);
        }
    }
}

void MainWindow::slot_netwManagerFinished(QNetworkReply *reply)
{
    //CONNECT m_netwManager SIGNAL finished
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
        return;
    }

    jpegData += reply->readAll();
	setImage(-1);
}

void MainWindow::setImage(int nextcountset)
{
	QGraphicsScene *scene = new QGraphicsScene(this);

    if (nextcountset == -1)
    {
		int current = ((jpegData.size() - 1) % maxFrame) + 1;
        int currentSize = jpegData.size() - 1;

		QPixmap pixmap;
        if (currentSize < jpegData.size())
		{
            pixmap.loadFromData(jpegData.at(currentSize));
			scene->addPixmap(pixmap);
		}
        else
			return;

		currentFrame = ui->centralWidget->findChild<QGraphicsView *>("graphicsView" + QString::number(current));
		currentFrame->setScene(scene);
        //currentframe->setFixedSize(currentframe->width(), currentframe->height());
		currentFrame->fitInView(currentFrame->sceneRect(), Qt::KeepAspectRatio);

		if ((current == maxFrame && (nextCode != "" || nodesArray.size() > maxFrame * nextCount + maxFrame)) || current == nodesArray.size())
		{
			ui->pushButton_Next->setEnabled(true);
			ui->pushButtonSearch->setEnabled(true);
			ui->pushButton_Back->setText("Search");
			if (nextCount > 0)
			{
				ui->pushButton_Back->setEnabled(true);
				ui->pushButton_Back->setText("Back");
			}
		}

		loaded[current - 1] = true;

		QString filename = (vio.cacheFolder);
		filename.append(QString::number(jpegData.size()));
		QFile file(filename + vio.imageExtension);
		file.open(QFile::WriteOnly);
		file.write(jpegData.at(currentSize));
		file.close();

		//Empty the current QByteArrayList
		jpegData[currentSize].clear();
    }
    else
	{
		QPixmap pixmap = vio.getPixMap(QString::number(nextcountset + 1));
		scene->addPixmap(pixmap);

		currentFrame = ui->centralWidget->findChild<QGraphicsView *>("graphicsView" + QString::number(nextcountset % maxFrame + 1));
		currentFrame->setScene(scene);
        //currentframe->setFixedSize(currentframe->width(), currentframe->height());
		currentFrame->fitInView(currentFrame->sceneRect(), Qt::KeepAspectRatio);

		loaded[nextcountset % maxFrame] = true;

		if (nextcountset % maxFrame + 1 == maxFrame && (nextCode != "" || nodesArray.size() > maxFrame * nextCount + maxFrame))
		{
			ui->pushButton_Next->setEnabled(true);
			ui->pushButtonSearch->setEnabled(true);
			ui->pushButton_Back->setText(pushButtonSearchDefault);
			if (nextCount > 0)
			{
				ui->pushButton_Back->setEnabled(true);
				ui->pushButton_Back->setText(pushButtonBackDefault);
			}
		}
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::loadNext()
{
    QString url = "";
	if (jpegData.size() + 12 <= nodesArray.size())
    {
        url = "";
    }
    else
    {
		url = urlFirstPart;
		url.append(hashTag);
		url.append(urlSecondPart);

		if (nextCode != "")
			url.append(urlNextPagePart + nextCode);
        else
        {
            qDebug() << "Error in loadNext()!";
            return;
        }
    }
    QNetworkRequest req(url);
    manager->get(req);
}

void MainWindow::loadingGif(int viewno)
{
	movie = new QMovie(movieLoadingGif);

	// Set movie into a QLabel
	QLabel *gif_anim = new QLabel();
	gif_anim->setMovie(movie);

	// Start the movie
    movie->start();
    QApplication::processEvents();

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addWidget(gif_anim);

	currentFrameload = ui->centralWidget->findChild<QGraphicsView *>("graphicsView" + QString::number(viewno));

	currentFrameload->setScene(scene);
	currentFrameload->fitInView(currentFrameload->sceneRect(), Qt::KeepAspectRatio);
	currentFrameload->setFixedSize(currentFrameload->width(), currentFrameload->height());
}

void MainWindow::on_pushButton_SetDefaultPrinter_clicked()
{
	QMessageBox::warning(this, "Default Printer", "Set your default printer by right clicking your printer.");
	qPrint();
}

void MainWindow::qPrint()
{
	QPrinter printer(QPrinter::HighResolution);

	QPrintDialog dialog(&printer, this);
	dialog.setWindowTitle(tr("Print Document"));
	if (dialog.exec() != QDialog::Accepted) {
		// return;
	}

	QPrinterInfo a;
	a = QPrinterInfo::defaultPrinter();
	if (a.isNull())
		qDebug("default is null!");
}

void MainWindow::on_pushButton_Next_clicked()
{
    ui->pushButton_Next->setEnabled(false);
	ui->pushButton_Back->setEnabled(false);
	ui->pushButtonSearch->setEnabled(false);
	ui->pushButton_Back->setText(pushButtonBackLoading);
	nextCount++;

	for (int i = 0; i < 12; i++)
		loaded[i] = false;

    loadNext();
}

void MainWindow::on_pushButton_Back_clicked()
{
	nextCount--;
	if (nextCount == 0)
        ui->pushButton_Back->setEnabled(false);

	for (int current = nextCount * maxFrame, i = 1; current < nextCount * maxFrame + 12 && i <= 12; current++, i++)
    {
		QPixmap pixmap = vio.getPixMap(QString::number(current + 1));

        QGraphicsScene *scene = new QGraphicsScene(this);
        scene->addPixmap(pixmap);

		currentFrame = ui->centralWidget->findChild<QGraphicsView *>("graphicsView" + QString::number(i));
		currentFrame->setFixedSize(currentFrame->width(), currentFrame->height());
		currentFrame->setScene(scene);
		currentFrame->fitInView(currentFrame->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::on_pushButton_cancel_clicked()
{
	// Back to the first tab
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::removeAllFiles()
{
	QDir dir(vio.cacheFolder);
	dir.setNameFilters(QStringList() << "*.*");
	dir.setFilter(QDir::Files);

	foreach(QString dirFile, dir.entryList())
	{
		dir.remove(dirFile);
	}
}
