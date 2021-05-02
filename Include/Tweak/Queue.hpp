// Joan Andrés (@Andres6936) Github.

#ifndef TWEAK_QUEUE_HPP
#define TWEAK_QUEUE_HPP

#include <cstdint>

namespace Tweak
{

	template <typename ItemType>
	class Queue
	{

	private:

		ItemType* items;

		std::size_t maxQue;

		std::size_t front;

		std::size_t rear;

	public:

		Queue()
		{
			maxQue = 501;
			front = maxQue - 1;
			rear = maxQue - 1;
			items = new ItemType[maxQue];
		}

		Queue(std::size_t max)
		{
			maxQue = max + 1;
			front = max - 1;
			rear = max - 1;
			items = new ItemType[maxQue];
		}

		~Queue()
		{
			delete [] items;
		}

		void makeEmpty()
		{
			front = maxQue - 1;
			rear = maxQue - 1;
		}

		bool isEmpty() const
		{
			return (rear == front);
		}

		bool isFull() const
		{
			return ((rear + 1) % maxQue == front);
		}

		void enqueue(ItemType item)
		{
			if (isFull())
			{
				throw "FullQueue";
			}
			else
			{
				rear = (rear + 1) % maxQue;
				items[rear] = item;
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

#endif //TWEAK_QUEUE_HPP
