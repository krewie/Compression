// Compression.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>

// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX_TREE_HT 100

using namespace std;

struct MinHeapNode
{
	char data;
	unsigned freq;
	struct MinHeapNode *left, *right;
};

struct MinHeap
{
	unsigned size;
	unsigned capacity;
	struct MinHeapNode **array;
};

/*
Utililty function for allocating a new min heap node given
a unique character data and frequency of the character.
*/
struct MinHeapNode* newNode(char data, unsigned freq)
{
	struct MinHeapNode* temp =
		(struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
	temp->left = temp->right = NULL;
	temp->data = data;
	temp->freq = freq;
	return temp;
}

struct MinHeap* newMinHeap(unsigned capacity)
{
	struct MinHeap* minHeap =
		(struct MinHeap *) malloc(sizeof(struct MinHeap));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =
		(struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode *));
	return minHeap;
}

/* Utility function for swapping two nodes in a heap*/

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

/* Ordering and sorting*/
void HeapifyMin(struct MinHeap* minHeap, int index)
{
	int smallest = index;
	int left = 2 * index + 1;
	int right = 2 * index + 2;

	if (left < minHeap->size &&
		minHeap->array[left]->freq < minHeap->array[smallest]->freq)
		smallest = left;

	if (right < minHeap->size &&
		minHeap->array[right]->freq < minHeap->array[smallest]->freq)
		smallest = right;

	if (smallest != index)
	{
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[index]);
		HeapifyMin(minHeap, smallest);
	}
}

//Check if size of heap equals size
int isSize(struct MinHeap* minHeap, int size)
{
	return (minHeap->size == size);
}

/*
Extract minimum value (extraction by removal)
*/
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	struct MinHeapNode* result = minHeap->array[0];
	minHeap->array[0] = minHeap->array[minHeap->size - 1];
	--minHeap->size;
	HeapifyMin(minHeap, 0);
	return result;
}

/* Insert a new node to min heap*/
void InsertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode)
{
	++minHeap->size;
	int i = minHeap->size - 1;
	while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq)
	{
		minHeap->array[i] = minHeap->array[(i - 1) / 2];
		i = (i - 1) / 2;
	}
	minHeap->array[i] = minHeapNode;
}

// A standard function to build min heap
void buildMinHeap(struct MinHeap* minHeap)
{
	int n = minHeap->size - 1;
	int i;
	for (i = (n - 1) / 2; i >= 0; --i)
		HeapifyMin(minHeap, i);
}

// A utility function to print an array of size n
void printArr(int arr[], int n)
{
	int i;
	for (i = 0; i < n; ++i)
		printf("%d", arr[i]);
	printf("\n");
}

// Utility function to check if this node is leaf
int isLeaf(struct MinHeapNode* root)
{
	return !(root->left) && !(root->right);
}

struct MinHeap* constructMinHeap(char data[], int freq[], int size)
{
	struct MinHeap* minHeap = newMinHeap(size);
	for (int i = 0; i < size; ++i)
		minHeap->array[i] = newNode(data[i], freq[i]);
	minHeap->size = size;
	buildMinHeap(minHeap);
	return minHeap;
}

//Functionality for building Huffman tree

struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size)
{
	struct MinHeapNode *left, *right, *top;

	//Create a min heap of capacity equal to size.
	struct MinHeap* minHeap = constructMinHeap(data, freq, size);

	//while heap size is not 1
	while (!isSize(minHeap, 1))
	{
		//Extract the two minimum freq items
		left = extractMin(minHeap);
		right = extractMin(minHeap);

		/* Create a new internal node with frequency equal to the sum of the two nodes freq. */
		top = newNode('$', left->freq + right->freq);
		top->left = left;
		top->right = right;
		InsertMinHeap(minHeap, top);
	}

	//The remaining node is the root node
	return extractMin(minHeap);
}

/* Function that prints huffman codes from the root of a huffman tree.*/

void huffmanPrint(struct MinHeapNode* root, int arr[], int top)
{
	if (root->left)
	{
		arr[top] = 0;
		huffmanPrint(root->left, arr, top + 1);
	}

	if (root->right)
	{
		arr[top] = 1;
		huffmanPrint(root->right, arr, top + 1);
	}

	if (isLeaf(root))
	{
		printf("%c: ", root->data);
		printArr(arr, top);
	}
}

// The main function that builds a Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char data[], int freq[], int size)
{
	//  Construct Huffman Tree
	struct MinHeapNode* root = buildHuffmanTree(data, freq, size);

	// Print Huffman codes using the Huffman tree built above
	int arr[MAX_TREE_HT], top = 0;
	huffmanPrint(root, arr, top);
}

static std::vector<char> ReadBytes(char const* filename)
{
	ifstream ifs(filename, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	std::vector<char> result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read(&result[0], pos);

	return result;
}


char* readFileBytes(const char *name)
{
	ifstream fl(name);
	fl.seekg(0, ios::end);
	size_t len = fl.tellg();
	char *ret = new char[len];
	fl.seekg(0, ios::beg);
	fl.read(ret, len);
	fl.close();
	return ret;
}

// Driver program to test above functions
int main()
{
	/*
	char arr[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
	int freq[] = { 5, 9, 12, 13, 16, 45 };
	int size = sizeof(arr) / sizeof(arr[0]);
	HuffmanCodes(arr, freq, size);
	*/

	std::ifstream fin("example_vid.avi", std::ifstream::binary);
	std::vector<char> buffer(2048, 0); //reads only the first 1024 bytes

	while (fin.read(buffer.data(), buffer.size())) {
		std::streamsize s = fin.gcount();
		///do with buffer

		fwrite(&buffer, 2048, 1, stdout);
	}
}

