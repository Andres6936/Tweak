// Joan Andrés (@Andres6936) Github.

#ifndef TWEAK_BINARYTREE_HPP
#define TWEAK_BINARYTREE_HPP

#include <cstdint>
#include <fstream>
#include <Tweak/Queue.hpp>

namespace Tweak
{
	template <typename ItemType>
	class TreeNode;

	enum class OrderType
	{
		PreOrder, InOrder, PostOrder
	};

	template <typename ItemType>
	class BinaryTree
	{

	private:

		// Property

		TreeNode<ItemType>* root {nullptr};

		Queue<ItemType> preQue;

		Queue<ItemType> inQue;

		Queue<ItemType> postQue;

		// Methods

		void preOrder(TreeNode<ItemType>* tree, Queue<ItemType>& preQueue)
		{
			if (tree not_eq nullptr)
			{
				preQue.enqueue(tree->info);
				preOrder(tree->left, preQue);
				preOrder(tree->right, preQue);
			}
		}

		void inOrder(TreeNode<ItemType>* tree, Queue<ItemType>& inQueue)
		{
			if (tree not_eq nullptr)
			{
				inOrder(tree->left, inQue);
				inQue.enqueue(tree->info);
				inOrder(tree->right, inQue);
			}
		}

		void postOrder(TreeNode<ItemType>* tree, Queue<ItemType>& postQueue)
		{
			if (tree not_eq nullptr)
			{
				postOrder(tree->left, postQue);
				postOrder(tree->right, postQue);
				postQue.enqueue(tree->info);
			}
		}

		void insert(TreeNode<ItemType>*& tree, ItemType item)
		{
			if (tree == nullptr)
			{
				tree = new TreeNode<ItemType>();
				tree->right = nullptr;
				tree->left = nullptr;
				tree->info = item;
			}
			else if (item < tree->info)
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
				copy->info = original->info;
				copyTree(copy->left, original->left);
				copyTree(copy->right, original->right);
			}
		}

		/**
		 * @return The number of nodes in the tree.
		 */
		std::size_t countNodes(TreeNode<ItemType>* tree) const
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

		// Constructor

		/**
		 * Initializes tree to empty state.
		 *
		 * @post Tree exists and is empty.
		 */
		BinaryTree() = default;

		BinaryTree(const BinaryTree<ItemType>& original)
		{
			copyTree(root, original.root);
		}

		// Destructor

		virtual ~BinaryTree()
		{
			destroy(root);
		}

		// Operators

		void operator=(const BinaryTree<ItemType>& original)
		{
			if (&original == this)
			{
				return;
			}

			destroy(root);
			copyTree(root, original.root);
		}

		// Methods

		/**
		 * Determine whether tree is empty.
		 *
		 * @post Function value = (tree is empty).
		 * @return True if the tree is empty and false otherwise.
		 */
		bool isEmpty() const
		{
			return root == nullptr;
		}

		/**
		 * Determine whether tree is full.
		 *
		 * @post Function value = (tree is full).
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

		/**
		 * Adds item to tree.
		 *
		 * @pre Tree is not full. Item is not in tree.
		 * @post Item in the tree. Binary search property is maintained.
		 *
		 * @param item
		 */
		void insertItem(ItemType item)
		{
			insert(root, item);
		}

		void deleteItem(ItemType item)
		{
			deleteItem(root, item);
		}

		void resetTree(OrderType order)
		{
			switch(order)
			{
			case OrderType::PreOrder:
				preOrder(root, preQue);
				break;
			case OrderType::InOrder:
				inOrder(root, inQue);
				break;
			case OrderType::PostOrder:
				postOrder(root, postQue);
				break;
			}
		}

		void print(std::ofstream& outFile) const
		{
			printTree(root, outFile);
		}

		/**
		 * Retrieves item whose key matches item's key (if present).
		 *
		 * @pre Key member of item is initialized.
		 * @post If there is a element someItem whose key matches item's key,
		 * then found = true and item is a copy of someItem; otherwise, found
		 * = false and item is unchanged. Tree is unchanged.
		 *
		 * @param item
		 * @param found
		 */
		void retrieveItem(ItemType& item, bool& found) const
		{
			retrieve(root, item, found);
		}

		void getNextItem(ItemType& item, OrderType order, bool& finished)
		{
			finished = false;
			switch(order)
			{
			case OrderType::PreOrder:
				preQue.dequeue(item);
				if (preQue.isEmpty())
				{
					finished = true;
				}
				break;
			case OrderType::InOrder:
				inQue.dequeue(item);
				if (inQue.isEmpty())
				{
					finished = true;
				}
				break;
			case OrderType::PostOrder:
				postQue.dequeue(item);
				if (postQue.isEmpty())
				{
					finished = true;
				}
				break;
			}
		}

		/**
		 * Determines the number of elements in tree.
		 *
		 * @post Function value = number of elements in tree.
		 */
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
