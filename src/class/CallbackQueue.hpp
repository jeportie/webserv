/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueue.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACKQUEUE_HPP
#define CALLBACKQUEUE_HPP

#include <queue>
#include "Callback.hpp"

class CallbackQueue
{
public:
    CallbackQueue();
    ~CallbackQueue();

    void push(Callback* callback);
    void processCallbacks();
    bool isEmpty() const;
    size_t size() const;

private:
    std::queue<Callback*> _queue;
};

#endif  // ********************************************** CALLBACKQUEUE_HPP //
