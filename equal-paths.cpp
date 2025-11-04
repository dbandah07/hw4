#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include <iostream>
#include <algorithm>
#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int checkHeight(Node* root) {
  if (root == nullptr) {
    return 0;
  }
  int left = checkHeight(root->left);
  int right = checkHeight(root->right);

  // if either side fails, or differs, return -1 (false)
  if (left == -1 || right == -1 || (left != 0 && right != 0 && left != right)) {
    return -1;
  }

  return max(left, right) + 1;
}

bool equalPaths(Node * root)
{
    // Add your code below
  if (root == nullptr) return true;

  return checkHeight(root) != -1; 
}

