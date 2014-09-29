HiggsML
=======

This is the code for my second place finish in Kaggle's [HiggsML challenge](http://www.kaggle.com/c/higgs-boson). It is a blend of a large number of boosted decision tree ensembles constructed using [Regularized Greedy Forest](http://arxiv.org/pdf/1109.0887.pdf).

**Reproducing my result can be done by taking the following steps:**

#### 1. Training

1. Place the training file "training.csv" in the data directory.
2. Run the following scripts in the python_train_predict directory:
  * start_rgf_models_7_f_s.py
  * start_rgf_models_7_f_s_exp.py
  * start_rgf_models_7_f_w.py
  * start_rgf_models_7_f_w_exp.py
  * start_rgf_models_7_f_wl.py
  * start_rgf_models_7_f_wl_exp.py

The scripts run 7 fold cross validation and train a model on the complete training set. Running these takes a lot of memory and CPU's. I recommend to run each script separately on a machine with 8+ cores and 64+ GB of memory, wait 2 days, and then to collect the results. I myself used the *m2.4xlarge* instances on [Amazon EC2](http://aws.amazon.com/ec2/). You can also download the results from my [Google drive](https://drive.google.com/file/d/0B4Zly9eEgwFsbUx5cm15UHpJZTg/edit?usp=sharing).

#### 2. Predicting the test set
  
1. Place the test file "test.csv" in the data directory.
2. Run "make_predictions.py" in the python_train_predict directory to run the trained RGF models on the test set. This is relatively lightweight and can be done on a single machine if you have some patience. (the predictions are also included in my [Google drive](https://drive.google.com/file/d/0B4Zly9eEgwFsbUx5cm15UHpJZTg/edit?usp=sharing))

#### 3. Blending

1. Make sure "training.csv" and "test.csv" are both in the data directory and you have all the results from the 2 steps above.
2. Run the MATLAB script "do_blend.m" in the matlab_blend directory. This produces my final prediction file and writes it to the matlab_blend directory.

## Prerequisites
* The scripts each train 8 models in parallel. This requires 64GB+ of memory.
* The [RGF package](http://stat.rutgers.edu/home/tzhang/software/rgf/) was built using 64bit Linux
* Perl should be installed and present on the path
* The Python packages pandas and numpy should be installed. These are included with any of the major Python distributions (Anaconda, EPD, Canopy, etc)
* The blending part requires MATLAB 2014

## Modeling insights
The problem being solved in this competition is a relatively standard classification problem where *collision events* (rows of data) are classified as either *signal* 's' or *background* 'b'. The accuracy of your solution was judged based on the *AMS metric*, which basically requires you to:

1. Build a model to estimate the ratio R = E(weight * I(label='s') | x) / E(weight * I(label='b') | x)
2. Choose a cutoff percentage and predict 's' for those rows where this estimated ratio is highest.

Here *I()* is a binary indicator function, *weight* is the weight for a row of data (not given for the test set), and *x* is the detector data that we base our prediction on.

Rather than focusing on the nonstandard AMS metric and empirical error minimization, my approach was to try and maximize statistical efficiency. The ratio given above can be decomposed as R = R1 * R2, where

R1 = P( label='s' | x) / P( label='b' | x)

R2 = E( weight | label='s', x ) / E( weight | label='b', x)

I figured it would be more statistically efficient to estimate the ratios R1 and R2 from the data and then to combine them rather than estimating R directly, as the effective sample size for estimating R1 and R2 is larger than for estimating R. Empirically I found that both this approach and the direct approach had some merit, so I combined them in a blend.

I'll try to write down some more of my thoughts on the modeling aspects in a blog post soon. (when I get some time) 

## References
* http://www.kaggle.com/c/higgs-boson/data "Higgs Boson Machine Learning Challenge"
* http://arxiv.org/pdf/1109.0887.pdf Rie Johnson and Tong Zhang. "Learning nonlinear functions using regularized greedy forest", *IEEE Transactions on Pattern Analysis and Machine Intelligence*, 36(5):942-954, May 2014.
* http://stat.rutgers.edu/home/tzhang/software/rgf/ "Regularized Greedy Forest (RGF) in C++"

