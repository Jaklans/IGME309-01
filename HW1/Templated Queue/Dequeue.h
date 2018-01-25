#pragma once
#include <iostream>
using namespace std;

template <class T>
class Dequeue
{
public:
	//Basic Constructor
	Dequeue() {
		data = new T[4];  
		count = 0;
		headIndex = 0;
		tailIndex = 0;
		capacity = 4;
	}

	//Copy Constructor
	Dequeue(const Dequeue& other) {
		//Copy the data over to a new array
		data = new T[other.count];
		memcpy(data, other.data, sizeof T * other.capacity);

		headIndex = other.headIndex;
		tailIndex = other.tailIndex;
		count = other.count;
		capacity = other.capacity;
	}

	//Copy Assignment Constructor
	Dequeue& operator=(const Dequeue& other) {
		//Assign the new data to the same location as the other
		data = other.data;
		headIndex = other.headIndex;
		tailIndex = other.tailIndex;
		count = other.count;
		capacity = other.capacity;
	}

	//Destructor
	~Dequeue() { 
		delete[] data;
		data = nullptr;
	}

	void Push_front(T data) {
		//Resize if head and tail will be the same
		if ((headIndex - 1) % capacity == tailIndex % capacity && count > 0) {
			ResizeArray();
		}
		headIndex--;
		this->data[headIndex % capacity] = data;
		count++;
	}

//BOUNUS Dequeue Functionality
	void Push_back(T data) {
		//Resize if head and tail will be the same
		if (headIndex % capacity == (tailIndex + 1) % capacity && count > 0) {
			ResizeArray();
		}
		if (count > 0) { tailIndex++; }
		this->data[tailIndex % capacity] = data;
		count++;
	}
	T Pop_front() {
		if (count <= 0) { return T(); }
		headIndex++;
		count--;
		return data[headIndex % capacity];
	}
	T Pop_back() {
		if (count <= 0) { return T(); }
		tailIndex--;
		count--;
		if (tailIndex < 0) { tailIndex += capacity; }
		return data[tailIndex % capacity];
	}

	void Print() {
		if (IsEmpty()) { cout << endl; return; }
		for (int i = 0; i < count - 1; i++) {
			cout << data[(headIndex + i) % capacity] << ", ";
		}
		cout << data[tailIndex % capacity] << endl;
	}

	int GetSize()
	{
		return count;
	}

	bool IsEmpty() { return count <= 0; }
//BOUNUS [] Operator Overload
	T operator[](int i) {
		return data[i - (headIndex % capacity)];
	}

protected:
	T * data;
	int headIndex;
	int tailIndex;
	int capacity;
	int count;

	void ResizeArray() {
		T* temp = new T[capacity * 2];
		memcpy(temp, data, sizeof T * capacity);
		//If the head is greater than the tail, the back
		//half of the array must be copied so that itteration
		//can be done propperly
		if (headIndex % capacity > tailIndex % capacity) {
			memcpy(
				//Destination: capacity plus the head to the
				//end of the new array
				temp + capacity + headIndex % capacity,
				//Source: the head to the end of the old array
				data + headIndex % capacity,
				sizeof T * (capacity - headIndex % capacity));
			headIndex += capacity;
			tailIndex %= capacity;
		}
		delete[] data;
		data = temp;
		capacity *= 2;
	}
};