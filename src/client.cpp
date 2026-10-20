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

  // Initialise Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0)
  {
    printf("WSAStartup failed: %d\n", iResult);
    return;
  }

  struct addrinfo *result = NULL,
                  *ptr = NULL,
                  hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

#define DEFAULT_PORT "8080"

  // Résoudre l’adresse et le port du serveur
  iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
  if (iResult != 0)
  {
    printf("getaddrinfo failed: %d\n", iResult);
    WSACleanup();
    return;
  }

  SOCKET ConnectSocket = INVALID_SOCKET;

  // Tenter de se connecter à la première adresse renvoyée par l'appel à getaddrinfo
  ptr = result;

  // Créer un socket pour vous connecter au serveur.
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

  int verb;

  std::cout << "Choisissez une methode HTTP :\n";
  std::cout << "1. GET\n";
  std::cout << "2. HEAD\n";
  std::cout << "3. PUT\n";
  std::cout << "4. DELETE\n";
  std::cout << "Votre choix : ";

  std::cin >> verb;

  std::string methode;
  std::string requeteHttp;

  if (verb == 1)
  {
    // 1. Définition des composants de l'URL
    std::string hote = "127.0.0.1";
    std::string chemin = "/index.html";

    // 2. Construction textuelle de la requête HTTP GET
    // Chaque ligne de l'en-tête DOIT se terminer par "\r\n" (Retour chariot + Fin de ligne)
    std::string requeteHttp =
        "GET " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "User-Agent: curl/8.6.0\r\n" +
        "Accept: */* \r\n" +
        "Connection: close\r\n" + // Demande au serveur de fermer la connexion après la réponse
        "\r\n";                   // Une ligne VIDE obligatoire pour marquer la fin des en-têtes

    // 3. Affichage du texte brut qui sera envoyé sur le réseau
    std::cout << "--- REQUÊTE HTTP GET ---\n";
    std::cout << requeteHttp;
  }
  else if (verb == 2)
  {
    std::string hote = "127.0.0.1";
    std::string chemin = "/";

    std::string requeteHttp =
        "HEAD " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "User-Agent: curl/8.6.0\r\n" +
        "Accept: / \r\n" +
        "Connection: close\r\n" +
        "\r\n";

    std::cout << "--- REQUETE HTTP HEAD ---\n";
    std::cout << requeteHttp;
  }
  else if (verb == 3)
  {
    std::string hote = "127.0.0.1";
    std::string chemin = "/nouveau.html";

    std::string corps = "Bonjour depuis C++ !";

    std::string requeteHttp =
        "PUT " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "Content-Type: text/plain; charset=UTF-8\r\n" +
        "Content-Length: " + std::to_string(corps.size()) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n" +
        corps;

    std::cout << "--- REQUETE HTTP PUT ---\n";
    std::cout << requeteHttp << "\n";
  }
  else if (verb == 4)
  {
    std::string hote = "127.0.0.1";
    std::string chemin = "/fichier.txt";

    std::string requeteHttp =
        "DELETE " + chemin + " HTTP/1.1\r\n" +
        "Host: " + hote + "\r\n" +
        "Connection: close\r\n" +
        "\r\n";

    std::cout << "--- REQUETE HTTP DELETE ---\n";
    std::cout << requeteHttp;
  }
  else
  {
    std::cout << "Cette méthode n'est pas dans les choix.";
  }

  // Envoyer un buffer initial
  iResult = send(ConnectSocket, requeteHttp.c_str(), (int)requeteHttp.length(), 0);
  if (iResult == SOCKET_ERROR)
  {
    printf("send failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
  }

  printf("Bytes Sent: %ld\n", iResult);

  // Fermer la connexion pour l'envoi, étant donné qu'aucune donnée
  //supplémentaire ne sera transmise;
  // Le client peut toutefois continuer à utiliser le
  //ConnectSocket pour recevoir des données.
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR)
  {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
  }

  // Recevoir des données jusqu’à ce que le serveur ferme la connexion
  do
  {
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
      std::cout.write(recvbuf, iResult);
    else if (iResult == 0)
      printf("Connection closed\n");
    else
      printf("recv failed: %d\n", WSAGetLastError());
  } while (iResult > 0);

  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR)
  {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
  }

  // Nettoyage
  closesocket(ConnectSocket);
  WSACleanup();
}

int main()
{
  raylib_start();

  return 0;
}