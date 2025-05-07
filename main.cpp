/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:49:58 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/02 15:19:03 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/webserv.h"
#include "src/class/Server.hpp"
#include <stdexcept>

int main()
{
    try
    {
        std::cout << "This is not yet a webserv X)" << std::endl;
        Server o;

        o.init_connect();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    }

    catch (const std::exception& e)
    {
        std::cerr << "Exceptions Error: " << e.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "Unknown Error: " << std::endl;
    }
    return (0);
}
