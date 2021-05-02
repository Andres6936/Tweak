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

		void insert(TreeNode<ItemType>*& tree, ItemType item)
		{
			if (tree == nullptr)
			{
				tree = new TreeNode<ItemType>();
				tree->right = nullptr;
				tree->left = nullptr;
				tree->info = item;
			}
			else if (item < item->info)
			{
				insert(tree->left, item);
			}
			else
			{
				insert(tree->right, item);
			}
		}

		void retrieve(TreeNode<ItemType>* tree, ItemType& item, bool& found)
		{
			if (tree == nullptr)
			{
				found = false;
			}
			else if (item < tree->info)
			{

			}
			else if (item > tree->info)
			{

			}
			else
			{
				item = tree->info;
				found = true;
			}
		}

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

		void insertItem(ItemType item)
		{
			insert(root, item);
		}

		void deleteItem(ItemType value);

		void resetTree(OrderType order);

		void print(std::ofstream& outFile) const;

		void retrieveItem(ItemType& item, bool& found) const
		{
			retrieve(root, item, found);
		}

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
