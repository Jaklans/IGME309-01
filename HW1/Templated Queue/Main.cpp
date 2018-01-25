#include "Main.h"

int main()
{
	Dequeue<int>* dQ = new Dequeue<int>();

	//Fill using Push_back
	for (int i = 1; i < 11; i++) {
		dQ->Push_back(i);
		dQ->Print();
	}

	//Print full capacity
	cout << endl << dQ->GetSize() << endl;

	//Empty using Pop_back
	for (int i = 1; i < 11; i++) {
		dQ->Pop_front();
		dQ->Print();
	}

	//Check to make sure empty
	dQ->Print();
	cout << endl << dQ->GetSize() << endl;

	//-------------------------------------//

	//Add to the begining and end
	for (int i = 1; i < 11; i++) {
		dQ->Push_front(i);
		dQ->Push_back(i);
		dQ->Print();
	}

	//Check size
	cout << endl << dQ->GetSize() << endl;

	//Remove from begining and end
	for (int i = 1; i < 11; i++) {
		dQ->Pop_front();
		dQ->Pop_back();
		dQ->Print();
	}


	//Call destructor!
	delete dQ;

	system("PAUSE");
}
