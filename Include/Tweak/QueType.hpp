// Joan Andrés (@Andres6936) Github.

#ifndef TWEAK_QUETYPE_HPP
#define TWEAK_QUETYPE_HPP

#include <cstdint>

namespace Tweak
{

	template <typename ItemType>
	class QueType
	{

	private:

		ItemType* items;

		std::size_t maxQue;

		std::size_t front;

		std::size_t near;

	public:

		QueType()
		{
			maxQue = 501;
			front = maxQue - 1;
			near = maxQue - 1;
			items = new ItemType[maxQue];
		}

		QueType(std::size_t max)
		{
			maxQue = max + 1;
			front = max - 1;
			near = max - 1;
			items = new ItemType[maxQue];
		}

		~QueType()
		{
			delete [] items;
		}

		void makeEmpty()
		{
			front = maxQue - 1;
			near = maxQue - 1;
		}

		bool isEmpty() const
		{
			return (near == front);
		}

		bool isFull() const
		{
			return ((near + 1) % maxQue == front);
		}

		void enqueue(ItemType item)
		{
			if (isFull())
			{
				throw "FullQueue";
			}
			else
			{
				near = (near + 1) % maxQue;
				items[near] = item;
			}
		}

		void dequeue(ItemType& item)
		{
			if (isEmpty())
			{
				throw "EmptyQueue";
			}
			else
			{
				front = (front + 1) % maxQue;
				item = items[front];
			}
		}

	};

}

#endif //TWEAK_QUETYPE_HPP
