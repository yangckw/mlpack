/**
  * @file single_tree_traverser_impl.hpp
  * @author Andrew Wells
  *
  * A class for traversing rectangle type trees with a given set of rules
  * which indicate the branches to prune and the order in which to recurse.
  * This is a depth-first traverser.
  */
#ifndef __MLPAC_CORE_TREE_RECTANGLE_TREE_SINGLE_TREE_TRAVERSER_IMPL_HPP
#define __MLPAC_CORE_TREE_RECTANGLE_TREE_SINGLE_TREE_TRAVERSER_IMPL_HPP

#include "single_tree_traverser.hpp"

#include <algorithm>
#include <stack>

namespace mlpack {
namespace tree {
  
template<typename SplitType,
         typename DescentType,
	 typename StatisticType,
         typename MatType>
template<typename RuleType>
RectangleTree<SplitType, DescentType, StatisticType, MatType>::
SingleTreeTraverser<RuleType>::SingleTreeTraverser(RuleType& rule) :
    rule(rule),
    numPrunes(0)
{ /* Nothing to do */ }

template<typename SplitType,
         typename DescentType,
	 typename StatisticType,
         typename MatType>
template<typename RuleType>
void RectangleTree<SplitType, DescentType, StatisticType, MatType>::
SingleTreeTraverser<RuleType>::Traverse(
    const size_t queryIndex,
    const RectangleTree<SplitType, DescentType, StatisticType, MatType>&
        referenceNode)
{
  
  // If we reach a leaf node, we need to run the base case.
  if(referenceNode.IsLeaf()) {
    std::cout << "we reached a leaf" << std::endl;
    for(size_t i = 0; i < referenceNode.Count(); i++) {
      rule.BaseCase(queryIndex, referenceNode.Points()[i]);
    }
    return;
  }
  
  // This is not a leaf node so we sort the children of this node by their scores.    
  std::vector<NodeAndScore> nodesAndScores(referenceNode.NumChildren());
  for(int i = 0; i < referenceNode.NumChildren(); i++) {
    nodesAndScores[i].node = referenceNode.Children()[i];
    nodesAndScores[i].score = rule.Score(queryIndex, *nodesAndScores[i].node);
  }
  
  std::sort(nodesAndScores.begin(), nodesAndScores.end(), nodeComparator);
  
  // Now iterate through them starting with the best and stopping when we reach
  // one that isn't good enough.
  for(int i = 0; i < referenceNode.NumChildren(); i++) {
    if(rule.Rescore(queryIndex, *nodesAndScores[i].node, nodesAndScores[i].score) != DBL_MAX)
      Traverse(queryIndex, nodesAndScores[i].node);
    else {
      std::cout << "we are pruning: " << referenceNode.NumChildren() - i << " nodes." << std::endl;
      numPrunes += referenceNode.NumChildren() - i;
      return;
    }
  }
  // We only get here if we couldn't prune any of them.
  return;
}

}; // namespace tree
}; // namespace mlpack

#endif