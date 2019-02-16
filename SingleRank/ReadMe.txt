Keyphrase Extraction Package version 1.0
----------------------------------------
SingleRank
----------
Code and its release notes written by Kazi Saidul Hasan (saidul@hlt.utdallas.edu).

Linux g++ compiler is needed to compile and run the code. The code is provided 'as is' without warranty of any kind.

The software package is free; anyone can use and/or modify its code for educational, research, and/or non-profit purposes only. 

Parameter settings
------------------
The code directory includes a file named 'paramsSingleRank.txt', which lists the parameters to set before running the code. Following is the content of an empty 'paramsSingleRank.txt' file.

Input_file_list=
Gold_standard_file_list=
Input_file_directory=
Gold_standard_file_directory=
Output_file_directory=
Keyphrases_per_file=
Window_size=

Input_file_list and Gold_standard_file_list refer to the files containing the list of input and gold standard file (if available) names, respectively. In case there is no gold standard file, the corresponding line in the 'paramsSingleRank.txt' file should be left blank after '='. Input_file_directory and Gold_standard_file_directory indicate the path to the directories where the input and gold standard files (if available) are stored, respectively. Output_file_directory refers to the path where the output files are created. Keyphrases_per_file denotes the number of keyphrases to be predicted for each input file. Finally, Window_size refers to the size of the co-occurrence window.

How to compile and run
----------------------
After unzipping the code, open a shell, go to the code directory, and type the following command to compile the code.

./compile.sh

Once compiled, the code can be executed using the following command.

./SingleRank paramsSingleRank.txt

