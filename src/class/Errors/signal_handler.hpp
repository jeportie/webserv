/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:27:26 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/06 17:29:53 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SIGNAL_HANDLER_HPP
# define SIGNAL_HANDLER_HPP

#include "../SocketManager/SocketManager.hpp"

void    handle_signal(int signum);
void    closeServer(SocketManager &sm);

#endif