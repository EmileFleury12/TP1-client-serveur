#include "entrypoint.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
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

#define DEFAULT_PORT "8080"

  struct addrinfo *result = NULL, *ptr = NULL, hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the local address and port to be used by the server
  iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
  if (iResult != 0)
  {
    printf("getaddrinfo failed: %d\n", iResult);
    WSACleanup();
  }

  SOCKET ListenSocket = INVALID_SOCKET;

  // Create a SOCKET for the server to listen for client connections

  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (ListenSocket == INVALID_SOCKET)
  {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
  }

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR)
  {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(ListenSocket);
    WSACleanup();
  }

  freeaddrinfo(result);

  if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
  {
    printf("Listen failed with error: %ld\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
  }

  SOCKET ClientSocket;

  ClientSocket = INVALID_SOCKET;

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET)
  {
    printf("accept failed: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
  }

  // No longer need server socket
  closesocket(ListenSocket);

#define DEFAULT_BUFLEN 512

  char recvbuf[DEFAULT_BUFLEN];
  int iSendResult;
  int recvbuflen = DEFAULT_BUFLEN;

  // Receive until the peer shuts down the connection
  do
  {
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
      printf("Bytes received: %d\n", iResult);

      // Echo the buffer back to the sender
      iSendResult = send(ClientSocket, recvbuf, iResult, 0);
      if (iSendResult == SOCKET_ERROR)
      {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
      }
      printf("Bytes sent: %d\n", iSendResult);
    }
    else if (iResult == 0)
      printf("Connection closing...\n");
    else
    {
      printf("recv failed: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
      WSACleanup();
    }

  } while (iResult > 0);
}

int main()
{
  /*
    // 1. Définition des composants de l'URL
    std::string hote = "127.0.0.1";
    std::string chemin = "/nouveau.html";

    // 2. Données à envoyer au serveur
    std::string corps = "Bonjour depuis C++ !";

    // 3. Construction de la requête HTTP PUT
    std::string requeteHttp =
        "PUT " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "Content-Type: text/plain; charset=UTF-8\r\n" +
        "Content-Length: " + std::to_string(corps.size()) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n" +
        corps;

    // 4. Affichage de la requête
    std::cout << "--- REQUETE HTTP PUT ---\n";
    std::cout << requeteHttp << "\n";
    std::cout << "------------------------\n";

    return 0;

    // 1. Définition des composants de l'URL
    std::string hote = "127.0.0.1";
    std::string chemin = "/fichier.txt";

    // 2. Construction de la requête HTTP DELETE
    std::string requeteHttp =
        "DELETE " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "Connection: close\r\n" +
        "\r\n";

    // 3. Affichage de la requête
    std::cout << "--- REQUETE HTTP DELETE ---\n";
    std::cout << requeteHttp;
    std::cout << "--------------------------\n";

    return 0;*/
  raylib_start();

  return 0;
}