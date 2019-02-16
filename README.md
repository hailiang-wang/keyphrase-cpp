Mirror codes of http://www.hlt.utdallas.edu/~saidul/code.html

# Keyphrase Extraction
Given a text, an automatic keyphrase extraction method retrieves phrase(s) conveying the main points elaborated in the text. The package provided here includes implementations of the following, widely-explored unsupervised approaches.

* Tf-Idf
* TextRank (Mihalcea & Tarau, 2004)
* SingleRank (Wan & Xiao, 2008)
* ExpandRank (Wan & Xiao, 2008)

Each of these methods takes as input one or more text file(s) annotated with Penn Treebank PoS tags (see a sample input file). A separate output file containing a list of keyphrases is created for each input file when a collection of files are given as input. Optionally, in case the set of input text files has its own gold standard keyphrases, each method generates its overall performance scores in terms of recall, precision, and F-score. A ReadMe file with each method describes how a user can set this and other parameters specific to each method.

## Download
Download the [Keyphrase Extraction Package version 1.0](http://www.hlt.utdallas.edu/~saidul/code/Keyphrase_Extractor-1.0.zip) including the implementations of the methods listed above.


## Paper to Cite
Please acknowledge your use of this software package by citing the following paper:

[Conundrums in Unsupervised Keyphrase Extraction: Making Sense of the State-of-the-Art](./acl14.pdf)
Kazi Saidul Hasan and Vincent Ng 
Proceedings of the 23rd International Conference on Computational Linguistics (COLING-10), Posters Volume, 2010.

## Versions
Date    Version #   Note
November 14, 2010

Version-1.0

Initial release


## Queries
Please send your questions, bug reports, comments, concerns, and suggestions to saidul AT hlt DOT utdallas DOT edu.