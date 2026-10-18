#include "entrypoint.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <string>

int iResult;

void raylib_start(void)
{
  WSADATA wsaData;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0)
  {
    printf("WSAStartup failed: %d\n", iResult);
  }

  struct addrinfo *result = NULL,
                  *ptr = NULL,
                  hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

#define DEFAULT_PORT "80"

  // Resolve the server address and port
  iResult = getaddrinfo("example.com", "80", &hints, &result);
  if (iResult != 0)
  {
    printf("getaddrinfo failed: %d\n", iResult);
    WSACleanup();
  }

  SOCKET ConnectSocket = INVALID_SOCKET;

  // Attempt to connect to the first address returned by
  // the call to getaddrinfo
  ptr = result;

  // Create a SOCKET for connecting to server
  ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                         ptr->ai_protocol);

  if (ConnectSocket == INVALID_SOCKET)
  {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
  }
  
  // Connect to server.
  iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
  if (iResult == SOCKET_ERROR)
  {
    printf("Error at connect(): %ld\n", WSAGetLastError());
    closesocket(ConnectSocket);
    ConnectSocket = INVALID_SOCKET;
  }

  // Should really try the next address returned by getaddrinfo
  // if the connect call failed
  // But for this simple example we just free the resources
  // returned by getaddrinfo and print an error message

  freeaddrinfo(result);

  if (ConnectSocket == INVALID_SOCKET)
  {
    printf("Unable to connect to server!\n");
    WSACleanup();
  }

#define DEFAULT_BUFLEN 512

  int recvbuflen = DEFAULT_BUFLEN;

  const char *sendbuf = "this is a test";
  char recvbuf[DEFAULT_BUFLEN];

  // Send an initial buffer
  iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
  if (iResult == SOCKET_ERROR)
  {
    printf("send failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
  }

  printf("Bytes Sent: %ld\n", iResult);

  // shutdown the connection for sending since no more data will be sent
  // the client can still use the ConnectSocket for receiving data
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR)
  {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
  }

  // Receive data until the server closes the connection
  do
  {
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
      printf("Bytes received: %d\n", iResult);
    else if (iResult == 0)
      printf("Connection closed\n");
    else
      printf("recv failed: %d\n", WSAGetLastError());
  } while (iResult > 0);

  // shutdown the send half of the connection since no more data will be sent
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR)
  {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
  }

  // cleanup
  closesocket(ConnectSocket);
  WSACleanup();
}

 int main() {
    // 1. Définition des composants de l'URL
    std::string hote = "example.com";
    std::string chemin = "/index.html";

    // 2. Construction textuelle de la requête HTTP GET
    // Chaque ligne de l'en-tête DOIT se terminer par "\r\n" (Retour chariot + Fin de ligne)
    std::string requeteHttp =
        "GET " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "User-Agent: MonApplicationCpp/1.0\r\n" +
        "Accept: text/plain, application/json\r\n" +
        "Connection: close\r\n" + // Demande au serveur de fermer la connexion après la réponse
        "\r\n";                   // Une ligne VIDE obligatoire pour marquer la fin des en-têtes

    // 3. Affichage du texte brut qui sera envoyé sur le réseau
    std::cout << "--- REQUÊTE HTTP GET ---\n";
    std::cout << requeteHttp;
    std::cout << "------------------------------------------\n";

    return 0;
/*
     // 1. Définition des composants de l'URL
    std::string hote = "example.com";
    std::string chemin = "/index.html";

    // 2. Construction de la requête HTTP HEAD
    std::string requeteHttp =
        "HEAD " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "User-Agent: MonApplicationCpp/1.0\r\n" +
        "Accept: text/plain, application/json\r\n" +
        "Connection: close\r\n" +
        "\r\n";

    // 3. Affichage de la requête
    std::cout << "--- REQUETE HTTP HEAD ---\n";
    std::cout << requeteHttp;
    std::cout << "-------------------------\n";

    return 0;*/
}