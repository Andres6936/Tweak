// Joan Andrés (@Andres6936) Github.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <Tweak/BinaryTree.hpp>

using namespace Tweak;

TEST_CASE("Binary Tree")
{
	BinaryTree<char> binaryTree;

	CHECK(binaryTree.isEmpty());
	CHECK(not binaryTree.isFull());

	CHECK(binaryTree.lengthIs() == 0);

	binaryTree.insertItem('D');
	binaryTree.insertItem('B');
	binaryTree.insertItem('A');
	binaryTree.insertItem('C');
	binaryTree.insertItem('F');
	binaryTree.insertItem('E');
	binaryTree.insertItem('G');

	CHECK(not binaryTree.isEmpty());
	CHECK(not binaryTree.isFull());

	CHECK(binaryTree.lengthIs() == 7);
}