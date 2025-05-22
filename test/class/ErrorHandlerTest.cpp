/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandlerTest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:45:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:45:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Errors/ErrorHandler.hpp"

TEST(ErrorHandlerTest, Singleton)
{
    ErrorHandler& handler1 = ErrorHandler::getInstance();
    ErrorHandler& handler2 = ErrorHandler::getInstance();
    
    // Both references should point to the same instance
    EXPECT_EQ(&handler1, &handler2);
    
    // Default state
    EXPECT_FALSE(handler1.shouldShutdown());
}
