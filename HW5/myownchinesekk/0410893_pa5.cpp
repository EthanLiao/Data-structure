#define Q 4
#include<algorithm>
#include<fstream>
#include<iostream>
#include<string>
#include <utility>
#include <map>
#include <vector>
#include <list>
#include <limits>
#include<stdio.h>
#include<stdlib.h>
#include <math.h>
//#include <malloc.h>
#include <cmath>
#include <cstdlib>
#include <stack>
#include <queue>
#include<ctime>
#include<cmath>
#include <limits>
#include <sstream>
#include <cctype>
#include <functional>
#include <stdexcept>
#include <limits.h>
using namespace std;
using std::cout;
using std::invalid_argument;
using std::length_error;
using std::max;

enum traversal_order {preorder, inorder, postorder};

template <typename E>
class AVLTree;  // Let node class know AVL exists.

template <typename E>
class ANode {
  E data;
  int height = 0;
  ANode<E>* parent = nullptr;
  ANode<E>* left = nullptr;
  ANode<E>* right = nullptr;
  friend class AVLTree<E>;
};


template <typename E>
class AVLTree {
private:
  ANode<E>* root;
  int num_of_nodes;

  // Trinode restructuring & balancing
  bool is_balanced(ANode<E>* &);  // Check if subtree is balanced
  void rebalance(ANode<E>* &);
  ANode<E>* tallest_grandchild(ANode<E>* &ptr);  // Find the tallest grandchild of node
  ANode<E>* restructure(ANode<E>* &);

  // Helper functions
  int height_helper(ANode<E>*);
  ANode<E>* insert_helper(ANode<E>* &, const E &);
  ANode<E>* remove_node(ANode<E>* &);
  ANode<E>* remove_helper(ANode<E>* &, const E &);
  E& retrieve_helper(ANode<E>* &, const E &);
  void print_helper(ANode<E>*, traversal_order);
  void remove_all_helper(ANode<E>* &);

  // Utility functions
  void expand_external(ANode<E>*);  // add external nodes
  bool is_external(ANode<E>*);
  void set_height(ANode<E>*);  // Set height of node.
  bool is_leaf_node(ANode<E>*);  // Check if node it a leaf node
  bool only_one_child(ANode<E>*);  // Check if node has only one child node
  ANode<E>* get_min_ptr(ANode<E>*);  // Find the pointer that points to the
                                     // minimum value.

public:
  AVLTree();
  ~AVLTree();

  bool is_empty();
  int size();
  int height();

  void insert(const E &);
  void remove(const E &);
  E& retrieve(const E &);

  void print(traversal_order);
  void remove_all();
};


template <typename E>
AVLTree<E>::AVLTree() {
  root = nullptr;
  num_of_nodes = 0;
}

template <typename E>
AVLTree<E>::~AVLTree() {
  remove_all_helper(root);
}

template <typename E>
bool AVLTree<E>::is_empty() {
  // Returns true if tree is empty, else false.
  return root == nullptr;
}

template <typename E>
void AVLTree<E>::expand_external(ANode<E>* ptr) {
  // Gives the node two external nodes.
  ptr->left = new ANode<E>;
  ptr->right = new ANode<E>;
  ptr->left->parent = ptr;
  ptr->right->parent = ptr;
}

template <typename E>
bool AVLTree<E>::is_external(ANode<E>* ptr) {
  // Check if node is external or not.
  if (ptr->right == nullptr && ptr->left == nullptr)
    return true;
  return false;
}

template <typename E>
bool AVLTree<E>::is_leaf_node(ANode<E>* ptr) {
  // Check if passed in pointer points to a leaf node.
  if (is_external(ptr->left) && is_external(ptr->right))
    return true;
  return false;
}

template <typename E>
bool AVLTree<E>::only_one_child(ANode<E>* ptr) {
  // Check if passed in pointer points to a node with only one child.
  if (is_external(ptr->left) && !is_external(ptr->right))
    return true;
  if (is_external(ptr->right) && !is_external(ptr->left))
    return true;
  return false;
}

template <typename E>
ANode<E>* AVLTree<E>::get_min_ptr(ANode<E>* ptr) {
  // Traverse the passed in pointer to the minimum value of the tree/subtree
  // aka the left-most node.
  if (ptr == nullptr || is_external(ptr))
    throw invalid_argument("cannot find minimum value; is null or external");

  if (is_external(ptr->left))
    return ptr;

  return get_min_ptr(ptr->left);
}

template <typename E>
int AVLTree<E>::size() {
  // Returns the number of nodes in the tree.
  return num_of_nodes;
}

template <typename E>
void AVLTree<E>::set_height(ANode<E>* ptr) {
  // Sets the height of the node by adding one to the height of the child with the
  // largest height.
    ptr->height = max(ptr->left->height, ptr->right->height) + 1;
}

template <typename E>
int AVLTree<E>::height() {
  // Check height of tree (how many nodes, not edges).
  if (is_empty())
    throw length_error("tree is empty");
  return root->height;
}

template <typename E>
bool AVLTree<E>::is_balanced(ANode<E>* &ptr) {
  // Checks if the subtree is balanced by comparing the heights of its child
  // nodes. If it finds that the height difference is greater than 1, then it
  // returns false; true otherwise.
  int bal = ptr->left->height - ptr->right->height;
  return ((bal >= -1) && (bal <= 1));
}

template <typename E>
ANode<E>* AVLTree<E>::tallest_grandchild(ANode<E>* &ptr) {
  // Returns the tallest grandchild of the passed in node. First it compares the
  // heights of the childs, the the heights of the child's child.
  ANode<E>* lchild = ptr->left;
  ANode<E>* rchild = ptr->right;

  if (lchild->height >= rchild->height) {
    if (lchild->left->height >= lchild->right->height)
      return lchild->left;
    else
      return lchild->right;
  }

  else {
    if (rchild->right->height >= rchild->left->height)
      return rchild->right;
    else
      return rchild->left;
  }
}

template <typename E>
ANode<E>* AVLTree<E>::restructure(ANode<E>* &x) {
  ANode<E>* y = x->parent;
  ANode<E>* z = y->parent;

  // We will be assigning a, b, c to their appropriate nodes, that is, we will
  // be assigning them inorder respective to z, y, x (see ascii art below)
  ANode<E>* a;  // First in inorder
  ANode<E>* b;  // Second in inorder
  ANode<E>* c;  // Third in inorder

  // We will assigned t0, t1, t2, t3 to their respective nodes. Like a, b, c,
  // we will be assigning them inorder respective to the four subtrees of z, y, x
  ANode<E>* t0;  // First subtree in inorder
  ANode<E>* t1;  // Second subtree in inorder
  ANode<E>* t2;  // Third subtree in inorder
  ANode<E>* t3;  // Fourth subtree in inorder


  if (z->right == y && y->right == x) {
    // single rotation (a)
    a = z; b = y; c = x;
    t0 = a->left; t1 = b->left; t2 = c->left; t3 = c->right;
  }
  else if (z->left == y && y->left == x) {
    // single rotation (b)
    a = x; b = y; c = z;
    t0 = a->left; t1 = a->right; t2 = b->right; t3 = c->right;
  }
  else if (z->right == y && y->left == x) {
    // double rotation (c)
    a = z; b = x; c = y;
    t0 = a->left; t1 = b->left; t2 = b->right; t3 = c->right;
  }
  else if (z->left == y && y->right == x) {
    // double rotation (d)
    a = y; b = x; c = z;
    t0 = a->left; t1 = b->left; t2 = b->right; t3 = c->right;
  }

  // Replace subtree rooted at z. For all cases, b will be the new root after
  // the restructure.
  if (z->parent != nullptr) {  // if z == root
    if (z->parent->right == z)
      z->parent->right = b;
    else
      z->parent->left = b;
  }
  b->parent = z->parent;

  if (root == z)
    root = b;

  // "Let a be the child of b and let t0 and t1 be the left and right subtrees
  // of a, respectively" (Goodrich 443). t0 is already left subtree.
  b->left = a;
  a->parent = b;
  a->right = t1;
  t1->parent = a;
  // "Let c be the child of b and let t2 and t3 be the left and right subtrees
  // of c, respectively" (Goodrich 443). t3 is already right subtree.
  b->right = c;
  c->parent = b;
  c->left = t2;
  t2->parent = c;

  return b;  // return new root of subtree
}

template <typename E>
void AVLTree<E>::rebalance(ANode<E>* &ptr) {
  // Checks to see if the tree needs any rebalancing. If it does it calls
  // for a trinode restructure. Here I begin to follow the naming convention
  // that Data Structures & Algorithms in C++ 2e - Goodrich follows on page
  // 441 (AVL Trees - 10.2.1 Update operations).
  ANode<E>* z = ptr;
  while (z != root) {
    // Check for balancing all the way up until the root.
    z = z->parent;
    set_height(z);

    if (!is_balanced(z)) {
      // If unbalanced, find tallest grandchild, x, perform trinode restructure,
      // and adjust the heights. We aquire the tallest grandchild so that we can
      // easily access the 3 nodes that we will perform the restructure on (the
      // three nodes will be x, x's parent, and x's parent's parent)
      ANode<E>* x = tallest_grandchild(z);
      z = restructure(x);
      set_height(z->left);
      set_height(z->right);
      set_height(z);
    }
  }
}

template <typename E>
ANode<E>* AVLTree<E>::insert_helper(ANode<E>* &ptr, const E &elem) {
  // Recursively traverse until you reach an external node. Once you've reached
  // an external node, save the data into the external node and then expand.

  if (is_empty() || is_external(ptr)) {
    if (is_empty())  // If tree is empty, create root node.
      ptr = new ANode<E>;
    // Add new node to tree.
    ptr->data = elem;
    expand_external(ptr);
    return ptr;
  }

  // Traverse
  if (elem < ptr->data)
    return insert_helper(ptr->left, elem);
  else if (elem > ptr->data)
    return insert_helper(ptr->right, elem);
  else if (elem == ptr->data)
    throw invalid_argument("value is already in tree");
}

template <typename E>
void AVLTree<E>::insert(const E &elem) {
  // To remove an element. Call to insert helper. Sets heights and rebalances
  // the tree if needed.
  ANode<E>* ptr = insert_helper(root, elem);
  num_of_nodes++;
  set_height(ptr);
  rebalance(ptr);
}

template <typename E>
ANode<E>* AVLTree<E>::remove_node(ANode<E>* &ptr) {
  // Remove node that the passed in pointer points to. There are three cases
  // that have to be taken cared of: The node that needs to be removed (1) is
  // a leaf node, (2) has only one child, or (3) has two child nodes. Returns
  // A pointer to where the node was removed.

  if (is_leaf_node(ptr)) {
    // If leaf node, make ptr be one of the external nodes, then deleted the node
    // that was ptr previously was, along with the other external node. If ptr is
    // root, just delete everthing and set ptr to null.

    if (ptr == root) {
      ANode<E>* left_extern = ptr->right;
      ANode<E>* right_extern = ptr->left;
      delete left_extern;
      delete right_extern;
      delete ptr;
      ptr = nullptr;
      return ptr;
    }

    ANode<E>* to_remove = ptr;
    ptr = ptr->right;  // Set ptr as an external node and link to its grandparent.
    ptr->parent = to_remove->parent;

    ANode<E>* extern_node = to_remove->left;
    delete extern_node;
    delete to_remove;
    return ptr;

  }

  else if (only_one_child(ptr)) {
    // If node only has one child node then (depending on whether it is a right
    // child or left child) set the pointer to point to the child node and
    // delete the node that the pointer previously pointed to.

    if (is_external(ptr->left)) {
      // Change ptr to point to its right child and delete the node that it
      // previously pointed to.
      ANode<E>* to_remove = ptr;
      ptr = ptr->right;
      ptr->parent = to_remove->parent;

      ANode<E>* extern_node = to_remove->left;
      delete extern_node;  // delete external node
      delete to_remove;
      return ptr;
    }
    else if (is_external(ptr->right)) {
      // Preform the same as above but with opposite child.
      ANode<E>* to_remove = ptr;
      ptr = ptr->left;
      ptr->parent = to_remove->parent;

      ANode<E>* extern_node = to_remove->right;
      delete extern_node;  // delete external node
      delete to_remove;
      return ptr;
    }
  }
  // Case in which there are two children.
  else {
    // Find the minimum value of the subtree with ptr->right as root, then
    // replace ptr's data with the minimum value. Finally, delete the node that
    // originally had the minimum value.
    ANode<E>* min = get_min_ptr(ptr->right);
    ptr->data = min->data;
    return remove_helper(ptr->right, min->data);
  }
}

template <typename E>
ANode<E>* AVLTree<E>::remove_helper(ANode<E>* &ptr, const E &elem) {
  // Traverses ptr to the node that needs to be deleted. Returns pointer that
  // points to where the deleted node use to be. Uses remove_node() to delete
  // the node.
  if (is_external(ptr))
    throw invalid_argument("value not in tree");

  if (ptr->data == elem)
    return remove_node(ptr);
  else if (elem < ptr->data)
    return remove_helper(ptr->left, elem);
  else if (elem > ptr->data)
    return remove_helper(ptr->right, elem);
}

template <typename E>
void AVLTree<E>::remove(const E &elem) {
  // To remove an element. Call to remove helper. Call to rebalance if needed.
  if (is_empty())
    throw length_error("tree is empty, nothing to remove");

  ANode<E>* ptr = remove_helper(root, elem);
  num_of_nodes--;
  rebalance(ptr);
};

template <typename E>
E& AVLTree<E>::retrieve_helper(ANode<E>* &ptr, const E &elem) {
  // Traverses to the node the matches elem. Returns that value.
  if (is_external(ptr))
    throw invalid_argument("value not in tree");

  if (ptr->data == elem)
    return ptr->data;
  else if (elem < ptr->data)
    return retrieve_helper(ptr->left, elem);
  else if (elem > ptr->data)
    return retrieve_helper(ptr->right, elem);
}

template <typename E>
E& AVLTree<E>::retrieve(const E &elem) {
  // Retrieve the node's data that matches elem. Call to retrieve_helper.
  if (is_empty())
    throw length_error("tree is empty, nothing to retrieve");
  return retrieve_helper(root, elem);
}

template <typename E>
void AVLTree<E>::print_helper(ANode<E>* ptr, traversal_order order) {
  // Performs preorder, inorder, and postorder traversal.
  if (is_external(ptr))
    return;
    // Pop off the recursive stack if reached external.

  if (order == preorder) {
    cout << ptr->data << " ";
    print_helper(ptr->left, preorder);
    print_helper(ptr->right, preorder);
  }
  else if (order == inorder) {
    print_helper(ptr->left, inorder);
    cout << ptr->data << " ";
    print_helper(ptr->right, inorder);
  }
  else if (order == postorder) {
    print_helper(ptr->left, postorder);
    print_helper(ptr->right, postorder);
    cout << ptr->data << " ";
  }
}

template <typename E>
void AVLTree<E>::print(traversal_order order) {
  // Print values in the tree in the given order (preorder, inorder, or postorder).
  if (is_empty())
    throw length_error("tree is empty, nothing to print");

  print_helper(root, order);
  cout.flush();  // Flush output buffer to avoid data from not appearing on console.
}

template <typename E>
void AVLTree<E>::remove_all_helper(ANode<E>* &ptr) {
  // Traverses the tree in postorder and removes the nodes one at a time,
  // including external nodes.
  if (ptr == nullptr)
    return;
    // Pop of the recursive stack if reached nullptr.
  remove_all_helper(ptr->left);
  remove_all_helper(ptr->right);
  delete ptr;
  ptr = nullptr;
}

template <typename E>
void AVLTree<E>::remove_all() {
  // Remove all nodes from tree. Call to remove_helper.
  if (is_empty())
    throw length_error("tree is empty, nothing to remove");

  remove_all_helper(root);
  num_of_nodes = 0;
}
std::string encrypt_data (std::string plain_text, std::string key)
{
    std::stringstream encrypted_stream;
    unsigned int text_len = plain_text.length();
    unsigned int key_len = key.length();
    unsigned int char_index = 0;
    unsigned int shift_len = 0;

    for (unsigned int i = 0; i < text_len; ++i)
    {
        if (isalpha(plain_text[i]))
        {
            // If current key char is uppercase, keyReferenceValue would be 'A', otherwise 'a'
            // We use keyReferenceValue to convert key char to ascii
            //
            char keyReferenceValue = (isupper(key[char_index % key_len])) ? 'A' : 'a';

            // Similarly, we use referenceValue to convert plain text char to ascii, it would
            //
            char referenceValue = (isupper(plain_text[i])) ? 'A' : 'a';
            shift_len = key[char_index % key_len] - keyReferenceValue;
            encrypted_stream << char(((plain_text[i] - referenceValue + shift_len) % 26) + referenceValue);
            ++char_index;
        }
        else
        {
            // for non alpha chars, we don't encrypt and keep them as it is.
            encrypted_stream <<  plain_text[i];
        }
    }
    return encrypted_stream.str();
}

std::string decrypt_data(std::string encrypted_text, std::string key)
{
    std::stringstream decrypted_stream;
    unsigned int text_len = encrypted_text.length();
    unsigned int key_len = key.length();
    unsigned int char_index = 0;
    unsigned int shift_len = 0;

    for (unsigned int i = 0; i < text_len; ++i)
    {
        if (isalpha(encrypted_text[i]))
        {
            // If current key char is uppercase, keyReferenceValue would be 'A', otherwise 'a'
            // We use keyReferenceValue to convert key char to ascii
            //
            char keyReferenceValue = (isupper(key[char_index % key_len])) ? 'A' : 'a';

            // Similarly, we use referenceValue to convert plain text char to ascii, it would
            //
            char referenceValue = (isupper(encrypted_text[i])) ? 'A' : 'a';
            shift_len = key[char_index % key_len] - keyReferenceValue;
            decrypted_stream << char((((encrypted_text[i] - referenceValue) + (26 - shift_len)) % 26) + referenceValue);
            ++char_index;
        }
        else
        {
            // We don't encrypt non-alpha chars, so keeping them as it is.
            decrypted_stream << encrypted_text[i];
        }
    }
    return decrypted_stream.str();
}


struct HeapNode
{
    char data;
    unsigned int frequency;
    HeapNode *left;
    HeapNode *right;

    HeapNode(char d, unsigned int fr):
		  data (d),
      frequency (fr),
      left (nullptr),
      right (nullptr)
    {}
};

// We need a comparator for comparing two nodes.
struct compare
{
    bool operator() (HeapNode* node1, HeapNode* node2)
    {
        return (node1->frequency > node2->frequency);
    }
};

void generateHuffmanCodeUtil(HeapNode* root, std::string str)
{
    if (!root)
        return;

    if (root->data != '$')
    {
        std::cout << root->data << " : " << str << std::endl;
    }

    generateHuffmanCodeUtil(root->left, str + '0');
    generateHuffmanCodeUtil(root->right, str + '1');
}

void generateHuffmanCode(std::string data, std::vector<int> frequencies)
{
    HeapNode *left, *right, *top;

    // Create a minimum heap based on frequency and inserts all characters in it.
    //
    std::priority_queue<HeapNode*, std::vector<HeapNode*>, compare> minHeap;
    unsigned int size = frequencies.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        minHeap.push(new HeapNode(data[i], frequencies[i]));
    }

    while (minHeap.size() != 1)
    {
        // Get two minimum nodes from heap.
        left = minHeap.top();
        minHeap.pop();

        right = minHeap.top();
        minHeap.pop();

        // Combine the above two nodes (their frequecies) to generate a
        // new node.Make these two nodes as left and right children of this new node.
        // Add this node to the min heap, also assign in '$' as data. (character for internal nodes)
        //
        top = new HeapNode('$', left->frequency + right->frequency);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }

    // We are done generating the tree, lets print it.
    //
    generateHuffmanCodeUtil(minHeap.top(), "");
}


#define LEFT 0
#define RIGHT 1

struct branch typedef Branch;

namespace tpa {
	class red_black_tree {
		Branch* root;
		int size;

	public:
		red_black_tree();
		void add(int value);
		void get(int value);
		void print(Branch* node);
		void print();

	private:
		Branch* grandfather(Branch* node);
		Branch* uncle(Branch* node);
		void balancing(Branch* node);
		void rotate(Branch* node, bool direction);
	};
}


#define RED 1
#define BLACK 0

struct branch {
	Branch* left = NULL;	// children
	Branch* right = NULL;	// children
	Branch* dad = NULL;
	int color;
	int index;
	int content;
};

tpa::red_black_tree::red_black_tree() {
	root = NULL;
}

void tpa::red_black_tree::add(int value) {		// iteractive insertion - no balancing yet
	if (!root) {
		root = new Branch;
		root->content = value;
		root->color = BLACK;
		return;
	}

	Branch* temp_branch = root;
	char flag = 0;

	while (!flag)
		if (value > temp_branch->content) {
			flag = temp_branch->right ? 0 : 1;
			temp_branch = temp_branch->right ? temp_branch->right : temp_branch;
		} else {
			flag = temp_branch->left ? 0 : -1;
			temp_branch = temp_branch->left ? temp_branch->left : temp_branch;
		}

	if (flag == -1) {
		temp_branch->left = new Branch;
		temp_branch->left->content = value;
		temp_branch->left->color = RED;
		temp_branch->left->dad = temp_branch;
		balancing(temp_branch->left);
	} else {
		temp_branch->right = new Branch;
		temp_branch->right->content = value;
		temp_branch->right->color = RED;
		temp_branch->right->dad = temp_branch;
		balancing(temp_branch->right);
	}
}

void tpa::red_black_tree::get(int value) {
	Branch* node = root;
	int temp_val;
	bool find;

	if (node)
		temp_val = node->content;

	while ((find = (value != temp_val)) && node) {
		node = value > node->content ? node->right : node->left;

		if (node)
			temp_val = node->content;
	}

	if (find)
		std::cout << "not find\n";
	else
		std::cout << "encontrado: " << temp_val << '\n';
}

void tpa::red_black_tree::print(Branch* node) {		// recursive printing
	if (node->left != NULL)
		print(node->left);

	std::cout << node->content << '|' << (node->color ? 'V' : 'B') << '|' << (node->dad ? node->dad->content : 'R') << " - ";

	if (node->right != NULL)
		print(node->right);
}

void tpa::red_black_tree::print() {
	print(root);
	std::cout << '\n';
}

Branch * tpa::red_black_tree::grandfather(Branch * node) {
	if (node && node->dad)
		return node->dad->dad;

	return NULL;
}

Branch * tpa::red_black_tree::uncle(Branch * node) {
	Branch* grandpa = grandfather(node);

	if (!grandpa)
		return NULL;

	if (grandpa->left == node->dad)
		return grandpa->right;
	else
		return grandpa->left;
}

void tpa::red_black_tree::balancing(Branch * node) {
	if (!node->dad)
		node->color = BLACK;
	else
		if (node->dad->color) {
			Branch* uncle_node = uncle(node);
			Branch* grandpa_node = grandfather(node);

			if (uncle_node && uncle_node->color) {
				node->dad->color = BLACK;
				uncle_node->color = BLACK;
				grandpa_node->color = RED;
				balancing(grandpa_node);
			} else {
				if ((node == node->dad->right) && (node->dad == grandpa_node->left)) {
					rotate(node->dad, LEFT);
					node = node->left;
				} else if ((node == node->dad->left) && (node->dad == grandpa_node->right)) {
					rotate(node->dad, RIGHT);
					node = node->right;
				}

				node->dad->color = BLACK;
				grandpa_node->color = RED;

				if ((node == node->dad->left) && (node->dad == grandpa_node->left))
					rotate(grandpa_node, RIGHT);
				else
					rotate(grandpa_node, LEFT);
			}
		}
}

//https://pt.wikipedia.org/wiki/%C3%81rvore_rubro-negra

void tpa::red_black_tree::rotate(Branch * node, bool direction) {
	Branch* temp_node;

	if (direction) {
		temp_node = node->left;
		node->left = temp_node->right;
		if (node->left) node->left->dad = node;
		temp_node->right = node;
	} else {
		temp_node = node->right;
		node->right = temp_node->left;
		if (node->right) node->right->dad = node;
		temp_node->left = node;
	}

	if (node->dad)
		if (node->dad->left == node)
			node->dad->left = temp_node;
		else
			node->dad->right = temp_node;
	else
		root = temp_node;

	temp_node->dad = node->dad;
	node->dad = temp_node;
}

string filename;
fstream input_file;
struct heapnode
{
	int key,degree;
	heapnode *left;
	heapnode *right;
	heapnode *parent;
	heapnode *child;
	bool childcut;

};

/*	-------FIbonacci Heap Class-------		*/

class fibheap
{
	private :
			int rcount,rnos;
			heapnode *heapmin;
			heapnode *q1[5000],*q2[5000];
			int f1,f2,r1,r2,ids;
			heapnode *ref[10000];
			int mx;

	public	:

		fibheap()			//Fibheap constructor
		{
			heapmin = NULL;
			f1=-1;f2=-1;r1=-1;r2=-1;ids=0;
			rcount=0;rnos=0;mx=0;

		}

		void hinsert(int b)		//Inserting into a Fibonacci heap
		{
			heapnode* hnode=new heapnode;
			hnode->key=b;
			hnode->degree=0;
			hnode->parent=NULL;
			hnode->child=NULL;
			hnode->childcut=0;
			hnode->left=hnode;
			hnode->right=hnode;
			if(heapmin==NULL)
			{
				heapmin=hnode;
			}
			else
			{
				hnode->left=heapmin->left;
				hnode->right=heapmin;
				hnode->left->right=hnode;
				heapmin->left=hnode;

				if(heapmin->key>hnode->key)
				{
					heapmin=hnode;
				}
			}
			rcount=rcount+1;			//Maintaining the count of number of nodes
			mx=mx+1;
			ref[mx]=hnode;				//Maintaining a reference array
		}

		void hdeletemin()				//Deleting the Minimum Element from Fibonacci Heap
		{
			heapnode *del;
			heapnode *delch;
			del=heapmin;
			int rmx=heapmin->key;
			for(int s=mx;s>0;s--)			//Searching reference array and deleting
			{
				if(heapmin==ref[s])
				{
					for(int g=s;g<=mx;g++){
					ref[g]=ref[g+1];}
					mx--;
				}
			}
			if(del!=NULL)
			{

				delch=heapmin->child;
		 		for(int i=1;i<(del->degree);i++)
	 			{
	 				delch->parent=NULL;
	 				delch=delch->right;
	 			}
				if(delch!=NULL)
				{

					del->left->right=del->child;
 					del->child->left=del->left;
 					delch->right=del->right;
 					del->right->left=delch;
				}
				else
 				{
 					del->left->right=del->right;
 					del->right->left=del->left;
 				}

				if(del==del->right)
				{
					heapmin=NULL;
				}
				else
				{
					heapmin=del->right;
					consolidate();
				}
				rcount=rcount-1;
			}
		}

		void consolidate()					//Consolidate function to perform Pairwise combination
		{

			heapnode *a,*b,*c;
			long int maxdegree=(log(rcount))/0.693;		//Calculating the maxdegree based on the number of nodes
			maxdegree++;
 			heapnode *darray[maxdegree];
			for(int i=0;i<=maxdegree;i++)
 				darray[i]=NULL;

			a=heapmin;
			rnos=1;
			if(heapmin==NULL)
				rnos=0;
 			else
 			{
				b=a->right;
		 		while(b!=a)
		 		{
		 			rnos++;
		 			b=b->right;
	 			}
		 	}
 			for(int i=0;i<rnos;i++)
 			{
			 	int d=a->degree;
 				c=a->right;
 				heapnode* y;

			 	while(darray[d]!=NULL)
		 		{
		 			y=darray[d];
 					if(a->key>y->key)
 					{
 						heapnode *temp=a;
 						a=y;
 						y=temp;
 					}

					y->left->right=y->right; 		//Removing from rootlist
					y->right->left=y->left;
					y->right=y;
					y->left=y;

					if(a->child==NULL)			//Making y child of a
					{
						a->child=y;
						a->child->left=y;
						a->child->right=y;
						y->parent=a;
						y->left=y;
						y->right=y;
					}
					else
 					{
						y->left=a->child->left;
				 		y->right=a->child;
				 		a->child->left->right=y;
				 		a->child->left=y;
				 		y->parent=a;
				 	}
					a->degree++;
					y->childcut=0;
					darray[d]=NULL;
 					d++;
				}
				darray[d]=a;
			 	a=c;
			}
			heapmin=NULL;
			for(int i2=0;i2<=maxdegree;i2++)
			{
			 	if(darray[i2]!=NULL)
 				{
 					if(heapmin==NULL)
 					{
 						darray[i2]->parent=NULL;
 						darray[i2]->left=darray[i2];
 						darray[i2]->right=darray[i2];
 						heapmin=darray[i2];
 					}
 					else
 					{
 						darray[i2]->left=heapmin->left;
 						darray[i2]->right=heapmin;
 						darray[i2]->left->right=darray[i2];
 						heapmin->left=darray[i2];
 						if(darray[i2]->key<heapmin->key)
 						{
 							heapmin=darray[i2];
 						}
 					}
 				}
 			}
		}

		void hdisplay()							//Writing the output to result_bino file
		{
			for(int i=0;i<5000;i++)
 			{
 				q1[i]=NULL;
 				q2[i]=NULL;
 			}
			long int maxdegree=(log(rcount))/0.693;
			maxdegree++;
			for(int i=0;i<=maxdegree;i++)
 			{
 				hlevelorder(i);
 			}
		}


		void hlevelorder(int d)						//Performing a levelorder traversal on each tree in the fibonacci heap
		{
			heapnode *hl1,*hl2;f1=-1;f2=-1;r1=-1;r2=-1;
			hl1=heapmin;
			rnos=1;
			ofstream fh2("result_bino.txt",ios::app);

 			if(heapmin==NULL)
				rnos=0;
			else
 			{
				hl2=hl1->right;
		 		while(hl2!=hl1)
		 		{
		 			rnos++;
		 			hl2=hl2->right;
		 		}
		 	}

			if(rnos==0)
				return;

			heapnode *my=heapmin;

 			if(my!=NULL)
 			{
 				for(int i3=0;i3<rnos;i3++)
 				{
				 	heapnode* z=my->right;
				 	if(my->degree==d)
	 				{
	 					fh2<<"###\n";
	 					if(f1==-1)
							f1++;
			 			r1++;
			 			q1[r1]=my;

						while(f1!=-1)
			 			{
			 				fh2<<q1[f1]->key<<" ";
			 				heapnode* tq=q1[f1];

	 						if(f1==r1)
	 						{
	 							f1=-1;
	 							r1=-1;
	 						}
	 						else
	 						{
	 							f1++;
	 						}

	 						heapnode *ch=tq->child;
	 			 			if(ch!=NULL)
							{
		 						for(int j=0;j<tq->degree;j++)
		 						{
		 							if(f2==-1)
										f2++;
		 							r2++;
		 							q2[r2]=ch;
		 							ch=ch->right;
		 						}

		 						if(f1==-1)
		 						{
		 							fh2<<"\n";
		 							f1=f2;
		 							r1=r2;

		 							for(int k=f2;k<=r2;k++)
		 							{
		 								q1[k]=q2[k];
		 							}

		 							f2=-1;
		 							r2=-1;
		 						}
							}
						}
						fh2<<endl;
	 				}
	 				my=z;
	 			}
	 		}
		fh2.close();
		}

	     	void hread()							//Reading from input file
     		{
	  		char op,op1;
			int num,num1;
			while(input_file.good())
			{
				input_file>>op;
				switch(op)
				{
					case 'I':				//Case for Insert on Fibonacci heap
		  				input_file>>num;
						hinsert(num);
						break;

					case 'R':				//Case for index based Remove on Fibonacci heap
		  				input_file>>num;
						remove(num);
						break;

					case 'D':
						input_file>>op1;
						if(op1=='K')				//Case for Decrease Key on Fibonacci heap
						{
							input_file>>num>>num1;
							if(num)
							{
								if(num1)
								decrease_key(num,num1);
							}
							break;
						}
						else					//Case for Deletemin on Fibonacci heap
						{
							if(heapmin==NULL)
							{
								break;
							}
							else
								hdeletemin();
							break;
						}

					case '*':				//Case to find end of file for Fibonacci heap
						break;

					default:
						break;
				}
	  	  	}

	      }

		void decrease_key(int w, int e)			//Decrease Key function for Fibonacci Heap
		{
			heapnode *dk;
			heapnode *dkp;
			if(e>=ref[w]->key)
			{
				exit(0);
			}
			if(e==-1)
			{
				ref[w]->key=ref[w]->key-ref[w]->key;
				heapmin=ref[w];
			}
			else
				ref[w]->key=ref[w]->key-e;
			dk=ref[w];
			dkp=ref[w]->parent;
			if((dk->parent!=NULL)&&(dk->key<dk->parent->key))
			{
				cut(dk);				//Removing the node
				cascade_cut(dkp);			//Cascade cut operation
			}

		}

		void cut(heapnode *om)					//Cut operation to remove the node from the fibonacci heap
		{
			heapnode *p;
			p=om->parent;
			if(p->child==om)
			{
				if(om->right==om)
				{
					p->child=NULL;
				}
				else
				{
					p->child=om->right;
					om->left->right=om->right;
					om->right->left=om->left;
					om->left=om;
					om->right=om;
				}
			}
			else
			{
				om->left->right=om->right;
				om->right->left=om->left;
				om->left=om->right=om;
			}
			p->degree--;
			om->parent=NULL;
			om->childcut=0;
			hinsert(om->key);
		}

		void cascade_cut(heapnode *pm)				//Cascade function to check childcut values
		{
			heapnode *ppm;
			ppm=pm->parent;
			if(pm->parent==NULL)
			{
				if(pm->childcut==0)
				{
					pm->childcut=1;
				}
				else
				{
					cut(pm);
					cascade_cut(ppm);
				}
			}
		}

		void remove(int x)					//Remove function based on index value
		{
			decrease_key(x,-1);
			hdeletemin();
		}

		void hrandom()						//Random function for Fibonacci heap
		{

			short int xrh;
			int yrh,zrh;
			double dth;
			double start_fibheap,time_fibheap;
			for(int i=0;i<5000;i++)
			{
				xrh=rand();
				xrh=xrh%10000;
				hinsert(xrh);
			}
			start_fibheap=clock();
			for(int i=0;i<5000;i++)
			{
				xrh=rand();
				xrh=xrh%10000;
				yrh=rand();
				zrh=yrh % 2;
				if(zrh)
				{
					hinsert(xrh);
				}
				else if(heapmin!=NULL)
						hdeletemin();
			}
			time_fibheap=clock()-start_fibheap;
			dth=time_fibheap/5000;
			cout<<"Running time for Fibonaci Heap (clock ticks) is :"<<dth<<endl;
			cout<<"Running time for Fibonaci Heap (milliseconds) is :"<<(dth*1000)/(double)CLOCKS_PER_SEC<<endl<<endl;
		}




};


/*	-------Structure for Leftist Tree Node-------	*/

struct treenode
{
    int s,value,level;
    treenode *left;
    treenode *right;
};


/*	-------Leftist Tree Class-------		*/


class leftist
{
	private :
              treenode* root;
	public :

	leftist()					//Leftist class constructor
	{
		root=NULL;
	}

	void insert(int key)				//Inserting into Leftist tree
	{

	        treenode* node=new treenode;
	        node->value=key;
	        node->left=NULL;
	        node->right=NULL;
		node->s=1;
		root=merge(root,node);
	}

	treenode* merge(treenode* nodeA, treenode* nodeB)		//Merging Leftist tree nodes
	{
                  if((nodeA==NULL)&&(nodeB!=NULL))
                                 return nodeB;
                  else if((nodeB==NULL)&&(nodeA!=NULL))
				return nodeA;
                  else if((nodeB==NULL)&&(nodeA==NULL))
				return nodeA;
		  else if(nodeA->value>nodeB->value)
                  {
          		treenode* temp=nodeA;
          		nodeA=nodeB;
          		nodeB=temp;
                  }
                  nodeA->right=merge(nodeA->right,nodeB);
		  if((nodeA->left==NULL)&&(nodeA->right!=NULL))
		  {
			nodeA->left=nodeA->right;
			nodeA->right=NULL;
			nodeA->s=1;
		  }
		  else
		  {
			if(nodeA->left->s < nodeA->right->s)
			{
				treenode* temp=nodeA->left;
          			nodeA->left=nodeA->right;
          			nodeA->right=temp;
			}
			nodeA->s=nodeA->right->s+1;
		  }
		  return nodeA;
	}

	void display()							//Display function for Leftist tree
	{

		if(root!=NULL)
		{
			root->level=0;
	        	levelorder(root);
		}

	}

	void levelorder(treenode* t)					//Level order traversal for Leftist tree
	{
		treenode* a[15000];
		treenode* c;
		ofstream f2("result_left.txt",ios::out);

		for(int i=0;i<15000;i++)
       			a[i]=NULL;
       		int size=0,x;
       		int f=0;
       		if(t==NULL)
		{
			f2<<"NULL TREE : No Elements";
		}
       		else
       		{
        		while(t!=NULL)
        		{
                   		f2<<t->value<<" ";
				x=t->level;

                		if(t->left!=NULL)
				{
					a[size++]=t->left;
					t->left->level=x+1;
				}

                		if(t->right!=NULL)
				{
					a[size++]=t->right;
					t->right->level=x+1;
				}

		                c=a[f];
				if(c!=NULL)
				{
					if(t->level!=c->level)
						f2<<endl;
					else if(t->level==c->level)
						f2<<" ";
				}
				else
					f2<<endl;
				t=a[f++];
	     	       }


		}
	  	f2.close();

	}

	void deletemin()							//Deleting the minimum element (root) function for Leftist tree
	{
		if(root!=NULL)
			  root=merge(root->right,root->left);
	}

	void random()								//Random mode for Leftist tree
	{
		short int xr;
		int yr,zr;
		double dtl;
		clock_t start_leftist,time_leftist;
		for(int i=0;i<5000;i++)
		{
			xr=rand();
			xr=xr%10000;
			insert(xr);
		}
		start_leftist=clock();
		for(int i=0;i<5000;i++)
		{
			xr=rand();
			xr=xr%10000;
			yr=rand();
			zr=yr % 2;
			if(zr)
			{
				insert(xr);
			}
			else
				deletemin();
		}
		time_leftist=clock()-start_leftist;
		dtl=time_leftist/5000;
		cout<<"Running time for leftist tree (clock ticks) is :"<<dtl<<endl;
		cout<<"Running time for leftist tree (milliseconds) is :"<<(dtl*1000)/CLOCKS_PER_SEC<<endl<<endl;
	}

	void read()								//Reading input from file for Leftist tree
	{
	  	char op;
		int num;
		while(input_file.good())
	  	{
			input_file>>op;
			switch(op)
			{
				case 'I':
	  				input_file>>num;
					insert(num);
					break;

				case 'D':
					deletemin();
					break;

				case '*':
					break;

				default:
					break;
			}
	  	  }

	}

};





void restoreDown(int arr[], int len, int index,
                                        int k)
{
    // child array to store indexes of all
    // the children of given node
    int child[k+1];

    while (1)
    {
        // child[i]=-1 if the node is a leaf
        // children (no children)
        for (int i=1; i<=k; i++)
            child[i] = ((k*index + i) < len) ?
                    (k*index + i) : -1;

        // max_child stores the maximum child and
        // max_child_index holds its index
        int max_child = -1, max_child_index ;

        // loop to find the maximum of all
        // the children of a given node
        for (int i=1; i<=k; i++)
        {
            if (child[i] != -1 &&
                arr[child[i]] > max_child)
            {
                max_child_index = child[i];
                max_child = arr[child[i]];
            }
        }

        // leaf node
        if (max_child == -1)
            break;

        // swap only if the key of max_child_index
        // is greater than the key of node
        if (arr[index] < arr[max_child_index])
            swap(arr[index], arr[max_child_index]);

        index = max_child_index;
    }
}

// Restores a given node up in the heap. This is used
// in decreaseKey() and insert()
void restoreUp(int arr[], int index, int k)
{
    // parent stores the index of the parent variable
    // of the node
    int parent = (index-1)/k;

    // Loop should only run till root node in case the
    // element inserted is the maximum restore up will
    // send it to the root node
    while (parent>=0)
    {
        if (arr[index] > arr[parent])
        {
            swap(arr[index], arr[parent]);
            index = parent;
            parent = (index -1)/k;
        }

        // node has been restored at the correct position
        else
            break;
    }
}

// Function to build a heap of arr[0..n-1] and alue of k.
void buildHeap(int arr[], int n, int k)
{
    // Heapify all internal nodes starting from last
    // non-leaf node all the way upto the root node
    // and calling restore down on each
    for (int i= (n-1)/k; i>=0; i--)
        restoreDown(arr, n, i, k);
}

// Function to insert a value in a heap. Parameters are
// the array, size of heap, value k and the element to
// be inserted
void insert(int arr[], int* n, int k, int elem)
{
    // Put the new element in the last position
    arr[*n] = elem;

    // Increase heap size by 1
    *n = *n+1;

    // Call restoreUp on the last index
    restoreUp(arr, *n-1, k);
}

// Function that returns the key of root node of
// the heap and then restores the heap property
// of the remaining nodes
int extractMax(int arr[], int* n, int k)
{
    // Stores the key of root node to be returned
    int max = arr[0];

    // Copy the last node's key to the root node
    arr[0] = arr[*n-1];

    // Decrease heap size by 1
    *n = *n-1;

    // Call restoreDown on the root node to restore
    // it to the correct position in the heap
    restoreDown(arr, *n, 0, k);

    return max;
}

// Driver program

typedef struct
{
	short int row,col,dir;
} element;

typedef struct
{
    short int vert,horiz;
} offsets;

offsets move_long[4]={{0,1},{1,0},{0,-1},{-1,0}};
                /*right down  left   upper  */
 // Establish a tow-dimesional array for the maze
char **maze;
char **mark;


class MatMaze
{
private :
 int row,column;    // index for the whole maze
 int start_row,start_column; //index for the start
 int end_row,end_column;     //indext foe the exist
public:
	 ~MatMaze();

void TypeError(void);
void readFile(string);
void solveMaze(void);
void outputFile(string);
};


MatMaze :: ~MatMaze()
{
  delete [] maze;
  delete [] mark;
}

void MatMaze :: TypeError()
{
   cout<<"plz restart";
}

void MatMaze :: readFile(string srcFilename)
{
   fstream srcfile;
   srcfile.open(&srcFilename[0]);
   char single_ch;
   int count=0;
   int row_count=0,column_count=0;
   srcfile>>column>>row;

    maze=new char*[row];
    mark=new char*[row];
      for(int k=0;k<row;k++)
      {
         maze[k]=new char[column];
         mark[k]=new char[column];
      }
    for(int i=0;i<row;i++)
      for(int j=0;j<column;j++)
       {
        srcfile>>maze[i][j];
        if(maze[i][j]=='S')
          {
                start_row=i;
                start_column=j;

          }

        if (maze[i][j]=='E')
          {
            end_row=i;
            end_column=j;
          }
       }
  srcfile.close();

}

void MatMaze :: solveMaze()
{

 //copy the maze to the mark map
 for(int i=0;i<row;i++)
    for(int j=0;j<column;j++)
      mark[i][j]=maze[i][j];

 int found=0; // If the Mat reach to the exist,we will set the found as 1
 int row,col,dir;
 int nextRow,nextCol;
 // maze entrance & direction east
 element position;
 position.row=start_row;
 position.col=start_column;
 position.dir=0; // intial direction is the east
 // initialize stack
 stack<element> s;
 s.push(position);

 while(!s.empty())
 {
 	if(found)
    {
      mark[row][col]='1';
      break;
    }
 	//cordinate and direcrion pop from stack
 	position=s.top();
 	row=position.row;
 	col=position.col;
 	dir=position.dir;
 	s.pop();
 	 while(dir<4) // if there exist more direction to go
 	 {
 	 	//cordinate of the next move
 	 	nextRow=row+move_long[dir].vert;
 	 	nextCol=col+move_long[dir].horiz;

        // if the next positon is the exit
 	 	if((nextRow==end_row) && (nextCol==end_column))
 	 	{
 	 		found=1;
 	 		break;
 	 	}

        //if the next postion don't encounter with the wall and havn't been visted
 	 	else if(maze[nextRow][nextCol]!='2' && mark[nextRow][nextCol]!='1')
 	 	{
 	 		mark[nextRow][nextCol]='1'; // mark the next position

 	 		// store the current position
 	 		position.row=row;
 	 		position.col=col;
 	 		position.dir=++dir;// store the next direction to try into stack
 	 		s.push(position);

 	 		//move to the next position
 	 		row=nextRow; col=nextCol; dir=0; // we will set the
 	 		                                 //default direction to try for the next position
 	 	}
 	 	//if the next position is the wall or have been here
 	 	else ++dir;
 	 }
 }
}

void MatMaze :: outputFile(string out)
{
  ofstream outfile;
  outfile.open(&out[0]);
  for(int i=0;i<row;i++)
    for(int j=0;j<column;j++)
    {
      outfile<<mark[i][j]<<" ";
      if(j==column-1)
        outfile<<"\n";
    }
    outfile.close();
}

/*Method two*/


typedef struct
{
  int row,col;
  int dist,dir; //distance for the queue node
}Point;


typedef struct
{
  int vert,horiz;
}deviate;

deviate move_short[4]={{0,1},{1,0},{0,-1},{-1,0}};
                    /* right  dowwn  left   upper*/

char **maze_short,**mark_short;
int **maze_short_final; //For the path distacne record

class MatMaze_short_Dist
{
private :
 int row,column;    // index for the whole maze
 int start_row,start_column; //index for the start
 int end_row,end_column;     //indext foe the exist
public:
	~MatMaze_short_Dist();
	void outputFile(string);
    void readFile(string);
	void solveMaze(void);
	void traceBack(void);
};

MatMaze_short_Dist :: ~MatMaze_short_Dist()
{
  delete [] maze_short;
  delete [] maze_short_final;
  delete [] mark_short;
}


void MatMaze_short_Dist :: readFile(string srcFilename)
{
   fstream srcfile;
   srcfile.open(&srcFilename[0]);
   char single_ch;
   int count=0;
   int row_count=0,column_count=0;
   srcfile>>column>>row;

    maze_short=new char*[row];
    mark_short=new char*[row];
    maze_short_final=new int*[row];
      for(int k=0;k<row;k++)
      {
         maze_short[k]=new char[column];
         mark_short[k]=new char[column];
         maze_short_final[k]=new int[column];
      }
    for(int i=0;i<row;i++)
      for(int j=0;j<column;j++)
       {
        srcfile>>maze_short[i][j];
        if(maze_short[i][j]=='S')
         {
                start_row=i;
                start_column=j;
          }

        if (maze_short[i][j]=='E')
          {
            end_row=i;
            end_column=j;
          }
       }
  srcfile.close();



}

void MatMaze_short_Dist :: solveMaze()
{
 //copy the maze to the mark map

  for(int i=0;i<row;i++)
    for(int j=0;j<column;j++)
    {
      mark_short[i][j]=maze_short[i][j];
      maze_short_final[i][j]=-1;
    }





 int found=0; // If the Mat reach to the exist,we will set the found as 1
 int row,col,dist,dir;
 int nextRow,nextCol;
 // maze entrance & distance
 Point position,nextPosition;
 position.row=start_row;
 position.col=start_column;
 position.dist=0; // intial the distance of start position is 0
 maze_short_final[start_row][start_column]=position.dist;
 position.dir=0;
 // initialize queue
 queue<Point> hq;
 hq.push(position);

 while(!hq.empty())
 {
 	//+if(found) mark[row][col]='E';
 	//cordinate and direcrion pop from stack
 	position=hq.front();
 	row=position.row;
 	col=position.col;
  dir=position.dir;
  //if(nextRow==end_row && nextCol==end_column)
      //maze_short_final[nextRow][nextRow]=dist;
  hq.pop();
 	 while(dir<4) // if there exist more direction to go
 	 {
 	 //cordinate of the next move
 	 nextRow=row+move_short[dir].vert;
 	 nextCol=col+move_short[dir].horiz;

      //if the next postion don't encounter with the wall and havn't been visted
   if((mark_short[nextRow][nextCol]!='1') && (mark_short[nextRow][nextCol]!='2'))
 	 	{
 	 	  mark_short[nextRow][nextCol]='1'; // mark the next position
 	 	  // enhqueue the current position
      // we will set the default direction to try for the next position */
      nextPosition.row=nextRow;
      nextPosition.col=nextCol;
      nextPosition.dist=position.dist+1;
      maze_short_final[nextRow][nextCol]=nextPosition.dist;
      nextPosition.dir=0;
      hq.push(nextPosition);
      dir++;
 	 	}
 	 	//if the next position is the wall or have been here
 	 	else ++dir;
 	 }
 }
}

deviate trace[4]={{0,-1},{-1,0},{0,1},{1,0},};
                /* left  upper  right  down */
void MatMaze_short_Dist :: traceBack()
{
 int row,col,dir,dist;
 int nextRow,nextCol;
 // maze entrance & distance
   Point position;
   position.row=end_row;
   position.col=end_column;
   position.dir=0;
   position.dist=maze_short_final[end_row][end_column];

   //cordinate and direcrion pop from stack
   row=position.row;
   col=position.col;
   dist=position.dist;
   dir=position.dir;
   while(dir<4) // if there exist more direction to go
   {
    //cordinate of the next move
    nextRow=row+trace[dir].vert;
    nextCol=col+trace[dir].horiz;
    // if the next positon is the exit
    if((nextRow==start_row) && (nextCol==start_column))
    {
      maze_short[row][col]='1';
      break;
    }
    //if the next postion don't encounter with the wall and havn't been visted
    else if(
            ( (nextRow!=row-1 || nextRow!=0 || nextCol!=column-1 || nextCol!=0))
            && maze_short_final[nextRow][nextCol]==dist-1
           )
    {
      maze_short[nextRow][nextCol]='1'; // mark the next position
      row=nextRow; col=nextCol; dir=0; dist--; // For the next position,we set default direction to try
    }
    //if the next position is the wall or have been here
    else ++dir;
  }
}

void MatMaze_short_Dist :: outputFile(string out)
{
	traceBack();
  ofstream outfile;
	outfile.open(&out[0]);
	for(int i=0;i<row;i++)
		for(int j=0;j<column;j++)
		{
      outfile<<maze_short[i][j]<<" ";
		  if(j==column-1)
		  	outfile<<"\n";
		}
  outfile.close();
}
struct Job
{
   char id;      // Job Id
   int dead;    // Deadline of job
   int profit;  // Profit if job is over before or on deadline
};

// This function is used for sorting all jobs according to profit
bool comparison(Job a, Job b)
{
     return (a.profit > b.profit);
}
#define MIN(A, B) (LESS((A),(B)) ? (A) : (B))
// Returns minimum number of platforms reqquired
void printJobScheduling(Job arr[], int n)
{
    // Sort all jobs according to decreasing order of prfit
    sort(arr, arr+n, comparison);

    int result[n]; // To store result (Sequence of jobs)
    bool slot[n];  // To keep track of free time slots

    // Initialize all slots to be free
    for (int i=0; i<n; i++)
        slot[i] = false;

    // Iterate through all given jobs
    for (int i=0; i<n; i++)
    {
       // Find a free slot for this job (Note that we start
       // from the last possible slot)
       for (int j=min(n, arr[i].dead)-1; j>=0; j--)
       {
          // Free slot found
          if (slot[j]==false)
          {
             result[j] = i;  // Add this job to result
             slot[j] = true; // Make this slot occupied
             break;
          }
       }
    }


}

#define EPSILON 0.000001
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

#define LESS_EQUAL(A, B) (LESS((A),(B)) || EQUAL((A),(B)))
/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        merge(arr, l, m, r);
    }
}
#define MAX(A, B) (LESS((A),(B)) ? (B) : (A))
void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
	 #define GREATER_EQUAL(A, B) (GREATER((A),(B)) || EQUAL((A),(B)))
int partition (int arr[], int low, int high)
{
    int pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

#define EQUAL(A, B) (fabs((A) - (B)) < EPSILON)

void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

#define INFINITO 1000000000.0

template <class T>
struct Node {
    T value;
    Node *left;
    Node *right;

    Node(T val) {
        this->value = val;
    }

    Node(T val, Node<T> left, Node<T> right) {
        this->value = val;
        this->left = left;
        this->right = right;
    }
};

#define LESS(A, B) ((B) - (A) > EPSILON)
template <class T>
class BST {

    private:
    Node<T> *root;

    void addHelper(Node<T> *root, T val) {
        if (root->value > val) {
            if (!root->left) {
                root->left = new Node<T>(val);
            } else {
                addHelper(root->left, val);
            }
        } else {
            if (!root->right) {
                root->right = new Node<T>(val);
            } else {
                addHelper(root->right, val);
            }
        }
    }

    void printHelper(Node<T> *root) {
        if (!root) return;
        printHelper(root->left);
        cout<<root->value<<' ';
        printHelper(root->right);
    }

    int nodesCountHelper(Node<T> *root) {
        if (!root) return 0;
        else return 1 + nodesCountHelper(root->left) + nodesCountHelper(root->right);
    }

    int heightHelper(Node<T> *root) {
        if (!root) return 0;
        else return 1 + max(heightHelper(root->left), heightHelper(root->right));
    }

    void printMaxPathHelper(Node<T> *root) {
        if (!root) return;
        cout<<root->value<<' ';
        if (heightHelper(root->left) > heightHelper(root->right)) {
            printMaxPathHelper(root->left);
        } else {
            printMaxPathHelper(root->right);
        }
    }

    bool deleteValueHelper(Node<T>* parent, Node<T>* current, T value) {
        if (!current) return false;
        if (current->value == value) {
            if (current->left == NULL || current->right == NULL) {
                Node<T>* temp = current->left;
                if (current->right) temp = current->right;
                if (parent) {
                    if (parent->left == current) {
                        parent->left = temp;
                    } else {
                        parent->right = temp;
                    }
                } else {
                    this->root = temp;
                }
            } else {
                Node<T>* validSubs = current->right;
                while (validSubs->left) {
                    validSubs = validSubs->left;
                }
                T temp = current->value;
                current->value = validSubs->value;
                validSubs->value = temp;
                return deleteValueHelper(current, current->right, temp);
            }
            delete current;
            return true;
        }
        return deleteValueHelper(current, current->left, value) ||
               deleteValueHelper(current, current->right, value);
    }

    public:
    void add(T val) {
        if (root) {
            this->addHelper(root, val);
        } else {
            root = new Node<T>(val);
        }
    }

    void print() {
        printHelper(this->root);
    }

    int nodesCount() {
        return nodesCountHelper(root);
    }

    int height() {
        return heightHelper(this->root);
    }

    void printMaxPath() {
        printMaxPathHelper(this->root);
    }

    bool deleteValue(T value) {
        return this->deleteValueHelper(NULL, this->root, value);
    }
};

#define GREATER(A, B) ((A) - (B) > EPSILON)

class BinaryHeap
{
public:
	BinaryHeap(): satellite(1), size(0) {};

	//Inserts (key k, satellite s) in the heap
	void Insert(double k, int s);

	//Deletes the element with minimum key and returns its satellite information
	int DeleteMin();

	//Changes the key of the element with satellite s
	void ChangeKey(double k, int s);

	//Removes the element with satellite s
	void Remove(int s);

	//Returns the number of elements in the heap
	int Size();

	//Resets the structure
	void Clear();

private:
	vector<double> key;//Given the satellite, this is its key
	vector<int> pos;//Given the satellite, this is its position in the heap
	vector<int> satellite;//This is the heap!

	//Number of elements in the heap
	int size;
};

void BinaryHeap::Clear()
{
	key.clear();
	pos.clear();
	satellite.clear();
}

void BinaryHeap::Insert(double k, int s)
{
	//Ajust the structures to fit new data
	if(s >= (int)pos.size())
	{
		pos.resize(s+1, -1);
		key.resize(s+1);
		//Recall that position 0 of satellite is unused
		satellite.resize(s+2);
	}
	//If satellite is already in the heap
	else if(pos[s] != -1)
	{
		throw "Error: satellite already in heap";
	}

	int i;
	for(i = ++size; i/2 > 0 && GREATER(key[satellite[i/2]], k); i /= 2)
	{
		satellite[i] = satellite[i/2];
		pos[satellite[i]] = i;
	}
	satellite[i] = s;
	pos[s] = i;
	key[s] = k;
}

int BinaryHeap::Size()
{
	return size;
}

int BinaryHeap::DeleteMin()
{
	if(size == 0)
		throw "Error: empty heap";

	int min = satellite[1];
	int slast = satellite[size--];


	int child;
	int i;
	for(i = 1, child = 2; child  <= size; i = child, child *= 2)
	{
		if(child < size && GREATER(key[satellite[child]], key[satellite[child+1]]))
			child++;

		if(GREATER(key[slast], key[satellite[child]]))
		{
			satellite[i] = satellite[child];
			pos[satellite[child]] = i;
		}
		else
			break;
	}
	satellite[i] = slast;
	pos[slast] = i;

	pos[min] = -1;

	return min;
}

void BinaryHeap::ChangeKey(double k, int s)
{
	Remove(s);
	Insert(k, s);
}

void BinaryHeap::Remove(int s)
{
	int i;
	for(i = pos[s]; i/2 > 0; i /= 2)
	{
		satellite[i] = satellite[i/2];
		pos[satellite[i]] = i;
	}
	satellite[1] = s;
	pos[s] = 1;

	DeleteMin();
}
int max_contigous_subsequence_sum( const std::vector<int> & v ) {

  //special case when all the elements are negative.
  bool allNegativeOrZero = true;
  int maxSum = std::numeric_limits<int>::min();
  for ( size_t i = 0; i < v.size(); ++i ) {
    if ( v[i] > 0 ) {
      allNegativeOrZero = false;
    }
    if (v[i] > maxSum) {
      maxSum = v[i];
    }
  }

  if (allNegativeOrZero) {
    return maxSum;
  }

  maxSum = 0;

  std::vector<int> M(v.size(), 0);
  if (v[0] > 0) {
    M[0] = v[0];
  }
  for (size_t i = 1; i < v.size(); ++i) {
    if ( M[i-1] + v[i] > 0 ) {
      M[i] = M[i-1] + v[i];
    } else {
      M[i] = 0;
    }
  }
  for ( size_t i = 0; i < v.size(); ++i ) {
    if (M[i] > maxSum) {
      maxSum = M[i];
    }
  }
  return maxSum;
}

int max_contigous_subsequence_sum2( std::vector<int> & v ) {
  int max_so_far = std::numeric_limits<int>::min();
  int sum_so_far = 0;

  //special case all negative or zero
  bool allNegativeOrZero = true;
  for ( size_t i = 0; i < v.size(); ++i ) {
    if ( v[i] > 0 ) {
      allNegativeOrZero = false;
    }
    if ( v[i] > max_so_far ) {
      max_so_far = v[i];
    }
  }
  if (allNegativeOrZero) {
    return max_so_far;
  }

  //case 2 normal case;
  max_so_far = 0;
  for ( size_t i = 0; i < v.size(); ++i ) {
    sum_so_far += v[i];
    if ( sum_so_far < 0 ) {
      sum_so_far = 0;
    }
    if ( max_so_far < sum_so_far ) {
      max_so_far = sum_so_far;
    }
  }
  return max_so_far;
}


void printVec(const std::vector<int> & vec) {
  for(auto x : vec) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
}

// An AVL tree node
struct node
{
    int key;
    struct node *left, *right;
};

/* Helper function that allocates a new node with the given key and
    NULL left and right pointers. */
struct node* newNode(int key)
{
    struct node* node = (struct node*)malloc(sizeof(struct node));
    node->key   = key;
    node->left  = node->right  = NULL;
    return (node);
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
struct node *rightRotate(struct node *x)
{
    struct node *y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
struct node *leftRotate(struct node *x)
{
    struct node *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

// This function brings the key at root if key is present in tree.
// If key is not present, then it brings the last accessed item at
// root.  This function modifies the tree and returns the new root
struct node *splay(struct node *root, int key)
{
    // Base cases: root is NULL or key is present at root
    if (root == NULL || root->key == key)
        return root;

    // Key lies in left subtree
    if (root->key > key)
    {
        // Key is not in tree, we are done
        if (root->left == NULL) return root;

        // Zig-Zig (Left Left)
        if (root->left->key > key)
        {
            // First recursively bring the key as root of left-left
            root->left->left = splay(root->left->left, key);

            // Do first rotation for root, second rotation is done after else
            root = rightRotate(root);
        }
        else if (root->left->key < key) // Zig-Zag (Left Right)
        {
            // First recursively bring the key as root of left-right
            root->left->right = splay(root->left->right, key);

            // Do first rotation for root->left
            if (root->left->right != NULL)
                root->left = leftRotate(root->left);
        }

        // Do second rotation for root
        return (root->left == NULL)? root: rightRotate(root);
    }
    else // Key lies in right subtree
    {
        // Key is not in tree, we are done
        if (root->right == NULL) return root;

        // Zag-Zig (Right Left)
        if (root->right->key > key)
        {
            // Bring the key as root of right-left
            root->right->left = splay(root->right->left, key);

            // Do first rotation for root->right
            if (root->right->left != NULL)
                root->right = rightRotate(root->right);
        }
        else if (root->right->key < key)// Zag-Zag (Right Right)
        {
            // Bring the key as root of right-right and do first rotation
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        }

        // Do second rotation for root
        return (root->right == NULL)? root: leftRotate(root);
    }
}

// The search function for Splay tree.  Note that this function
// returns the new root of Splay Tree.  If key is present in tree
// then, it is moved to root.
struct node *search(struct node *root, int key)
{
    return splay(root, key);
}

// A utility function to print preorder traversal of the tree.
// The function also prints height of every node
void preOrder(struct node *root)
{
    if (root != NULL)
    {
        //printf("%d ", root->key);
        preOrder(root->left);
        preOrder(root->right);
    }
}


class Graph
{
public:
	//n is the number of vertices
	//edges is a list of pairs representing the edges (default = empty list)
	Graph(int n, const list< pair<int, int> > & edges = list< pair<int, int> >());

	//Default constructor creates an empty graph
	Graph(): n(0), m(0) {};

	//Returns the number of vertices
	int GetNumVertices() const { return n; };
	//Returns the number of edges
	int GetNumEdges() const { return m; };

	//Given the edge's index, returns its endpoints as a pair
	pair<int, int> GetEdge(int e) const;
	//Given the endpoints, returns the index
	int GetEdgeIndex(int u, int v) const;

	//Adds a new vertex to the graph
	void AddVertex();
	//Adds a new edge to the graph
	void AddEdge(int u, int v);

	//Returns the adjacency list of a vertex
	const list<int> & AdjList(int v) const;

	//Returns the graph's adjacency matrix
	const vector< vector<bool> > & AdjMat() const;
private:
	//Number of vertices
	int n;
	//Number of edges
	int m;

	//Adjacency matrix
	vector< vector<bool> > adjMat;

	//Adjacency lists
	vector< list<int> > adjList;

	//Array of edges
	vector< pair<int, int> > edges;

	//Indices of the edges
	vector< vector<int> > edgeIndex;
};

Graph::Graph(int n, const list< pair<int, int> > & edges):
	n(n),
	m(edges.size()),
	adjMat(n, vector<bool>(n, false)),
	adjList(n),
	edges(),
	edgeIndex(n, vector<int>(n, -1))
{
	for(list< pair<int, int> >::const_iterator it = edges.begin(); it != edges.end(); it++)
	{
		int u = (*it).first;
		int v = (*it).second;

		AddEdge(u, v);
	}
}

pair<int, int> Graph::GetEdge(int e) const
{
	if(e > (int)edges.size())
		throw "Error: edge does not exist";

	return edges[e];
}

int Graph::GetEdgeIndex(int u, int v) const
{
	if( u > n or
		v > n )
		throw "Error: vertex does not exist";

	if(edgeIndex[u][v] == -1)
		throw "Error: edge does not exist";

	return edgeIndex[u][v];
}

void Graph::AddVertex()
{
	for(int i = 0; i < n; i++)
	{
		adjMat[i].push_back(false);
		edgeIndex[i].push_back(-1);
	}
	n++;
	adjMat.push_back( vector<bool>(n, false) );
	edgeIndex.push_back( vector<int>(n, -1) );
	adjList.push_back( list<int>() );
}

void Graph::AddEdge(int u, int v)
{
	if( u > n or
		v > n )
		throw "Error: vertex does not exist";

	if(adjMat[u][v]) return;

	adjMat[u][v] = adjMat[v][u] = true;
	adjList[u].push_back(v);
	adjList[v].push_back(u);

	edges.push_back(pair<int, int>(u, v));
	edgeIndex[u][v] = edgeIndex[v][u] = m++;
}

const list<int> & Graph::AdjList(int v) const
{
	if(v > n)
		throw "Error: vertex does not exist";

	return adjList[v];
}

const vector< vector<bool> > & Graph::AdjMat() const
{
	return adjMat;
}


pair< vector<int>, vector<double> > Dijkstra(const Graph & G, int origin, const vector<double> & cost)
{
	BinaryHeap B;

	int n = G.GetNumVertices();

	//Father of each vertex in the optimal path tree
	vector<int> father(n, -1);

	//Used to indicate whether a vertex is permanently labeled
	vector<bool> permanent(n, false);

	vector<double> pathCost(n, numeric_limits<double>::infinity());

	//Put s in the heap
	B.Insert(0, origin);
	pathCost[origin] = 0;

	for(int i = 0; i < n; i++)
	{
		//Select the vertex that can be reached with smallest cost
		int u = B.DeleteMin();

		permanent[u] = true;

		//Update the heap with vertices adjacent to u
		for(list<int>::const_iterator it = G.AdjList(u).begin(); it != G.AdjList(u).end(); it++)
		{
			int v = *it;

			if(permanent[v])
				continue;

			double c = pathCost[u] + cost[G.GetEdgeIndex(u,v)];

			//v has not been discovered yet
			if(father[v] == -1)
			{
				father[v] = u;
				pathCost[v] = c;
				B.Insert(c, v);
			}
			//we found a cheaper connection to v
			else if( LESS(c, pathCost[v]) )
			{
				father[v] = u;
				pathCost[v] = c;
				B.ChangeKey(c, v);
			}
		}
	}

	if(B.Size() > 0)
		throw "Error: graph is not connected";

	return make_pair(father, pathCost);
}


#define EVEN 2
#define ODD 1
#define UNLABELED 0



class Matching
{
public:
	Matching(const Graph & G);
	pair< list<int>, double > SolveMinimumCostPerfectMatching(const vector<double> & cost);

	list<int> SolveMaximumMatching();

private:
	void Grow();
	void Expand(int u, bool expandBlocked);
	void Augment(int u, int v);
	void Reset();
	int Blossom(int u, int v);
	void UpdateDualCosts();
	void Clear();
	void DestroyBlossom(int t);
	void Heuristic();
	void PositiveCosts();
	list<int> RetrieveMatching();

	int GetFreeBlossomIndex();
	void AddFreeBlossomIndex(int i);
	void ClearBlossomIndices();

	//An edge might be blocked due to the dual costs
	bool IsEdgeBlocked(int u, int v);
	bool IsEdgeBlocked(int e);
	//Returns true if u and v are adjacent in G and not blocked
	bool IsAdjacent(int u, int v);

	const Graph & G;

	list<int> free;

	vector<int> outer;
	vector< list<int> > deep;//deep[v] is a list of all the original vertices contained inside v, deep[v] = v if v is an original vertex
	vector< list<int> > shallow;//shallow[v] is a list of the vertices immediately contained inside v, shallow[v] is empty is the default
	vector<int> tip;//tip[v] is the tip of blossom v
	vector<bool> active;//true if a blossom is being used

	vector<int> type;//Even, odd, neither (2, 1, 0)
	vector<int> forest;//forest[v] gives the father of v in the alternating forest
	vector<int> root;//root[v] gives the root of v in the alternating forest

	vector<bool> blocked;//A blossom can be blocked due to dual costs, this means that it behaves as if it were an original vertex and cannot be expanded
	vector<double> dual;//dual multipliers associated to the blossoms, if dual[v] > 0, the blossom is blocked and full
	vector<double> slack;//slack associated to each edge, if slack[e] > 0, the edge cannot be used
	vector<int> mate;//mate[v] gives the mate of v

	int m, n;

	bool perfect;

	list<int> forestList;
	vector<int> visited;
};


Matching::Matching(const Graph & G):
	G(G),
	outer(2*G.GetNumVertices()),
	deep(2*G.GetNumVertices()),
	shallow(2*G.GetNumVertices()),
	tip(2*G.GetNumVertices()),
	active(2*G.GetNumVertices()),
	type(2*G.GetNumVertices()),
	forest(2*G.GetNumVertices()),
	root(2*G.GetNumVertices()),
	blocked(2*G.GetNumVertices()),
	dual(2*G.GetNumVertices()),
	slack(G.GetNumEdges()),
	mate(2*G.GetNumVertices()),
	m(G.GetNumEdges()),
	n(G.GetNumVertices()),
	visited(2*G.GetNumVertices())
{
}

void Matching::Grow()
{
	Reset();

	//All unmatched vertices will be roots in a forest that will be grown
	//The forest is grown by extending a unmatched vertex w through a matched edge u-v in a BFS fashion
	while(!forestList.empty())
	{
		int w = outer[forestList.front()];
		forestList.pop_front();

		//w might be a blossom
		//we have to explore all the connections from vertices inside the blossom to other vertices
		for(list<int>::iterator it = deep[w].begin(); it != deep[w].end(); it++)
		{
			int u = *it;

			int cont = false;
			for(list<int>::const_iterator jt = G.AdjList(u).begin(); jt != G.AdjList(u).end(); jt++)
			{
				int v = *jt;

				if(IsEdgeBlocked(u, v)) continue;

				//u is even and v is odd
				if(type[outer[v]] == ODD) continue;

				//if v is unlabeled
				if(type[outer[v]] != EVEN)
				{
					//We grow the alternating forest
					int vm = mate[outer[v]];

					forest[outer[v]] = u;
					type[outer[v]] = ODD;
					root[outer[v]] = root[outer[u]];
					forest[outer[vm]] = v;
					type[outer[vm]] = EVEN;
					root[outer[vm]] = root[outer[u]];

					if(!visited[outer[vm]])
					{
						forestList.push_back(vm);
						visited[outer[vm]] = true;
					}
				}
				//If v is even and u and v are on different trees
				//we found an augmenting path
				else if(root[outer[v]] != root[outer[u]])
				{
					Augment(u,v);
					Reset();

					cont = true;
					break;
				}
				//If u and v are even and on the same tree
				//we found a blossom
				else if(outer[u] != outer[v])
				{
					int b = Blossom(u,v);

					forestList.push_front(b);
					visited[b] = true;

					cont = true;
					break;
				}
			}
			if(cont) break;
		}
	}

	//Check whether the matching is perfect
	perfect = true;
	for(int i = 0; i < n; i++)
		if(mate[outer[i]] == -1)
			perfect = false;
}

bool Matching::IsAdjacent(int u, int v)
{
	return (G.AdjMat()[u][v] and not IsEdgeBlocked(u, v));
}

bool Matching::IsEdgeBlocked(int u, int v)
{
	return GREATER(slack[ G.GetEdgeIndex(u, v) ], 0);
}

bool Matching::IsEdgeBlocked(int e)
{
	return GREATER(slack[e], 0);
}

//Vertices will be selected in non-decreasing order of their degree
//Each time an unmatched vertex is selected, it is matched to its adjacent unmatched vertex of minimum degree
void Matching::Heuristic()
{
	vector<int> degree(n, 0);
	BinaryHeap B;

	for(int i = 0; i < m; i++)
	{
		if(IsEdgeBlocked(i)) continue;

		pair<int, int> p = G.GetEdge(i);
		int u = p.first;
		int v = p.second;

		degree[u]++;
		degree[v]++;
	}

	for(int i = 0; i < n; i++)
		B.Insert(degree[i], i);

	while(B.Size() > 0)
	{
		int u = B.DeleteMin();
		if(mate[outer[u]] == -1)
		{
			int min = -1;
			for(list<int>::const_iterator it = G.AdjList(u).begin(); it != G.AdjList(u).end(); it++)
			{
				int v = *it;

				if(IsEdgeBlocked(u, v) or
					(outer[u] == outer[v]) or
					(mate[outer[v]] != -1) )
					continue;

				if(min == -1 or degree[v] < degree[min])
					min = v;
			}
			if(min != -1)
			{
				mate[outer[u]] = min;
				mate[outer[min]] = u;
			}
		}
	}
}

//Destroys a blossom recursively
void Matching::DestroyBlossom(int t)
{
	if((t < n) or
		(blocked[t] and GREATER(dual[t], 0))) return;

	for(list<int>::iterator it = shallow[t].begin(); it != shallow[t].end(); it++)
	{
		int s = *it;
		outer[s] = s;
		for(list<int>::iterator jt = deep[s].begin(); jt != deep[s].end(); jt++)
			outer[*jt] = s;

		DestroyBlossom(s);
	}

	active[t] = false;
	blocked[t] = false;
	AddFreeBlossomIndex(t);
	mate[t] = -1;
}

void Matching::Expand(int u, bool expandBlocked = false)
{
	int v = outer[mate[u]];

	int index = m;
	int p = -1, q = -1;
	//Find the regular edge {p,q} of minimum index connecting u and its mate
	//We use the minimum index to grant that the two possible blossoms u and v will use the same edge for a mate
	for(list<int>::iterator it = deep[u].begin(); it != deep[u].end(); it++)
	{
		int di = *it;
		for(list<int>::iterator jt = deep[v].begin(); jt != deep[v].end(); jt++)
		{
			int dj = *jt;
			if(IsAdjacent(di, dj) and G.GetEdgeIndex(di, dj) < index)
			{
				index = G.GetEdgeIndex(di, dj);
				p = di;
				q = dj;
			}
		}
	}

	mate[u] = q;
	//If u is a regular vertex, we are done
	if(u < n or (blocked[u] and not expandBlocked)) return;

	bool found = false;
	//Find the position t of the new tip of the blossom
	for(list<int>::iterator it = shallow[u].begin(); it != shallow[u].end() and not found; )
	{
		int si = *it;
		for(list<int>::iterator jt = deep[si].begin(); jt != deep[si].end() and not found; jt++)
		{
			if(*jt == p )
				found = true;
		}
		it++;
		if(not found)
		{
			shallow[u].push_back(si);
			shallow[u].pop_front();
		}
	}

	list<int>::iterator it = shallow[u].begin();
	//Adjust the mate of the tip
	mate[*it] = mate[u];
	it++;
	//
	//Now we go through the odd circuit adjusting the new mates
	while(it != shallow[u].end())
	{
		list<int>::iterator itnext = it;
		itnext++;
		mate[*it] = *itnext;
		mate[*itnext] = *it;
		itnext++;
		it = itnext;
	}

	//We update the sets blossom, shallow, and outer since this blossom is being deactivated
	for(list<int>::iterator it = shallow[u].begin(); it != shallow[u].end(); it++)
	{
		int s = *it;
		outer[s] = s;
		for(list<int>::iterator jt = deep[s].begin(); jt != deep[s].end(); jt++)
			outer[*jt] = s;
	}
	active[u] = false;
	AddFreeBlossomIndex(u);

	//Expand the vertices in the blossom
	for(list<int>::iterator it = shallow[u].begin(); it != shallow[u].end(); it++)
		Expand(*it, expandBlocked);

}

//Augment the path root[u], ..., u, v, ..., root[v]
void Matching::Augment(int u, int v)
{
	//We go from u and v to its respective roots, alternating the matching
	int p = outer[u];
	int q = outer[v];
	int fp = forest[p];
	mate[p] = q;
	mate[q] = p;
	Expand(p);
	Expand(q);
	while(fp != -1)
	{
		q = outer[forest[p]];
		p = outer[forest[q]];
		fp = forest[p];

		mate[p] = q;
		mate[q] = p;
		Expand(p);
		Expand(q);
	}

	p = outer[v];
	fp = forest[p];
	while(fp != -1)
	{
		q = outer[forest[p]];
		p = outer[forest[q]];
		fp = forest[p];

		mate[p] = q;
		mate[q] = p;
		Expand(p);
		Expand(q);
	}
}

void Matching::Reset()
{
	for(int i = 0; i < 2*n; i++)
	{
		forest[i] = -1;
		root[i] = i;

		if(i >= n and active[i] and outer[i] == i)
			DestroyBlossom(i);
	}

	visited.assign(2*n, 0);
	forestList.clear();
	for(int i = 0; i < n; i++)
	{
		if(mate[outer[i]] == -1)
		{
			type[outer[i]] = 2;
			if(!visited[outer[i]])
				forestList.push_back(i);
			visited[outer[i]] = true;
		}
		else type[outer[i]] = 0;
	}
}

int Matching::GetFreeBlossomIndex()
{
	int i = free.back();
	free.pop_back();
	return i;
}

void Matching::AddFreeBlossomIndex(int i)
{
	free.push_back(i);
}

void Matching::ClearBlossomIndices()
{
	free.clear();
	for(int i = n; i < 2*n; i++)
		AddFreeBlossomIndex(i);
}

//Contracts the blossom w, ..., u, v, ..., w, where w is the first vertex that appears in the paths from u and v to their respective roots
int Matching::Blossom(int u, int v)
{
	int t = GetFreeBlossomIndex();

	vector<bool> isInPath(2*n, false);

	//Find the tip of the blossom
	int u_ = u;
	while(u_ != -1)
	{
		isInPath[outer[u_]] = true;

		u_ = forest[outer[u_]];
	}

	int v_ = outer[v];
	while(not isInPath[v_])
		v_ = outer[forest[v_]];
	tip[t] = v_;

	//Find the odd circuit, update shallow, outer, blossom and deep
	//First we construct the set shallow (the odd circuit)
	list<int> circuit;
	u_ = outer[u];
	circuit.push_front(u_);
	while(u_ != tip[t])
	{
		u_ = outer[forest[u_]];
		circuit.push_front(u_);
	}

	shallow[t].clear();
	deep[t].clear();
	for(list<int>::iterator it = circuit.begin(); it != circuit.end(); it++)
	{
		shallow[t].push_back(*it);
	}

	v_ = outer[v];
	while(v_ != tip[t])
	{
		shallow[t].push_back(v_);
		v_ = outer[forest[v_]];
	}

	//Now we construct deep and update outer
	for(list<int>::iterator it = shallow[t].begin(); it != shallow[t].end(); it++)
	{
		u_ = *it;
		outer[u_] = t;
		for(list<int>::iterator jt = deep[u_].begin(); jt != deep[u_].end(); jt++)
		{
			deep[t].push_back(*jt);
			outer[*jt] = t;
		}
	}

	forest[t] = forest[tip[t]];
	type[t] = EVEN;
	root[t] = root[tip[t]];
	active[t] = true;
	outer[t] = t;
	mate[t] = mate[tip[t]];

	return t;
}

void Matching::UpdateDualCosts()
{
	double e1 = 0, e2 = 0, e3 = 0;
	int inite1 = false, inite2 = false, inite3 = false;
	for(int i = 0; i < m; i++)
	{
		int u = G.GetEdge(i).first,
			v = G.GetEdge(i).second;

		if( (type[outer[u]] == EVEN and type[outer[v]] == UNLABELED) or (type[outer[v]] == EVEN and type[outer[u]] == UNLABELED) )
		{
			if(!inite1 or GREATER(e1, slack[i]))
			{
				e1 = slack[i];
				inite1 = true;
			}
		}
		else if( (outer[u] != outer[v]) and type[outer[u]] == EVEN and type[outer[v]] == EVEN )
		{
			if(!inite2 or GREATER(e2, slack[i]))
			{
				e2 = slack[i];
				inite2 = true;
			}
		}
	}
	for(int i = n; i < 2*n; i++)
	{
		if(active[i] and i == outer[i] and type[outer[i]] == ODD and (!inite3 or GREATER(e3, dual[i])))
		{
			e3 = dual[i];
			inite3 = true;
		}
	}
	double e = 0;
	if(inite1) e = e1;
	else if(inite2) e = e2;
	else if(inite3) e = e3;

	if(GREATER(e, e2/2.0) and inite2)
		e = e2/2.0;
	if(GREATER(e, e3) and inite3)
		e = e3;

	for(int i = 0; i < 2*n; i++)
	{
		if(i != outer[i]) continue;

		if(active[i] and type[outer[i]] == EVEN)
		{
			dual[i] += e;
		}
		else if(active[i] and type[outer[i]] == ODD)
		{
			dual[i] -= e;
		}
	}

	for(int i = 0; i < m; i++)
	{
		int u = G.GetEdge(i).first,
			v = G.GetEdge(i).second;

		if(outer[u] != outer[v])
		{
			if(type[outer[u]] == EVEN and type[outer[v]] == EVEN)
				slack[i] -= 2.0*e;
			else if(type[outer[u]] == ODD and type[outer[v]] == ODD)
				slack[i] += 2.0*e;
			else if( (type[outer[v]] == UNLABELED and type[outer[u]] == EVEN) or (type[outer[u]] == UNLABELED and type[outer[v]] == EVEN) )
				slack[i] -= e;
			else if( (type[outer[v]] == UNLABELED and type[outer[u]] == ODD) or (type[outer[u]] == UNLABELED and type[outer[v]] == ODD) )
				slack[i] += e;
		}
	}
	for(int i = n; i < 2*n; i++)
	{
		if(GREATER(dual[i], 0))
		{
			blocked[i] = true;
		}
		else if(active[i] and blocked[i])
		{
			//The blossom is becoming unblocked
			if(mate[i] == -1)
			{
				DestroyBlossom(i);
			}
			else
			{
				blocked[i] = false;
				Expand(i);
			}
		}
	}
}

pair< list<int>, double> Matching::SolveMinimumCostPerfectMatching(const vector<double> & cost)
{
	SolveMaximumMatching();
	if(!perfect)
		throw "Error: The graph does not have a perfect matching";

	Clear();

	//Initialize slacks (reduced costs for the edges)
	slack = cost;

	PositiveCosts();

	//If the matching on the compressed graph is perfect, we are done
	perfect = false;
	while(not perfect)
	{
		//Run an heuristic maximum matching algorithm
		Heuristic();
		//Grow a hungarian forest
		Grow();
		UpdateDualCosts();
		//Set up the algorithm for a new grow step
		Reset();
	}

	list<int> matching = RetrieveMatching();

	double obj = 0;
	for(list<int>::iterator it = matching.begin(); it != matching.end(); it++)
		obj += cost[*it];

	double dualObj = 0;
	for(int i = 0; i < 2*n; i++)
	{
		if(i < n) dualObj += dual[i];
		else if(blocked[i]) dualObj += dual[i];
	}

	return pair< list<int>, double >(matching, obj);
}

void Matching::PositiveCosts()
{
	double minEdge = 0;
	for(int i = 0; i < m ;i++)
		if(GREATER(minEdge - slack[i], 0))
			minEdge = slack[i];

	for(int i = 0; i < m; i++)
		slack[i] -= minEdge;
}

list<int> Matching::SolveMaximumMatching()
{
	Clear();
	Grow();
	return RetrieveMatching();
}

//Sets up the algorithm for a new run
void Matching::Clear()
{
	ClearBlossomIndices();

	for(int i = 0; i < 2*n; i++)
	{
		outer[i] = i;
		deep[i].clear();
		if(i<n)
			deep[i].push_back(i);
		shallow[i].clear();
		if(i < n) active[i] = true;
		else active[i] = false;

		type[i] = 0;
		forest[i] = -1;
		root[i] = i;

		blocked[i] = false;
		dual[i] = 0;
		mate[i] = -1;
		tip[i] = i;
	}
	slack.assign(m, 0);
}

list<int> Matching::RetrieveMatching()
{
	list<int> matching;

	for(int i = 0; i < 2*n; i++)
		if(active[i] and mate[i] != -1 and outer[i] == i)
			Expand(i, true);

	for(int i = 0; i < m; i++)
	{
		int u = G.GetEdge(i).first;
		int v = G.GetEdge(i).second;

		if(mate[u] == v)
			matching.push_back(i);
	}
	return matching;
}

bool Connected(const Graph & G)
{
    vector<bool> visited(G.GetNumVertices(), false);
    list<int> L;

    int n = 0;
    L.push_back(0);
    while(not L.empty())
    {
        int u = L.back();
        L.pop_back();
        if(visited[u]) continue;

        visited[u] = true;
        n++;

        for(list<int>::const_iterator it = G.AdjList(u).begin(); it != G.AdjList(u).end(); it++)
		{
			int v = *it;
		    L.push_back(v);
		}
    }

    return G.GetNumVertices() == n;
}

/*
Solves the chinese postman problem
returns a pair containing a list and a double
the list is the sequence of vertices in the solution
the double is the solution cost
*/
pair< list<int>, double > solver(const Graph & G, const vector<double> & cost)
{
	//Check if the graph if connected
	if(not Connected(G))
		throw "Error: Graph is not connected";

	//Build adjacency lists using edges in the graph
	vector< list<int> > A(G.GetNumVertices(), list<int>());
	for(int u = 0; u < G.GetNumVertices(); u++)
	    A[u] = G.AdjList(u);

	//Find vertices with odd degree
	vector<int> odd;
	for(int u = 0; u < G.GetNumVertices(); u++)
		if(A[u].size() % 2)
			odd.push_back(u);

    //If there are odd degree vertices
	if(not odd.empty())
	{
		//Create a graph with the odd degree vertices
		Graph O(odd.size());
		for(int u = 0; u < (int)odd.size(); u++)
			for(int v = u+1; v < (int)odd.size(); v++)
				O.AddEdge(u, v);

        vector<double> costO(O.GetNumEdges());

        //Find the shortest paths between all odd degree vertices
		vector< vector<int> > shortestPath(O.GetNumVertices());
		for(int u = 0; u < (int)odd.size(); u++)
		{
			pair< vector<int>, vector<double> > sp = Dijkstra(G, odd[u], cost);

			shortestPath[u] = sp.first ;

			//The cost of an edge uv in O will be the cost of the corresponding shortest path in G
			for(int v = 0; v < (int)odd.size(); v++)
			    if(v != u)
    			    costO[ O.GetEdgeIndex(u, v) ] = sp.second[odd[v]];
		}

	    //Find the minimum cost perfect matching of the graph of the odd degree vertices
	    Matching M(O);
	    pair< list<int>, double > p = M.SolveMinimumCostPerfectMatching(costO);
	    list<int> matching = p.first;

	    //If an edge uv is in the matching, the edges in the shortest path from u to v should be duplicated in G
	    for(list<int>::iterator it = matching.begin(); it != matching.end(); it++)
	    {
		    pair<int, int> p = O.GetEdge(*it);
		    int u = p.first, v = odd[p.second];

		    //Go through the path adding the edges
		    int w = shortestPath[u][v];
		    while(w != -1)
		    {
		        A[w].push_back(v);
		        A[v].push_back(w);
		        v = w;
		        w = shortestPath[u][v];
		    }
	    }
	}

	//Find an Eulerian cycle in the graph implied by A
	list<int> cycle;
	//This is to keep track of how many times we can traverse an edge
	vector<int> traversed(G.GetNumEdges(), 0);
	for(int u = 0; u < G.GetNumVertices(); u++)
	{
		for(list<int>::iterator it = A[u].begin(); it != A[u].end(); it++)
		{
			int v = *it;

			//we do this so that the edge is not counted twice
			if(v < u) continue;

			traversed[G.GetEdgeIndex(u, v)]++;
		}
	}

	cycle.push_back(0);
	list<int>::iterator itp = cycle.begin();
	double obj = 0;
	while(itp != cycle.end())
	{
		//Let u be the current vertex in the cycle, starting at the first
		int u = *itp;
		list<int>::iterator jtp = itp;
		jtp++;

		//if there are non-traversed edges incident to u, find a subcycle starting at u
		//replace u in the cycle by the subcycle
		while(not A[u].empty())
		{
			while(not A[u].empty() and traversed[ G.GetEdgeIndex(u, A[u].back()) ] == 0)
				A[u].pop_back();

			if(not A[u].empty())
			{
				int v = A[u].back();
				A[u].pop_back();
				cycle.insert(jtp, v);
				traversed[G.GetEdgeIndex(u, v)]--;
		        obj += cost[ G.GetEdgeIndex(u, v) ];
				u = v;
			}
		}

		//go to the next vertex in the cycle and do the same
		itp++;
	}

	return pair< list<int>, double >(cycle, obj);
}

class GGG
{
    int V;    // No. of vertices

    // Pointer to an array containing
    // adjacency lists
    list<int> *adj;

    // A recursive function used by DFS
    void DFSUtil(int v, bool visited[]);
public:
    GGG(int V);   // Constructor

    // function to add an edge to graph
    void addEdge(int v, int w);

    // DFS traversal of the vertices
    // reachable from v
    void DFS(int v);
};

GGG::GGG(int V)
{
    this->V = V;
    adj = new list<int>[V];
}

void GGG::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
}

void GGG::DFSUtil(int v, bool visited[])
{
    // Mark the current node as visited and
    // print it
    visited[v] = true;
    //cout << v << " ";

    // Recur for all the vertices adjacent
    // to this vertex
    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            DFSUtil(*i, visited);
}

// DFS traversal of the vertices reachable from v.
// It uses recursive DFSUtil()
void GGG::DFS(int v)
{
    // Mark all the vertices as not visited
    bool *visited = new bool[V];
    for (int i = 0; i < V; i++)
        visited[i] = false;

    // Call the recursive helper function
    // to print DFS traversal
    DFSUtil(v, visited);
}

class StackNode {
    public:
    int data;
    StackNode *next;

    StackNode(int data)
    {
        this->data = data;
        this->next = NULL;
    }
};

class Stack {

    StackNode *top;

    public:

    // Push and pop operations
    void push(int data)
    {
        if (top == NULL) {
            top = new StackNode(data);
            return;
        }
        StackNode *s = new StackNode(data);
        s->next = top;
        top = s;
    }

    StackNode* pop()
    {
        StackNode *s = top;
        top = top->next;
        return s;
    }

    // prints contents of stack
    void display()
    {
        StackNode *s = top;
        while (s != NULL) {
            cout << s->data << " ";
            s = s->next;
        }
        cout << endl;
    }

    // Reverses the stack using simple
    // linked list reversal logic.
    void reverse()
    {
        StackNode *prev, *cur, *succ;
        cur = prev = top;
        cur = cur->next;
        prev->next = NULL;
        while (cur != NULL) {

            succ = cur->next;
            cur->next = prev;
            prev = cur;
            cur = succ;
        }
        top = prev;
    }
};

// implementation of traveling Salesman Problem
int travllingSalesmanProblem(int graph[][Q], int s)
{
    // store all vertex apart from source vertex
    vector<int> vertex;
    for (int i = 0; i < Q; i++)
        if (i != s)
            vertex.push_back(i);

    // store minimum weight Hamiltonian Cycle.
    int min_path = INT_MAX;
    do {

        // store current Path weight(cost)
        int current_pathweight = 0;

        // compute current path weight
        int k = s;
        for (int i = 0; i < vertex.size(); i++) {
            current_pathweight += graph[k][vertex[i]];
            k = vertex[i];
        }
        current_pathweight += graph[k][s];

        // update minimum
        min_path = min(min_path, current_pathweight);

    } while (next_permutation(vertex.begin(), vertex.end()));

    return min_path;
}

void buildMaxHeap(int arr[], int n)
{
    for (int i = 1; i < n; i++)
    {
        // if child is bigger than parent
        if (arr[i] > arr[(i - 1) / 2])
        {
            int j = i;

            // swap child and parent until
            // parent is smaller
            while (arr[j] > arr[(j - 1) / 2])
            {
                swap(arr[j], arr[(j - 1) / 2]);
                j = (j - 1) / 2;
            }
        }
    }
}

void heapSort(int arr[], int n)
{
    buildMaxHeap(arr, n);

    for (int i = n - 1; i > 0; i--)
    {
        // swap value of first indexed
        // with last indexed
        swap(arr[0], arr[i]);

        // maintaining heap property
        // after each swapping
        int j = 0, index;

        do
        {
            index = (2 * j + 1);

            // if left child is smaller than
            // right child point index variable
            // to right child
            if (arr[index] < arr[index + 1] &&
                                index < (i - 1))
                index++;

            // if parent is smaller than child
            // then swapping parent with child
            // having higher value
            if (arr[j] < arr[index] && index < i)
                swap(arr[j], arr[index]);

            j = index;

        } while (index < i);
    }
}


class Algorithm
{
public:
    Algorithm(std::string const &name);
    virtual ~Algorithm();

    virtual long Run(std::list<std::list<int> > &cliques) = 0;
    virtual void Run() {}

    void SetName(std::string const &name);
    std::string GetName() const;

    void AddCallBack(std::function<void(std::list<int> const&)> callback);

    void ExecuteCallBacks(std::list<int> const &vertexSet) const;

    void SetQuiet(bool const quiet);
    bool GetQuiet() const;

private:
    std::string m_sName;
    bool m_bQuiet;
    std::vector<std::function<void(std::list<int> const&)> > m_vCallBacks;
};
Algorithm::Algorithm(std::string const &name)
 : m_sName(name)
 , m_bQuiet(false)
 , m_vCallBacks()
{
}

Algorithm::~Algorithm()
{
}

void Algorithm::AddCallBack(std::function<void(std::list<int> const&)> callback)
{
    m_vCallBacks.push_back(callback);
}

void Algorithm::ExecuteCallBacks(std::list<int> const &vertexSet) const
{
    for (auto &function : m_vCallBacks) {
        function(vertexSet);
    }
}

void Algorithm::SetName(string const &name)
{
    m_sName = name;
}

string Algorithm::GetName() const
{
    return m_sName;
}

void Algorithm::SetQuiet(bool const quiet)
{
    m_bQuiet = quiet;
}

bool Algorithm::GetQuiet() const
{
    return m_bQuiet;
}
//modifies the matrix
void modify(bool arr[3][4],int r,int c)
{ //for knowing whether the first row and column needs to be made true ,as they are to be used for storing indices
	int rowFlag=0;
	int colFlag=0;
	/*here the first row and column of matrix will be used for storing the indices of columns and rows  respectively*/
	//check for the first row
	for (int i = 0; i < c; ++i)
	{
		if(arr[0][i]==true)
		{
			rowFlag=1;
			break;
		}
	}
	//check for the first row
	for (int i = 0; i < r; ++i)
	{
		if(arr[i][0]==true)
		{
			colFlag=1;
			break;
		}
	}

	//check for true value in rows and columns from 2nd row and column
	//and save their indices
	for (int i = 1; i < r; ++i)
	{
		for (int j = 1; j < c; ++j)
		{
			if(arr[i][j]==true)
			{
			//save indices of columns in first row and indices of rows in first column
			//since first row already might contain the indices of columns having true value,same goes for first column
				arr[0][j]=true;		//1st row
				arr[i][0]=true;		//1st column
			}
		}
	}
//modify the ith row
	for (int i = 1; i < r; ++i)
	{
		if(arr[i][0]==true)
		{
			for (int j = 1; j < c; ++j)
				arr[i][j]=true;
		}
	}
//modify the jth column
	for (int j = 1; j < c; ++j)
		if(arr[0][j]==true)
		{
			for (int i = 1; i < r; ++i)
				arr[i][j]=true;
		}

//make first row and first column true based on flags
	if(rowFlag)
		for (int i = 0; i < c; ++i)
			arr[0][i]=true;

	if(colFlag)
		for (int i = 0; i < r; ++i)
			arr[i][0]=true;

}
void printSolution(vector< vector<int> > sol){
	for(int i = 0; i<sol[0].size(); i++){
		for(int j = 0; j<sol[0].size(); j++){
			cout<<sol[i][j]<<" ";
		}
		cout<<endl;
	}
}

//checks if the position for the queen is valid or not
//queen can attack in all directions
bool isValidPosition(vector< vector<int> > &sol, int row, int col){
	int n = sol[0].size();
	//check in the column
	for (int i = 0; i < n; ++i)
	{
		if(sol[i][col] == 1)
			return false;
	}

	int i,j;
	//diagonal checking
	//upwards left
	for(i = row,j = col; i>=0 && j>=0; j--,i--)
		if(sol[i][j] == 1)
			return false;
	//downwards right
	for(i = row,j = col; i<n && j<n; j++,i++)
		if(sol[i][j] == 1)
			return false;
	//upwards right
	for(i = row,j = col; i>=0 && j<n; j++,i--)
		if(sol[i][j] == 1)
			return false;
	//downwards left
	for(i = row,j = col; i<n && j>=0; j--,i++)
		if(sol[i][j] == 1)
			return false;

	return true;
}

//for placing the N queens
bool placeNQueens(vector< vector<int> > &sol, int queens, int row){
	if(queens == 0)
		return true;
	//we check for a col for the queen to place in for a fixed row
	//and each time increase the row index
	for (int j = 0; j < sol[0].size(); ++j){
		if(isValidPosition(sol,row,j)){
			//	cout<<"valid "<<row<<" "<<j<<endl;
				sol[row][j] = 1;
				if(placeNQueens(sol,queens-1,row+1))
					return true;
				else
					sol[row][j] = 0;
			}
	}

	return false;
}

//driver function for N queens
void placeNQueensDriver(int n){
	//create a NxN 2d matrix and initialise with 0
	vector< vector<int> > sol(n, vector<int>(n,0));

	//for keeping count of queens placed
	int queens = n;
	//starting index for the matrix
	int l = 0;

	if(placeNQueens(sol,queens,l))
		printSolution(sol);
	else
		cout<<"Queens can't be placed";
}

void printPath(vector<int> path){
	for (int i = 0; i < path.size(); ++i)
	{
		cout<<path[i]<<" ";
	}
	cout<<endl;
}

//for checking if the move is valid or not
bool isValidMove(int move_x, int move_y,
				 vector< vector<int> > &sol,
				 vector< vector<int> > &maze){

	return (move_x >= 0 && move_x < sol.size() && move_y >= 0 && move_y < sol.size()
			&& sol[move_x][move_y] == 0
			&& maze[move_x][move_y] == 1);
}

//for finding if there is any path inside the maze for the rat to reach its dest or not
bool findMazePath(int curr_x, int curr_y, vector< vector<int> > &sol,
					vector<int> x_coor, vector<int> y_coor, int n,
					vector< vector<int> > &maze){
	//check if the dest is reached or not,
	//assuming the dest is always 1
	if(curr_x == n-1 && curr_y == n-1){
		sol[n-1][n-1] = 1;
		return true;
	}

	int x_move ;
	int y_move ;
	//it has two possible moves
	for (int i = 0; i < 2; ++i)
	{
		x_move = curr_x + x_coor[i];
		y_move = curr_y + y_coor[i];

		if(isValidMove(x_move, y_move, sol, maze)){
			sol[curr_x][curr_y] = 1;
			if(findMazePath(x_move, y_move, sol, x_coor, y_coor, n, maze))
				return true;
			else
				sol[curr_x][curr_y] = 0;
		}
	}
	return false;
}
//trie structure
struct TrieNode
{
  struct TrieNode* child[26];
  bool is_str_end;
};

//to create a trieNode type node
struct TrieNode* createNode()
{
  struct TrieNode *t=new struct TrieNode;
  if(t)
  {
    t->is_str_end=false;
    for (int i = 0; i < 26; ++i)
    {
        t->child[i]=NULL;
    }
    return t;
  }
  else
    return NULL;
}

//to insert a new character in the Trie ,if it is already present then ignore and
//keep on traversing else insert the key
void InsertNode(struct TrieNode *root,string word)
{
  int index;    //for storing the index of the character in a-z
  struct TrieNode *t=root;

  for(int i=0;i<word.length();i++)
  {
    index=word[i]-'a';
    if(t->child[index]==NULL)
        t->child[index]=createNode();

    t=t->child[index];
  }

  //when finally the string end
  t->is_str_end=true;
}

//searches the string in the Trie
bool searchString(struct TrieNode *root,string word)
{
   int index;
   struct TrieNode *t=root;

   for (int i = 0; i <word.length(); ++i)
   {
      index=word[i]-'a';
      if(t->child[index]==NULL)
      {
        return false;
       }
       t=t->child[index];
   }
   if(t!=NULL && t->is_str_end==true)
      return true;
}

//driver function for finding the path inside the maze
void findMazePathDriver(vector< vector<int> > maze){

	//sol contains the path for the rat to escape the maze
	vector < vector<int> > sol(maze[0].size(), vector<int>(maze[0].size(),0));

	//valid set of moves ,rat can move forward and down only
	vector<int> x_coor ;
	vector<int> y_coor ;

	//mark the starting
	sol[0][0] = 1;
	int curr_x = 0;
	int curr_y = 0;

	//for finding the dest index
	int n = maze[0].size();
	if(findMazePath(curr_x, curr_y, sol, x_coor, y_coor, n, maze))
		printSolution(sol);
	else
		cout<<"No path is possible !";
}

//checks if a node can be considered for being part of hamiltonian path
bool isSafe(vector<int> path, int curr_v, int count){
	for (int i = 0; i < count; ++i)
	{
		if(path[i] == curr_v)
			return false;
	}
	return true;
}

bool hamiltonianCycle(vector< vector<bool> > &graph, vector<int> &path,
					 int curr_v, int count){
	//when all nodes are visited and the start node has an edge with the
	//last node
	if (count >= graph[0].size() && graph[curr_v][path[0]] == true)
		return true;

	//for each vertex visit its adjacent vertices
	for (int i = 0; i < graph[0].size(); ++i)
	{
		//whether there is an edge with the vertex or not
		if(graph[curr_v][i] == true){
				//check for that vertex
			if(isSafe(path, i, count)){
				//add the vertex to the path
				path[count] = i;

				if(hamiltonianCycle(graph,path,i,count+1))
					return true;

			}
		}
	}
	return false;
}

///driver function for finding the presence of hamiltonian cycle in a graph
void hamiltonianCycleDriver(vector< vector<bool> > &graph){
	//for keeping track of sequence of vertices already covered ,initially all are marked as unvisited
	vector<int> path(graph[0].size(),-1);

	//for keeping track of vertices visited
	//we start with the first node 0
	int count = 0;
	//shows that the first node for the hamiltonian path is 0
	path[count] = 0;
	int curr_v = 0;
	if( hamiltonianCycle(graph,path,curr_v,count+1))
		printPath(path);
	else
		cout<<"No hamiltonian cycle is present\n";
}


map<int,pair<int,int> > vertex;
//pair<GGG,vector<double> >
pair<Graph,vector<double> > buildGrapgh(string srcfile)
{
  ifstream infile;
  infile.open(srcfile);
  int column,row;
  infile>>column;
  infile>>row;
  //cout<<row<<" "<<column;

  int **map=new int*[row];
  int **mark=new int*[row];
  for(int i=0;i<row;i++)
  {
    mark[i]=new int[column];
    map[i]=new int[column];
  }


  vector<pair<int,int> > edge;


  int vertexMark=0;
  int amtEdges=0,amtVertices=0;
  for(int r=row-1;r>=0;r--)
   for(int c=0;c<=column-1;c++)
   {
       //cout<<r<< " "<<c<<"\n";
       int tmp;
       infile>>tmp;
       if(tmp==0)
       {
         map[r][c]=tmp;
         mark[r][c]=vertexMark;
         vertex[vertexMark]=make_pair(r,c);
         amtVertices++;
         vertexMark++;
         // check whether exist edge
         if((r<row-1 && r >0)&&(c<column-1 && c >0)) // chek within the wall
         {
          if(map[r+1][c]==0)
          {
            edge.push_back(make_pair(mark[r+1][c],mark[r][c]));
            amtEdges++;
          }
          if(map[r][c-1]==0)
          {
           edge.push_back(make_pair(mark[r][c-1],mark[r][c]));
           amtEdges++;
          }
         }
       }
      else
      {
          map[r][c]=tmp;
          mark[r][c]=-1;
      }
     }
     Graph G(amtVertices);
     vector<double> weight(amtEdges);
     for(int i=0;i<amtEdges;i++)
     {
       G.AddEdge(edge[i].first,edge[i].second);
       weight[G.GetEdgeIndex(edge[i].first,edge[i].second)]=1.0;
     }
     infile.close();
     return make_pair(G, weight);

}


int main(int argc, char const *argv[]) {
  pair<Graph,vector<double> > p=buildGrapgh(argv[1]);
	int arrss[] = {10, 20, 15, 17, 9, 21};
	int nss = sizeof(arrss) / sizeof(arrss[0]);
  heapSort(arrss, nss);
	const int capacity = 100;
	int arrpp[capacity] = {4, 5, 6, 7, 8, 9, 10};

	buildHeap(arrpp, 7, 3);
  Graph g=p.first;
  vector<double> weight=p.second;
Job arrayq[] = { {'a', 2, 100}, {'b', 1, 19}, {'c', 2, 27},
								 {'d', 1, 25}, {'e', 3, 15}};
int b = sizeof(arrayq)/sizeof(arrayq[0]);
printJobScheduling(arrayq, b);
printJobScheduling(arrayq, b);

struct node *root = newNode(100);
root->left = newNode(50);
root->right = newNode(200);
root->left->left = newNode(40);
root->left->left->left = newNode(30);
root->left->left->left->left = newNode(20);
root = search(root, 20);
 preOrder(root);
 pair<list<int>,double> sol= solver(g,weight);
 list<int> s=sol.first;
 int graph[][Q] = { { 0, 10, 15, 20 },
										{ 10, 0, 35, 25 },
										{ 15, 35, 0, 30 },
										{ 20, 25, 30, 0 } };
 int len = 0;
 int mpp=travllingSalesmanProblem(graph, len);
 int arr[] = {10, 7, 8, 9, 1, 5};
int n = sizeof(arr)/sizeof(arr[0]);
quickSort(arr, 0, n-1);
mergeSort(arr, 0, n - 1);
MatMaze obj_1;
MatMaze_short_Dist obj_2;
obj_1.readFile(argv[1]);
obj_2.readFile(argv[1]);
  ofstream outfile;
  outfile.open(argv[2]);
  for(list<int>::iterator it=s.begin();it!=s.end();it++)
   outfile<<vertex[*it].second<<" "<<vertex[*it].first<<"\n";
	 GGG M(4);
	M.addEdge(0, 1);
	M.addEdge(0, 2);
	M.addEdge(1, 2);
	M.addEdge(2, 0);
	M.addEdge(2, 3);
	M.addEdge(3, 3);
	M.DFS(2);
	Stack *w = new Stack();
	w->push(1);
	w->push(2);
	w->push(3);
	w->push(4);
	w->reverse();
  outfile.close();

  return 0;
}
