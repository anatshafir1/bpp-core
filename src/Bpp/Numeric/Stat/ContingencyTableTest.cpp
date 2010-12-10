//
// File: ContingencyTableTest.cpp
// Created by: Julien Dutheil
// Created on: Thu Dec 09 14:20 2010
//

/*
Copyright or © or Copr. Bio++ Development Team, (November 17, 2004)

This software is a computer program whose purpose is to provide classes
for numerical calculus.

This software is governed by the CeCILL  license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#include "ContingencyTableTest.h"
#include "../Random/ContingencyTableGenerator.h"

#include "../../App/ApplicationTools.h"
#include "../VectorTools.h"
#include "../Random/RandomTools.h"

#include <iostream>
#include <algorithm>

using namespace bpp;
using namespace std;

ContingencyTableTest::ContingencyTableTest(const std::vector< std::vector<unsigned int> >& table, unsigned int nbPermutations):
  statistic_(0),
  pvalue_(0),
  df_(0),
  margin1_(table.size()),
  margin2_(0)
{
  //Compute marginals:
  unsigned int n = table.size();
  if (n < 2)
    throw Exception("ContingencyTableTest. Table size should be at least 2x2!");
  unsigned int m = table[0].size();
  if (m < 2)
    throw Exception("ContingencyTableTest. Table size should be at least 2x2!");
  margin2_.resize(m);
  for (unsigned int j = 0; j < m; ++j)
    margin2_[j] = 0.;
  bool test = false;
  for (size_t i = 0; i < n; ++i) {
    if (table[i].size() != m)
      throw Exception("ContingencyTableTest. Input array has non-homogeneous dimensions!");
    for (size_t j = 0; j < m; ++j) {
      double c = table[i][j];
      if (c <= 5) test = true;
      margin1_[i] += c;
      margin2_[j] += c;
    }
  }
  unsigned int tot = VectorTools::sum(margin1_);
  df_ = static_cast<double>((m - 1) * (n - 1));
  
  RowMatrix<double> expc(n, m);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      double c = table[i][j];
      double e = static_cast<double>(margin1_[i] * margin2_[j]) / static_cast<double>(tot);
      expc(i, j) = e;
      statistic_ += std::pow(c - e, 2.) / e;
    }
  }

  if (nbPermutations > 0) {
    unsigned int count = 0;
    ContingencyTableGenerator ctgen(margin1_, margin2_);
    for (unsigned int k = 0; k < nbPermutations; ++k) {
      //Randomize:
      RowMatrix<unsigned int> table_rep = ctgen.rcont2();
      //Recompute statistic:
      double stat_rep = 0;
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
          double c = table_rep(i , j);
          double e = expc(i, j);
          stat_rep += std::pow(c - e, 2.) / e;
        }
      }
      if (stat_rep >= statistic_)
        count++;
    }
    pvalue_ = static_cast<double>(count + 1) / static_cast<double>(nbPermutations + 1);
  } else {
    if (test)
      ApplicationTools::displayWarning("Unsufficient observations, p-value might be incorrect.");

    //Compute p-value:
    pvalue_ = 1. - RandomTools::pChisq(statistic_, df_);
  }
}

