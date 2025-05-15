/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackManagerTest.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:35:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:35:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/CallbackManager.hpp"

TEST(CallbackManagerTest, DefaultConstructor)
{
    CallbackManager manager;
    EXPECT_FALSE(manager.hasPendingCallbacks());
}
