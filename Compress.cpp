//Tarek Khalifa
#include <iostream>
#include <string>
#include <fstream>
#include <bitset>
#include <map>
#include <queue>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <string.h>
using namespace std;
map<char, string> codes;
map<string, char> codes2;
int wordsread = 0, wordswritten = 0, byteswritten, bytesindecompress = 0;
double bytesshouldwrite = 0, H = 0;
string inputfilename;
string outputfilename;
unsigned long long letters[256] = {0};
struct node
{
	char character;
	int count = 0;
	string code = "";
	node *right = NULL;
	node *left = NULL;
	node *parent = NULL;
};
struct bigger
{
	bool operator()(const node *a, const node *b) const
	{
		return a->count > b->count;
	}
};
priority_queue<node *, vector<node *>, bigger> heapify()
{
	ifstream ifile(inputfilename.c_str());
	unsigned char currentletter;
	priority_queue<node *, vector<node *>, bigger> heap;
	while (!ifile.eof())
	{
		ifile >> noskipws >> currentletter;
		letters[int(currentletter)]++;
		wordswritten++;
	}

	wordswritten--;
	letters[int(currentletter)]--;
	for (int i = 0; i < 256; i++)
		if (letters[i] > 0)
		{
			node *x = new node;
			x->count = letters[i];
			x->character = i;
			x->right = NULL;
			x->left = NULL;
			heap.push(x);
		}
	ifile.close();
	node *h;
	priority_queue<node *, vector<node *>, bigger> heap2;
	heap2 = heap;
	int ii = heap.size();
	for (int i = 0; i < ii; i++)
	{
		h = heap.top();
		cout << h->character << "\t" << h->count << endl;
		heap.pop();
	}
	return heap2;
}
void giveCode(node *tree, string name)
{
	if (tree->right != NULL && tree->left != NULL)
	{
		if (tree->parent == NULL)
		{
		}
		else
		{
			tree->code = tree->parent->code + name;
		}
		giveCode(tree->right, "1");
		giveCode(tree->left, "0");
	}
	else
	{
		tree->code = tree->parent->code + name;
		codes.insert({tree->character, tree->code});
		cout << tree->character << "\t" << tree->code << endl;
	}
}
void makeTree(priority_queue<node *, vector<node *>, bigger> h)
{
	if (h.size() == 1)
	{
		node *tree = new node;
		tree = h.top();
		tree->code = "0";
		codes.insert({tree->character, "0"});
		h.pop();
		cout << tree->character << "\t" << tree->code << endl;
		return;
	}
	else
	{
		while (h.size() > 1)
		{
			node *one = new node;
			node *two = new node;
			node *result = new node;
			one = h.top();
			h.pop();
			two = h.top();
			h.pop();
			one->code = "1";
			two->code = "0";
			one->parent = result;
			two->parent = result;
			result->right = one;
			result->left = two;
			result->count = one->count + two->count;
			result->parent = NULL;
			h.push(result);
		}
		node *tree = new node;
		tree = h.top();
		cout << endl;
		cout << endl;
		cout << endl;
		giveCode(tree, "");
	}
}
void compress()
{
	ifstream ifile(inputfilename.c_str());
	ofstream ofile(outputfilename.c_str(), std::ofstream::out | std::ofstream::trunc | ios::binary);
	map<char, string>::iterator mapi;

	ofile << codes.size() << "\t" << wordswritten << " ";
	for (mapi = codes.begin(); mapi != codes.end(); mapi++)
	{
		ofile << noskipws << mapi->first << '=' << mapi->second << ';';
		bytesshouldwrite += mapi->second.length()* (double)letters[mapi->first]/(double)wordswritten;
		H += (double)letters[mapi->first] / (double)wordswritten * (double)log2(1.0 / ((double)letters[mapi->first] / (double)wordswritten));
		//cout << mapi->second.length() << " " << letters[mapi->first] << " " << wordswritten << " " << bytesshouldwrite << endl;
	}
	bitset<8> bits = 0;
	char byte, temp;
	ifile.get(temp);
	string tempcode = codes[temp];
	int ib = 7;
	int is = 0;
	while (!ifile.eof())
	{
		//cout << "Hello" << endl;
		while ((ib >= 0) && (is < tempcode.length()))
		{
			bits[ib] = (tempcode[is] == '1') ? 1 : 0;
			ib--;
			is++;
		}
		if (ib == -1)
		{
			byte = (char)bits.to_ulong();
			ofile << noskipws << byte;
			byteswritten++;
			bits = 0;
			ib = 7;
		}
		if (is == tempcode.length())
		{
			ifile.get(temp);
			if (ifile.eof())
			{
				byte = (char)bits.to_ulong();
				ofile << noskipws << byte;
				byteswritten++;
			}
			tempcode = codes[temp];
			is = 0;
		}
	}
	ifile.close();
	ofile.close();
}
void decompress()
{
	ifstream ifile(inputfilename.c_str(), ios_base::in | ios_base::binary);
	ofstream ofile(outputfilename.c_str());
	int n, words;
	char currentbyte;
	char equal;
	char character;
	ifile >> (n);
	ifile >> (words);
	ifile >> noskipws >> equal;
	string currentcode = "";
	for (int i = 0; i < n; i++)
	{
		currentcode = "";
		ifile.get(character);
		ifile.get(equal);
		if (equal != '=')
			cout << "wrong" << endl;
		ifile.get(currentbyte);
		while (currentbyte != ';')
		{
			currentcode += currentbyte;
			ifile.get(currentbyte);
		}

		codes2.insert({currentcode, character});
	}
	map<string, char>::iterator mapi;
	for (mapi = codes2.begin(); mapi != codes2.end(); mapi++)
		cout << noskipws << mapi->second << '\t' << mapi->first << endl;
	int ib = 7;
	string codetofind = "";
	bitset<8> bits = 0;
	ifile.get(currentbyte);
	bits = currentbyte;
	while (!ifile.eof() || wordsread < words)
	{
		while (ib >= 0)
		{
			codetofind += to_string(bits[ib]);
			if (codes2.find(codetofind) != codes2.end())
			{
				if (wordsread < words)
				{
					ofile << codes2[codetofind];
					wordsread++;
				}
				char c = codes2[codetofind];
				codetofind = "";
			}
			ib--;
		}
		ifile.get(currentbyte);
		bytesindecompress++;
		bits = currentbyte;
		ib = 7;
	}
}
int process(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Invalid number of arguments.\nExiting...\n");
		return 1;
	}
	if ((strcmp(argv[1], "compress") != 0) && (strcmp(argv[1], "decompress") != 0))
	{
		printf("Invalid command.\nExiting...\n");
		return 1;
	}
	ofstream check1("namesfiles.txt");
	check1.close();
	system("powershell ls -name >> namesfiles.txt");
	ifstream check("namesfiles.txt");
	char t;
	string t1;
	while (check.get(t))
		t1 += t;
	string findd(argv[2]);
	findd += '\n';
	if (t1.find(findd) == string::npos)
	{
		printf("Invalid input file.\nExiting...\n");
		cout << findd;
		return 1;
	}
	check.close();
	remove("namesfiles.txt");
	if (strcmp(argv[1], "compress") == 0)
	{
		inputfilename = argv[2];
		outputfilename = argv[3];
		makeTree(heapify());
		compress();
		cout << "Compression ratio = " << bytesshouldwrite / 8.0 << endl;
		cout << "H = " << H << endl;
		cout << "The effeciency is: " << (H/(double)bytesshouldwrite) * 100.0 << "%" << endl;
	}
	else
	{
		inputfilename = argv[2];
		outputfilename = argv[3];
		decompress();
	}
	return 0;
}
int main(int argc, char *argv[])
{
	if (process(argc, argv))
		return 1;
	else
		return 0;
}