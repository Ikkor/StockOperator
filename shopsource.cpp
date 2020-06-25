#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using namespace std;
/*
A brief description: 
1. Contents from Stock.txt are loaded into arrays of size 1000.
2. Menu is shown with options: Cashier(1), Stock operator(2).
3. Cashier can record purchases(1) as well as query for an item's price(2).
4. While recording purchases, cashier can view purchase list at any time(-2).
5. When viewing purchases, cashier has option to remove/edit item quantity(-2) or confirm purchases(-3).
6. Confirmed purchases are recorded in a timestamped file and stocklist is updated accordingly.
7. Notifications for items that are low on stock will be displayed on stock operator's window.
8. Stock Operator will input barcode and if barcode exist, will be prompted to either edit item information(-3) or add qty of new arrivals(-2). Otherwise, re-enter barcode.
9. Use -1 at any time to go back

Note: 
i) While recording purchases, if the user exits and chooses stock operator, the purchase list will be cleared as the stockfile needs to be updated and reinitialized.
ii) Purchase list is reset after confirmation of purchase.

*/
// stocklist format: [barcode] [itemname] [qtyinstock] [reorderlevel] [buying price] [sellingprice] [%vat]

ifstream Stockread; 
ofstream Stockupdate;
ofstream TimestampedFile;

const int arrSize = 1000; //all array size

int ArrBarcode[arrSize]; //Product's barcode
string ArrProduct[arrSize];  //Product's name
int ReOrderLevel[arrSize]; //Product's Reorder level 
int ArrStockQuantity[arrSize]; //Current Stock Quantity of product
double ArrSellPrice[arrSize]; //selling price of product
double ArrBuyPrice[arrSize]; //buying price of product
double ArrVatPerItem[arrSize]; //Vat % of product
int ArrPurchaseQty[arrSize]; // purchase quantity per consumer
int ItemsWhichHaveLowStock[arrSize]; //this flags all items with low stock

int Userinput = 0; // user input variable used in almost all functions

void CheckInput(int &x) { //this function is for integer inputs. It makes sure that wrong datatype doesn't cause endless loops. it inputs data to the referenced parameter.

	for (;;) {
		if (cin >> x) {

			break;
		}
		else {
			
			cin.clear();
			cin.ignore();
		}

	}


}

//read stock.txt and store the contents into arrays 
int StockInitialize() { //loads all the data from stock.txt to the arrays.
	Stockread.open("stock.txt");
	if (Stockread.fail()) {
		cout << "Stock file is missing.\n";
		system("pause");
		exit(0); //if stock.txt doesn't exist, there is no point in running the program. 
	}



	for (int i = 0; i < arrSize; i++) { //initialise empty indexes to some values 

		ArrBarcode[i] = -1;
		ArrStockQuantity[i] = -1;
		ArrSellPrice[i] = -1;
		ArrBuyPrice[i] = -1;
		ArrVatPerItem[i] = -1;
		ReOrderLevel[i] = -1;
		ArrPurchaseQty[i] = 0;
		ItemsWhichHaveLowStock[i] = -1;

	}


	for (int i = 0; i < arrSize; i++) { //initialise the data from text into arrays

		Stockread >> ArrBarcode[i];

		Stockread >> ArrProduct[i];

		Stockread >> ArrStockQuantity[i];

		Stockread >> ReOrderLevel[i];

		Stockread >> ArrBuyPrice[i];

		Stockread >> ArrSellPrice[i];

		Stockread >> ArrVatPerItem[i];

		if (Stockread.eof())break;

	}

	for (int i = 0; i < arrSize; i++) { //Check items having low stock count
		if (ReOrderLevel[i] > ArrStockQuantity[i]) {
			ItemsWhichHaveLowStock[i] = 1;

		}

	}//below is just for debug 
	//for (int i = 0; i < arrSize; i++) {
	//	if (ArrSellPrice[i] == -1)break;
	//	cout << ArrBarcode[i] << " " << ArrProduct[i] <<" "<<ItemsWhichHaveLowStock[i]<< endl;//

	//}
	cout << "Sucessfully loaded stocklist!!\n";
	//cin.ignore();
	Stockread.close();
}

//function to return index position of an item by passing barcode as parameter
int SearchByBarcode(int x) {//this functions searches the item list and takes barcode as parameter


	for (int i = 0; i < arrSize; i++) {
		bool found = 0;
		if (x == ArrBarcode[i]) {
			return i;

		}
	}

	return -1; //return -1 if nothing is found
}

//function to update stock.txt 
void StockUpdate() { //update is done after purchases
	Stockupdate.open("stock.txt"); //inserts the arrays data to the stock.txt

	for (int i = 0; i < arrSize; i++) {
		if (ArrSellPrice[i] == -1)break;
		Stockupdate << ArrBarcode[i] << "\t" << ArrProduct[i] << "\t" << ArrStockQuantity[i] << "\t" << ReOrderLevel[i] << "\t" << ArrBuyPrice[i] << "\t" << ArrSellPrice[i] << "\t" << ArrVatPerItem[i] << endl;
	}
	cout << "Stock list updated successfully! \n";

	Stockupdate.close();


}

//function to edit purchase list [accessible when viewing purchase list from cashier operator]
void EditPurchase() {
	//User is required to input barcode of item wishing to edit, the quantities are displayed in realtime. -1 to go back.
	do {
		system("CLS");

	
		for (int i = 0; i < arrSize; i++) {
			if (ArrPurchaseQty[i] > 0) {

				cout << ArrBarcode[i] << " " << ArrProduct[i] << " x" << ArrPurchaseQty[i] << endl;
			}


		}
		cout << "-------------------\n";
		cout << "Editing purchase list..\n";
		cout << "Which item's quantity to edit? Input -1 to exit edit mode, 0 to remove from purchase list\n"; //to remove item from cart, set to purhchase qty 0

		int NewQuantity = 0;
		int IndexofQty = 0;

		cout << "Enter barcode: ";
		CheckInput(Userinput);

		if (Userinput == -1)return;
		IndexofQty = SearchByBarcode(Userinput);

		if (IndexofQty == -1) {
			cout << "Invalid Barcode!\n";
			system("Pause");
		}
		else if (ArrPurchaseQty[IndexofQty] <= 0) {
			cout << "User did not purchase this item. \n";
			system("Pause");

		}

		else {
			cout << "Enter new quantity for: " << ArrProduct[IndexofQty] << "\n";
			CheckInput(NewQuantity);
			if (NewQuantity < 0) {
				cout << "Cannot be negative!\n";
				system("Pause");

			}
			else if (NewQuantity > ArrStockQuantity[IndexofQty]) {
				cout << "Specified quantity is more than stock quantity.\n";
				system("Pause");


			}

			else
			{
				ArrPurchaseQty[IndexofQty] = NewQuantity;
			}
		}

	} while (Userinput != -1);

} 

//function to confirm purchases & print receipt [accessible when viewing purchase list from cashier operator]
void ConfirmPurchase() { //function to print receipt and create timestamped file when purchase is confirmed
	system("CLS");
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	string TimeStamp = to_string(lt.wDay) + "_" + to_string(lt.wMonth) + "_" + to_string(lt.wYear) + "_" + to_string(lt.wHour) + "_" + to_string(lt.wMinute) + "_" + to_string(lt.wSecond);
	TimestampedFile.open(TimeStamp + ".txt");
	
	double TotNoVat = 0;
	double TotInclVat = 0;
	double TotVatOnly = 0;

	cout << "Item Name" << "      " << "ItemQty" << "      " << "ItemPriceVatExcl" << "      " << "ItemPriceVatIncl\n";
	TimestampedFile << "Item Name" << "      " << "ItemQty" << "      " << "ItemPriceVatExcl" << "      " << "ItemPriceVatIncl\n";

	for (int i = 0; i < arrSize; i++) {
		if (ArrPurchaseQty[i] > 0) { //check if item is purchased

			double VatIncl = (ArrSellPrice[i] + (ArrSellPrice[i] * (ArrVatPerItem[i] / 100)))*ArrPurchaseQty[i]; //calculate price with vat
			double VatOnly = (ArrSellPrice[i] * (ArrVatPerItem[i] / 100))*ArrPurchaseQty[i]; //calculate how much vat per item
			TotInclVat += VatIncl; //Total price of all item with vat included
			TotVatOnly += VatOnly; // Total of the vats only
			TotNoVat += ArrSellPrice[i] * ArrPurchaseQty[i]; //total of price without vat
			cout << ArrProduct[i] << "       " << ArrPurchaseQty[i] << "            " << ArrSellPrice[i]*ArrPurchaseQty[i] << "                  " << VatIncl << endl;

			TimestampedFile<< ArrProduct[i] << "       " << ArrPurchaseQty[i] << "       " << ArrSellPrice[i]*ArrPurchaseQty[i] << "       " << VatIncl << endl;
			//TotVatOnly 
			ArrStockQuantity[i] = ArrStockQuantity[i] - ArrPurchaseQty[i];//subtract purchase qty from stock.
		}

	}//this piece of code above is for receipt that appears on console


	cout <<"----------------------------" << endl;
	cout <<"Total Excluding VAT: "<< TotNoVat << endl;
	cout << "Total VAT: " << TotVatOnly << endl;
	cout << "Total Incl. VAT: " << TotInclVat << endl;
	cout << "Date: " << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
	cout << "Time: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << endl;
	//the code above prints total details

	TimestampedFile <<"-----------------------------" << endl;
	TimestampedFile << "Total Excluding VAT: " << TotNoVat << endl;
	TimestampedFile << "Total VAT: " << TotVatOnly << endl;
	TimestampedFile << "Total Incl. VAT: " << TotInclVat << endl;
	TimestampedFile << "Date: " << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
	TimestampedFile << "Time: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << endl;

	cout << "\n\n";
	StockUpdate(); //update the stock.txt
	StockInitialize(); //load it again
	system("pause");
	system("cls");

	
}

//function to view purchase list(temporary receipt) [accessible from cashier operator]
void ViewPurchase() {  //function to display purchase list 

	while(true) {
		SYSTEMTIME lt;
		GetLocalTime(&lt);


		double TotNoVat = 0;
		double TotInclVat = 0;
		double TotVatOnly = 0;

		cout << "Item Name" << "      " << "ItemQty" << "      " << "ItemPriceVatExcl" << "      " << "ItemPriceVatIncl\n";
		for (int i = 0; i < arrSize; i++) {
			if (ArrPurchaseQty[i] > 0) { //check if item is purchased
				double VatIncl = (ArrSellPrice[i] + (ArrSellPrice[i] * (ArrVatPerItem[i] / 100)))*ArrPurchaseQty[i]; //calculate price with vat
				double VatOnly = (ArrSellPrice[i] * (ArrVatPerItem[i] / 100))*ArrPurchaseQty[i]; //calculate how much vat per item
				TotInclVat += VatIncl; //Total price of all item with vat included
				TotVatOnly += VatOnly; // Total of the vats only
				TotNoVat += ArrSellPrice[i] * ArrPurchaseQty[i]; //total of price without vat
				cout << ArrProduct[i] << "       " << ArrPurchaseQty[i] << "            " << ArrSellPrice[i]*ArrPurchaseQty[i] << "                  " << VatIncl << endl;
				//TotVatOnly 
			}

		}//this piece of code above is for receipt that appears on console


		cout << "----------------------------\n";
		cout << "Total Excluding VAT: " << TotNoVat << endl;
		cout << "Total VAT: " << TotVatOnly << endl;
		cout << "Total Incl. VAT: " << TotInclVat << endl;
		cout << "Date: " << lt.wDay << "/" << lt.wMonth << "/" << lt.wYear << endl;
		cout << "Time: " << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << endl;
		cout << "\n---------------------\n-1 to go back, -2 to edit purchase, -3 to confirm purchase\n";

		CheckInput(Userinput);

		switch (Userinput) {
		case -1: return;
		case -2: EditPurchase();
			break;
		case -3: ConfirmPurchase();
			return;

		default: cout << "Invalid input.\n";

		}


		cout << "\n\n";

		system("cls");
	}

}

//function for item price query [accessible from cashier operator]
void ItemPriceQuery() {
	//This function is for querying item's price
	system("CLS");
	cout << "\t\t Query for Item Price, -1 to exit\n";
	
	while (true) {
		cout << "Enter barcode: ";
		CheckInput(Userinput); 
		if (Userinput == -1) {
		
			return;
		}


		int index = SearchByBarcode(Userinput); //get position of item in array
		if (index == -1) { //if nothing is found
			cout << "Invalid barcode\n";
		}
		else {
			cout << ArrProduct[index] << " Rs" << ArrSellPrice[index] << "\n---------\n";
		}
	}
}

//function to record purchase by barcode [accessible from cashier operator]
void RecordPurchase() {
	//Cashier inputs barcode here. 
		for(;;){
			system("CLS");
			cout << "\t\tRecord Purchase\t\t\n\n '-2' to view purchases list, '-1' to exit\n\n";
			while (true) {
				cout << "Enter barcode: ";
				CheckInput(Userinput);

				if (Userinput == -1) { return; }

				else if (Userinput == -2) {
					system("CLS");
					ViewPurchase();
					break;
				}
				else {

					int index = SearchByBarcode(Userinput); //get position of item in array
					if (index == -1) { //if nothing is found
						cout << "Invalid barcode\n";
					}

					//check if stock quantity < re order level or if purchase qty > stock qty
					else if ((ArrPurchaseQty[index] > ArrStockQuantity[index]) || (ItemsWhichHaveLowStock[index] == 1))
					{
						cout << ArrProduct[index] << " is currently low on stock!\n" << endl; //if cashier tries buy low stock item or buy more than is available, prevent him!

					}
					else { //if everything is good, proceed to add in purchase list
						ArrPurchaseQty[index]++;
						cout << ArrProduct[index] << " " << ArrPurchaseQty[index] << " " << endl;
					}

				}
			}
	}
}
	
	
//the cashier operator function
void CashierOperator() {
	for (;;) {
		system("CLS");
		cout << "\t\tCashier Operator\t\t \n 1. Record Purchase 2. Price Query, -1 to exit\n\n";
		
			
		CheckInput(Userinput);


		switch (Userinput) {
		case -1: return;
		case 1: RecordPurchase();
			break;
		case 2: ItemPriceQuery();
			break;
		default: 
			cout << "Invalid input.\n";
			system("Pause");
			break;
		}
	}
}

//the stock operator function
void StockOperator() {
	while (true) {
	system("CLS");
	int NewQTY = 0;
	cout << "\t\t STOCK OPERATOR\n\n";
	for (int i = 0; i < arrSize; i++) { //display notifications first
		if (ItemsWhichHaveLowStock[i] == 1) {//the condition is self-explanatory
			cout << ArrBarcode[i] << "-" << ArrProduct[i] << " only " << ArrStockQuantity[i] << " remaining! " "[ReOrder Lvl: "<< ReOrderLevel[i] << "]\n";
		}
		else continue;
	}
		cout << "------------------------------\nEnter barcode (-1 to abort): ";

		CheckInput(Userinput);
		if (Userinput == -1)return;
		int index = SearchByBarcode(Userinput); //only 2 possibilities, either found item or not found(return -1)

		if (index == -1) {
	
			cout << "Item not found. ";
			system("pause");

		}
		else {
	
			
			cout << "Updating product: " << ArrProduct[index] << ", QTY:" << ArrStockQuantity[index] << "\n";
			cout << "\n-2. Add new delivery, -3. Edit item information\n";
			CheckInput(Userinput);
			if (Userinput == -1)return;
			if (Userinput == -2) {
				cout << "\nEnter amount of new delivery received: ";

				do {
					CheckInput(NewQTY);
					if (NewQTY == -1)return;
				} while (NewQTY < 0);



				ArrStockQuantity[index] += NewQTY;
				cout << "New QTY of " << ArrProduct[index] << " is: " << ArrStockQuantity[index] << "\n";
				system("pause");
			}
			else if (Userinput == -3) {
				string NewName;
				int NewSPrice;
				cout << "Enter new name: ";
				cin >> NewName;
				ArrProduct[index] = NewName;
				cout << "Enter new selling price: ";
				CheckInput(NewSPrice); //since input is integer, we validate it because inputing string might cause crash.
				ArrSellPrice[index] = NewSPrice;
				cout << "Product " << ArrBarcode[index] << " updated: \n Name: " << ArrProduct[index] << ", Sell. Price: " << ArrSellPrice << endl;
				system("pause");

			}
				
		}

		StockUpdate(); //stockfile needs to be updated.
		StockInitialize(); //stockfile needs to be loaded again
	
	}
	Userinput == 0; //since this is a global variable, it must be re-initialised or else the wrong inputs might cause a loop
}


int main()
{
	
	while (true) {
	StockInitialize();
		

		system("cls");	//system("cls") is used to clean the screen :)

		cout << "Welcome to small market subsystem, enter your input. \n 1: Cashier Operator\n 2. Stock Operator\n\n";

		CheckInput(Userinput);
			
	
		if (Userinput == -1)break;

		switch (Userinput) {
		case 1:
			system("cls");
			CashierOperator();
			break;
		case 2: 
			StockOperator();
			break;


		}
	}
	return 0;
}


	

	
