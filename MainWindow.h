#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QOAuthHttpServerReplyHandler>
#include <QUrlQuery>
#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

class GraphClient : public QObject
{
    Q_OBJECT

public:
    explicit GraphClient(QObject *parent = nullptr);

    bool authenticated() const { return m_authenticated; }
    QString token() const { return m_token; }

signals:
    void authenticatedChanged();
    void tokenChanged();

public slots:
    void authenticate();

private:
    QOAuth2AuthorizationCodeFlow *m_authFlow;
    bool m_authenticated{false};

    QString m_token;
    QTimer m_refreshTokenTimer;
};
#endif // MAINWINDOW_H
