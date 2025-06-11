/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:27:10 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/10 15:57:51 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signal_handler.hpp"
#include <signal.h>
#include "../Callbacks/WriteCallback.hpp"


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
    if (sm.getEpollFd() != -1)
    {
        close(sm.getEpollFd());
    }

    std::map<int, CallbackQueue>& waitingList = sm.getWaitingList();
    std::map<int, CallbackQueue>::iterator it_waitlist = waitingList.begin();
    for(it_waitlist = waitingList.begin(); it_waitlist != waitingList.end(); ++it_waitlist)
    {
    while (!it_waitlist->second.getQueue().empty())
        {
            WriteCallback* to_delete =  (WriteCallback*)it_waitlist->second.getQueue().front();
            if(to_delete->get_file_fd() != -1)
                close (to_delete->get_file_fd());
            delete to_delete;
            it_waitlist->second.getQueue().pop();
        }
    }
 

    //traverser la waiting iste et fermer les file fd qui ne snt pas a -1; et supprimer lesobjet de la waiting
}



   ///  ICMAP         _clientSockets;     Close Socket Fd and delete    
   /// LSVECTOR      _listeningSockets;  < Close Listenniong Socket Fd
   /// CallbackQueue _callbackQueue;     < Simple callback queue