// Joan Andrés (@Andres6936) Github.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <Tweak/QueType.hpp>

using namespace Tweak;

TEST_CASE("Queue Type")
{
	QueType<char> queueChar;

	CHECK(queueChar.isEmpty());
	CHECK(not queueChar.isFull());
}