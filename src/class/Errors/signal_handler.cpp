/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:27:10 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/06 17:40:33 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signal_handler.hpp"
#include <signal.h>


extern volatile sig_atomic_t g_stop;

void handle_signal(int signum)
{
    (void) signum;
    g_stop = 1;
}

typedef std::map<int, ClientSocket*> ICMAP;

void closeServer(SocketManager &sm)
{
    // Fermer et delete tous les clients
    std::map<int, ClientSocket *>& clientSocketmap = sm.getClientMapNonConst();
    for (std::map<int, ClientSocket *>::iterator it = clientSocketmap.begin(); it != clientSocketmap.end(); ++it)
    {
        close(it->first);
        delete it->second;
    }
    clientSocketmap.clear();

    // Fermer tous les sockets d'écoute
    std::vector<ListeningSocket>& ListeningSocketVector = sm.getVectorListeningSocket();
    for (std::vector<ListeningSocket>::iterator it_ls = ListeningSocketVector.begin(); it_ls != ListeningSocketVector.end(); ++it_ls)
    {
        close(it_ls->getFd()); // Supposons que ListeningSocket a une méthode getFd()
    }
    ListeningSocketVector.clear();

    // Détruire tous les callbacks
    std::queue<Callback*>& queue = sm.getCallbackQueue().getQueue();
    while (!queue.empty())
    {
        delete queue.front();
        queue.pop();
    }
}



   ///  ICMAP         _clientSockets;     Close Socket Fd and delete    
   /// LSVECTOR      _listeningSockets;  < Close Listenniong Socket Fd
   /// CallbackQueue _callbackQueue;     < Simple callback queue