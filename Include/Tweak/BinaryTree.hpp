// Joan Andrés (@Andres6936) Github.

#ifndef TWEAK_BINARYTREE_HPP
#define TWEAK_BINARYTREE_HPP

#include <cstdint>

namespace Tweak
{
	template <typename ItemType>
	class TreeNode;

	enum class OrderType
	{
		PreOrder, InOrder, PostOrder
	};

	template <typename ItemType>
	class TreeType
	{

	private:

		/**
		 * @return The number of nodes in the tree.
		 */
		std::size_t countNodes(TreeNode<ItemType>* tree)
		{
			if (tree == nullptr)
			{
				return 0;
			}
			else
			{
				return countNodes(tree->left) + countNodes(tree->right) + 1;
			}
		}

	public:

		// Property

		TreeNode<ItemType>* root {nullptr};

		// Constructor

		TreeNode();

		TreeType(const TreeType<ItemType>& original);

		// Destructor

		~TreeType();

		// Operators

		void operator=(TreeType<ItemType>& original);

		// Methods

		void makeEmpty();

		/**
		 * @return True if the tree is empty and false otherwise.
		 */
		bool isEmpty() const;
		{
			return root == nullptr;
		}

		/**
		 * @return True if the free store has no room for another node and false otherwise.
		 */
		bool isFull() const
		{
			TreeNode<ItemType>* location;
			try
			{
				location = new TreeNode<ItemType>();
				delete location;
				return false;
			}
			catch (std::bad_alloc& exception)
			{
				return true;
			}
		}

		void insertItem(ItemType value);

		void deleteItem(ItemType value);

		void resetTree(OrderType order);

		void print(std::ofstream& outFile) const;

		void retrieveItem(ItemType& item, bool& found) const;

		void getNextItem(ItemType& item, OrderType order, bool& finished);

		std::size_t lengthIs() const
		{
			return countNodes(root);
		}

	};

	template <typename ItemType>
	class TreeNode
	{

	public:

		// Property

		ItemType info;

		// Children

		TreeNode<ItemType>* left { nullptr};

		TreeNode<ItemType>* right { nullptr};
	};

}

#endif //TWEAK_BINARYTREE_HPP
