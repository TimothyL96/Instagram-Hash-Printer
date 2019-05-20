#ifndef VIEWIMAGE_H
#define VIEWIMAGE_H

#include <QDialog>

namespace Ui {
class viewImage;
}

class viewImage : public QDialog
{
    Q_OBJECT

public:
    explicit viewImage(QWidget *parent = 0);
    ~viewImage();

	void setImage(int img);
	QPixmap getPixMap(QString index);

	// Constant
	const QString iconPrint = ":/loadimg/if_print_172530.png";
	const QString iconBack = ":/loadimg/if_17_330399.png";
	const QString cacheFolder = "./cache/";
	const QString imageExtension = ".jpg";
	const QString imageError = ":/loadimg/if_ic_error_48px_351983.png";

private slots:
	void showEvent(QShowEvent *);
    void on_pushButton_back_clicked();
	void on_pushButton_print_clicked();

private:
    Ui::viewImage *ui;

	// Members
	int currentNum = 0;

};

#endif // VIEWIMAGE_H
