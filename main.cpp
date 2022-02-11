#include <iostream>
#include "avl_tree.hpp"

int main() {
    avl_tree<int, std::string> tree;
    tree.insert(5, "asd");
    tree.insert(6, "sdfs");
    tree.insert(8, "sdfsddfs");

    tree.erase(6);
    auto var = tree.find(5);

    //this is some god damn ugly solution
    //still better than returning a null pointer i guess
    std::cout << var.value_or(std::make_shared< avl_node <int, std::string > >(-1, std::string("error")))->data;
    return 0;
}
