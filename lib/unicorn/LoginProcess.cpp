#include "LoginProcess.h"
#include "QMessageBoxBuilder.h"

#include <lastfm/ws.h>
#include <lastfm/misc.h>
#include <lastfm/XmlQuery>

#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QTcpServer>
#include <QTcpSocket>

#ifdef WIN32
#include <windows.h>
#endif

namespace unicorn
{

TinyWebServer::TinyWebServer( QObject* parent )
    : QObject( parent )
    , m_tcpServer( 0 )
{
    m_tcpServer = new QTcpServer( this );
    m_tcpServer->listen( QHostAddress( QHostAddress::LocalHost ), 0 );
    connect( m_tcpServer, SIGNAL( newConnection() ), this, SLOT( onNewConnection() ) );
}

void
TinyWebServer::onNewConnection()
{
    Q_ASSERT( m_tcpServer );
    m_clientSocket = m_tcpServer->nextPendingConnection();
    if ( m_clientSocket )
    {
        connect( m_clientSocket, SIGNAL( disconnected() ), m_clientSocket, SLOT( deleteLater() ) );
        connect( m_clientSocket, SIGNAL( readyRead() ), this, SLOT( readFromSocket() ) );
    }
}

int
TinyWebServer::serverPort() const
{
    Q_ASSERT( m_tcpServer );
    return m_tcpServer->serverPort();
}

QHostAddress
TinyWebServer::serverAddress() const
{
    Q_ASSERT( m_tcpServer );
    return m_tcpServer->serverAddress();
}

void
TinyWebServer::readFromSocket()
{
    Q_ASSERT( m_clientSocket );

    m_header += m_clientSocket->read( m_clientSocket->bytesAvailable() );
    if ( m_header.endsWith( "\r\n\r\n" ) )
    {
        processRequest();
        m_clientSocket->disconnectFromHost();
        m_tcpServer->close();
    }
}

void
TinyWebServer::processRequest()
{
    QRegExp rx( "token=(\\d|\\w)+" );
    if ( rx.indexIn( m_header ) != -1 )
    {
        m_token = rx.cap( 0 ).split( "=" )[ 1 ];
        sendRedirect();
        emit gotToken( m_token );
    }
}

void
TinyWebServer::sendRedirect()
{
    char* redirectHeader = "HTTP/1.1 302 Found\r\nLocation: http://www.last.fm/\r\n\r\n\0";
    m_clientSocket->write( redirectHeader ) ;

}


/** LoginProcess **/
LoginProcess::LoginProcess( QObject* parent )
    : QObject( parent )
    , m_webServer( 0 )
    , m_lastError( lastfm::ws::NoError )
    , m_lastNetworkError( QNetworkReply::NoError )
{
}

void
LoginProcess::authenticate()
{
    if ( m_webServer )
    {
        delete m_webServer;
        m_webServer = 0;
    }

    m_webServer = new TinyWebServer( this );

    m_authUrl = QUrl( "http://www.last.fm/api/auth/" );
    QString callbackUrl = "http://" + m_webServer->serverAddress().toString()
                          + ":" + QString::number( m_webServer->serverPort() );
    m_authUrl.addQueryItem( "api_key", lastfm::ws::ApiKey );
    m_authUrl.addQueryItem( "token", "" );
    m_authUrl.addQueryItem( "cb", callbackUrl );

    if ( QDesktopServices::openUrl( m_authUrl ) )
    {
        connect( m_webServer, SIGNAL( gotToken( QString ) ), this, SLOT( getSession( QString ) ) );
    }
}

QString
LoginProcess::token() const
{
    return m_token;
}

Session
LoginProcess::session() const
{
    return m_session;
}

QUrl
LoginProcess::authUrl() const
{
    return m_authUrl;
}

void
LoginProcess::getSession( QString token )
{
    m_token = token;
    connect( unicorn::Session::getSession( m_token ), SIGNAL( finished() ),
             this, SLOT( onGotSession() ) );
}


void
LoginProcess::onGotSession()
{
    try
    {
        m_session = unicorn::Session( static_cast<QNetworkReply*>( sender() ) );
        emit gotSession( m_session );
        delete m_webServer;
        m_webServer = 0;
    }
    catch ( lastfm::ws::ParseError& e )
    {
        m_lastError = e;
        qWarning() << e.what();
        if ( m_lastError.enumValue() == lastfm::ws::UnknownError )
        {
           m_lastNetworkError = ( QNetworkReply::NetworkError )static_cast<QNetworkReply*>( sender() )->error();
        }
    }
}

void
LoginProcess::cancel()
{
    disconnect( m_webServer, SIGNAL( gotToken( QString ) ), this, SLOT( getSession( QString ) ) );
    qDeleteAll( findChildren<QNetworkReply*>() );
}

void
LoginProcess::showError() const
{
    switch ( m_lastError.enumValue() )
    {
        case lastfm::ws::AuthenticationFailed:
            // COPYTODO
            QMessageBoxBuilder( 0 )
                    .setIcon( QMessageBox::Critical )
                    .setTitle( tr("Login Failed") )
                    .setText( tr("Sorry, we don't recognise that username, or you typed the password wrongly.") )
                    .exec();
            break;

        default:
            // COPYTODO
            QMessageBoxBuilder( 0 )
                    .setIcon( QMessageBox::Critical )
                    .setTitle( tr("Last.fm Unavailable") )
                    .setText( tr("There was a problem communicating with the Last.fm services. Please try again later.") )
                    .exec();
            break;

        case lastfm::ws::TryAgainLater:
        case lastfm::ws::UnknownError:
            switch ( m_lastNetworkError )
            {
                case QNetworkReply::ProxyConnectionClosedError:
                case QNetworkReply::ProxyConnectionRefusedError:
                case QNetworkReply::ProxyNotFoundError:
                case QNetworkReply::ProxyTimeoutError:
                case QNetworkReply::ProxyAuthenticationRequiredError: //TODO we are meant to prompt!
                case QNetworkReply::UnknownProxyError:
                case QNetworkReply::UnknownNetworkError:
                    break;
                default:
                    return;
            }

            // TODO proxy prompting?
            // COPYTODO
            QMessageBoxBuilder( 0 )
                    .setIcon( QMessageBox::Critical )
                    .setTitle( tr("Cannot connect to Last.fm") )
                    .setText( tr("Last.fm cannot be reached. Please check your firewall or proxy settings.") )
                    .exec();

#ifdef WIN32
            // show Internet Settings Control Panel
            HMODULE h = LoadLibraryA( "InetCpl.cpl" );
            if (!h) break;
            BOOL (WINAPI *cpl)(HWND) = (BOOL (WINAPI *)(HWND)) GetProcAddress( h, "LaunchConnectionDialog" );
            if (cpl) cpl( qApp->activeWindow()->winId() );
            FreeLibrary( h );
#endif
            break;
    }
}



}// namespace unicorn
