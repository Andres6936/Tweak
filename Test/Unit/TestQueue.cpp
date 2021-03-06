// Joan Andrés (@Andres6936) Github.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <Tweak/Queue.hpp>

using namespace Tweak;

TEST_CASE("Queue Type")
{
	Queue<char> queueChar;

	CHECK(queueChar.isEmpty());
	CHECK(not queueChar.isFull());

	queueChar.enqueue('A');
	queueChar.enqueue('B');
	queueChar.enqueue('C');
	queueChar.enqueue('D');

	CHECK(not queueChar.isEmpty());
	CHECK(not queueChar.isFull());

	char result;

	queueChar.dequeue(result);
	CHECK(result == 'A');
	queueChar.dequeue(result);
	CHECK(result == 'B');
	queueChar.dequeue(result);
	CHECK(result == 'C');
	queueChar.dequeue(result);
	CHECK(result == 'D');

	CHECK(queueChar.isEmpty());
	CHECK(not queueChar.isFull());
}