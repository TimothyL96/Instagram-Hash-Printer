#include "viewimage.h"
#include "ui_viewimage.h"
#include <QFile>
#include <QPrinter>

viewImage::viewImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::viewImage)
{
    ui->setupUi(this);

	// Set window maximimze during launch
	// this->setWindowState(Qt::WindowMaximized);

	// Set the window as a popup
	// this->setWindowFlags(Qt::Popup);

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	QPixmap printico(iconPrint);
    QIcon printIcon(printico);
    ui->pushButton_print->setIcon(printIcon);
    ui->pushButton_print->setIconSize(QSize(60, 60));

	QPixmap backico(iconBack);
    QIcon backIcon(backico);
    ui->pushButton_back->setIcon(backIcon);
    ui->pushButton_back->setIconSize(QSize(60, 60));
}

viewImage::~viewImage()
{
    delete ui;
}

void viewImage::setImage(int img)
{
	currentNum = img + 1;

	// Get pixmap
	QPixmap pixmap = this->getPixMap(QString::number(img + 1));

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(pixmap);
    ui->graphicsView->setScene(scene);
}

void viewImage::showEvent(QShowEvent *)
{
    ui->graphicsView->fitInView(ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
}

void viewImage::on_pushButton_back_clicked()
{
    this->close();
}

void viewImage::on_pushButton_print_clicked()
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setPaperSize(QSize(102, 152), QPrinter::Millimeter);

	// Get pixmap
	QPixmap pixmap = this->getPixMap(QString::number(this->currentNum));

	// If Landscape
	if (pixmap.width() > pixmap.height())
	{
		qDebug("Landscape!");
		printer.setOrientation(QPrinter::Landscape);
		pixmap = pixmap.scaled(QSize(3591, 2409), Qt::KeepAspectRatio); //4R 4" × 6" or 102 × 152 mm
	}else
	{
		//Portrait
		qDebug("Portrait!");
		pixmap = pixmap.scaled(QSize(2409, 3591), Qt::KeepAspectRatio); //4R 4" × 6" or 102 × 152 mm
	}

	// Draw image to painter
	QPainter painter;
	painter.begin(&printer);
	painter.drawPixmap(0, 0, pixmap);
	painter.end();

	this->close();
}

QPixmap viewImage::getPixMap(QString index)
{
	QString filename = (this->cacheFolder);
	filename.append(index);

	QFile file(filename + this->imageExtension);
	file.open(QFile::ReadOnly);

	QPixmap pixmap;
	if (!pixmap.load(filename))
	{
		// What if image deleted? We will get them again here
		// show error image error.jpg
		pixmap.load(this->imageError);

		qDebug("No image to view!");
	}
	file.close();

	return pixmap;
}
