/*
	CSCI203:Algorithms and Data Structures - Assignment 1
	Filename: assignment1.cpp
	By Ivana Ozakovic

    Purpose:    This program reads, processes and reports on the contents of a
                text file.
*/

/*
-----------------------------------------------------------
Specifications for the assignment
-----------------------------------------------------------
Steps to be followed:
    1.  Read the name of the text file from the console.  
    2.  Read in a text file, line by line.  
    3.  Split each line into words, discarding punctuation and folding all letters into  lower case.  
    4.  Store the unique words and maintain a count of each different word.  
    5.  Sort the words first by decreasing count and, if there are multiple words with  the same count, alphabetically.  
    6.  Output the first ten words in the sorted list, along with their counts.  
    7.  Output the last ten words in the list, along with their counts.  

Requirements and Notes:
    1.  You must choose appropriate data structures and algorithms to accomplish this task.
    2.  In the context of this assignment, appropriate choices will be efficient and will not use excessive instructions or data.
    3.  Where a punctuation mark appears between two letters, the sequence is to be treated as a single word. 
        Thus, it’s will become its, you’ll will become youll and loop-hole will become loophole.
    4.  You can assume that the input file contains no more than 50,000 different words.
    5.  A small sample input file “test.txt” is provided for you to test your program. 
        A larger text file will be used for final assessment.
    6.  You may use any data structures or algorithms that have been presented in class up to the end of week 4. 
        If you use other data structures or algorithms appropriate references must be provided.
    7.  Programs should be appropriately documented with comments.
    8.  Standard libraries of data structures and algorithms such as STL may NOT be used.

A pdf file describing your solution should also be produced. This file should contain at least:
    1. A high‐level description of the overall solution strategy:
    2. A list of all of the data structures used, where they are used and the reasons for
    their choice.
    3. A list of any standard algorithms used, where they are used and why they are used.

*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <ctime>

using namespace std;

//********************************************//
//**************	Constants	**************//
//********************************************//
const int MAX_TREE = 50000;					//max tree size
const int MAX_WORD = 20;					//max word size - NOTE: Not 10 because it will be larger when read in.
const int MAX_LINE = 255;					//max line size


//********************************************//
//**********	Global variables	**********//
//********************************************//

//structure to hold node information inserted into the tree
struct Node
{
	char word[MAX_WORD];					//word read in
	int count;								//word occurence

	Node *left;								//pointer to the left child node
	Node *right;							//pointer to the right child node

	int height;								//variable for storing node height - for balancing AVL tree

};

//AVL tree globals
Node AVL[MAX_TREE];							//AVL tree array
int tree_size = 0;							//current tree size, also index of the next position for a new node
Node *root = NULL;							//pointer to a tree root

//array globals
Node temp[MAX_TREE];						//temporary array of words read in
int temp_count = 0;							//count of words read in


//********************************************//
//*******	Function declarations	**********//
//********************************************//

//Utility functions
void parse_word(char *word, int length);					//function is used to parse word, removes punctuation characters read in
int max(int a, int b);										//function returns maximum of 2 integers
 
//Array functions
bool search_word_temp(char *word);							//function searches array of read in words and keeps count of occurences

//AVL Tree functions
Node* insert(Node* node, int key, char *word);				//insert node into AVL tree
Node *initialize_node(char *word, Node *node, int count);	//function initializes new node
Node *get_new_node();										//function returns a pointer to a new node
void in_order_traversal(Node *current);						//function performs in-order traversal of the tree (using recursion) and
															//prints first 10 and last 10 elements in a tree
//Tree balancing functions
//References:
// - lectures
// - http://www.geeksforgeeks.org/avl-tree-set-1-insertion/
//the AVL tree insertion and balancing functions were written in reference of lecture notes and above link
int get_balance(Node *node);								//returns balancing factor
Node *balance(Node *node);									//function balances the tree based on balancing factor (uses get_balance)
int height(Node *node);										//function returns the height of the node
//rotation functions
Node *rotate_left(Node *x);									//left rotation
Node *rotate_right(Node *y);								//right rotation
Node *double_rotate_right(Node *z);							//double right rotation (left right rotation)
Node *double_rotate_left(Node *x);							//double left rotation (right left rotation)

//Not implemented (ran out of time) - was trying to delete the node then insert it again with new count into the tree
//Node * minValueNode(Node* node);
//Node* deleteNode(Node* root, int key, char *word);


//********************************************//
//***********	Main function	**************//
//********************************************//
int main(int argc, char *argv[])
{ 
	
	ifstream ins;									//input stream
    char filename[50];								//variable for storing filename
 
    cout << "Enter an input filename: ";
    cin.getline(filename, sizeof filename);			//get filename
 
 	ins.open(filename);								//open file

    if(!ins.good())									//if cannot open the file, print error message
    {
        cerr << "Unable to open the file. Program will now terminate." << endl;
        return 1;
    }

    char line[MAX_LINE] = {0};							//initialize input line
    char word[MAX_WORD] = {0};							//initialize input word

    //while not end of file
    while(!ins.eof())
    {
    	//double check if reached end of file
    	if (ins.eof())
	    	break;

    	ins.getline(line, MAX_LINE);					//read in line

    	const size_t line_length = strlen(line);		//get size of the line
    	line[line_length] = 0;							//set null byte to indicate end of the string

    	for(int n = 0, m = 0; m < line_length; m += n)
    	{
        	sscanf(&line[m], "%s%n" , word, &n);		//using sscanf to read in words from line
	     	word[n] = 0;								//set null byte to indicate end of the string

        	if(m + n <= line_length)
        	{
        		//parse word - remove punctuation
        		parse_word(word, n);

        		//check if word already read in, if so, increment word count
        		bool is_found = search_word_temp(word);

        		//if word not read in already, insert word into an array with count 1
        		if (!is_found)
        		{
        			strcpy(temp[temp_count].word, word);
        			temp[temp_count].count = 1;

        			++temp_count;
        		}

        	}
        	//reset word
        	word[0] = '\0';
    	}
    	//reset line
    	line[0] = '\0';
     }

    //close the file
    ins.close();

    //insert elements into AVL - insertion will sort it 
    for (int i = 0; i < temp_count; ++i)
    	root = insert(root, temp[i].count, temp[i].word);

    //use in order traversal to print out the 
 	in_order_traversal(root);

	return 0;
}


//********************************************//
//*******	Function definitions	**********//
//********************************************//

//function is used to parse word, removes punctuation characters read in
void parse_word(char *word, int length)
{

	char temp[MAX_WORD] = {0};
	strcpy(temp, word);
	
	int index = 0;			//word index

	//find if word contains punctuation character
	for (int i = 0; i < length; ++i)
	{
		//if not punctuation character, copy letter into word array
		if(!ispunct(temp[i]))
		{
			//convert letter to lowercase
			temp[i] = tolower(temp[i]);
			//copy letter of the word to it's splace
			word[index] = temp[i];			
			//increment word index
			++index;			
		}
	}
	word[index] = 0;
}

//function searches array of read in words and keeps count of occurences
bool search_word_temp(char *word)
{
	for (int i = 0; i < temp_count; ++i)
	{
		if(strcmp(temp[i].word, word) == 0)
		{
			++temp[i].count;
			return true;
		}
	}

	return false;

}

//function returns a new position in the array for the next element
Node *get_new_node()
{
	if(tree_size == MAX_TREE)
		return NULL;

	return &AVL[tree_size++];

}

Node *initialize_node(char *word, Node *node, int count)
{
	strcpy(node->word, word);					//copy word into a new node
	node->count = count;						//initialize count of the new word to 1

	node->left = NULL;							//set pointer to left node to NULL
	node->right = NULL;							//set pointer to right node to NULL

	node->height = 1;

	return node;

}

//function performs in-order traversal of the tree (using recursion)
//by traversing left node first, then right node
void in_order_traversal(Node *current)
{
	//static variables for counters as this is a recursive function - static variables initialized to 0
	static int start_first;						//starting index of fist 10 elements	
	static int start_last = tree_size - 10;		//set starting index of last 10 elements
	static int at;								//current index

	if (current == NULL) return;

	in_order_traversal(current->left);

	//print first 10 elements
	if(start_first < 10)
	{
		if(at == 0)
		{
			cout << "\nFirst 10 words" << endl;
			cout << "--------------------------------" << endl;
			cout << setw(20) << left << "Word" << setw(15) << "Occurences" << endl;
			cout << "--------------------------------" << endl;
		}
		cout << setw(20) << left << current->word << setw(15) << current->count << endl;

		++start_first;
	}
	//print last 10 elements
	if(at >= start_last)
	{	
		if(at == start_last)
		{
			cout << "\nLast 10 words" << endl;
			cout << "--------------------------------" << endl;
			cout << setw(20) << left << "Word" << setw(15) << "Occurences" << endl;
			cout << "--------------------------------" << endl;
		}
		cout << setw(20) << left << current->word << setw(15) << current->count << endl;
	}
	//increment current index
	++at;

	in_order_traversal(current->right);
}

//rotate right
Node *rotate_right(Node *y)
{
    Node *x = y->left;
    Node *temp = x->right;
 
    //rotate nodes
    x->right = y;
    y->left = temp;
 
    //update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
 
    //return new root node
    return x;
}

//double right rotation
Node *double_rotate_right(Node *z)
{
	Node *temp;

	//rotate nodes
    temp = rotate_left(z);
    temp = rotate_right(z);
 
    //return new root node
    return temp;
}

//double left rotation
Node *double_rotate_left(Node *x)
{
	Node *temp;

	//rotate nodes
    temp = rotate_right(x);
    temp = rotate_left(x);
 
    // Return new root
    return temp;
}
 
//left rotation
Node *rotate_left(Node *x)
{
    Node *y = x->right;
    Node *temp = y->left;
 
    //rotate nodes
    y->left = x;
    x->right = temp;
 
    //update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
 
    //return new root node
    return y;
}
 
//get balancing factor of the node
int get_balance(Node *node)
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}
 

//AVL Insertion function modified in order to store words with same key alphabetically
Node* insert(Node* node, int key, char *word)
{
	if(node == NULL)
	{
		//create new node and initialize with input data
		node = get_new_node();
		node = initialize_node(word, node, key);

		return node;

	}

	//compare word that is being inserted with the word in the current node
	int compare_words = strcmp(word, node->word);

	//if key(count) of inserted word is larger than count in the current node
    if(key > node->count)								
		node->left  = insert(node->left, key, word);		//insert new word into left child
	//else if key(count) of inserted word is smaller than count in the current node
    else if(key < node->count)							
        node->right = insert(node->right, key, word);		//insert new word into right child
    //if key (count) of inserted word is equal to count in the current node
    //insert new node in alphabetic order by comparing words and inserting accordingly 
    else if(key == node->count && compare_words < 0) 
    	node->left  = insert(node->left, key, word);
    else if(key == node->count && compare_words > 0)
    	node->right = insert(node->right, key, word);
    
    //update height
    node->height = 1 + max(height(node->left), height(node->right));
 	
 	//balance node 
    node = balance(node);
 
    return node;

}

//function for balancing tree node after insertion, based on height of nodes
Node *balance(Node *node)
{
	int balance = get_balance(node);
 
    //cases when the tree is unbalanced:
    //1.rotate left case
    if (balance > 1 && node->count < node->left->count)
        return rotate_right(node);
 
    //2.rotate right case
    if (balance < -1 && node->count > node->right->count)
        return rotate_left(node);
 
    //3.double right case
    if (balance > 1 && node->count > node->left->count)
    {
        node->left =  rotate_left(node->left);
        return rotate_right(node);
    } 
    //4.double left case
    if (balance < -1 && node->count < node->right->count)
    {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

//function gets height of the node
int height(Node *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}
 
//function gets maximum of 2 integers
int max(int a, int b)
{
    return (a > b)? a : b;
}


/****************** NOT IMPLEMENTED with rest of the assignment ******************************/
//Function deletes node with given key and word
/*Node* deleteNode(Node* root, int key, char *word)
{
 
    if (root == NULL)
        return root;
 	
 	int compare_words = strcmp(word, root->word);
    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if(key < root->count)
        root->left = deleteNode(root->left, key, word); 
    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if(key > root->count)
        root->right = deleteNode(root->right, key, word);
 	else if(key == root->count && compare_words < 0)
 	{
 		root->left  = deleteNode(root->left, key, word);

 	}
 	else if(key == root->count && compare_words > 0)
 	{
 		root->left  = deleteNode(root->left, key, word);

 	}
    //if key and word matches, delete this node
    else if (key == root->count && compare_words == 0)
    {
    	
	    // node with only one child or no child
	    if( (root->left == NULL) || (root->right == NULL) )
	    {
	        Node *temp = root->left ? root->left : root->right;
	 
	        //no child
	        if (temp == NULL)
	        {
	            temp = root;
	            root = NULL;

	        }
	        else //one child
	        	*root = *temp; //copy not empty child
	    }
	    else
	    {
	            //get smallest in right subtree
	            Node* temp = minValueNode(root->right);
	            //copy data to the node
	            root->count = temp->count;	 
	            //delete node
	            root->right = deleteNode(root->right, temp->count, temp->word);
	    }


    } 

    // If the tree had only one node then return
    if (root == NULL)
      return root;
 
    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + max(height(root->left), height(root->right));
 
    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced) 
 
    root = balance(root);
 
    return root;
}


Node * minValueNode(Node* node)
{
    Node* current = node;
 
    //loop down to find the leftmost leaf
    while (current->left != NULL)
        current = current->left;
 
    return current;
}

*/


