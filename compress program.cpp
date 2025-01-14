#include <iostream>

#include <map>

#include <fstream>

#include <string>

#include <vector>

#include <algorithm>

using namespace std;
// to make the program faster comment all the debug statments
string input(string text)
{

	string x;

	cout << text;

	getline(cin >> ws, x);//to take the white space

	cout << endl;

	return x;

}

class byte_int
{

private:

	short pewpew(short x)//because 2 ^ n is an operation happend within specific range i will use cache memoery concept, its o(1) complex time now
	{

		switch (x)
		{

		case 0:
			return 1;
		case 1:
			return 2;
		case 2:
			return 4;
		case 3:
			return 8;
		case 4:
			return 16;
		case 5:
			return 32;
		case 6:
			return 64;
		case 7:
			return 128;

		}

	}

	void reverse(string& x)
	{

		int i = -1, j = 8;

		while (i + 1 != j)
		{

			i++;

			j--;

			x[i] += x[j];

			x[j] = x[i] - x[j];

			x[i] -= x[j];

		}

	}

public:

	int to_int(string byte)
	{

		short result = 0;

		for (int i = 0; i <= 7; i++)
			if (byte[i] - 48 == 1)// to make the ascii value equal 0 or 1 (from  number char format to number int format);
				result += pewpew(i);


		return result;
	}

	string to_byte(int num)
	{

		string result = "00000000";

		int c = 0;
		short i = 7, n;
		while (num != 0)
		{
			//cout << num << endl;
			n = pewpew(i);

			if (num >= n)
			{
				result[c] = '1';

				num -= n;
			}
			else
				result[c] = '0';

			i--;

			c++;

			//cout << "res == " << result << endl;

		}

		//cout << "out \n";

		reverse(result);

		//cout << "after [" << result << "]" << endl;

		return result;

	}

};


class compress// three main steps , 1 get the charachters repeated number, 2 make the huffman tree, 3 make the compressed file
{

private:

	struct hufftree
	{

		bool operator()(const pair<char, int>& x, const pair<char, int>& y)
		{

			return x.second > y.second;

		}

	};


	void debug1(map<char, int> x)//for debug
	{

		for (auto it : x)
			cout << it.first << ", " << it.second << endl;
		cout << endl;

	}

	string return_char(string& bits)
	{

		string res = "", newbits = "";

		int ssize = bits.size();
		cout << "size of bits string  = " << ssize << endl;
		for (int i = 0; i < ssize; i++)
			if (i < 8)
				res += bits[i];
			else
				newbits += bits[i];

		cout << " after <<<<<<< size of bits string  = " << newbits.size() << endl;


		bits = newbits;

		return res;

	}

	string fillwith_bits(string x, int& number)
	{

		number = 7 - x.size();//start from 1

		for (int i = 0; i <= number; i++)
			x += '0';
		return x;
	}

public:

	map <char, int> step1(string name)
	{

		fstream file;

		file.open(name, ios::in | ios::binary);

		map <char, int> iter; //iteration 

		char temp;

		while (file.get(temp))
			iter[temp]++;

		file.close();

		return iter;
	}

	vector <pair <char, string>> step2(map<char, int> iter_mapp, string name)
	{

		vector <pair<char, int>> x(iter_mapp.begin(), iter_mapp.end());
		sort(x.begin(), x.end(), hufftree());

		vector <pair <char, string>>huffmancode;



		name += ".huff";
		ofstream huff(name, ios::out | ios::binary);

		string temp = "";

		for (int i = 0; i < x.size(); i++)
		{

			if (i + 1 == x.size())
			{


				huff.put(x[i].first);

				for (int j = 0; j < temp.size(); j++)
					huff.put(temp[j]);
				huff.put('\\');
				huffmancode.push_back(make_pair(x[i].first, temp));
			}
			else
			{
				cout << x[i].first << temp + '1' << endl;

				huff.put(x[i].first);

				for (int j = 0; j < temp.size(); j++)
					huff.put(temp[j]);
				huff.put('1');
				huff.put('\\');
				huffmancode.push_back(make_pair(x[i].first, temp + '1'));
			}
			temp += '0';
		}

		huff.close();

		return huffmancode;

	}

	void step3(vector <pair <char, string>> code, fstream file, string new_name)
	{

		ofstream newfile(new_name + ".comp", ios::out | ios::binary);

		//put vector code into map to get random access to the elements

		map <char, string> code_map(code.begin(), code.end());


		byte_int change;

		char temp;

		int num_of_trash;//number of trash bits that will be in the last byte

		string bits = "";

		bool gate = true;

		bool gate2 = true; //to stop the bits from increas after arrive
		while (true)
		{

			if (!file.get(temp))
				gate = false;

			cout << "char[" << temp << "],,,,,,,,,,";
			if (gate2 && gate)//the orginal was gate2
				bits += code_map[temp];

			cout << "bits -* [" << bits << ']' << endl;

			if (gate)
			{

				if (bits.size() >= 8)//the size function return number start from 1
				{
					string z = return_char(bits);

					cout << "string of chars --->" << z << endl;
					cout << "the int " << (char)change.to_int(z) << endl;
					newfile.put(change.to_int(z));//call 1 is for extract the byte , 2 for convert to int , 3 to put char in the newfile

				}
			}
			else
			{
				cout << "lol\n\n\n";
				if (gate2)
					gate2 = false;

				if (bits.size() >= 8)//the size function return number start from 1
				{
					cout << "here\n";
					newfile.put(change.to_int(return_char(bits)));//call 1 is for extract the byte , 2 for convert to int , 3 to put char in the newfile
					//make a condition if bits.size() == 0 then break
				}
				else if (bits.size() < 8 && bits.size() != 0)
				{
					cout << "end , size = " << bits.size() << "\\" << endl;
					newfile.put(change.to_int(fillwith_bits(bits, num_of_trash)));
					break;
				}
				else
					break;

			}

		}

		cout << " (remember this )number of trash bytes = " << num_of_trash << "\n";

		newfile.close();

	}

	void the_operation(string name_of_file)//to compress
	{

		map <char, int> iter = step1(name_of_file);
		compress b;

		string huff_name = input("enter a name for the assitant file : ");
		string new_name = input("enter the name of the result file : ");

		/* making the huffman tree and extract the code of each char but will not do that
		there is a sequence in huffmantree and it is (from the most iteration char to the least)
		1, 01, 001, 0001, 00001, 00000 , steps of step two take the elements out of the map then sort it in vector
		and then type on screen the results or in file or or or but in this version i will type it on the screen*/

		vector <pair <char, string>> tree = step2(iter, huff_name);

		// step3 will take each char from the original file then put in string the code that presient that char and when complete byte will put it in the new file

		step3(tree, fstream(name_of_file, ios::in | ios::binary), new_name);

	}

};

class decompress
{
	/*
	 it consist of 2 steps
	 one - is taking the huffman code from file

	 two - extract bytes and compare it with the huffman code map to add a char into the new  file
	*/
private:

	string enter_string(string temp)
	{

		cout << temp;

		getline(cin >> ws, temp);

		cout << endl;

		return temp;

	}

	int trashnumber()
	{

		int x;

		cout << "enter trash number : ";

		cin >> x;

		cout << endl;

		return x;

	}

	map <string, char> step1(string name)
	{

		map <string, char> codemap;

		fstream hufffile(name, ios::in | ios::binary);


		string temp = "", t = "";
		char charr, huff_char;

		//bool gate = true;
		bool gate = 1;
		while (hufffile.get(charr))
		{

			if (gate)
				huff_char = charr;


			if (charr == '\\')
			{

				codemap.insert({ temp, huff_char });
				temp = "";
				gate = true;
			}
			else if (!gate)
				temp += charr;

			if (charr != '\\')
				gate = false;

		}

		//for (auto out : codemap)//for debug
			//cout << out.first << ",,[" << out.second << "],,,,\n";

		hufffile.close();

		return codemap;

	}

	void get_char(bool& gate, string& bits, map<string, char> codemap, string& key, ofstream& file)
	{

		int i = 0;



		while (bits[i] != '\0')
		{

			key += bits[i];

			bool it = codemap.count(key);

			//cout << "bits = " << bits << endl;

			//cout << "key = " << key << endl;

			//cout << "it = " << it << endl;

			if (it) // when key string match any key of map 
			{

				cout << "char --> [" << codemap[key] << ']' << endl;

				file.put(codemap[key]);

				key = "";

			}

			i++;
		}

		bits = "";

	}

public:

	void step2(string old, string neww, map <string, char> code, int trash)//old is for the compreesed file, and neww is for the decoded file, and trash is for number of trashbits that geted added in compress operation
	{

		ofstream decoded(neww, ios::out | ios::binary);

		fstream encoded(old, ios::in | ios::binary);

		//cout << encoded.is_open() << endl;
		//cout << decoded.is_open() << endl;

		byte_int conv;

		char num;

		string temp, tool = "", container = "";

		bool gate = false;

		char lastbyte = encoded.get();

		char r;

		while (encoded.get(num))
		{

			r = num;

			num = lastbyte;

			lastbyte = r;


			cout << (int) (unsigned char) num << ", " << num << endl;

			container = conv.to_byte((int)(unsigned char)num);

			cout << "container = " << container << endl;



			temp += container;

			get_char(gate, temp, code, tool, decoded);//**

			cout << endl;
			/*cout << "res = " << num << endl << endl;

			if (gate)
				decoded.put(num);*/



		}
		cout << "end";
		container = conv.to_byte((int)(unsigned char)lastbyte);

		for (int i = 0; i < container.size() - trash; i++)
			temp += container[i];

		get_char(gate, temp, code, tool, decoded);

		decoded.close();
		encoded.close();

	}

	void operation()
	{

		string tree = enter_string("enter the name of huffman code file : ");

		string name_of_old = enter_string("enter name of file to decompress : ");

		map <string, char> huffmap = step1(tree);

		int trash = trashnumber();

		string new_file = enter_string("enter name and extention for the decompreesed file : ");

		step2(name_of_old, new_file, huffmap, trash);

	}

};

int
main()
{

	string x = input("enter 1 for compress, 2 for decompress, anything else to end : ");

	if (x[0] == '1' && x.size() == 1)
	{
		compress b;

		x = input("enter the name of the file with the extention , and with the path if its not in the same path as the compress program : ");

		b.the_operation(x);
	}
	else if (x[0] == '2' && x.size() == 1)
	{

		//x = input("enter the name of the file with the extention , and with the path if its not in the same path as the compress program : ");
		decompress c;

		c.operation();

	}

	return 0;

}
