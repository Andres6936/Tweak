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

		void getPredecessor(TreeNode<ItemType>* tree, ItemType& data)
		{
			while (tree->right not_eq nullptr)
			{
				tree = tree->right;
			}

			data = tree->info;
		}

		void deleteNode(TreeNode<ItemType>*& tree)
		{
			ItemType data;
			TreeNode<ItemType>* tempPtr;

			tempPtr = tree;
			if (tree->left == nullptr)
			{
				tree = tree->right;
				delete tempPtr;
			}
			else if (tree->right == nullptr)
			{
				tree = tree->left;
				delete tempPtr;
			}
			else
			{
				getPredecessor(tree->left, data);
				tree->info = data;
				deleteItem(tree->left, data);
			}
		}

		void deleteItem(TreeNode<ItemType>*& tree, ItemType item)
		{
			if (item < item->info)
			{
				deleteItem(tree->left, item);
			}
			else if (item > tree->info)
			{
				deleteItem(tree->right, item);
			}
			else
			{
				deleteNode(tree);
			}
		}

		void printTree(TreeNode<ItemType>* tree, std::ofstream& outFile)
		{
			if (tree not_eq nullptr)
			{
				printTree(tree->left, outFile);
				outFile << tree->info;
				printTree(tree->right, outFile);
			}
		}

		void destroy(TreeNode<ItemType>*& tree)
		{
			if (tree not_eq nullptr)
			{
				destroy(tree->left);
				destroy(tree->right);
				delete tree;
			}
		}

		void copyTree(TreeNode<ItemType>*& copy, const TreeNode<ItemType>* original)
		{
			if (original == nullptr)
			{
				copy = nullptr;
			}
			else
			{
				copy = new TreeNode<ItemType>();
				copy->info = original.info;
				copyTree(copy->left, original->left);
				copyTree(copy->right, original->right);
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

		TreeNode() = default;

		TreeType(const TreeType<ItemType>& original)
		{
			copyTree(root, original.root);
		}

		// Destructor

		virtual ~TreeType()
		{
			destroy(root);
		}

		// Operators

		void operator=(const TreeType<ItemType>& original)
		{
			if (&original == this)
			{
				return;
			}

			destroy(root);
			copyTree(root, original.root);
		}

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

		void deleteItem(ItemType item)
		{
			deleteItem(root, item);
		}

		void resetTree(OrderType order);

		void print(std::ofstream& outFile) const
		{
			printTree(root, outFile);
		}

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
