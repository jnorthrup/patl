 # PATL (Practical Algorithm Template Library) #
 
  Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)

 ## Major features 
 * STL-compatible, high-performance template containers and auxiliary classes useful for stringology with fine-grained low level design

 ### docs ( http://jnorthrup.github.io/patl/ )

## Library features

  * set & map containers with trie (digital/radix search tree) functionality :
  
  trie_set & trie_map;

  * set & map containers with linear time & space online suffix tree functionality:
 
 suffix_set & suffix_map - 16 bytes per symbol, alphabet-independent;

  * bunch of iterators over each type of containers - standard (const_)(reverse_)iterator, special (const_)(post|pre)order_iterator for post- and preorder traversal, partial match iterators (const_)partimator and suffix_cont-special match_iterator & (super_)maxrep_iterator for linear-time matching statistics and (super)maximal repeats;

  * partimator's can be specialized with one of the decision functors: partial_match (filter strings with joker pattern), hamming_distance (filter strings with Hamming automata), levenshtein_(tp_|ms_)distance (filter strings with Levenshtein automata with optional operations like transposition or merge-split);

  * lowest common ancestor (LCA) constant time requests after linear time & memory postprocessing of the suffix_cont - lca_oracle;

   *    patricia_dot_creator:
    utility class to create .dot graphviz input files for internal data structure visualization.

All features of this library can be examined with small and clever demo programs delivered within.

  ## Compilers: Visual C++ 2003/2005/2008, GCC G++ 3.x 

 ## In russian ##
 
  или с чего все начиналось: в сентябре 2005 (http://rsdn.ru/Forum/Message.aspx?mid=1362686&only=) и продолжение в году 2008 (http://rsdn.ru/Forum/Message.aspx?mid=2961681&only=).

В текущем состоянии в библиотеке реализовано множество алгоритмов, востребованных в обработке текстов и двоичных данных, в методах сжатия информации и выделении повторяющихся структур.
