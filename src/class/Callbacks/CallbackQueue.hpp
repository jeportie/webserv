/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueue.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 17:59:13 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACKQUEUE_HPP
#define CALLBACKQUEUE_HPP

#define LOG_CBQUEU_CONST "CallbackQueue Constructor called"
#define LOG_CBQUEU_DEST "CallbackQueue Destructor called"
#define LOG_CBQUEU_FULL "Error: Callback Queu is full!"
#define LOG_CBQUEU_NULL "Error: Null callback in queue!"
#define LOG_CBQUEU_UNKWN "Unknown error executing callback"

#include <cstddef>
#include <queue>
#include "Callback.hpp"

class CallbackQueue
{
public:
    CallbackQueue();
    ~CallbackQueue();

    void   push(Callback* callback);
    void   processCallbacks();
    bool   tryExecute(Callback* callback);
    bool   isEmpty() const;
    size_t size() const;

private:
    std::queue<Callback*> _queue;
};

#endif  // ********************************************** CALLBACKQUEUE_HPP //
