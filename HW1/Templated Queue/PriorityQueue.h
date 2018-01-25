#pragma once
#include <iostream>
using namespace std;

template <class T>
class PriorityQueue {
public:
	PriorityQueue() {
		headNode = nullptr;
		endNode = nullptr;
		count = 0;
	}

	void Push(T data) {
		endNode.nextNode = new node(data);
		endNode = endNode.nextNode;

		if (count = 0) {
			headNode = endNode;
		}

		count++;
	}

	T Pop() {
		if (count = 0) {
			return NULL;
		}
		
		T temp = headNode.data;
		headNode = headNode.nextNode;

		count--;
		return temp;
	}

	void Print() {
		node* temp = headNode;

		while (count != 0 && temp != nullptr) {
			cout << temp->data << ", ";
			temp = temp.nextNode;
		}
	}
private:
	node* headNode;
	node* endNode;
	int count;
};

template <class T>
struct node {
	node(T newData) {
		data = newData;
		nextNode = nullptr;
	}

	T data;
	node* nextNode;
};