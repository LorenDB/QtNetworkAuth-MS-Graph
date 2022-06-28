#include "MainWindow.h"

#include <QLabel>

GraphClient::GraphClient(QObject *parent)
    : QObject{parent},
      m_authFlow{new QOAuth2AuthorizationCodeFlow{this}}
{
    m_authFlow->setReplyHandler(new QOAuthHttpServerReplyHandler{1337, m_authFlow});
    m_authFlow->setAuthorizationUrl(QStringLiteral("https://login.microsoftonline.com/common/oauth2/v2.0/authorize"));
    m_authFlow->setAccessTokenUrl(QStringLiteral("https://login.microsoftonline.com/common/oauth2/v2.0/token"));
    m_authFlow->setScope(QStringLiteral("user.read"));
    m_authFlow->setClientIdentifier(QStringLiteral("<insert app id/client id here>"));
    m_authFlow->setState(QByteArrayLiteral("basically random"));

    connect(m_authFlow, &QOAuth2AuthorizationCodeFlow::statusChanged, this, [this](QAbstractOAuth::Status s) {
        if (s == QAbstractOAuth::Status::Granted)
        {
            qDebug("authed");
            m_authenticated = true;
            emit authenticatedChanged();

            m_token = m_authFlow->token();
            emit tokenChanged();

            m_refreshTokenTimer.start();
        }
    });
    connect(m_authFlow, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    // Graph API tokens expire every 60-90 minutes; since Qt doesn't provide a way to find the expiration time, we'll just
    // make sure we keep the access token up-to-date all the time
    m_refreshTokenTimer.setInterval(1000 * 60 * 50);
    m_refreshTokenTimer.setSingleShot(false);
    m_refreshTokenTimer.callOnTimeout(m_authFlow, &QOAuth2AuthorizationCodeFlow::refreshAccessToken);
}

void GraphClient::authenticate()
{
    m_authFlow->grant();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto l = new QLabel{"authing...", this};
    setCentralWidget(l);

    auto t = new GraphClient{this};
    connect(t, &GraphClient::authenticatedChanged, this, [t, l] {
        if (t->authenticated())
            l->setText("authed!");
        else
            l->setText("no auth :(");
    });
    t->authenticate();
}

MainWindow::~MainWindow()
{
}

