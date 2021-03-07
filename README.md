# Algorithms for Adaptive Seed Minimization

Codes for the Trim, i.e., the adaptive algorithm for adaptive SM problem that provides expected approximation ratio. More details refer to paper: https://arxiv.org/abs/1907.09668.



### Running Environment

Linux-based OS



### Input requirements

**Dataset files**: dataset files are referred to $\textit{Tested-Dataset}$ repository.

**Realizations files**: please refer to $\textit{Tools}$ repository for source code to generate realizations.

*Remark*: please note that generated realization files are supposed to locate in the same folder of dataset file according to my implementation.



### How to run

#### Running command

./algo -dataset path_to_dataset -model IC -epsilon ε -Q percentage -batch batch_size -seedfile filename -time time_number

#### Explain

--epsilon:  an float number in range (0,1) to control the approximation error.

--Q: the percentage of nodes required to be activated.

--batch: the size of batch b selected each time. 

--seedfile: the file records the k seed nodes selected.

--time: the number of the algorithm repeated.

#### For example:

./trim -dataset dataset/hep/ -model IC -epsilon 0.5 -Q 0.1 -batch 10 -seedfile seed -time 1



### Datasets

Tested datasets can be downloaded in $\textit{Tested-Dataset}$ repository.



### Remark

If there are any problems, please contact khuang005@ntu.edu.sg / kkhuang@nus.edu.sg. (https://sites.google.com/view/kekehuang/)

