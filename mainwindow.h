#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArrayList>
#include <QGraphicsView>
#include <QString>
#include <QJsonArray>
#include <QLabel>
#include "viewimage.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    bool eventFilter(QObject *watched, QEvent *event);

    void replyFinished(QNetworkReply *reply);
    void slot_netwManagerFinished(QNetworkReply *reply);

	void setImage(int nextcountset);
	void qPrint();
    void loadNext();
	void loadingGif(int viewno);

	void on_pushButtonExit_clicked();
	void on_pushButtonSearch_clicked();
	void on_pushButton_SetDefaultPrinter_clicked();
    void on_pushButton_Next_clicked();
    void on_pushButton_Back_clicked();
	void on_pushButton_cancel_clicked();
	void on_actionExit_triggered();

	private:

    Ui::MainWindow *ui;

	// Members
	QByteArrayList jpegData;
	int nextCount = 0;
	bool loaded[12] = { false }; // 0 to 11 for graphicsview 1 to 12
	QString nextCode = "";
	QString hashTag = "";
	QJsonArray nodesArray;
    QNetworkRequest request;
	QGraphicsView *currentFrame  = new QGraphicsView;
	QGraphicsView *currentFrameload  = new QGraphicsView;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QNetworkAccessManager *m_netwManager = new QNetworkAccessManager(this);
    QMovie *movie;
	viewImage vio;

	// Constants
	const QString windowTitle = "Hash Tag Printer"; // Application window title
	const QString windowIcon = ":/loadimg/icon.png"; // Application icon
	const int maxFrame = 12; // Number of image frames can be display at one time
	const QString movieLoadingGif = ":/loadimg/Spinner.gif";
	const QString pushButtonBackLoading = "Back ...... Please wait";
	const QString pushButtonBackDefault = "Back";
	const QString pushButtonSearchDefault = "Search";
	const QString urlFirstPart = "https://www.instagram.com/explore/tags/";
	const QString urlSecondPart = "/?__a=1";
	const QString urlNextPagePart = "&max_id=";

	// Methods
	void removeAllFiles();
};

#endif // MAINWINDOW_H
