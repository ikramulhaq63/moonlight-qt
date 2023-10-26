// CustomNetworkManager.cpp
#include "CustomNetworkManager.h"
// Include other necessary headers

#include <iostream>
#include <ostream>
#include <cstring>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <fstream>
#include <ctime>

const char *hostname = "127.0.0.1:5679";
const char *apiPath = "/api/endpoint";
std::string postData = "";

CustomNetworkManager::CustomNetworkManager(QObject *parent) : QObject(parent) {}

int customErrorCallback(const char *str, size_t len, void *logStream)
{
    std::ostream *logFile = reinterpret_cast<std::ostream *>(logStream);
    (*logFile) << "OpenSSL Error: " << std::string(str, len) << std::endl;
    return 0;
}

void logOpenSSLErrors(std::ostream &logFile)
{
    ERR_print_errors_cb(customErrorCallback, &logFile);
}

void CustomNetworkManager::sendRequest()
{
    StreamingPreferences streamingPreferences;
    streamingPreferences.reload();
    postData = "{\"bandwidth\":\"" + std::to_string(streamingPreferences.bitrateKbps) + "Kbps\"}";

    // Generate a filename based on the current datetime
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);

    char filename[100];
    std::strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S.log", parts);

    // Create and open a log file
    std::ofstream logFile(filename);

    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file." << std::endl;
    }

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx)
    {
        logFile << "SSL_CTX_new() failed." << std::endl;
        logOpenSSLErrors(logFile);
    }

    SSL *ssl = SSL_new(ctx);
    if (!ssl)
    {
        logFile << "SSL_new() failed." << std::endl;
        logOpenSSLErrors(logFile);
    }

    BIO *bio = BIO_new_ssl_connect(ctx);
    if (!bio)
    {
        logFile << "BIO_new_ssl_connect() failed." << std::endl;
        logOpenSSLErrors(logFile);
    }

    BIO_set_conn_hostname(bio, hostname);
    if (BIO_do_connect(bio) <= 0)
    {
        logFile << "BIO_do_connect() failed." << std::endl;
        logOpenSSLErrors(logFile);
    }

    std::string request = "POST " + std::string(apiPath) + " HTTP/1.1\r\n";
    request += "Host: " + std::string(hostname) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + std::to_string(postData.length()) + "\r\n";
    request += "\r\n";
    request += postData;

    int bytesWritten = BIO_write(bio, request.c_str(), request.length());
    if (bytesWritten <= 0)
    {
        logFile << "BIO_write() failed." << std::endl;
        logOpenSSLErrors(logFile);
    }

    char buffer[1024];
    int bytesRead;
    while ((bytesRead = BIO_read(bio, buffer, sizeof(buffer))) > 0)
    {
        logFile.write(buffer, bytesRead);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    ERR_free_strings();

    logFile.close();
}
